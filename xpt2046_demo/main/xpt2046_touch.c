#include "xpt2046_touch.h"
#include "esp_lcd_touch_xpt2046.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"

// --- Pin macros for CYD v3 ---
#define XPT2046_PIN_MOSI   32
#define XPT2046_PIN_MISO   39
#define XPT2046_PIN_CLK    25
#define XPT2046_PIN_CS     33
#define XPT2046_PIN_IRQ    36

static esp_lcd_touch_handle_t xpt2046_handle = NULL;

esp_err_t xpt2046_touch_init(void)
{
    // SPI bus config
    spi_bus_config_t buscfg = {
        .mosi_io_num = XPT2046_PIN_MOSI,
        .miso_io_num = XPT2046_PIN_MISO,
        .sclk_io_num = XPT2046_PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO));

    // Panel IO config
    esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = XPT2046_PIN_CS,
        .dc_gpio_num = -1,
        .spi_mode = 0,
        .pclk_hz = 2 * 1000 * 1000,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .trans_queue_depth = 16,
    };

    esp_lcd_panel_io_handle_t io_handle;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI3_HOST, &io_config, &io_handle));

    // Touch config
    esp_lcd_touch_config_t touch_config = {
        .x_max = 240,
        .y_max = 320,
        .rst_gpio_num = -1,
        .int_gpio_num = XPT2046_PIN_IRQ,
    };

    esp_err_t ret;

    ret = esp_lcd_touch_new_spi_xpt2046(io_handle, &touch_config, &xpt2046_handle);
    esp_lcd_touch_set_mirror_x(xpt2046_handle, true);
    return ret;
}

bool xpt2046_touch_read(uint16_t *x, uint16_t *y)
{
    esp_lcd_touch_point_data_t points[1];
    uint8_t point_cnt = 0;

    esp_lcd_touch_read_data(xpt2046_handle);
    if (esp_lcd_touch_get_data(xpt2046_handle, points, &point_cnt, 1) == ESP_OK && point_cnt > 0) {
        *x = points[0].x;
        *y = points[0].y;
        return true;
    }
    return false;
}
