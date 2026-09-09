#include "hal/hal_i2c.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "sentinel_hw.h"
static const char *TAG = "I2C";
static bool g_i2c_ok = false;
void hal_i2c_init(void) {
    i2c_config_t conf = { .mode = I2C_MODE_MASTER, .sda_io_num = PIN_I2C_SDA, .scl_io_num = PIN_I2C_SCL, .sda_pullup_en = GPIO_PULLUP_ENABLE, .scl_pullup_en = GPIO_PULLUP_ENABLE, .master.clk_speed = I2C_FREQ };
    esp_err_t err = i2c_param_config(I2C_NUM_0, &conf);
    if (err != ESP_OK) { ESP_LOGE(TAG, "fail"); return; }
    err = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
    if (err != ESP_OK) { ESP_LOGE(TAG, "driver fail"); return; }
    g_i2c_ok = true;
    ESP_LOGI(TAG, "OK");
}
void hal_i2c_scan(void) {
    if (!g_i2c_ok) return;
    ESP_LOGI(TAG, "Scan...");
    for (uint8_t addr = 1; addr < 127; addr++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(50));
        i2c_cmd_link_delete(cmd);
        if (ret == ESP_OK) ESP_LOGI(TAG, "  Found 0x%02X", addr);
    }
}
esp_err_t hal_i2c_write(uint8_t addr, uint8_t reg, uint8_t data) {
    if (!g_i2c_ok) return ESP_ERR_INVALID_STATE;
    uint8_t buf[2] = {reg, data};
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, buf, 2, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return ret;
}
esp_err_t hal_i2c_read(uint8_t addr, uint8_t reg, uint8_t *data) {
    if (!g_i2c_ok) return ESP_ERR_INVALID_STATE;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 1, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return ret;
}
