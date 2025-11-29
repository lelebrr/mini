#ifndef PWN_POWER_H
#define PWN_POWER_H

#include <Arduino.h>
#include <Wire.h>
#include "XPowersLib.h"
#include <esp_sleep.h>
#include "pin_config.h"

class PwnPower {
private:
    static XPowersPMU pmu;
    static bool initialized;
    static bool is_critical;

public:
    static void init() {
        Wire.begin(IIC_SDA, IIC_SCL);

        if (!pmu.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
            Serial.println("[Power] PMU init failed");
            initialized = false;
            return;
        }

        initialized = true;
        pmu.enableBattDetection();
        pmu.enableVbusVoltageMeasure();
        pmu.enableBattVoltageMeasure();
        pmu.enableSystemVoltageMeasure();

        // Basic rails; fine tuning can be done later if needed
        pmu.enableALDO1();
        pmu.enableALDO2();
        pmu.enableALDO3();
        pmu.enableALDO4();
        pmu.enableBLDO1();

        setCpuFrequencyMhz(160);
        Serial.println("[Power] PMU ready");
    }

    static void setPerformanceMode(int level) {
        switch (level) {
            case 0:
                setCpuFrequencyMhz(80);
                break;
            case 1:
                setCpuFrequencyMhz(160);
                break;
            case 2:
                setCpuFrequencyMhz(240);
                break;
        }
    }

    static void enterDeepSleep() {
        Serial.println("[Power] Deep sleep");
        esp_deep_sleep_start();
    }

    static void lightSleep(uint64_t time_us) {
        esp_sleep_enable_timer_wakeup(time_us);
        esp_light_sleep_start();
    }

    static int getBatteryPercent() {
        if (!initialized) return 100;
        return pmu.getBatteryPercent();
    }

    static float getBatteryVoltage() {
        if (!initialized) return 0.0f;
        return pmu.getBattVoltage();
    }

    static float getSystemCurrent() {
        if (!initialized) return 0.0f;
        return pmu.isDischarge() ? pmu.getBattDischargeCurrent() : 0.0f;
    }

    static void checkCritical() {
        if (!initialized) return;
        if (is_critical) return;

        int pct = getBatteryPercent();
        if (pct > 0 && pct < 10) {
            is_critical = true;
            Serial.println("[Power] Battery critical, lowering performance");
            setPerformanceMode(0);
        }
    }

    static bool isCritical() { return is_critical; }
};

inline XPowersPMU PwnPower::pmu;
inline bool PwnPower::initialized = false;
inline bool PwnPower::is_critical = false;

#endif
