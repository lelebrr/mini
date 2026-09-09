#include "hal/hal_audio.h"
#include "esp_log.h"
#include "sentinel_hw.h"

static const char *TAG = "AUDIO";

void hal_audio_init(void) {
    ESP_LOGI(TAG, "init ES8311...");
    ESP_LOGW(TAG, "codec init deferred (requires esp_codec_dev)");
}

void hal_audio_beep(int freq, int dur_ms) {
    (void)freq;
    (void)dur_ms;
    vTaskDelay(pdMS_TO_TICKS(dur_ms));
}
