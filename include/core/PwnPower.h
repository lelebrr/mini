#ifndef PWN_POWER_H
#define PWN_POWER_H

#include <Arduino.h>
#include "XPowersLib.h"
#include <esp_sleep.h>
#include <driver/rtc_io.h>
#include <soc/rtc_cntl_reg.h>
#include <esp_pm.h>
#include "pin_config.h"

// Definições de Pinos do PMU AXP2101
// Waveshare usa I2C compartilhado (6/7)
#define PMU_SDA 6
#define PMU_SCL 7
#define TOUCH_INT_PIN 5 // GT1151 INT

// RTC Memory para Modo Zumbi
struct RTC_SaveData {
    uint32_t magic;
    int xp;
    int level;
    int hunger;
    int handshakes;
};
RTC_DATA_ATTR RTC_SaveData rtc_save;

class PwnPower {
private:
    static XPowersPMU pmu;
    static unsigned long last_activity;
    static bool is_critical;

public:
    static void init() {
        if (!pmu.begin(Wire, AXP2101_SLAVE_ADDRESS, PMU_SDA, PMU_SCL)) {
            Serial.println("[Power] PMU Falhou!");
        } else {
            Serial.println("[Power] PMU Online");
            pmu.enableBattDetection();
            pmu.enableVbusVoltageMeasure();
            pmu.enableBattVoltageMeasure();
            pmu.enableSystemVoltageMeasure();

            // Configurar voltagens específicas (ALDO1=AMOLED Logic, BLDO1=Backlight, etc)
            pmu.setALDO1Voltage(1800); pmu.enableALDO1();
            pmu.setALDO2Voltage(2800); pmu.enableALDO2(); // Touch
            pmu.setALDO3Voltage(3000); pmu.enableALDO3(); // Audio
            pmu.setALDO4Voltage(3300); pmu.enableALDO4(); // Mic Bias
            pmu.setBLDO1Voltage(3300); pmu.enableBLDO1(); // AMOLED Power
        }

        setCpuFrequencyMhz(160);
    }

    static void setPerformanceMode(int level) {
        switch(level) {
            case 0: // Sleepy/Bored
                setCpuFrequencyMhz(80);
                Serial.println("[Power] Clock: 80MHz (Eco)");
                break;
            case 1: // Normal
                setCpuFrequencyMhz(160);
                Serial.println("[Power] Clock: 160MHz (Normal)");
                break;
            case 2: // Attack/Active
                setCpuFrequencyMhz(240);
                Serial.println("[Power] Clock: 240MHz (Turbo)");
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
        return pmu.getBatteryPercent();
    }

    static float getBatteryVoltage() {
        return pmu.getBattVoltage();
    }

    static float getSystemCurrent() {
        // AXP2101 pode não ter getSystemCurrent direto, usar discharge ou Vbus
        return pmu.isDischarge() ? pmu.getBattDischargeCurrent() : 0;
    }

    static String getPowerStatus() {
        float ma = pmu.isDischarge() ? pmu.getBattDischargeCurrent() : 0;
        int pct = pmu.getBatteryPercent();
        float hours = (pct > 0 && ma > 0) ? (1000.0 * (pct/100.0)) / ma : 0;

        char buf[64];
        snprintf(buf, sizeof(buf), "%dmA - %.1fh", (int)ma, hours);
        return String(buf);
    }

    static void checkCritical() {
        if (getBatteryPercent() < 10 && !is_critical) {
            is_critical = true;
            Serial.println("[Power] BATERIA CRITICA (<10%)");
            WiFi.mode(WIFI_OFF);
            setPerformanceMode(0);
        }
    }

    static bool isCritical() { return is_critical; }
};

XPowersPMU PwnPower::pmu;
unsigned long PwnPower::last_activity = 0;
bool PwnPower::is_critical = false;

#endif
