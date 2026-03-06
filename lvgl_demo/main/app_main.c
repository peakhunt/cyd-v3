#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "st7789_lcd.h"
#include "xpt2046_touch.h"
#include "lvgl_port_intf.h"
#include "esp_lvgl_port.h"
#include "fw_upd_demo.h"


// Track current mode: 0 = quad screen, 1 = full screen
void app_main(void)
{
  st7789_init_lcd();
  xpt2046_touch_init();
  lvgl_port_intf_init();

  //app_main_display3();
  fw_upd_demo_init();

  while (true) {
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
