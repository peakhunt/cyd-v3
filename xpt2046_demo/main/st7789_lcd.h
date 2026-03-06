#pragma once

extern void st7789_init_lcd(void);
extern void st7789_fill_screen(uint16_t color);
extern void st7789_disp_on_off(bool on);
extern void st7789_backlight_on_off(bool on);
extern void st7789_draw_quad_screen(uint16_t tl_color, uint16_t tr_color, uint16_t bl_color, uint16_t br_color);
