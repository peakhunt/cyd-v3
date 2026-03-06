#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "st7789_lcd.h"

static const char *TAG = "lcd_example";


void
app_main(void)
{
  st7789_init_lcd();

  // Step 6: Draw something
  uint16_t r = 0xF800; // Red
  uint16_t g = 0x07E0; // Green
  uint16_t b = 0x001F; // Blue

  // Stay alive
  while (true) {
    ESP_LOGI(TAG, "ST7789 filled with red color");
    st7789_fill_screen(r);
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "ST7789 filled with green color");
    st7789_fill_screen(g);
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "ST7789 filled with blue color");
    st7789_fill_screen( b);
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "ST7789 filled with black color");
    st7789_fill_screen( 0x0000);
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "ST7789 filled with screen off");
    st7789_disp_on_off(false);
    st7789_backlight_on_off(false);
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_LOGI(TAG, "ST7789 filled with screen on");
    st7789_disp_on_off(true);
    st7789_backlight_on_off(true);
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
