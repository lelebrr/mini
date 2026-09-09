import sys
sys.stdout.reconfigure(encoding="utf-8")

# app_main.c
code = """#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "hal/hal_i2c.h"
#include "hal/hal_pmu.h"
#include "hal/hal_tca9554.h"
#include "hal/hal_display.h"
#include "hal/hal_touch.h"
#include "hal/hal_rtc.h"
#include "hal/hal_imu.h"
#include "hal/hal_sd.h"
#include "hal/hal_audio.h"
#include "hal/hal_thermal.h"
#include "hal/hal_led.h"
#include "ui/ui_boot.h"

static const char *TAG = "SENTINEL";

void app_main(void) {
    ESP_LOGI(TAG, "SENTINEL-S3 v1.0.0 booting...");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    hal_i2c_init(); hal_i2c_scan();
    hal_pmu_init();
    hal_tca9554_init();
    hal_display_init(); hal_display_set_brightness(200);
    hal_touch_init();
    hal_rtc_init();
    hal_imu_init();
    hal_sd_init();
    hal_audio_init(); hal_audio_beep(1760, 70);
    hal_thermal_init();
    hal_led_init(); hal_led_set_color(0, 0, 50);
    ui_boot_splash();
    ESP_LOGI(TAG, "SENTINEL-S3 ready. BLUE disarmed");
    while (1) { vTaskDelay(pdMS_TO_TICKS(1000)); }
}
"""

with open('F:\mini\sentinels3\main\app_main.c', 'w', encoding='utf-8') as f:
    f.write(code)
print('OK')
