#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "esp_lvgl_port.h"
#include "esp_check.h"
#include "st7789_lcd.h"
#include "xpt2046_touch.h"
#include "lvgl_port_intf.h"

#define USE_DOUBLE_BUFFER       (1)

static const char* TAG = "lvgl_port_intf";

static lv_disp_t * lvgl_disp        = NULL;
static lv_indev_t *lvgl_touch_indev = NULL;

LV_IMG_DECLARE(esp_logo)

static void
_app_button_cb(lv_event_t *e)
{
  ESP_LOGI(TAG, "_app_button_cb");

  lvgl_port_lock(0);
  lv_disp_rotation_t rotation = lv_disp_get_rotation(lvgl_disp);
  rotation++;
  if (rotation > LV_DISPLAY_ROTATION_270) {
    rotation = LV_DISPLAY_ROTATION_0;
  }

  /* LCD HW rotation */
  lv_disp_set_rotation(lvgl_disp, rotation);
  lvgl_port_unlock();
}

void
app_main_display(void)
{
  lvgl_port_lock(0);

  lv_obj_t *scr = lv_scr_act();


  /* Your LVGL objects code here .... */

  /* Create image */
  lv_obj_t *img_logo = lv_img_create(scr);
  lv_img_set_src(img_logo, &esp_logo);
  lv_obj_align(img_logo, LV_ALIGN_TOP_MID, 0, 20);

  /* Label */
  lv_obj_t *label = lv_label_create(scr);
  lv_obj_set_width(label, LCD_WIDTH);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
#if LVGL_VERSION_MAJOR == 8
  lv_label_set_recolor(label, true);
  lv_label_set_text(label,
      "#FF0000 "LV_SYMBOL_BELL" Hello world Espressif and LVGL "LV_SYMBOL_BELL"#\n#FF9400 "LV_SYMBOL_WARNING" For simplier initialization, use BSP "LV_SYMBOL_WARNING" #");
#else
  lv_label_set_text(label,
      LV_SYMBOL_BELL" Hello world Espressif and LVGL "LV_SYMBOL_BELL"\n "LV_SYMBOL_WARNING" For simplier initialization, use BSP "LV_SYMBOL_WARNING);
#endif
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 20);

  /* Button */
  lv_obj_t *btn = lv_btn_create(scr);
  label = lv_label_create(btn);
  lv_label_set_text_static(label, "Rotate screen");
  lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -30);
  lv_obj_add_event_cb(btn, _app_button_cb, LV_EVENT_CLICKED, NULL);

  /* Task unlock */
  lvgl_port_unlock();
}

static void set_temp(void * bar, int32_t temp)
{
  lvgl_port_lock(0);
  lv_bar_set_value((lv_obj_t *)bar, temp, LV_ANIM_ON);
  lvgl_port_unlock();
}

/**
 * A temperature meter example
 */
void
app_main_display2(void)
{
  static lv_style_t style_indic;

  lvgl_port_lock(0);
  lv_style_init(&style_indic);
  lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
  lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_RED));
  lv_style_set_bg_grad_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));
  lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_VER);

  lv_obj_t * bar = lv_bar_create(lv_screen_active());
  lv_obj_add_style(bar, &style_indic, LV_PART_INDICATOR);
  lv_obj_set_size(bar, 20, 200);
  lv_obj_center(bar);
  lv_bar_set_range(bar, -20, 40);

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_exec_cb(&a, set_temp);
  lv_anim_set_duration(&a, 3000);
  lv_anim_set_reverse_duration(&a, 3000);
  lv_anim_set_var(&a, bar);
  lv_anim_set_values(&a, -20, 40);
  lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
  lv_anim_start(&a);
  lvgl_port_unlock();
}

static int32_t hr_value = 98;
static int8_t hr_step = 1;
static lv_obj_t * needle_line = NULL;
static lv_obj_t * hr_value_label = NULL;
static lv_obj_t * bpm_label = NULL;
static lv_obj_t * scale = NULL;

typedef struct {
  lv_style_t items;
  lv_style_t indicator;
  lv_style_t main;
} section_styles_t;

static section_styles_t zone1_styles;
static section_styles_t zone2_styles;
static section_styles_t zone3_styles;
static section_styles_t zone4_styles;
static section_styles_t zone5_styles;

static lv_color_t get_hr_zone_color(int32_t hr)
{
  if(hr < 117) return lv_palette_main(LV_PALETTE_GREY); /* Zone 1 */
  else if(hr < 135) return lv_palette_main(LV_PALETTE_BLUE); /* Zone 2 */
  else if(hr < 158) return lv_palette_main(LV_PALETTE_GREEN); /* Zone 3 */
  else if(hr < 176) return lv_palette_main(LV_PALETTE_ORANGE); /* Zone 4 */
  else return lv_palette_main(LV_PALETTE_RED); /* Zone 5 */
}

static void hr_anim_timer_cb(lv_timer_t * timer)
{
  lvgl_port_lock(0);
  LV_UNUSED(timer);

  hr_value += hr_step;

  if(hr_value >= 195) {
    hr_value = 195;
    hr_step = -1;
  }
  else if(hr_value <= 98) {
    hr_value = 98;
    hr_step = 1;
  }

  /* Update needle */
  lv_scale_set_line_needle_value(scale, needle_line, 180, hr_value);

  /* Update HR text */
  lv_label_set_text_fmt(hr_value_label, "%ld", hr_value);

  /* Update text color based on zone */
  lv_color_t zone_color = get_hr_zone_color(hr_value);
  lv_obj_set_style_text_color(hr_value_label, zone_color, 0);
  lv_obj_set_style_text_color(bpm_label, zone_color, 0);
  lvgl_port_unlock();
}

static void init_section_styles(section_styles_t * styles, lv_color_t color)
{
  lv_style_init(&styles->items);
  lv_style_set_line_color(&styles->items, color);
  lv_style_set_line_width(&styles->items, 0);

  lv_style_init(&styles->indicator);
  lv_style_set_line_color(&styles->indicator, color);
  lv_style_set_line_width(&styles->indicator, 0);

  lv_style_init(&styles->main);
  lv_style_set_arc_color(&styles->main, color);
  lv_style_set_arc_width(&styles->main, 20);
}

static void add_section(lv_obj_t * target_scale,
    int32_t from,
    int32_t to,
    const section_styles_t * styles)
{
  lv_scale_section_t * sec = lv_scale_add_section(target_scale);
  lv_scale_set_section_range(target_scale, sec, from, to);
  lv_scale_set_section_style_items(target_scale, sec, &styles->items);
  lv_scale_set_section_style_indicator(target_scale, sec, &styles->indicator);
  lv_scale_set_section_style_main(target_scale, sec, &styles->main);
}

void app_main_display3(void)
{
  lvgl_port_lock(0);
  scale = lv_scale_create(lv_screen_active());
  lv_obj_center(scale);
  lv_obj_set_size(scale, 200, 200);

  lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
  lv_scale_set_range(scale, 98, 195);
  lv_scale_set_total_tick_count(scale, 15);
  lv_scale_set_major_tick_every(scale, 3);
  lv_scale_set_angle_range(scale, 280);
  lv_scale_set_rotation(scale, 130);
  lv_scale_set_label_show(scale, false);

  lv_obj_set_style_length(scale, 6, LV_PART_ITEMS);
  lv_obj_set_style_length(scale, 10, LV_PART_INDICATOR);
  lv_obj_set_style_arc_width(scale, 0, LV_PART_MAIN);

  /* Zone 1: (Grey) */
  init_section_styles(&zone1_styles, lv_palette_main(LV_PALETTE_GREY));
  add_section(scale, 98, 117, &zone1_styles);

  /* Zone 2: (Blue) */
  init_section_styles(&zone2_styles, lv_palette_main(LV_PALETTE_BLUE));
  add_section(scale, 117, 135, &zone2_styles);

  /* Zone 3: (Green) */
  init_section_styles(&zone3_styles, lv_palette_main(LV_PALETTE_GREEN));
  add_section(scale, 135, 158, &zone3_styles);

  /* Zone 4: (Orange) */
  init_section_styles(&zone4_styles, lv_palette_main(LV_PALETTE_ORANGE));
  add_section(scale, 158, 176, &zone4_styles);

  /* Zone 5: (Red) */
  init_section_styles(&zone5_styles, lv_palette_main(LV_PALETTE_RED));
  add_section(scale, 176, 195, &zone5_styles);

  needle_line = lv_line_create(scale);

  /* Optional styling */
  lv_obj_set_style_line_color(needle_line, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_line_width(needle_line, 12, LV_PART_MAIN);
  lv_obj_set_style_length(needle_line, 20, LV_PART_MAIN);
  lv_obj_set_style_line_rounded(needle_line, false, LV_PART_MAIN);
  lv_obj_set_style_pad_right(needle_line, 50, LV_PART_MAIN);

  int32_t current_hr = 145;

  lv_scale_set_line_needle_value(scale, needle_line, 50, current_hr);

  lv_obj_t * circle = lv_obj_create(lv_screen_active());
  lv_obj_set_size(circle, 130, 130);
  lv_obj_center(circle);

  lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, 0);

  lv_obj_set_style_bg_color(circle, lv_obj_get_style_bg_color(lv_screen_active(), LV_PART_MAIN), 0);
  lv_obj_set_style_bg_opa(circle, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(circle, 0, LV_PART_MAIN);

  lv_obj_t * hr_container = lv_obj_create(circle);
  lv_obj_center(hr_container);
  lv_obj_set_size(hr_container, lv_pct(100), LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(hr_container, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(hr_container, 0, 0);
  lv_obj_set_layout(hr_container, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(hr_container, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(hr_container, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_row(hr_container, 0, 0);
  lv_obj_set_flex_align(hr_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  hr_value_label = lv_label_create(hr_container);
  lv_label_set_text_fmt(hr_value_label, "%ld", current_hr);
  lv_obj_set_style_text_font(hr_value_label, &lv_font_montserrat_40, 0);
  lv_obj_set_style_text_align(hr_value_label, LV_TEXT_ALIGN_CENTER, 0);

  bpm_label = lv_label_create(hr_container);
  lv_label_set_text(bpm_label, "bpm");
  lv_obj_set_style_text_font(bpm_label, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_align(bpm_label, LV_TEXT_ALIGN_CENTER, 0);

  lv_color_t zone_color = get_hr_zone_color(current_hr);
  lv_obj_set_style_text_color(hr_value_label, zone_color, 0);
  lv_obj_set_style_text_color(bpm_label, zone_color, 0);

  lv_timer_create(hr_anim_timer_cb, 80, NULL);
  lvgl_port_unlock();
}

esp_err_t
lvgl_port_intf_init(void)
{
  const lvgl_port_cfg_t lvgl_cfg = 
  {
    .task_priority = 4,         /* LVGL task priority */
    .task_stack = 4096*2,         /* LVGL task stack size */
    .task_affinity = -1,        /* LVGL task pinned to core (-1 is no affinity) */
    .task_max_sleep_ms = 500,   /* Maximum sleep in LVGL task */
    .timer_period_ms = 5        /* LVGL timer tick period in ms */
  };
  ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port initialization failed");

  const lvgl_port_display_cfg_t disp_cfg = 
  {
    .io_handle = _io_handle,
    .panel_handle = _panel_handle,
    .buffer_size = LCD_WIDTH * DRAW_BUFFER_HEIGHT,
    .double_buffer = USE_DOUBLE_BUFFER,
    .hres = LCD_WIDTH,
    .vres = LCD_HEIGHT,
    .monochrome = false,
    .color_format = LV_COLOR_FORMAT_RGB565,
    .rotation = {
      .swap_xy = false,
      .mirror_x = false,
      .mirror_y = false,
    },
    .flags = {
      .buff_dma = true,
      .swap_bytes = false,
    }
  };
  lvgl_disp = lvgl_port_add_disp(&disp_cfg);

  /* Add touch input (for selected screen) */
  const lvgl_port_touch_cfg_t touch_cfg =
  {
    .disp = lvgl_disp,
    .handle = xpt2046_handle, 
  };
  lvgl_touch_indev = lvgl_port_add_touch(&touch_cfg);

  lv_disp_set_rotation(lvgl_disp, LV_DISPLAY_ROTATION_90);
  return ESP_OK;
}
