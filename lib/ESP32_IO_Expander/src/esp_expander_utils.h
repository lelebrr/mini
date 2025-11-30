/*
 * Adapted for Mini Lele project:
 * Removemos a dependência da biblioteca externa esp-lib-utils (esp_lib_utils.h)
 * e fornecemos stubs mínimos de logging e macros de checagem para uso com
 * Arduino + PlatformIO.
 *
 * Mantemos a estrutura das macros esperadas pelo código da Espressif.
 */
#pragma once

#include <Arduino.h>

// Tag de log (mantida para compatibilidade, mas usamos Serial para saída)
#define ESP_UTILS_LOG_TAG "Expander"

// Níveis de log (compatíveis com esp-lib-utils, mas simplificados)
#define ESP_UTILS_LOG_LEVEL_DEBUG   0
#define ESP_UTILS_LOG_LEVEL_INFO    1
#define ESP_UTILS_LOG_LEVEL_WARNING 2
#define ESP_UTILS_LOG_LEVEL_ERROR   3
#define ESP_UTILS_LOG_LEVEL_NONE    4

// Nível de log de compilação: escolha um (DEBUG gera mais mensagens)
#ifndef ESP_UTILS_CONF_LOG_LEVEL
#define ESP_UTILS_CONF_LOG_LEVEL ESP_UTILS_LOG_LEVEL_INFO
#endif

// Funções de log básicas usando Serial
#define ESP_UTILS_LOGD(fmt, ...) \
    do { if (ESP_UTILS_CONF_LOG_LEVEL <= ESP_UTILS_LOG_LEVEL_DEBUG)   Serial.printf("[D][%s] " fmt "\n", ESP_UTILS_LOG_TAG, ##__VA_ARGS__); } while (0)

#define ESP_UTILS_LOGI(fmt, ...) \
    do { if (ESP_UTILS_CONF_LOG_LEVEL <= ESP_UTILS_LOG_LEVEL_INFO)    Serial.printf("[I][%s] " fmt "\n", ESP_UTILS_LOG_TAG, ##__VA_ARGS__); } while (0)

#define ESP_UTILS_LOGW(fmt, ...) \
    do { if (ESP_UTILS_CONF_LOG_LEVEL <= ESP_UTILS_LOG_LEVEL_WARNING) Serial.printf("[W][%s] " fmt "\n", ESP_UTILS_LOG_TAG, ##__VA_ARGS__); } while (0)

#define ESP_UTILS_LOGE(fmt, ...) \
    do { if (ESP_UTILS_CONF_LOG_LEVEL <= ESP_UTILS_LOG_LEVEL_ERROR)   Serial.printf("[E][%s] " fmt "\n", ESP_UTILS_LOG_TAG, ##__VA_ARGS__); } while (0)

// Macros de trace simplificadas: apenas logam entrada/saída com o ponteiro this
#define ESP_UTILS_LOG_TRACE_ENTER_WITH_THIS() \
    ESP_UTILS_LOGD("enter %s this=%p", __FUNCTION__, this)

#define ESP_UTILS_LOG_TRACE_EXIT_WITH_THIS() \
    ESP_UTILS_LOGD("exit  %s this=%p", __FUNCTION__, this)

// Macros de checagem simplificadas (retornam valor em caso de falha)
#define ESP_UTILS_CHECK_FALSE_RETURN(cond, ret, msg) \
    do {                                             \
        if (!(cond)) {                               \
            ESP_UTILS_LOGE("%s", msg);               \
            return (ret);                            \
        }                                            \
    } while (0)

#define ESP_UTILS_CHECK_FALSE_EXIT(cond, msg) \
    do {                                      \
        if (!(cond)) {                        \
            ESP_UTILS_LOGE("%s", msg);        \
            return;                           \
        }                                     \
    } while (0)

// Para funções que retornam erro de ESP-IDF (esp_err_t) mas aqui só precisamos de bool
#define ESP_UTILS_CHECK_ERROR_RETURN(err_expr, ret, msg) \
    do {                                                 \
        auto _err = (err_expr);                          \
        if (_err != ESP_OK) {                            \
            ESP_UTILS_LOGE("%s (err=0x%X)", msg, _err);  \
            return (ret);                                \
        }                                                \
    } while (0)

#define ESP_UTILS_CHECK_ERROR_EXIT(err_expr, msg) \
    do {                                          \
        auto _err = (err_expr);                   \
        if (_err != ESP_OK) {                     \
            ESP_UTILS_LOGE("%s (err=0x%X)", msg, _err); \
            return;                               \
        }                                         \
    } while (0)
