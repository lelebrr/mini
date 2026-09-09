#include "hal/hal_touch.h"
#include "hal/hal_i2c.h"
#include "hal/hal_tca9554.h"
#include "esp_log.h"
#include "sentinel_hw.h"

static const char *TAG = "TOUCH";
static const char *g_chip = "none";
static int g_addr = 0;

void hal_touch_init(void) {
    ESP_LOGI(TAG, "init...");
    hal_tca9554_write_pin(TCA_PIN_TP_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(5));
    hal_tca9554_write_pin(TCA_PIN_TP_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(20));
    uint8_t data;
    if (hal_i2c_read(ADDR_FT3168, 0x00, &data) == ESP_OK) {
        g_chip = "FT3168";
        g_addr = ADDR_FT3168;
        ESP_LOGI(TAG, "FT3168 at 0x%02X", g_addr);
    } else if (hal_i2c_read(ADDR_CST820, 0x00, &data) == ESP_OK) {
        g_chip = "CST820";
        g_addr = ADDR_CST820;
        ESP_LOGI(TAG, "CST820 at 0x%02X", g_addr);
    } else {
        ESP_LOGW(TAG, "No touch chip found");
    }
}

int hal_touch_read(int *x, int *y) {
    if (!g_addr) return 0;
    uint8_t data[6];
    if (hal_i2c_read_bytes(g_addr, 0x02, data, 6) != ESP_OK) return 0;
    if (!(data[0] & 0x01)) return 0;
    *x = ((data[1] & 0x0F) << 8) | data[2];
    *y = ((data[3] & 0x0F) << 8) | data[4];
    return 1;
}

const char* hal_touch_get_chip_name(void) { return g_chip; }
