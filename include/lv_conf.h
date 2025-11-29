#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*
 * Minimal LVGL configuration for ESP32-S3 + Waveshare 1.8" AMOLED
 * using LVGL v9.x. All unspecified options fall back to LVGL defaults.
 */

/* Color settings */
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1

/* Fonts used by the UI */
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/* Text encoding */
#define LV_TXT_ENC LV_TXT_ENC_UTF8

/* Disable logging to save flash/RAM */
#define LV_USE_LOG 0

/* Don't build built-in examples/demos (we provide our own UI) */
#define LV_BUILD_EXAMPLES 0
#define LV_BUILD_DEMOS 0

#endif /* LV_CONF_H */
