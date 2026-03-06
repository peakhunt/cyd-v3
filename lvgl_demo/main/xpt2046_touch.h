#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>
#include "xpt2046_touch.h"
#include "esp_lcd_touch_xpt2046.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"

extern esp_lcd_touch_handle_t xpt2046_handle;
esp_err_t xpt2046_touch_init(void);
bool xpt2046_touch_read(uint16_t *x, uint16_t *y);
