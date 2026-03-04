/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

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
