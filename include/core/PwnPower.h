#ifndef PWN_POWER_H
#define PWN_POWER_H

#include <Arduino.h>
#include <WiFi.h>
#include "XPowersLib.h"
#include <esp_sleep.h>
#include "pin_config.h"

/**
 * PwnPower
 * --------
 * Gerencia o PMU AXP2101: alimentação dos periféricos, medição de bateria,
 * CARREGAMENTO via USB-C e decisões de energia (modo crítico, deep sleep).
 *
 * >>> Bateria alvo: LiPo PL502030, 250 mAh, 3.7 V (1 célula) <<<
 * O AXP2101 faz o carregamento por hardware assim que o USB-C é conectado.
 * Aqui apenas configuramos parâmetros SEGUROS para uma célula pequena:
 *   - Corrente constante (CC): 100 mA  (~0,4C — recomendado p/ 250 mAh)
 *   - Tensão de corte:         4,2 V   (padrão LiPo)
 *   - Corrente de término:     25 mA   (~0,1C)
 *   - Detecção do pino TS:     DESLIGADA (a célula não tem termistor;
 *     se ficar ligada, o AXP2101 recusa/erra o carregamento).
 */

struct RTC_SaveData {
    uint32_t magic;
    int      xp;
    int      level;
    int      hunger;
    int      handshakes;
};

// Definido em core_singletons.cpp (mantém-se na RAM RTC entre deep sleeps).
extern RTC_DATA_ATTR RTC_SaveData rtc_save;

class PwnPower {
private:
    static XPowersPMU   pmu;
    static bool         pmu_ok;
    static bool         is_critical_flag;
    static bool         last_vbus;     // estado anterior do USB (para logs)
    static bool         last_charging;
    static int          battery_capacity_mah;  // configurável (troca de célula)

public:
    static void init() {
        // Usa o barramento I2C principal definido em pin_config.h
        if (!pmu.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
            Serial.println("[Power] Falha ao inicializar AXP2101.");
            pmu_ok = false;
            return;
        }

        pmu_ok = true;
        Serial.printf("[Power] AXP2101 online (ChipID 0x%x).\n", pmu.getChipID());

        // ---- Medição / ADC ----
        pmu.enableBattDetection();
        pmu.enableVbusVoltageMeasure();
        pmu.enableBattVoltageMeasure();
        pmu.enableSystemVoltageMeasure();

        // ---- Rails dos periféricos (valores de referência Waveshare) ----
        pmu.setALDO1Voltage(1800); pmu.enableALDO1(); // Lógica AMOLED
        pmu.setALDO2Voltage(2800); pmu.enableALDO2(); // Touch
        pmu.setALDO3Voltage(3000); pmu.enableALDO3(); // Áudio
        pmu.setALDO4Voltage(3300); pmu.enableALDO4(); // Mic bias
        pmu.setBLDO1Voltage(3300); pmu.enableBLDO1(); // AMOLED Power

        // ---- CARREGAMENTO (seguro para PL502030 250 mAh) ----
        configureCharging();

        setCpuFrequencyMhz(160); // clock padrão
    }

    // Configura o carregador para a célula pequena de 250 mAh.
    static void configureCharging() {
        if (!pmu_ok) return;

        // Célula sem termistor: DESLIGAR TS obrigatoriamente.
        pmu.disableTSPinMeasure();

        // Corte em 4,2 V (padrão LiPo de célula única).
        pmu.setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V2);

        // Corrente constante 100 mA (~0,4C p/ 250 mAh). Ajustável em runtime.
        pmu.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_100MA);

        // Término em 25 mA (~0,1C) — encerra a carga corretamente.
        pmu.setChargerTerminationCurr(XPOWERS_AXP2101_CHG_ITERM_25MA);

        // LED de carga controlado automaticamente pelo PMU.
        pmu.setChargingLedMode(XPOWERS_CHG_LED_CTRL_CHG);

        // Proteções de descarga: desliga o sistema em 2,6 V.
        pmu.setSysPowerDownVoltage(2600);
        pmu.setLowBatWarnThreshold(10);       // aviso em 10%
        pmu.setLowBatShutdownThreshold(5);    // desliga em 5%

        Serial.println("[Power] Carregador configurado: 100mA CC, 4.2V, term 25mA (LiPo 250mAh).");
    }

    // Define a capacidade da bateria (mAh) usada nos cálculos de autonomia.
    // Permite trocar a célula (ex.: 250 -> 500/1000 mAh) só mudando a config,
    // sem recompilar. Também reajusta a corrente de carga sugerida (<=0,5C).
    static void setBatteryCapacity(int mah) {
        if (mah < 40) mah = 40;          // sanidade
        battery_capacity_mah = mah;
        Serial.printf("[Power] Capacidade da bateria: %d mAh.\n", mah);
    }
    static int getBatteryCapacity() { return battery_capacity_mah; }

    // Corrente de carga máxima recomendada (0,5C) para a capacidade atual.
    static int recommendedMaxChargeMa() { return battery_capacity_mah / 2; }

    // Permite mudar a corrente de carga em runtime (via WebUI/config).
    // Valores suportados: 100, 125, 150, 200 mA. Recomendado <= 125 mA p/ 250 mAh.
    static void setChargeCurrentMa(int ma) {
        if (!pmu_ok) return;
        if (ma > recommendedMaxChargeMa()) {
            Serial.printf("[Power] AVISO: %dmA > 0,5C (%dmAh). Recomendado <= %dmA.\n",
                          ma, battery_capacity_mah, recommendedMaxChargeMa());
        }
        switch (ma) {
            case 200: pmu.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_200MA); break;
            case 150: pmu.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_150MA); break;
            case 125: pmu.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_125MA); break;
            case 100:
            default:  pmu.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_100MA); ma = 100; break;
        }
        Serial.printf("[Power] Corrente de carga ajustada para %d mA.\n", ma);
    }

    static void setPerformanceMode(int level) {
        switch (level) {
            case 0: setCpuFrequencyMhz(80);  Serial.println("[Power] Clock: 80 MHz (Eco)");    break;
            case 1: setCpuFrequencyMhz(160); Serial.println("[Power] Clock: 160 MHz (Normal)"); break;
            case 2: setCpuFrequencyMhz(240); Serial.println("[Power] Clock: 240 MHz (Turbo)");  break;
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

    // ---- Leituras de bateria ----
    static int getBatteryPercent() {
        if (!pmu_ok) return 0;
        return pmu.getBatteryPercent();
    }

    static float getBatteryVoltage() {
        if (!pmu_ok) return 0.0f;
        return pmu.getBattVoltage() / 1000.0f; // mV -> V
    }

    static float getVbusVoltage() {
        if (!pmu_ok) return 0.0f;
        return pmu.getVbusVoltage() / 1000.0f; // mV -> V
    }

    // ---- Estado de carga ----
    static bool isVbusIn()   { return pmu_ok && pmu.isVbusIn(); }      // USB-C conectado
    static bool isCharging() { return pmu_ok && pmu.isCharging(); }    // carregando ativamente
    static bool isBatteryConnected() { return pmu_ok && pmu.isBatteryConnect(); }

    static String getChargeStatusStr() {
        if (!pmu_ok) return "PMU off";
        if (!isBatteryConnected()) return isVbusIn() ? "USB (sem bateria)" : "Sem bateria";
        uint8_t s = pmu.getChargerStatus();
        switch (s) {
            case XPOWERS_AXP2101_CHG_TRI_STATE:  return "Pré-carga (tri)";
            case XPOWERS_AXP2101_CHG_PRE_STATE:  return "Pré-carga";
            case XPOWERS_AXP2101_CHG_CC_STATE:   return "Carregando (CC)";
            case XPOWERS_AXP2101_CHG_CV_STATE:   return "Carregando (CV)";
            case XPOWERS_AXP2101_CHG_DONE_STATE: return "Carga completa";
            case XPOWERS_AXP2101_CHG_STOP_STATE: return isVbusIn() ? "Alimentado (USB)" : "Descarregando";
            default: return "—";
        }
    }

    // Consumo médio estimado em mA (o AXP2101 não expõe a corrente de descarga
    // da bateria de forma confiável nesta lib). Usamos um valor nominal por modo
    // de energia para estimar autonomia sem depender de método indisponível.
    static float getSystemCurrent() {
        if (!pmu_ok || isVbusIn()) return 0.0f;   // 0 quando no USB
        // Nominal por clock atual: Eco ~70mA, Normal ~130mA, Turbo ~210mA.
        uint32_t mhz = getCpuFrequencyMhz();
        if (mhz <= 80)  return 70.0f;
        if (mhz <= 160) return 130.0f;
        return 210.0f;
    }

    // Estimativa de autonomia com base na capacidade real da célula (250 mAh).
    static float getEstimatedHours() {
        int   pct = getBatteryPercent();
        float ma  = getSystemCurrent();
        if (pct <= 0 || ma <= 0.0f) return 0.0f;
        return ((float)battery_capacity_mah * (pct / 100.0f)) / ma;
    }

    static String getPowerStatus() {
        char buf[80];
        if (isVbusIn()) {
            snprintf(buf, sizeof(buf), "%d%% • %s • %.2fV",
                     getBatteryPercent(), getChargeStatusStr().c_str(), getBatteryVoltage());
        } else {
            snprintf(buf, sizeof(buf), "%d%% • %.0fmA • ~%.1fh",
                     getBatteryPercent(), getSystemCurrent(), getEstimatedHours());
        }
        return String(buf);
    }

    // Chamado periodicamente do loop principal (monitoramento + otimização).
    static void monitor() {
        if (!pmu_ok) return;

        bool vbus = isVbusIn();
        bool chg  = isCharging();

        if (vbus != last_vbus) {
            Serial.println(vbus ? "[Power] USB-C conectado — carregando."
                                : "[Power] USB-C removido — em bateria.");
            last_vbus = vbus;
            // Ao conectar o cabo, sai do modo crítico e volta ao normal.
            if (vbus && is_critical_flag) {
                is_critical_flag = false;
                setPerformanceMode(1);
            }
        }
        if (chg != last_charging) {
            if (chg) Serial.println("[Power] Carga iniciada.");
            else if (vbus) Serial.println("[Power] Carga concluída / pausada.");
            last_charging = chg;
        }

        checkCritical();
    }

    static void checkCritical() {
        // Não entra em modo crítico se estiver no USB (carregando).
        if (is_critical_flag || isVbusIn()) return;

        if (getBatteryPercent() < 10) {
            is_critical_flag = true;
            Serial.println("[Power] BATERIA CRÍTICA (<10%) – reduzindo consumo.");
            WiFi.mode(WIFI_OFF);
            setPerformanceMode(0);
        }
    }

    static bool isCritical() { return is_critical_flag; }
};

// Definições estáticas declaradas em core_singletons.cpp
#endif
