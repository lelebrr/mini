#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lvgl.h"

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

static const char *TAG = "MAIN";

static lv_display_t *lv_disp = NULL;

static void lvgl_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    (void)disp;
    (void)area;
    (void)px_map;
    lv_display_flush_ready(disp);
}

static void init_lvgl(void) {
    lv_init();
    lv_disp = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    if (lv_disp) {
        lv_display_set_flush_cb(lv_disp, lvgl_flush);
        size_t buf_size = LCD_WIDTH * LCD_HEIGHT / 4 * sizeof(lv_color_t);
        lv_color_t *buf1 = heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
        lv_color_t *buf2 = heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
        if (buf1 && buf2) lv_display_set_buffers(lv_disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_DIRECT);
        ESP_LOGI(TAG, "LVGL init OK, buffers in PSRAM");
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "SENTINEL-S3 v1.0.0 booting...");
    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    hal_i2c_init();
    hal_i2c_scan();
    hal_pmu_init();
    hal_tca9554_init();
    hal_display_init();
    hal_touch_init();
    hal_rtc_init();
    hal_imu_init();
    hal_sd_init();
    hal_audio_init();
    hal_audio_beep(1760, 70);
    hal_thermal_init();
    hal_led_init();
    hal_led_set_color(0, 0, 50);
    
    init_lvgl();
    ui_boot_splash();
    
    int boot_pin = gpio_get_level(PIN_ARM);
    if (boot_pin) {
        ESP_LOGI(TAG, "ARM GPIO9 HIGH: RED available");
        hal_led_set_color(50, 0, 0);
    } else {
        ESP_LOGI(TAG, "ARM GPIO9 LOW: BLUE mode (safe)");
    }
    
    ESP_LOGI(TAG, "SENTINEL-S3 ready");
    
    while (1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
