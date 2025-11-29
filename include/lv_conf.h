/**
 * @file lv_conf.h
 * @brief LVGL 9.2 Configuration for Waveshare ESP32-S3 AMOLED
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

// -----------------------------------------------------------------------------
// COLOR SETTINGS
// -----------------------------------------------------------------------------
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1 // SPI/QSPI needs swap usually

// -----------------------------------------------------------------------------
// MEMORY SETTINGS
// -----------------------------------------------------------------------------
#define LV_MEM_CUSTOM 0
#define LV_MEM_SIZE (128 * 1024) // 128KB internal RAM heap for LVGL objects
#define LV_MEM_ADR 0             // 0: unused

// -----------------------------------------------------------------------------
// HAL SETTINGS
// -----------------------------------------------------------------------------
#define LV_DEF_REFR_PERIOD 16    // 60 FPS target
#define LV_DPI_DEF 130           // ~1.8 inch 368x448

// -----------------------------------------------------------------------------
// FEATURE CONFIGURATION
// -----------------------------------------------------------------------------

// Drawing
#define LV_USE_DRAW_SW 1
#define LV_DRAW_SW_ASM 1         // Use optimizations if available

// OS
#define LV_USE_OS   LV_OS_FREERTOS

// File System
#define LV_USE_FS_FATFS 0
#define LV_USE_FS_STDIO 0
#define LV_USE_FS_POSIX 0
#define LV_USE_FS_WIN32 0
#define LV_USE_FS_MEMFS 0

// Images
#define LV_USE_PNG 1
#define LV_USE_BMP 1
#define LV_USE_SJPG 1
#define LV_USE_GIF 1
#define LV_USE_QRCODE 1

// Text
#define LV_USE_FONT_COMPRESSED 1
#define LV_FONT_MONTSERRAT_10 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_UNII 1

// Widgets
#define LV_USE_CANVAS 1
#define LV_USE_BAR 1
#define LV_USE_SLIDER 1
#define LV_USE_BUTTON 1
#define LV_USE_LABEL 1
#define LV_USE_IMAGE 1
#define LV_USE_LINE 1
#define LV_USE_TABLE 1
#define LV_USE_CHECKBOX 1
#define LV_USE_SWITCH 1

// Themes
#define LV_USE_THEME_DEFAULT 1

// Optimizations
#define LV_LAYER_SIMPLE_BUF_SIZE (24 * 1024)
#define LV_GRAD_CACHE_DEF_SIZE   4096

// Logging
#define LV_USE_LOG 0

#endif
