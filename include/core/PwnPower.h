#ifndef PWN_POWER_H
#define PWN_POWER_H

#include <Arduino.h>
#include <WiFi.h>
#include "XPowersLib.h"
#include <esp_sleep.h>
#include "pin_config.h"

/**
 * PwnPower
 *
 * Gerencia o PMU AXP2101 (bateria, tensões) e algumas decisões
 * simples de energia (modo crítico, deep sleep, etc.).
 *
 * Este módulo é deliberadamente simplificado em relação às versões
 * antigas, mas mantém a API usada pelas outras partes do projeto.
 */

struct RTC_SaveData {
    uint32_t magic;
    int      xp;
    int      level;
    int      hunger;
    int      handshakes;
};

RTC_DATA_ATTR RTC_SaveData rtc_save;

class PwnPower {
private:
    static XPowersPMU   pmu;
    static bool         pmu_ok;
    static bool         is_critical_flag;

public:
    static void init() {
        // Usa o barramento I2C principal definido em pin_config.h
        if (!pmu.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
            Serial.println("[Power] Falha ao inicializar AXP2101.");
            pmu_ok = false;
            return;
        }

        pmu_ok = true;
        Serial.println("[Power] AXP2101 online.");

        pmu.enableBattDetection();
        pmu.enableVbusVoltageMeasure();
        pmu.enableBattVoltageMeasure();
        pmu.enableSystemVoltageMeasure();

        // Tensões básicas para o board Waveshare (valores de referência)
        pmu.setALDO1Voltage(1800); pmu.enableALDO1(); // Lógica AMOLED
        pmu.setALDO2Voltage(2800); pmu.enableALDO2(); // Touch
        pmu.setALDO3Voltage(3000); pmu.enableALDO3(); // Áudio
        pmu.setALDO4Voltage(3300); pmu.enableALDO4(); // Mic bias
        pmu.setBLDO1Voltage(3300); pmu.enableBLDO1(); // AMOLED Power

        setCpuFrequencyMhz(160); // clock padrão
    }

    static void setPerformanceMode(int level) {
        switch (level) {
            case 0: // Eco
                setCpuFrequencyMhz(80);
                Serial.println("[Power] Clock: 80 MHz (Eco)");
                break;
            case 1: // Normal
                setCpuFrequencyMhz(160);
                Serial.println("[Power] Clock: 160 MHz (Normal)");
                break;
            case 2: // Turbo
                setCpuFrequencyMhz(240);
                Serial.println("[Power] Clock: 240 MHz (Turbo)");
                break;
        }
    }

    static void enterDeepSleep() {
        Serial.println("[Power] Entrando em Deep Sleep...");

        if (getBatteryPercent() < 3) {
            rtc_save.magic = 0xDEADBEEF;
            Serial.println("[Power] MODO ZUMBI ATIVADO");
        }

        esp_deep_sleep_start();
    }

    static void lightSleep(uint64_t time_us) {
        esp_sleep_enable_timer_wakeup(time_us);
        esp_light_sleep_start();
    }

    static int getBatteryPercent() {
        if (!pmu_ok) return 0;
        return pmu.getBatteryPercent();
    }

    static float getBatteryVoltage() {
        if (!pmu_ok) return 0.0f;
        return pmu.getBattVoltage();
    }

    // Corrente aproximada (mA) – usa corrente de descarga da bateria
    static float getSystemCurrent() {
        if (!pmu_ok) return 0.0f;
        if (!pmu.isDischarge()) return 0.0f;
        return pmu.getBattDischargeCurrent();
    }

    static String getPowerStatus() {
        int   pct = getBatteryPercent();
        float ma  = getSystemCurrent();
        float hours = (pct > 0 && ma > 0.0f)
                      ? (1000.0f * (pct / 100.0f)) / ma
                      : 0.0f;

        char buf[64];
        snprintf(buf, sizeof(buf), "%d%% %.0fmA ~%.1fh", pct, (int)ma, hours);
        return String(buf);
    }

    static void checkCritical() {
        if (is_critical_flag) return;

        if (getBatteryPercent() < 10) {
            is_critical_flag = true;
            Serial.println("[Power] BATERIA CRÍTICA (<10%) – reduzindo consumo.");
            WiFi.mode(WIFI_OFF);
            setPerformanceMode(0);
        }
    }

    static bool isCritical() {
        return is_critical_flag;
    }
};

// Definições estáticas declaradas em core_singletons.cpp
#endif
