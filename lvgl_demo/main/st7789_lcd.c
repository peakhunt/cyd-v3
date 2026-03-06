#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "driver/gpio.h"
#include "st7789_lcd.h"

#define LCD_HOST    SPI2_HOST
#define PIN_NUM_MOSI 13
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   15 
#define PIN_NUM_DC   2
#define PIN_NUM_RST  -1
#define PIN_NUM_BCKL 21

esp_lcd_panel_handle_t _panel_handle;
esp_lcd_panel_io_handle_t _io_handle;

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
st7789_init_lcd(void)
{
  // Step 1: Configure SPI bus
  spi_bus_config_t buscfg = {
    .mosi_io_num = PIN_NUM_MOSI,
    .miso_io_num = -1,
    .sclk_io_num = PIN_NUM_CLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = LCD_WIDTH * DRAW_BUFFER_HEIGHT * sizeof(uint16_t),
  };
  ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

  // Step 2: Create panel IO (SPI)
  esp_lcd_panel_io_spi_config_t io_config = {
    .cs_gpio_num = PIN_NUM_CS,
    .dc_gpio_num = PIN_NUM_DC,
    .spi_mode = 0,
    .trans_queue_depth = 10,
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
    //.color_space = ESP_LCD_COLOR_SPACE_RGB,
    .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
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
