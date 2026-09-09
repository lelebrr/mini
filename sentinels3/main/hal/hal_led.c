#include "hal/hal_led.h"
#include "esp_log.h"
#include "sentinel_hw.h"
#include "driver/rmt_tx.h"

static const char *TAG = "LED";
static bool g_led_ok = false;

void hal_led_init(void) {
    gpio_set_direction(PIN_LED_RGB, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_LED_RGB, 0);
    g_led_ok = true;
    ESP_LOGI(TAG, "LED on GPIO%d", PIN_LED_RGB);
}

void hal_led_set_color(uint8_t r, uint8_t g, uint8_t b) {
    if (!g_led_ok) return;
    uint32_t color = (g << 16) | (r << 8) | b;
    gpio_set_level(PIN_LED_RGB, color ? 1 : 0);
}
