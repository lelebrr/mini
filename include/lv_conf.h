<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
#ifndef LV_CONF_H
#define LV_CONF_H

/**
 * lv_conf.h
 * Configuração do LVGL para Waveshare ESP32-S3 AMOLED.
 */

#include <stdint.h>

/*=========================
   CONFIGURAÇÕES GERAIS
 *=========================*/
#define LV_USE_STDLIB_MALLOC    LV_STDLIB_CLIB
#define LV_USE_STDLIB_STRING    LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_CLIB

#define LV_MEM_CUSTOM           0
#define LV_MEM_SIZE             (64 * 1024U) // 64KB Buffer para LVGL
#define LV_MEM_ADR              0
#define LV_MEM_POOL_INCLUDE     ""
#define LV_MEM_BUF_MAX_NUM      16
#define LV_MEM_UNMAPPED         1

/*=========================
   HARDWARE ABSTRACTION LAYER
 *=========================*/
#define LV_DEFAULT_DISP_ID      NULL
#define LV_TICK_CUSTOM          1
#define LV_TICK_CUSTOM_INCLUDE  "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())

/*=========================
   FUNCIONALIDADES E WIDGETS
 *=========================*/
#define LV_USE_LOG              1
#define LV_LOG_LEVEL            LV_LOG_LEVEL_INFO
#define LV_LOG_PRINTF           1

// Fontes (Habilitar necessárias)
#define LV_FONT_MONTSERRAT_14   1
#define LV_FONT_MONTSERRAT_20   1
#define LV_FONT_DEFAULT         &lv_font_montserrat_14

// Widgets usados no projeto
#define LV_USE_LABEL            1
#define LV_USE_BTN              1
#define LV_USE_ARC              1
#define LV_USE_BAR              1
#define LV_USE_SLIDER           1
#define LV_USE_SWITCH           1
#define LV_USE_IMG              1

#endif /*LV_CONF_H*/
<<<<<<< HEAD
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
