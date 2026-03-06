#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t xpt2046_touch_init(void);
bool xpt2046_touch_read(uint16_t *x, uint16_t *y);

#ifdef __cplusplus
}
#endif
