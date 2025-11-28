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
