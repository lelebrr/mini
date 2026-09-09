#include "hal/hal_display.h"
#include "hal/hal_i2c.h"
#include "hal/hal_tca9554.h"
#include "esp_log.h"
#include "sentinel_hw.h"

static const char *TAG = "DISP";
static const char *g_chip = "unknown";

void hal_display_init(void) {
    ESP_LOGI(TAG, "init...");
    hal_tca9554_write_pin(TCA_PIN_LCD_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    hal_tca9554_write_pin(TCA_PIN_LCD_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(120));
    uint8_t data;
    esp_err_t err = hal_i2c_read(ADDR_FT3168, 0x00, &data);
    if (err == ESP_OK) {
        g_chip = "SH8601";
        ESP_LOGI(TAG, "Rev1 SH8601 detected via FT3168 touch");
    } else {
        g_chip = "CO5300";
        ESP_LOGI(TAG, "Rev2 CO5300 assumed");
    }
    ESP_LOGI(TAG, "Display: %s (%dx%d)", g_chip, LCD_WIDTH, LCD_HEIGHT);
}

void hal_display_set_brightness(uint8_t b) {
    hal_i2c_write(ADDR_AXP2101, 0x16, b > 200 ? 0x33 : 0x10);
}

const char* hal_display_get_chip_name(void) { return g_chip; }
void hal_display_fill(uint16_t color) { (void)color; }
int hal_display_get_width(void) { return LCD_WIDTH; }
int hal_display_get_height(void) { return LCD_HEIGHT; }
