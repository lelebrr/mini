#include "hal/hal_tca9554.h"
#include "hal/hal_i2c.h"
#include "esp_log.h"
#include "sentinel_hw.h"

static const char *TAG = "TCA";
static bool g_tca_ok = false;

void hal_tca9554_init(void) {
    ESP_LOGI(TAG, "init...");
    uint8_t data = 0xFF;
    esp_err_t err = hal_i2c_write_bytes(ADDR_TCA9554, 0x03, &data, 1);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "TCA9554 not found at 0x%02X", ADDR_TCA9554);
        return;
    }
    g_tca_ok = true;
    hal_tca9554_write_pin(TCA_PIN_LCD_RST, 1);
    hal_tca9554_write_pin(TCA_PIN_TP_RST, 1);
    hal_tca9554_write_pin(TCA_PIN_SD_CS, 1);
    ESP_LOGI(TAG, "OK");
}

void hal_tca9554_write_pin(int pin, int val) {
    if (!g_tca_ok) return;
    uint8_t data;
    if (hal_i2c_read(ADDR_TCA9554, 0x01, &data) != ESP_OK) return;
    if (val) data |= (1 << pin);
    else data &= ~(1 << pin);
    hal_i2c_write(ADDR_TCA9554, 0x01, data);
}

int hal_tca9554_read_pin(int pin) {
    if (!g_tca_ok) return -1;
    uint8_t data;
    if (hal_i2c_read(ADDR_TCA9554, 0x00, &data) != ESP_OK) return -1;
    return (data >> pin) & 1;
}
