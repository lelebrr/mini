#include "hal/hal_thermal.h"
#include "esp_log.h"
#include "driver/temperature_sensor.h"

static const char *TAG = "THERMAL";
static temperature_sensor_handle_t g_temp = NULL;

void hal_thermal_init(void) {
    temperature_sensor_config_t cfg = { .range_min = 10, .range_max = 85 };
    esp_err_t err = temperature_sensor_install(&cfg, &g_temp);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "temp sensor not available");
        return;
    }
    temperature_sensor_enable(g_temp);
    ESP_LOGI(TAG, "OK");
}

float hal_thermal_read(void) {
    if (!g_temp) return 25.0f;
    float temp;
    temperature_sensor_get_celsius(g_temp, &temp);
    return temp;
}
