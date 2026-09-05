#ifndef PWN_SLEEP_H
#define PWN_SLEEP_H

#include <Arduino.h>
#include <esp_sleep.h>
#include "core/PwnPower.h"
#include "pin_config.h"

/**
 * PwnSleep
 * --------
 * Economia de energia (crítico para a célula pequena de 250 mAh):
 *   1. Sem atividade por `dim_after` s  -> escurece a tela (auto-dim)
 *   2. Sem atividade por `off_after` s  -> apaga o AMOLED (brilho 0) + clock Eco
 *   3. (Opcional) Sem atividade por `deep_after` s e SEM USB -> deep sleep,
 *      acordando ao tocar na tela (INT do touch, GPIO21 / ext0).
 *
 * Qualquer toque ou movimento (chamado via notifyActivity()) restaura tudo.
 *
 * Não inclui a lib do display: usa um callback de brilho fornecido pelo main
 * (setBrightnessCb), mantendo o módulo desacoplado.
 */
class PwnSleep {
public:
    typedef void (*BrightnessFn)(int);

private:
    static BrightnessFn  set_brightness;
    static uint32_t      last_activity_ms;
    static int           full_brightness;
    static int           dim_after_s;
    static int           off_after_s;
    static int           deep_after_s;
    static bool          deep_enabled;
    static bool          dimmed;
    static bool          screen_off;

public:
    static void init(BrightnessFn cb, int brightness,
                     int screen_off_sec, bool enable_deep, int deep_after_sec) {
        set_brightness   = cb;
        full_brightness  = brightness > 0 ? brightness : 200;
        off_after_s      = screen_off_sec > 0 ? screen_off_sec : 30;
        dim_after_s      = off_after_s / 2;        // escurece na metade do tempo
        deep_enabled     = enable_deep;
        deep_after_s     = deep_after_sec > 0 ? deep_after_sec : 300;
        last_activity_ms = millis();
        dimmed = false; screen_off = false;
    }

    static void notifyActivity() {
        last_activity_ms = millis();
        if (screen_off || dimmed) {
            if (set_brightness) set_brightness(full_brightness);
            PwnPower::setPerformanceMode(1);  // volta ao Normal
            dimmed = false; screen_off = false;
        }
    }

    static bool isScreenOff() { return screen_off; }

    static void setFullBrightness(int b) { full_brightness = b; if (!dimmed && !screen_off && set_brightness) set_brightness(b); }
    static void setScreenTimeout(int s) { off_after_s = s > 0 ? s : 30; dim_after_s = off_after_s / 2; }

    // Chamado periodicamente do loop.
    static void tick() {
        uint32_t idle = (millis() - last_activity_ms) / 1000;

        // 1. Auto-dim
        if (!dimmed && !screen_off && (int)idle >= dim_after_s) {
            if (set_brightness) set_brightness(max(15, full_brightness / 4));
            dimmed = true;
        }
        // 2. Tela off + Eco
        if (!screen_off && (int)idle >= off_after_s) {
            if (set_brightness) set_brightness(0);
            PwnPower::setPerformanceMode(0);  // Eco (80 MHz)
            screen_off = true;
            Serial.println("[Sleep] Tela desligada (economia).");
        }
        // 3. Deep sleep (só na bateria, se habilitado)
        if (deep_enabled && screen_off && !PwnPower::isVbusIn() &&
            (int)idle >= deep_after_s) {
            enterDeep();
        }
    }

    // Deep sleep acordando ao tocar (INT do touch, ativo em nível baixo).
    static void enterDeep() {
        Serial.println("[Sleep] Entrando em deep sleep (toque para acordar)...");
        if (set_brightness) set_brightness(0);
        // TP_INT (GPIO21) é RTC-capable no ESP32-S3; acorda em nível baixo.
        esp_sleep_enable_ext0_wakeup((gpio_num_t)TP_INT, 0);
        delay(50);
        PwnPower::enterDeepSleep();  // salva estado crítico e dorme
    }
};

// Definições estáticas em core_singletons.cpp
#endif
