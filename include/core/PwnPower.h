#ifndef PWN_POWER_H
#define PWN_POWER_H

#include <Arduino.h>
#include "XPowersLib.h"
#include <esp_sleep.h>
#include <driver/rtc_io.h>
#include <soc/rtc_cntl_reg.h>
#include <esp_pm.h>

// Definições de Pinos (Reuso)
#define PMU_SDA 38
#define PMU_SCL 39
#define TOUCH_INT_PIN -1 // Definir pino real se disponível no hardware (GPIO 3?)

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

            // Otimização 24: Monitoramento de Corrente (Configuração)
            // pmu.setTSPinMode(XPOWERS_TS_PIN_OFF); // Economia
        }

        // Otimização 7: Start em 160MHz (Fome Baixa/Normal)
        setCpuFrequencyMhz(160);

        // Configura Wakeup Sources
        // esp_sleep_enable_ext0_wakeup((gpio_num_t)IMU_INT_PIN, 1); // Exemplo
    }

    // Otimização 7 & 8: Scaling Dinâmico
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

    // Otimização 1: Deep Sleep Total
    static void enterDeepSleep() {
        Serial.println("[Power] Entrando em Deep Sleep...");

        // Otimização 4: Desliga Display/Backlight
        // (Assumindo controle via Expander/PMU no main)

        // Otimização 5: Desliga Audio
        // es8311_voice_mute(true);

        // Otimização 22: QMI Low Power (Feito no driver)

        // Salva estado crítico se necessário
        if (getBatteryPercent() < 3) {
            // Otimização 29: Modo Zumbi
            rtc_save.magic = 0xDEADBEEF;
            // rtc_save.xp = PwnPet::getXP(); ...
            Serial.println("[Power] MODO ZUMBI ATIVADO");
        }

        esp_deep_sleep_start();
    }

    // Otimização 2: Light Sleep entre scans
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
        // Otimização 24: Monitoramento
        return pmu.getSystemVoltage(); // AXP2101 pode não ter getSystemCurrent direto na lib, usar discharge
    }

    static String getPowerStatus() {
        float ma = pmu.isDischarge() ? pmu.getBattDischargeCurrent() : 0;
        int pct = pmu.getBatteryPercent();
        // Estimativa (Bat 1000mAh ex)
        float hours = (pct > 0 && ma > 0) ? (1000.0 * (pct/100.0)) / ma : 0;

        char buf[64];
        snprintf(buf, sizeof(buf), "%dmA - %.1fh", (int)ma, hours);
        return String(buf);
    }

    static void checkCritical() {
        if (getBatteryPercent() < 10 && !is_critical) {
            // Otimização 13: Modo Crítico
            is_critical = true;
            Serial.println("[Power] BATERIA CRITICA (<10%)");
            // Desliga WiFi
            WiFi.mode(WIFI_OFF);
            // Reduz Clock
            setPerformanceMode(0);
            // UI deve atualizar para relógio monocromático
        }
    }

    static bool isCritical() { return is_critical; }
};

XPowersPMU PwnPower::pmu;
unsigned long PwnPower::last_activity = 0;
bool PwnPower::is_critical = false;

#endif
