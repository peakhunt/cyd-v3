#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LCD_HOST    SPI2_HOST
#define PIN_NUM_MOSI 13
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   15 
#define PIN_NUM_DC   2
#define PIN_NUM_RST  -1
#define PIN_NUM_BCKL 21

#define LCD_WIDTH   240
#define LCD_HEIGHT  320
#define CHUNK_LINES 10   // number of lines per chunk
                         //
//static const char *TAG = "st7789_lcd";

static esp_lcd_panel_handle_t _panel_handle;
static esp_lcd_panel_io_handle_t _io_handle;

void
st7789_disp_on_off(bool on)
{
  esp_lcd_panel_disp_on_off(_panel_handle, on);
}

void
st7789_backlight_on_off(bool on)
{
  gpio_set_level(PIN_NUM_BCKL, on ? 1 : 0);
}

void
st7789_fill_screen(uint16_t color)
{
  static uint16_t line_buf[LCD_WIDTH * CHUNK_LINES];

  for (int i = 0; i < LCD_WIDTH * CHUNK_LINES; i++)
  {
    line_buf[i] = color;
  }

  for (int y = 0; y < LCD_HEIGHT; y += CHUNK_LINES)
  {
    int y_end = y + CHUNK_LINES;
    if (y_end > LCD_HEIGHT) y_end = LCD_HEIGHT;

    // XXX
    // this is actually a bug but for this one, it's okay
    // following esp_lcd_panel_draw_bitmap code, you will see
    // that it's kinda async using queue and spi transfer complete interrupt
    //
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(
          _panel_handle,
          0, y, LCD_WIDTH, y_end,
          line_buf
          ));
  }
}

void
st7789_init_lcd(void)
{
  // Step 1: Configure SPI bus
  spi_bus_config_t buscfg = {
    .mosi_io_num = PIN_NUM_MOSI,
    .miso_io_num = -1,
    .sclk_io_num = PIN_NUM_CLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = LCD_WIDTH * CHUNK_LINES * 2 + 10,
  };
  ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

  // Step 2: Create panel IO (SPI)
  esp_lcd_panel_io_spi_config_t io_config = {
    .cs_gpio_num = PIN_NUM_CS,
    .dc_gpio_num = PIN_NUM_DC,
    .spi_mode = 0,
    .trans_queue_depth = 16,
    .pclk_hz = 40 * 1000 * 1000,
    .lcd_cmd_bits = 8,
    .lcd_param_bits = 8,
    .on_color_trans_done = NULL,
    .user_ctx = NULL,
  };
  ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(LCD_HOST, &io_config, &_io_handle));

  // Step 3: Create ST7789 panel driver
  esp_lcd_panel_dev_config_t panel_config = {
    .reset_gpio_num = PIN_NUM_RST,
    .color_space = ESP_LCD_COLOR_SPACE_RGB,
    .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,
    .bits_per_pixel = 16,
  };
  ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(_io_handle, &panel_config, &_panel_handle));

  // Step 4: Initialize panel
  ESP_ERROR_CHECK(esp_lcd_panel_reset(_panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_init(_panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_invert_color(_panel_handle, false));
  ESP_ERROR_CHECK(esp_lcd_panel_set_gap(_panel_handle, 0, 0));
  esp_lcd_panel_disp_on_off(_panel_handle, true);

  // Step 5: Turn on backlight
  gpio_config_t bklt_cfg = {
    .pin_bit_mask = 1ULL << PIN_NUM_BCKL,
    .mode = GPIO_MODE_OUTPUT,
  };
  gpio_config(&bklt_cfg);
  gpio_set_level(PIN_NUM_BCKL, 1);
}
