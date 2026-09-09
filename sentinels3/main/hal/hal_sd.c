#include "hal/hal_sd.h"
#include "hal/hal_tca9554.h"
#include "esp_log.h"
#include "sentinel_hw.h"
#include "sdmmc_cmd.h"
#include "vfs_fat_sdmmc.h"

static const char *TAG = "SD";
static bool g_mounted = false;

void hal_sd_init(void) {
    hal_tca9554_write_pin(TCA_PIN_SD_CS, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
    slot.gpio_cd = SDMMC_SLOT_NO_CD;
    slot.gpio_wp = SDMMC_SLOT_NO_WP;
    slot.width = 1;
    slot.gpio_clk = PIN_SD_CLK;
    slot.gpio_cmd = PIN_SD_CMD;
    slot.gpio_d0 = PIN_SD_D0;
    esp_vfs_fat_sdmmc_mount_config_t mount = { .format_if_mount_failed = false, .max_files = 5, .allocation_unit_size = 8192 };
    sdmmc_card_t *card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sd", &host, &slot, &mount, &card);
    if (ret == ESP_OK) {
        g_mounted = true;
        ESP_LOGI(TAG, "SD mounted at /sd");
        FILE *f = fopen("/sd/SENTINEL.txt", "w");
        if (f) { fprintf(f, "SENTINEL-S3 v1.0.0\n"); fclose(f); }
    } else {
        ESP_LOGW(TAG, "SD mount failed: %s", esp_err_to_name(ret));
    }
}

bool hal_sd_is_mounted(void) { return g_mounted; }
