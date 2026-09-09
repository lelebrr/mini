#include "hal/hal_pmu.h"
#include "hal/hal_i2c.h"
#include "esp_log.h"
#include "sentinel_hw.h"

#define AXP2101_LDO_ENABLE 0x10
#define AXP2101_ALDO1_CFG 0x12
#define AXP2101_ALDO2_CFG 0x13
#define AXP2101_ALDO3_CFG 0x14
#define AXP2101_BLDO1_CFG 0x16
#define AXP2101_BAT_ADC 0x62
#define AXP2101_BAT_VOLT_H 0x34
#define AXP2101_BAT_VOLT_L 0x35
#define AXP2101_CHARGE_CTL 0x31
#define AXP2101_TEMP 0x5E

static const char *TAG = "PMU";

void hal_pmu_init(void) {
    ESP_LOGI(TAG, "init AXP2101...");
    hal_i2c_write(ADDR_AXP2101, AXP2101_LDO_ENABLE, 0x0F);
    hal_i2c_write(ADDR_AXP2101, AXP2101_ALDO1_CFG, 0x1C);
    hal_i2c_write(ADDR_AXP2101, AXP2101_ALDO2_CFG, 0x2C);
    hal_i2c_write(ADDR_AXP2101, AXP2101_ALDO3_CFG, 0x30);
    hal_i2c_write(ADDR_AXP2101, AXP2101_BLDO1_CFG, 0x33);
    ESP_LOGI(TAG, "ALDO1-4 + BLDO1 enabled");
}

float hal_pmu_get_battery_voltage(void) {
    uint8_t h, l;
    if (hal_i2c_read(ADDR_AXP2101, AXP2101_BAT_VOLT_H, &h) != ESP_OK) return 0;
    if (hal_i2c_read(ADDR_AXP2101, AXP2101_BAT_VOLT_L, &l) != ESP_OK) return 0;
    uint16_t raw = ((uint16_t)h << 4) | (l & 0x0F);
    return raw * 1.1f;
}

int hal_pmu_get_battery_percent(void) {
    float v = hal_pmu_get_battery_voltage();
    if (v < 3.3f) return 0;
    if (v > 4.2f) return 100;
    return (int)((v - 3.3f) / 0.9f * 100.0f);
}

bool hal_pmu_is_charging(void) {
    uint8_t data;
    if (hal_i2c_read(ADDR_AXP2101, AXP2101_CHARGE_CTL, &data) != ESP_OK) return false;
    return (data & 0x01) != 0;
}

float hal_pmu_get_temperature(void) {
    uint8_t data;
    if (hal_i2c_read(ADDR_AXP2101, AXP2101_TEMP, &data) != ESP_OK) return 25.0f;
    return (float)data;
}
