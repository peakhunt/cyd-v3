#pragma once

#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"

#define LCD_WIDTH   240
#define LCD_HEIGHT  320
#define DRAW_BUFFER_HEIGHT      (LCD_HEIGHT / 4)

extern esp_lcd_panel_handle_t _panel_handle;
extern esp_lcd_panel_io_handle_t _io_handle;

extern void st7789_init_lcd(void);
extern void st7789_fill_screen(uint16_t color);
extern void st7789_disp_on_off(bool on);
extern void st7789_backlight_on_off(bool on);
extern void st7789_draw_quad_screen(uint16_t tl_color, uint16_t tr_color, uint16_t bl_color, uint16_t br_color);
