#include "hal/hal_imu.h"
#include "hal/hal_i2c.h"
#include "esp_log.h"
#include "sentinel_hw.h"

static const char *TAG = "IMU";
static bool g_imu_ok = false;

#define QMI8658_REG_CTRL1 0x02
#define QMI8658_REG_CTRL7 0x08
#define QMI8658_REG_AX 0x0D

void hal_imu_init(void) {
    uint8_t data;
    if (hal_i2c_read(ADDR_QMI8658, 0x00, &data) != ESP_OK) {
        ESP_LOGW(TAG, "not found at 0x%02X", ADDR_QMI8658);
        return;
    }
    g_imu_ok = true;
    hal_i2c_write(ADDR_QMI8658, QMI8658_REG_CTRL1, 0x60);
    hal_i2c_write(ADDR_QMI8658, QMI8658_REG_CTRL7, 0x03);
    ESP_LOGI(TAG, "OK");
}

void hal_imu_read(float *ax, float *ay, float *az, float *gx, float *gy, float *gz) {
    uint8_t raw[12];
    if (!g_imu_ok || hal_i2c_read_bytes(ADDR_QMI8658, QMI8658_REG_AX, raw, 12) != ESP_OK) {
        *ax = *ay = *az = *gx = *gy = *gz = 0;
        return;
    }
    *ax = (int16_t)(raw[0] | (raw[1] << 8)) / 16384.0f;
    *ay = (int16_t)(raw[2] | (raw[3] << 8)) / 16384.0f;
    *az = (int16_t)(raw[4] | (raw[5] << 8)) / 16384.0f;
    *gx = (int16_t)(raw[6] | (raw[7] << 8)) / 131.0f;
    *gy = (int16_t)(raw[8] | (raw[9] << 8)) / 131.0f;
    *gz = (int16_t)(raw[10] | (raw[11] << 8)) / 131.0f;
}
