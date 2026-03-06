set(srcs "app_main.c"
         "st7789_lcd.c"
         "xpt2046_touch.c")

 set(includes ".")

idf_component_register(SRCS ${srcs}
                       REQUIRES esp_lcd esp_driver_spi
                       INCLUDE_DIRS ${includes})
