#include "ui/ui_boot.h"
#include "esp_log.h"

static const char *TAG = "BOOT";

void ui_boot_splash(void) {
    ESP_LOGI(TAG, "================================");
    ESP_LOGI(TAG, "  SENTINEL-S3 v1.0.0");
    ESP_LOGI(TAG, "  AUTHORIZED USE ONLY");
    ESP_LOGI(TAG, "  USO AUTORIZADO APENAS");
    ESP_LOGI(TAG, "================================");
    vTaskDelay(pdMS_TO_TICKS(1500));
}
