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
#if 1
#define PIN_NUM_MOSI 13
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   15 
#define PIN_NUM_DC   2
#define PIN_NUM_RST  -1
#define PIN_NUM_BCKL 21
#else
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  25
#define PIN_NUM_CS   15 
#define PIN_NUM_DC   2
#define PIN_NUM_RST  18
#define PIN_NUM_BCKL 21
#endif

#define LCD_WIDTH   240
#define LCD_HEIGHT  320
#define CHUNK_LINES 20   // number of lines per chunk

static const char *TAG = "lcd_example";

void fill_screen(esp_lcd_panel_handle_t panel_handle, uint16_t color)
{
    static uint16_t line_buf[LCD_WIDTH * CHUNK_LINES];
    for (int i = 0; i < LCD_WIDTH * CHUNK_LINES; i++) {
        line_buf[i] = color;
    }

    for (int y = 0; y < LCD_HEIGHT; y += CHUNK_LINES) {
        int y_end = y + CHUNK_LINES;
        if (y_end > LCD_HEIGHT) y_end = LCD_HEIGHT;

        ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(
            panel_handle,
            0, y, LCD_WIDTH, y_end,
            line_buf
        ));
    }
}

void app_main(void)
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
    esp_lcd_panel_io_handle_t io_handle;
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
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(LCD_HOST, &io_config, &io_handle));

    // Step 3: Create ST7789 panel driver
    esp_lcd_panel_handle_t panel_handle;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_RST,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    // Step 4: Initialize panel
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, false));
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 0, 0));
    esp_lcd_panel_disp_on_off(panel_handle, true);

    // Step 5: Turn on backlight
    gpio_config_t bklt_cfg = {
        .pin_bit_mask = 1ULL << PIN_NUM_BCKL,
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&bklt_cfg);
    gpio_set_level(PIN_NUM_BCKL, 1);

    // Step 6: Draw something
    uint16_t r = 0xF800; // Red
    uint16_t g = 0x07E0; // Green
    uint16_t b = 0x001F; // Blue

    ESP_LOGI(TAG, "ST7789 initialized and filled with red color");

    // Stay alive
    while (true) {
        fill_screen(panel_handle, r);
        vTaskDelay(pdMS_TO_TICKS(1000));

        fill_screen(panel_handle, g);
        vTaskDelay(pdMS_TO_TICKS(1000));

        fill_screen(panel_handle, b);
        vTaskDelay(pdMS_TO_TICKS(1000));

        fill_screen(panel_handle, 0x0000);
        vTaskDelay(pdMS_TO_TICKS(1000));

        esp_lcd_panel_disp_on_off(panel_handle, false);
        gpio_set_level(PIN_NUM_BCKL, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
        esp_lcd_panel_disp_on_off(panel_handle, true);
        gpio_set_level(PIN_NUM_BCKL, 1);
    }
}

#if 0
static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
static uint8_t s_led_state = 0;

static void
blink_led(void)
{
  /* Set the GPIO level according to the state (LOW or HIGH)*/
  bool r, g, b;

  r = s_led_state & 0x01;
  g = s_led_state & 0x02;
  b = s_led_state & 0x04;

  gpio_set_level(CONFIG_BLINK_GPIO_RED, !r);
  gpio_set_level(CONFIG_BLINK_GPIO_GREEN, !g);
  gpio_set_level(CONFIG_BLINK_GPIO_BLUE, !b);
}

static void
configure_led(void)
{
  ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
  gpio_reset_pin(CONFIG_BLINK_GPIO_RED);
  gpio_reset_pin(CONFIG_BLINK_GPIO_GREEN);
  gpio_reset_pin(CONFIG_BLINK_GPIO_BLUE);

  /* Set the GPIO as a push/pull output */
  gpio_set_direction(CONFIG_BLINK_GPIO_RED, GPIO_MODE_OUTPUT);
  gpio_set_direction(CONFIG_BLINK_GPIO_GREEN, GPIO_MODE_OUTPUT);
  gpio_set_direction(CONFIG_BLINK_GPIO_BLUE, GPIO_MODE_OUTPUT);
}

void app_main(void)
{

  /* Configure the peripheral according to the LED type */
  configure_led();

  while (1) {
    ESP_LOGI(TAG, "Turning the LED 0x%x!", s_led_state);
    blink_led();
    /* Toggle the LED state */
    s_led_state = (s_led_state < 7) ? s_led_state + 1 : 0;
    vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
  }
}
#endif
