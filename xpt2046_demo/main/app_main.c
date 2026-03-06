#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "st7789_lcd.h"
#include "xpt2046_touch.h"

static const char *TAG = "lcd_example";

#define COLOR_RED   0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE  0x001F
#define COLOR_WHITE 0xffff

typedef struct
{
  uint16_t    topLeft;
  uint16_t    topRight;
  uint16_t    bottomLeft;
  uint16_t    bottomRight;
} ColorAssign;

static ColorAssign  _colorAssign =
{
  .topLeft      = COLOR_RED,
  .topRight     = COLOR_BLUE,
  .bottomLeft   = COLOR_WHITE,
  .bottomRight  = COLOR_GREEN,
};

// Track current mode: 0 = quad screen, 1 = full screen
static int current_mode = 0;
static uint16_t current_color = COLOR_WHITE;

static void draw_quad_screen(void)
{
  st7789_draw_quad_screen(
    _colorAssign.topLeft,
    _colorAssign.topRight,
    _colorAssign.bottomLeft,
    _colorAssign.bottomRight
  );
}

static
uint16_t get_quad_color(uint16_t x, uint16_t y)
{
  if (x < 120 && y < 160)
  {
    return _colorAssign.topLeft;
  }
  else if (x >= 120 && y < 160)
  {
    return _colorAssign.topRight;
  }
  else if (x < 120 && y >= 160)
  {
    return _colorAssign.bottomLeft;
  }
  return _colorAssign.bottomRight;
}

void app_main(void)
{
  st7789_init_lcd();
  xpt2046_touch_init();

  // Start with quad screen
  draw_quad_screen();
  current_mode = 0;

  while (true) {
    uint16_t x, y;
    if (xpt2046_touch_read(&x, &y)) {
      ESP_LOGI(TAG, "Touch at (%d,%d)", x, y);

      if (current_mode == 0) {
        // Quad mode → determine quadrant
        current_color = get_quad_color(x, y);
        st7789_fill_screen(current_color);
        current_mode = 1;
      } else {
        // Fullscreen mode → back to quad
        draw_quad_screen();
        current_mode = 0;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
