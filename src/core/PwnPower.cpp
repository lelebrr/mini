#include "core/PwnPower.h"
#include "core/ConfigManager.h"
#include <Wire.h>

void PwnPower::init() {
    pmu_ok = false;
    is_critical_flag = false;
    last_vbus = false;
    last_charging = false;

    if (!pmu.begin(Wire, AXP2101_SLAVE_ADDRESS, -1, -1)) {
        Serial.println("[PwnPower] ERRO: AXP2101 nao encontrado no I2C!");
        return;
    }

    pmu_ok = true;
    Serial.println("[PwnPower] AXP2101 encontrado");

    // Habilitar trilhos de potência necessários para AMOLED
    pmu.setALDO1Voltage(1800);  // LCD VDDI 1.8V
    pmu.setALDO2Voltage(2800);  // VCI 2.8V
    pmu.setALDO3Voltage(3000);  // VDD 3.0V
    pmu.setALDO4Voltage(3300);  // VDDIO 3.3V
    pmu.setBLDO1Voltage(1800);  // VBL 1.8V

    pmu.enableALDO1();
    pmu.enableALDO2();
    pmu.enableALDO3();
    pmu.enableALDO4();
    pmu.enableBLDO1();

    // Habilitar ADC para leitura de bateria
    pmu.enableBattVoltageMeasure();
    pmu.enableVbusVoltageMeasure();
    pmu.enableTemperatureMeasure();

    configureCharging();
    Serial.println("[PwnPower] PMU inicializado com sucesso");
}

void PwnPower::configureCharging() {
    if (!pmu_ok) return;
    pmu.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_200MA);
    pmu.setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V2);
    pmu.enableCellbatteryCharge();
}

void PwnPower::setBatteryCapacity(int mah) {
    battery_capacity_mah = constrain(mah, 100, 3000);
    ConfigManager::getInstance()->set("pwr_battery_capacity_mah", (int64_t)battery_capacity_mah);
}

void PwnPower::setChargeCurrentMa(int ma) {
    if (!pmu_ok) return;
    if (ma <= 100) pmu.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_100MA);
    else if (ma <= 150) pmu.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_150MA);
    else if (ma <= 200) pmu.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_200MA);
    else if (ma <= 300) pmu.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_300MA);
    else pmu.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_400MA);
    ConfigManager::getInstance()->set("pwr_charge_current_ma", (int64_t)ma);
}

void PwnPower::setPerformanceMode(int mode) {
    ConfigManager::getInstance()->set("pwr_cpu_freq_max", (int64_t)(mode >= 2 ? 240 : 160));
}

void PwnPower::monitor() {
    if (!pmu_ok) return;

    bool vbus = pmu.isVbusIn();
    bool charging = pmu.isCharging();

    if (vbus != last_vbus) {
        last_vbus = vbus;
        Serial.printf("[PwnPower] VBUS %s\n", vbus ? "CONECTADO" : "DESCONECTADO");
    }

    if (charging != last_charging) {
        last_charging = charging;
        Serial.printf("[PwnPower] Carregamento %s\n", charging ? "INICIOU" : "PAROU");
    }

    int pct = getBatteryPercent();
    if (pct <= 10 && !is_critical_flag) {
        setCriticalFlag(true);
        Serial.println("[PwnPower] ALERTA: Bateria critica!");
    } else if (pct > 15 && is_critical_flag) {
        setCriticalFlag(false);
    }
}

bool PwnPower::isCharging() {
    return pmu_ok && pmu.isCharging();
}

float PwnPower::getBatteryVoltage() {
    if (!pmu_ok) return 0.0f;
    return pmu.getBattVoltage() / 1000.0f;
}

int PwnPower::getBatteryPercent() {
    if (!pmu_ok) return 0;
    if (pmu.isBatteryConnect()) {
        return pmu.getBatteryPercent();
    }
    // Fallback: estimate from voltage
    float v = getBatteryVoltage();
    if (v <= 3.0f) return 0;
    if (v >= 4.2f) return 100;
    return (int)((v - 3.0f) / 1.2f * 100.0f);
}

float PwnPower::getTemperature() {
    if (!pmu_ok) return 0.0f;
    return pmu.getTemperature();
}

void PwnPower::enterDeepSleep() {
    Serial.println("[PwnPower] Entrando em deep sleep...");
    pmu.disableALDO1();
    pmu.disableALDO2();
    pmu.disableALDO3();
    pmu.disableALDO4();
    pmu.disableBLDO1();
    ESP.deepSleep(10000000);
}

void PwnPower::setCriticalFlag(bool flag) {
    is_critical_flag = flag;
}

bool PwnPower::isCritical() {
    return is_critical_flag;
}
