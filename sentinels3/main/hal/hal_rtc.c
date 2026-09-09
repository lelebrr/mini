#include "hal/hal_rtc.h"
#include "hal/hal_i2c.h"
#include "esp_log.h"
#include "sentinel_hw.h"
#include <time.h>

static const char *TAG = "RTC";
static bool g_rtc_ok = false;
static time_t g_time = 0;

void hal_rtc_init(void) {
    uint8_t data;
    if (hal_i2c_read(ADDR_PCF85063, 0x00, &data) == ESP_OK) {
        g_rtc_ok = true;
        ESP_LOGI(TAG, "found PCF85063 at 0x%02X", ADDR_PCF85063);
    } else {
        ESP_LOGW(TAG, "PCF85063 not found");
    }
}

bool hal_rtc_get_time(char *buf, size_t len) {
    if (!g_rtc_ok) {
        struct tm t;
        localtime_r(&g_time, &t);
        strftime(buf, len, "%Y-%m-%d %H:%M:%S", &t);
        return true;
    }
    uint8_t reg[7];
    if (hal_i2c_read_bytes(ADDR_PCF85063, 0x04, reg, 7) != ESP_OK) return false;
    int y = 2000 + (reg[6] & 0x3F);
    int m = reg[5] & 0x1F;
    int d = reg[4] & 0x3F;
    int h = reg[3] & 0x3F;
    int min = reg[2] & 0x7F;
    int s = reg[1] & 0x7F;
    snprintf(buf, len, "%04d-%02d-%02d %02d:%02d:%02d", y, m, d, h, min, s);
    return true;
}

void hal_rtc_set_time(int y, int m, int d, int h, int min, int s) {
    if (!g_rtc_ok) return;
    uint8_t reg[7] = {0, (uint8_t)(s & 0x7F), (uint8_t)(min & 0x7F), (uint8_t)(h & 0x3F), (uint8_t)(d & 0x3F), (uint8_t)(m & 0x1F), (uint8_t)((y - 2000) & 0x3F)};
    hal_i2c_write_bytes(ADDR_PCF85063, 0x04, reg, 7);
}
