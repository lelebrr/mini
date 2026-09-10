#ifndef PWNPOWER_H
#define PWNPOWER_H

#include <Arduino.h>
#include <XPowersLib.h>

#define PMU_SLEEP_MS 50

struct RTC_SaveData {
    uint32_t pet_level;
    uint32_t pet_xp;
    uint32_t pet_hunger;
    uint32_t pet_happiness;
    uint32_t last_sync;
    uint32_t boot_count;
    uint32_t total_handshakes;
    uint32_t checksum;
};

extern RTC_SaveData rtc_save;

class PwnPower {
public:
    static XPowersPMU pmu;
    static bool pmu_ok;
    static bool is_critical_flag;
    static bool last_vbus;
    static bool last_charging;
    static int battery_capacity_mah;

    static void init();
    static void configureCharging();
    static void setBatteryCapacity(int mah);
    static void setChargeCurrentMa(int ma);
    static void setPerformanceMode(int mode);
    static void monitor();
    static bool isCharging();
    static float getBatteryVoltage();
    static int getBatteryPercent();
    static float getTemperature();
    static void enterDeepSleep();
    static void setCriticalFlag(bool flag);
    static bool isCritical();
};

#endif
