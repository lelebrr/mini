/**
 * @file lv_conf.h
 * @brief Minimal LVGL 9 configuration for Waveshare ESP32-S3 AMOLED
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*=========================
 * Color settings
 *=========================*/
#define LV_COLOR_DEPTH      16
#define LV_COLOR_16_SWAP    1

/*=========================
 * Memory settings
 *=========================*/
#define LV_MEM_CUSTOM       0
#define LV_MEM_SIZE         (64U * 1024U)
#define LV_MEM_ADR          0

/*=========================
 * OS / Tick
 *=========================*/
#define LV_USE_OS           LV_OS_FREERTOS

#define LV_TICK_CUSTOM              1
#define LV_TICK_CUSTOM_INCLUDE      "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())

/*=========================
 * Logging
 *=========================*/
#define LV_USE_LOG          0

/*=========================
 * Fonts
 *=========================*/
#define LV_FONT_MONTSERRAT_14   1
#define LV_FONT_MONTSERRAT_20   1

/*=========================
 * Default display refresh
 *=========================*/
#define LV_DEF_REFR_PERIOD  16

#endif /* LV_CONF_H */
