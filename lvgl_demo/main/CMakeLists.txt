set(srcs "app_main.c"
         "st7789_lcd.c"
         "xpt2046_touch.c"
         "lvgl_port_intf.c")

 set(includes ".")

idf_component_register(SRCS ${srcs}
                       REQUIRES esp_lcd esp_driver_spi
                       INCLUDE_DIRS ${includes})

lvgl_port_create_c_image("images/esp_logo.png" "images/" "ARGB8888" "NONE")
lvgl_port_add_images(${COMPONENT_LIB} "images/")
