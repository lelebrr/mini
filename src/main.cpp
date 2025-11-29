#include <Arduino.h>
#include <lvgl.h>
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
#include <esp_task_wdt.h>

// Core Headers
#include "core/ConfigManager.h" // 1. Config Manager first
#include "core/PwnPower.h"
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
// Core Headers
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
#include <esp_task_wdt.h>

// Core Headers
#include "core/PwnPower.h"
<<<<<<< HEAD
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
#include "core/PwnPet.h"
#include "core/PwnAttack.h"
#include "core/PwnUI.h"
#include "core/PwnVoice.h"
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
#include "core/Gamification.h"
#include "web/WebHandler.h" // 2. Web Handler
=======
#include "core/Gamification.h"
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
#include "core/Gamification.h"
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
#include "core/Gamification.h"
#include "web/WebHandler.h" // 2. Web Handler
>>>>>>> origin/pwntamagotchi-br-v2-webui-final

// Drivers
#include "pin_config.h"
#include "drivers/PwnIMU.h"
#include "ESP_IOExpander_Library.h"
#include <SD_MMC.h>
#include "WiFiTools.h"
#include <Wire.h>

// Audio Drivers
#include <driver/i2s.h>
#include "../lib/es8311/es8311.h"

// Graphics Drivers
#include <Arduino_GFX_Library.h>
#include <TouchLib.h>

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#define IRAM_ATTR_OPT __attribute__((section(".iram1")))

=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
// Otimização Block 4: IRAM Attributes
#define IRAM_ATTR_OPT __attribute__((section(".iram1")))

>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
#define IRAM_ATTR_OPT __attribute__((section(".iram1")))

>>>>>>> origin/pwntamagotchi-br-v2-webui-final
// Globals
ESP_IOExpander *expander;
SemaphoreHandle_t gui_mutex;
PwnIMU imu;

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
// Hardware Objects - QSPI SH8601 (Waveshare Standard)
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCK, LCD_D0, LCD_D1, LCD_D2, LCD_D3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);
=======
// Hardware Objects
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0, false, LCD_WIDTH, LCD_HEIGHT);
>>>>>>> main
=======
// Hardware Objects
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0, false, LCD_WIDTH, LCD_HEIGHT);
>>>>>>> origin/merge-ready-mini-lele-v2
=======
// Hardware Objects
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0, false, LCD_WIDTH, LCD_HEIGHT);
>>>>>>> origin/mini-lele-v2-complete-verified
=======
// Hardware Objects
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0, false, LCD_WIDTH, LCD_HEIGHT);
>>>>>>> origin/mini-lele-v2-final-verified
=======
// Hardware Objects - QSPI SH8601 (Waveshare Standard)
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCK, LCD_D0, LCD_D1, LCD_D2, LCD_D3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
// Hardware Objects - QSPI SH8601 (Waveshare Standard)
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCK, LCD_D0, LCD_D1, LCD_D2, LCD_D3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
// Hardware Objects - QSPI SH8601 (Waveshare Standard)
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCK, LCD_D0, LCD_D1, LCD_D2, LCD_D3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
// Hardware Objects - QSPI SH8601 (Waveshare Standard)
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCK, LCD_D0, LCD_D1, LCD_D2, LCD_D3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
// Hardware Objects - QSPI SH8601 (Waveshare Standard)
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCK, LCD_D0, LCD_D1, LCD_D2, LCD_D3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
// Hardware Objects
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0, false, LCD_WIDTH, LCD_HEIGHT);
>>>>>>> origin/mini-lele-v2-rebrand
=======
// ST7701S Init Operations (Standard sequence for 480x480 or similar - adapted for 320x240 RGB)
// This is a placeholder for the actual massive init array usually required by ST7701
Arduino_DataBus *bus = new Arduino_ESP32RGBPanel(
    LCD_DE, LCD_VSYNC, LCD_HSYNC, LCD_PCLK,
    LCD_R0, LCD_R1, LCD_R2, LCD_R3, LCD_R4,
    LCD_G0, LCD_G1, LCD_G2, LCD_G3, LCD_G4, LCD_G5,
    LCD_B0, LCD_B1, LCD_B2, LCD_B3, LCD_B4
);

// Fallback init if header not present
// Real ST7701S requires ~100 bytes of init commands via SPI before RGB starts
// Assuming Arduino_GFX has a default or user provides it.
// We use a generic placeholder here to allow compilation.
Arduino_GFX *gfx = new Arduino_ST7701_RGBPanel(
    bus, LCD_RST, 0, true, LCD_WIDTH, LCD_HEIGHT
);
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
// Hardware Objects - QSPI SH8601 (Waveshare Standard)
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCK, LCD_D0, LCD_D1, LCD_D2, LCD_D3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
// Hardware Objects
// Configuração para SH8601 368x448 (Waveshare Standard)
// Se usuário realmente tiver ST7701S RGB, a pinagem e driver seriam drasticamente diferentes.
// Assume-se que o usuário quer o driver SH8601 que roda na placa citada.
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0, false, LCD_WIDTH, LCD_HEIGHT);
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
// Hardware Objects
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0, false, LCD_WIDTH, LCD_HEIGHT);
>>>>>>> origin/pwntamagotchi-br-v2-webui-final

TouchLib *touch = NULL;

// LVGL 9 Display Object
lv_display_t * disp;
lv_indev_t * indev;

<<<<<<< HEAD
<<<<<<< HEAD
// Double Buffer PSRAM
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
#define BUFFER_SIZE (LCD_WIDTH * LCD_HEIGHT)
static lv_color_t *buf1;
static lv_color_t *buf2;

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-rebrand
#define BUFFER_SIZE (LCD_WIDTH * LCD_HEIGHT / 10)
=======
// Double Buffer PSRAM (Block 4: Opt 1)
#define BUFFER_SIZE (LCD_WIDTH * LCD_HEIGHT / 10) // 1/10th screen buffer
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
// Double Buffer PSRAM
#define BUFFER_SIZE (LCD_WIDTH * LCD_HEIGHT / 10)
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
static lv_color_t *buf1;
static lv_color_t *buf2;

// Siesta
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> main
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/mini-lele-v2-rebrand
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
// Hardware Objects
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0 /* rotation */, false /* IPS */);
TouchLib *touch = NULL;

// LVGL Buffers
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

// Otimização 23: Siesta Timer
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
unsigned long siesta_start = 0;
bool in_siesta = false;

// -------------------------------------------------------------------------
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
// Display Callback (LVGL 9)
// -------------------------------------------------------------------------
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t * px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
<<<<<<< HEAD
<<<<<<< HEAD
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t*)px_map, w, h);
=======

    // DMA2D Simulation via GFX (Block 4: Opt 4)
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t*)px_map, w, h);

>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t*)px_map, w, h);
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    lv_display_flush_ready(disp);
}

// -------------------------------------------------------------------------
// Touch Callback (LVGL 9)
// -------------------------------------------------------------------------
void my_touch_read(lv_indev_t * indev, lv_indev_data_t * data) {
<<<<<<< HEAD
<<<<<<< HEAD
=======
// Display Callback
// -------------------------------------------------------------------------
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
    lv_disp_flush_ready(disp);
}

// -------------------------------------------------------------------------
// Touch Callback
// -------------------------------------------------------------------------
void my_touch_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
    // Block 4: Opt 17 (Interrupt check would happen here if wired)
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    if (touch && touch->read()) {
        TP_Point t = touch->getPoint(0);
        data->point.x = t.x;
        data->point.y = t.y;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
=======
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
        data->state = LV_INDEV_STATE_PRESSED;

        // Reset Kiosk Timer (Opt 82)
        // last_touch_time = millis();
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    }
}

// -------------------------------------------------------------------------
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
// Audio Init
// -------------------------------------------------------------------------
void initAudio() {
=======
// Audio Init (ES8311 + I2S)
// -------------------------------------------------------------------------
void initAudio() {
    // 1. Configurar I2S
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
// Audio Init
// -------------------------------------------------------------------------
void initAudio() {
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
// Audio Init
// -------------------------------------------------------------------------
void initAudio() {
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
=======
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, // Mono
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = true,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_LRCK,
        .data_out_num = I2S_DOUT,
        .data_in_num = I2S_DIN
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-rebrand
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
        .bck_io_num = BCLKPIN,
        .ws_io_num = WSPIN,
        .data_out_num = DOPIN,
        .data_in_num = DIPIN
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> main
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/mini-lele-v2-rebrand
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_zero_dma_buffer(I2S_NUM_0);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDR);
=======
    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDRRES_0);
>>>>>>> main
=======
    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDRRES_0);
>>>>>>> origin/merge-ready-mini-lele-v2
=======
    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDRRES_0);
>>>>>>> origin/mini-lele-v2-complete-verified
=======
    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDRRES_0);
>>>>>>> origin/mini-lele-v2-final-verified
=======
    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDR);
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDR);
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDR);
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDR);
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDR);
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDRRES_0);
>>>>>>> origin/mini-lele-v2-rebrand
=======
    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDR);
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDR);
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDRRES_0);
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDRRES_0);
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    if (es_dev) {
         es8311_clock_config_t cfg = {0};
         cfg.sample_frequency = 16000;
         es8311_init(es_dev, &cfg, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16);
         es8311_voice_volume_set(es_dev, 60, NULL);
         es8311_microphone_config(es_dev, false);
         es8311_voice_mute(es_dev, false);
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

    pinMode(PA_EN, OUTPUT);
    digitalWrite(PA_EN, HIGH);
=======
>>>>>>> main
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets

    // Liga o Amplificador (GPIO 46)
    pinMode(PA_EN, OUTPUT);
    digitalWrite(PA_EN, HIGH);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======

    pinMode(PA_EN, OUTPUT);
    digitalWrite(PA_EN, HIGH);
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/mini-lele-v2-rebrand
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
}

// -------------------------------------------------------------------------
// Task Definitions
// -------------------------------------------------------------------------
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
void scanTask(void *pvParameters) {
    while(1) {
        if (PwnAttack::isRunning()) {
            PwnAttack::tick();
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
<<<<<<< HEAD
=======
    // 2. Configurar ES8311 (Via I2C)
    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDRRES_0);
    if (es_dev) {
         es8311_clock_config_t cfg;
         cfg.mclk_inverted = false;
         cfg.sclk_inverted = false;
         cfg.mclk_from_mclk_pin = false; // MCLK from SCLK/BCLK logic internal
         cfg.mclk_frequency = 0;
         cfg.sample_frequency = 16000;

         es8311_init(es_dev, &cfg, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16);
         es8311_voice_volume_set(es_dev, 60, NULL);
         es8311_microphone_config(es_dev, false); // Analog Mic
         es8311_voice_mute(es_dev, false);
         Serial.println("[Audio] ES8311 Iniciado");
    } else {
         Serial.println("[Audio] Falha ES8311");
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
// Block 4: Opt 9 (Core 0 Pinned Task for WiFi)
void scanTask(void *pvParameters) {
    while(1) {
        if (PwnAttack::isRunning()) {
            // Priority logic inside PwnAttack
            PwnAttack::tick();
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    }
}

void setup() {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
    Serial.begin(115200);
    setCpuFrequencyMhz(240);

    // I2C 400kHz
    Wire.begin(IIC_SDA, IIC_SCL, 400000);

    PwnPower::init();

<<<<<<< HEAD
=======
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
    // Serial nativa (USB CDC) para debug
    Serial.begin(115200);
    // Nota: Nao esperar Serial aqui, senao trava sem USB conectado em boot
=======
    #if CORE_DEBUG_LEVEL > 0
    Serial.begin(115200);
    #endif
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final

    setCpuFrequencyMhz(240);

    // I2C Init (Shared Bus)
    Wire.begin(IIC_SDA, IIC_SCL);

    // PMU Init (Critical for voltage rails)
    PwnPower::init();

    // IO Expander (TCA9554)
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
    expander = new ESP_IOExpander_TCA95xx_8bit((i2c_port_t)0, 0x20, IIC_SCL, IIC_SDA);
    expander->init();
    expander->begin();
    expander->pinMode(0, OUTPUT); // LCD RST
    expander->pinMode(1, OUTPUT); // Touch RST
<<<<<<< HEAD

    // Reset Sequence
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
    expander->digitalWrite(0, LOW); delay(20);
    expander->digitalWrite(0, HIGH); delay(100);

    expander->digitalWrite(1, LOW); delay(20);
    expander->digitalWrite(1, HIGH); delay(100);

    // SD Card
    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_D0);
    if (!SD_MMC.begin("/sdcard", true, true)) {
<<<<<<< HEAD
=======
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
    expander->pinMode(6, OUTPUT); // Audio PA

    // Reset Sequence
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
    expander->digitalWrite(0, LOW); delay(10);
    expander->digitalWrite(0, HIGH); delay(50);

    expander->digitalWrite(1, LOW); delay(10);
    expander->digitalWrite(1, HIGH); delay(50);

<<<<<<< HEAD
    // SD Card (SDMMC 1-bit mode)
    // GPIOs 2 e 1 usados para CLK e CMD
    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_D0);
    if (!SD_MMC.begin("/sdcard", true, true)) { // true=1bit, true=format_if_fail
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
    // Audio Power On
    expander->digitalWrite(6, HIGH);

    // SD Card (SDMMC 1-bit mode standard for this board)
    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_D0);
    if (!SD_MMC.begin("/sdcard", true, true)) { // true=1bit, true=format_if_fail
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
        Serial.println("SD MMC Init Failed!");
    } else {
        ConfigManager::getInstance()->load();
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-rebrand
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
    // Opt 26: Serial off in runtime
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    #if CORE_DEBUG_LEVEL > 0
    Serial.begin(115200);
    #endif

<<<<<<< HEAD
<<<<<<< HEAD
    setCpuFrequencyMhz(240);

<<<<<<< HEAD
    Wire.begin(IIC_SDA, IIC_SCL);

    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
    bool sd_ok = SD_MMC.begin("/sdcard", true);

    ConfigManager::getInstance()->load();

=======
    setCpuFrequencyMhz(240); // Boot turbo (Opt 7 handles scaling later)

    Wire.begin(IIC_SDA, IIC_SCL);
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
    setCpuFrequencyMhz(240);

    Wire.begin(IIC_SDA, IIC_SCL);

    // 0. Mount SD First (Config needs it)
    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
    bool sd_ok = SD_MMC.begin("/sdcard", true);

    // 1. Config Manager Init
    ConfigManager::getInstance()->load();

>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    PwnPower::init();

    expander = new ESP_IOExpander_TCA95xx_8bit((i2c_port_t)0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, IIC_SCL, IIC_SDA);
    expander->init();
    expander->begin();
<<<<<<< HEAD
<<<<<<< HEAD
    expander->pinMode(0, OUTPUT);
    expander->pinMode(6, OUTPUT);
    expander->digitalWrite(0, LOW); delay(10);
    expander->digitalWrite(0, HIGH); delay(50);
    expander->digitalWrite(6, HIGH);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> main
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-rebrand
=======
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    expander->pinMode(0, OUTPUT); // LCD RST
    expander->pinMode(6, OUTPUT); // Audio PA
    expander->digitalWrite(0, LOW); delay(10);
    expander->digitalWrite(0, HIGH); delay(50);
    expander->digitalWrite(6, HIGH);
<<<<<<< HEAD
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final

    gfx->begin();
    gfx->fillScreen(BLACK);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    touch = new TouchLib(Wire, IIC_SDA, IIC_SCL, FT3168_ADDR);
    if (!touch->init()) {
=======
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
    Wire.begin(TOUCH_SDA, TOUCH_SCL);

    // Power Init (AXP2101)
    PwnPower::init();

    // SD Card Init (SD_MMC 1-bit or 4-bit)
    // Using 1-bit mode often safer if pins conflict, but we try 4-bit first per pin_config
    SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3);
    if (!SD_MMC.begin("/sdcard", true)) {
        Serial.println("SD MMC Init Failed!");
        // Try 1-bit mode fallback
        SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0);
        if (!SD_MMC.begin("/sdcard", true, true)) {
             Serial.println("SD MMC 1-Bit Failed!");
        }
    }

    // Now we can load config
    ConfigManager::getInstance()->load();
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final

    // Init GFX
    gfx->begin();
    gfx->fillScreen(BLACK);

<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
    // Init Touch (FT3168 or GT1151 fallback)
    touch = new TouchLib(Wire, IIC_SDA, IIC_SCL, FT3168_ADDR);
    if (!touch->init()) {
         // Try GT1151 address
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
    touch = new TouchLib(Wire, IIC_SDA, IIC_SCL, FT3168_ADDR);
    if (!touch->init()) {
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
         delete touch;
         touch = new TouchLib(Wire, IIC_SDA, IIC_SCL, 0x14);
         touch->init();
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-rebrand
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    Wire.beginTransmission(GT1151_DEVICE_ADDRESS);
    if (Wire.endTransmission() == 0) {
        touch = new TouchLib(Wire, IIC_SDA, IIC_SCL, GT1151_DEVICE_ADDRESS);
    } else {
        touch = new TouchLib(Wire, IIC_SDA, IIC_SCL, FT3168_DEVICE_ADDRESS);
    }
    touch->init();
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> main
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/mini-lele-v2-rebrand
=======
    touch = new TouchLib(Wire, TOUCH_SDA, TOUCH_SCL, GT1151_ADDR);
    touch->init();
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final

    lv_init();

    buf1 = (lv_color_t*)heap_caps_malloc(BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    buf2 = (lv_color_t*)heap_caps_malloc(BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);

    disp = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_flush_cb(disp, my_disp_flush);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    lv_display_set_buffers(disp, buf1, buf2, BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_FULL);
=======
    lv_display_set_buffers(disp, buf1, buf2, BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
>>>>>>> main
=======
    lv_display_set_buffers(disp, buf1, buf2, BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
>>>>>>> origin/merge-ready-mini-lele-v2
=======
    lv_display_set_buffers(disp, buf1, buf2, BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
>>>>>>> origin/mini-lele-v2-complete-verified
=======
    lv_display_set_buffers(disp, buf1, buf2, BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
>>>>>>> origin/mini-lele-v2-final-verified
=======
    lv_display_set_buffers(disp, buf1, buf2, BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_FULL);
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
    lv_display_set_buffers(disp, buf1, buf2, BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_FULL);
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
    lv_display_set_buffers(disp, buf1, buf2, BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_FULL);
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
    lv_display_set_buffers(disp, buf1, buf2, BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_FULL);
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
    lv_display_set_buffers(disp, buf1, buf2, BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_FULL);
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
    lv_display_set_buffers(disp, buf1, buf2, BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
>>>>>>> origin/mini-lele-v2-rebrand
=======
    lv_display_set_buffers(disp, buf1, buf2, BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_FULL);
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
    lv_display_set_buffers(disp, buf1, buf2, BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_FULL);
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final

=======

    lv_init();

    // Allocate Buffers in PSRAM
    buf1 = (lv_color_t*)heap_caps_malloc(BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    buf2 = (lv_color_t*)heap_caps_malloc(BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);

    // Create Display
    disp = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, buf1, buf2, BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);

    // Create Input
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
    lv_display_set_buffers(disp, buf1, buf2, BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);

>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touch_read);

    imu.init(Wire);
    imu.enableWakeOnMotion();

<<<<<<< HEAD
<<<<<<< HEAD
=======
    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
    SD_MMC.begin("/sdcard", true);

>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    initAudio();

    gui_mutex = xSemaphoreCreateMutex();
    Gamification::init();
    PwnPet::init();
<<<<<<< HEAD
<<<<<<< HEAD

    PwnAttack::init();
    PwnUI::init();

    WebHandler::init();

    xTaskCreatePinnedToCore(scanTask, "WiFiScan", 4096, NULL, 1, NULL, 0);

<<<<<<< HEAD
<<<<<<< HEAD
    Serial.println("Mini Lele (v2.0) Iniciado!");
=======
    Serial.println("PwnTamagotchi BR - Edicao Lendaria Iniciado!");
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
    Serial.println("Mini Lele (v2.0) Iniciado!");
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
    Serial.begin(115200);
    setCpuFrequencyMhz(160);

    // 1. Power & I2C Init
    Wire.begin(IIC_SDA, IIC_SCL);
    PwnPower::init();

    // 2. IO Expander Init
    expander = new ESP_IOExpander_TCA95xx_8bit((i2c_port_t)0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, IIC_SCL, IIC_SDA);
    expander->init();
    expander->begin();

    // Configura pinos do Expander
    expander->pinMode(0, OUTPUT); // LCD RST
    expander->pinMode(6, OUTPUT); // Audio PA

    // Reset Display
    expander->digitalWrite(0, LOW); delay(10);
    expander->digitalWrite(0, HIGH); delay(50);

    // Audio Start
    expander->digitalWrite(6, HIGH);

    // 3. Display Init (SH8601 QSPI)
    gfx->begin();
    gfx->fillScreen(BLACK);

    // 4. Touch Init (FT3168 ou GT1151)
    // Tenta GT1151 primeiro conforme pedido
    Wire.beginTransmission(GT1151_DEVICE_ADDRESS);
    if (Wire.endTransmission() == 0) {
        touch = new TouchLib(Wire, IIC_SDA, IIC_SCL, GT1151_DEVICE_ADDRESS);
        Serial.println("[Touch] GT1151 Detectado");
    } else {
        // Fallback FT3168
        touch = new TouchLib(Wire, IIC_SDA, IIC_SCL, FT3168_DEVICE_ADDRESS);
        Serial.println("[Touch] Usando FT3168 (Fallback)");
    }
    touch->init();

    // 5. LVGL Init
    lv_init();

    // Otimização 19: Buffer Dinâmico PSRAM
    size_t screen_size = LCD_WIDTH * LCD_HEIGHT;
    disp_draw_buf = (lv_color_t *)heap_caps_malloc(screen_size * sizeof(lv_color_t) / 4, MALLOC_CAP_SPIRAM);
    if (!disp_draw_buf) {
        disp_draw_buf = (lv_color_t *)malloc(LCD_WIDTH * 100 * sizeof(lv_color_t));
        lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, LCD_WIDTH * 100);
    } else {
        lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screen_size / 4);
    }

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_WIDTH;
    disp_drv.ver_res = LCD_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Input Device
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touch_read;
    lv_indev_drv_register(&indev_drv);

    // 6. Drivers Restantes
    imu.init(Wire);

    // Otimização 22: IMU Wake Config (Placeholder)
    // imu.enableWakeOnMotion();

    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
    if (!SD_MMC.begin("/sdcard", true)) {
        Serial.println("SD Falhou!");
        // Otimização 14: Unmount auto se falha?
    }

    initAudio();

    // 7. Core Logic Init
    gui_mutex = xSemaphoreCreateMutex();
    PwnPet::init();
    PwnAttack::init();
    PwnUI::init();

    Serial.println("PwnTamagotchi BR (Otimizado + Full HW) Iniciado!");
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
    PwnAttack::init();
    PwnUI::init();

    // Create Scan Task Pinned to Core 0
    xTaskCreatePinnedToCore(scanTask, "WiFiScan", 4096, NULL, 1, NULL, 0);

    // Watchdog Init (Opt 30)
    // esp_task_wdt_init(10, true);
    // esp_task_wdt_add(NULL);
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======

    // Configura PwnAttack baseado no Config
    // PwnAttack::init() agora deve ler config internamente ou ser configurado aqui
    PwnAttack::init();

    PwnUI::init();

    // 2. WebUI Init
    WebHandler::init();

    xTaskCreatePinnedToCore(scanTask, "WiFiScan", 4096, NULL, 1, NULL, 0);
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
}

void loop() {
    unsigned long now = millis();

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    float current = PwnPower::getSystemCurrent();
    if (current > 130) PwnPower::setPerformanceMode(0);

=======
// Drivers
#include "pin_config.h"
#include "XPowersLib.h"
#include "ESP_IOExpander_Library.h"
#include <SD_MMC.h>
#include "WiFiTools.h"

// Globals
XPowersPMU power;
ESP_IOExpander *expander;
SemaphoreHandle_t gui_mutex;

void setup() {
    Serial.begin(115200);
    gui_mutex = xSemaphoreCreateMutex();

    // 1. Power
    Wire.begin(IIC_SDA, IIC_SCL);
    power.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
    power.enableALDO1(); power.enableALDO2(); power.enableALDO3(); power.enableALDO4();
    power.enableBLDO1(); power.enableBLDO2();

    // 2. IO Expander
    expander = new ESP_IOExpander_TCA95xx_8bit((i2c_port_t)0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, IIC_SCL, IIC_SDA);
    expander->init(); expander->begin();
    expander->pinMode(0, OUTPUT); expander->digitalWrite(0, HIGH); // LCD RST

    // 3. SD Card
    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
    SD_MMC.begin("/sdcard", true);

    // 4. Core Init
    PwnPet::init();
    PwnAttack::init();

    // 5. UI Init
    // (Assume LVGL driver setup code is reused from previous main.cpp snippet here for brevity,
    // real impl would strictly copy the display/touch driver code)
    // ... Display Driver Init ...
    lv_init();
    // ... Buffer Init ...

    PwnUI::init();

    Serial.println("PwnTamagotchi BR Iniciado!");
}

void loop() {
    // 1. LVGL Logic
>>>>>>> origin/pwn-tamagotchi-br-release
=======
    // PwnPower::getSystemCurrent() relies on initialized PMU
    float current = PwnPower::getSystemCurrent();
    if (current > 130) PwnPower::setPerformanceMode(0);

>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
    float current = PwnPower::getSystemCurrent();
    if (current > 130) PwnPower::setPerformanceMode(0);

>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
    // Otimização 24: Monitor de Corrente
    float current = PwnPower::getSystemCurrent(); // Assumindo método disponível ou simulado
    // if (current > 130) PwnPower::setPerformanceMode(0); // Força downclock

    // LVGL
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
    // Opt 24: Monitor Current
    float current = PwnPower::getSystemCurrent();
    if (current > 130) PwnPower::setPerformanceMode(0);

>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
    float current = PwnPower::getSystemCurrent();
    if (current > 130) PwnPower::setPerformanceMode(0);

>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    if (xSemaphoreTake(gui_mutex, 5)) {
        lv_timer_handler();
        PwnUI::update();
        xSemaphoreGive(gui_mutex);
    }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
    int minute = (now / 60000) % 60;
    if (minute == 0 && !in_siesta) {
        in_siesta = true;
=======
    // Game Logic
    // Otimização 23: Siesta
    int minute = (now / 60000) % 60;
    if (minute == 0 && !in_siesta) {
        in_siesta = true;
        // PwnPower::enterDeepSleep(); // ou Light sleep por 3 min
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
    int minute = (now / 60000) % 60;
    if (minute == 0 && !in_siesta) {
        in_siesta = true;
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
    int minute = (now / 60000) % 60;
    if (minute == 0 && !in_siesta) {
        in_siesta = true;
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    } else if (minute > 3) {
        in_siesta = false;
    }

    if (!in_siesta) {
        PwnPet::tick();
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        Gamification::tick();
    }
<<<<<<< HEAD
<<<<<<< HEAD
=======
    // 2. Game Logic
    PwnPet::tick();

    // 3. Attack Logic
    PwnAttack::tick();
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
        PwnAttack::tick();
    }
>>>>>>> origin/pwntamagotchi-br-final-90-features

=======
        Gamification::tick();
    }

    // esp_task_wdt_reset();
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
        Gamification::tick();
    }

>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    delay(5);
=======
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-fix-v2
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include <ESP_IOExpander_Library.h>
#include "pin_config.h"
#include <Wire.h>
#include <SD_MMC.h>
#include "es8311.h"
#include <driver/i2s.h>
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

// --- Global Objects ---
=======
=======
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
#include "XPowersLib.h"
#include <SensorQMI8658.hpp>
#include "AudioHandler.h"
#include "OfflineVoice.h"
#include "CommandHandler.h"
#include "FaceHandler.h"
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
#include "Gamification.h"
#include "WiFiTools.h"
#include "OnlineCrack.h"
#include "EvilPortal.h"

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
// --- Mutex ---
SemaphoreHandle_t gui_mutex;

// --- Objetos ---
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
// --- Objects ---
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======

// --- Objects ---
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
// --- Objects ---
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
#include "XPowersLib.h"

// --- Global Objects ---
>>>>>>> origin/waveshare-s3-amoled-fix-v2
=======
#include "Gamification.h"
#include "WiFiTools.h"

// --- Objects ---
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
  LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */, LCD_SDIO1 /* SDIO1 */,
  LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST /* RST (-1) */,
                                      0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
  std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

ESP_IOExpander *expander = NULL;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

// Touch (FT3168)
=======
=======
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
XPowersPMU power;
SensorQMI8658 qmi;
IMUdata acc;

// --- Touch ---
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
XPowersPMU power;

// Touch (FT3168)
>>>>>>> origin/waveshare-s3-amoled-fix-v2
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
void Arduino_IIC_Touch_Interrupt(void);
std::unique_ptr<Arduino_IIC> FT3168(new Arduino_FT3x68(IIC_Bus, FT3168_DEVICE_ADDRESS,
                                                       DRIVEBUS_DEFAULT_VALUE, TP_INT, Arduino_IIC_Touch_Interrupt));

void Arduino_IIC_Touch_Interrupt(void) {
  FT3168->IIC_Interrupt_Flag = true;
}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-fix-v2
// LVGL Buffers
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf1;
static lv_color_t *buf2;

// --- LVGL Functions ---
#if LV_USE_LOG != 0
void my_print(const char *buf) {
  Serial.printf(buf);
  Serial.flush();
}
#endif

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

<<<<<<< HEAD
=======
=======
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
// --- UI Objects ---
lv_obj_t *label_status;
lv_obj_t *label_battery;
lv_obj_t *btn_voice;
lv_obj_t *label_btn;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
lv_obj_t *label_instr;
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
lv_obj_t *label_game;
lv_obj_t *label_sys;
lv_obj_t *btn_tools;
lv_obj_t *btn_portal;
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
lv_obj_t *btn_sniffer;

// --- Global States ---
bool sniffer_active = false;
unsigned long last_interaction = 0;
bool screen_on = true;

// --- Helpers ---
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
lv_obj_t *label_instr;
lv_obj_t *label_game;
lv_obj_t *label_sys;
lv_obj_t *btn_tools;
lv_obj_t *btn_portal; // New
=======
lv_obj_t *label_instr;
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
lv_obj_t *btn_sniffer; // New Sniffer Btn

// --- Global States ---
bool sniffer_active = false;
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
lv_obj_t *label_instr;
lv_obj_t *label_game; // Level/XP
lv_obj_t *label_sys;  // Mem/Temp
>>>>>>> origin/waveshare-s3-amoled-full-plugins

// --- Helper Functions ---
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-fix-v2
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======

>>>>>>> origin/waveshare-s3-amoled-fix-v2
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    int32_t touchX = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
    int32_t touchY = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-fix-v2

    FT3168->IIC_Interrupt_Flag = true;

    if (touchX > 0 && touchY > 0) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
<<<<<<< HEAD
=======
=======
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
    FT3168->IIC_Interrupt_Flag = true;
    if (touchX > 0 && touchY > 0) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX; data->point.y = touchY;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        last_interaction = millis();
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-fix-v2
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
        last_interaction = millis();
>>>>>>> origin/waveshare-s3-amoled-full-review-406
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-fix-v2
// --- Audio Init ---
esp_err_t audio_init() {
  // 1. I2C Init for ES8311 (Already done via Wire)
  es8311_handle_t es_handle = es8311_create(0, ES8311_ADDRRES_0);
  if (!es_handle) {
      Serial.println("ES8311 Create Failed");
      return ESP_FAIL;
  }

  const es8311_clock_config_t es_clk = {
    .mclk_inverted = false,
    .sclk_inverted = false,
    .mclk_from_mclk_pin = true,
    .mclk_frequency = 16000 * 256,
    .sample_frequency = 16000
  };

  if (es8311_init(es_handle, &es_clk, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16) != ESP_OK) {
      Serial.println("ES8311 Init Failed");
<<<<<<< HEAD
      return ESP_FAIL;
  }

  es8311_voice_volume_set(es_handle, 60, NULL);
=======
      // Don't return fail yet, maybe just config issue, allow I2S setup
  }

  es8311_voice_volume_set(es_handle, 70, NULL);
>>>>>>> origin/waveshare-s3-amoled-fix-v2
  es8311_microphone_gain_set(es_handle, ES8311_MIC_GAIN_18DB);
  es8311_microphone_config(es_handle, false); // Analog Mic

  // 2. I2S Init
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 6,
    .dma_buf_len = 160,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = BCLKPIN,
    .ws_io_num = WSPIN,
    .data_out_num = DOPIN,
    .data_in_num = DIPIN
  };

  if (i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) != ESP_OK) {
      Serial.println("I2S Driver Install Failed");
      return ESP_FAIL;
  }

  if (i2s_set_pin(I2S_NUM_0, &pin_config) != ESP_OK) {
      Serial.println("I2S Set Pin Failed");
      return ESP_FAIL;
  }

<<<<<<< HEAD
  // MCLK is often generated by ESP32 on GPIO 0, 1, or 3, or specifically GPIO2 on some boards
  // For ESP32-S3, MCLK can be routed to any pin via GPIO Matrix if using proper clock source
  // However, often specific pins are better.
  // We will assume the pin_config.h MCLKPIN (2) is correct.
  // Need to enable MCLK output.
=======
  // Enable MCLK
>>>>>>> origin/waveshare-s3-amoled-fix-v2
  PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[MCLKPIN], PIN_FUNC_GPIO);
  gpio_set_direction((gpio_num_t)MCLKPIN, GPIO_MODE_OUTPUT);

  Serial.println("Audio Initialized (ES8311 + I2S)");
<<<<<<< HEAD
=======
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
void ui_set_face(FaceType type) {
    if (xSemaphoreTake(gui_mutex, portMAX_DELAY)) {
        FaceHandler::setFace(type);
        xSemaphoreGive(gui_mutex);
    }
}

void ui_set_label(lv_obj_t* label, const char* text) {
    if (xSemaphoreTake(gui_mutex, portMAX_DELAY)) {
        lv_label_set_text(label, text);
        xSemaphoreGive(gui_mutex);
    }
}

// --- Voice Task ---
void voice_processing_task(void * param) {
    ui_set_face(FACE_LOOK_R);
    AudioHandler::playWav("/listening_pt.wav");

    // Caminho ajustado: /arquivos_cartao_sd/recording.wav
    bool recorded = AudioHandler::recordWav("/recording.wav", 4, true);

    if (recorded) {
        ui_set_face(FACE_INTENSE);
        int syllables = OfflineVoice::analyzeCommand("/recording.wav");
        if (syllables > 0) {
            if (xSemaphoreTake(gui_mutex, portMAX_DELAY)) {
                 CommandHandler::processSyllables(syllables);
                 xSemaphoreGive(gui_mutex);
            }
            Gamification::addXP(10);
        } else {
            ui_set_face(FACE_BORED);
            AudioHandler::playWav("/error_pt.wav");
        }
    } else {
        ui_set_face(FACE_SAD);
<<<<<<< HEAD
=======
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
// --- Voice Task ---
=======
// --- Voice Task (Offline) ---
>>>>>>> origin/waveshare-s3-amoled-full-plugins
void voice_processing_task(void * param) {
    FaceHandler::setFace(FACE_LOOK_R);
    AudioHandler::playWav("/listening_pt.wav");
    bool recorded = AudioHandler::recordWav("/recording.wav", 4, true);

    if (recorded) {
        FaceHandler::setFace(FACE_INTENSE);
        int syllables = OfflineVoice::analyzeCommand("/recording.wav");
        if (syllables > 0) {
            CommandHandler::processSyllables(syllables);
<<<<<<< HEAD
            Gamification::addXP(10);
        } else {
<<<<<<< HEAD
=======
// --- Voice Task (Offline) ---
void voice_processing_task(void * param) {
    Serial.println("Voice Task: Iniciado (Offline)");

    // Set Listening Face
    FaceHandler::setFace(FACE_LOOK_R);

    // 1. Play Prompt
    AudioHandler::playWav("/listening_pt.wav");

    // 2. Record (Smart VAD)
    bool recorded = AudioHandler::recordWav("/recording.wav", 4, true);

    if (recorded) {
        // Processing Face
        FaceHandler::setFace(FACE_INTENSE);

        // 3. Analyze Offline
        int syllables = OfflineVoice::analyzeCommand("/recording.wav");
        if (syllables > 0) {
            CommandHandler::processSyllables(syllables); // Sets success face
        } else {
            Serial.println("Voice: Nenhum comando detectado");
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
            Gamification::addXP(10); // Reward for command
        } else {
>>>>>>> origin/waveshare-s3-amoled-full-plugins
            FaceHandler::setFace(FACE_BORED);
            AudioHandler::playWav("/error_pt.wav");
        }
    } else {
        FaceHandler::setFace(FACE_SAD);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
        AudioHandler::playWav("/error_pt.wav");
    }

    vTaskDelay(3000 / portTICK_PERIOD_MS);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    ui_set_face(FACE_NEUTRAL);
    ui_set_label(label_btn, "Falar Comando");
=======
    FaceHandler::setFace(FACE_NEUTRAL);
    lv_label_set_text(label_btn, "Falar Comando");
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
        AudioHandler::playWav("/error_pt.wav");
    }

    // Return to default after a while?
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    FaceHandler::setFace(FACE_NEUTRAL);
    lv_label_set_text(label_btn, "Falar Comando");
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
    FaceHandler::setFace(FACE_NEUTRAL);
    lv_label_set_text(label_btn, "Falar Comando");
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
    FaceHandler::setFace(FACE_NEUTRAL);
    lv_label_set_text(label_btn, "Falar Comando");
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
    ui_set_face(FACE_NEUTRAL);
    ui_set_label(label_btn, "Falar Comando");
>>>>>>> origin/waveshare-s3-amoled-full-review-406

    vTaskDelete(NULL);
}

static void event_handler_voice_btn(lv_event_t * e) {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
=======
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
>>>>>>> origin/waveshare-s3-amoled-full-review-406
        lv_label_set_text(label_btn, "Ouvindo...");
        xTaskCreate(voice_processing_task, "VoiceTask", 8192, NULL, 1, NULL);
    }
}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
// --- Sniffer Toggle ---
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
static void event_handler_sniffer_btn(lv_event_t * e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (!sniffer_active) {
            WiFiTools::startSniffer();
            sniffer_active = true;
            lv_label_set_text(lv_obj_get_child(btn_sniffer, 0), "Parar Sniff");
            FaceHandler::setFace(FACE_INTENSE);
        } else {
            WiFiTools::stopSniffer();
            sniffer_active = false;
            lv_label_set_text(lv_obj_get_child(btn_sniffer, 0), "Sniffer");
            FaceHandler::setFace(FACE_NEUTRAL);
        }
    }
}

<<<<<<< HEAD
<<<<<<< HEAD
=======
// --- Portal Toggle ---
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
static void event_handler_portal_btn(lv_event_t * e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (!EvilPortal::isRunning()) {
            EvilPortal::start("Cafe_Gratis", "/evil_portal/03_cafe_gratis.html");
            lv_label_set_text(lv_obj_get_child(btn_portal, 0), "Parar Portal");
        } else {
            EvilPortal::stop();
<<<<<<< HEAD
<<<<<<< HEAD
            lv_label_set_text(lv_obj_get_child(btn_portal, 0), "EVIL PORTAL");
=======
            lv_label_set_text(lv_obj_get_child(btn_portal, 0), "Evil Portal");
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
            lv_label_set_text(lv_obj_get_child(btn_portal, 0), "EVIL PORTAL");
>>>>>>> origin/waveshare-s3-amoled-full-review-406
        }
    }
}

<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
void upload_task(void * param) {
    ui_set_face(FACE_UPLOAD);
    ui_set_label(label_status, "Conectando e Enviando...");

    int sent = OnlineCrack::uploadAll();

    if (sent >= 0) {
        char buf[32]; snprintf(buf, sizeof(buf), "Sucesso! Enviados: %d", sent);
        ui_set_label(label_status, buf);
        ui_set_face(FACE_COOL);
        AudioHandler::playWav("/success_pt.wav");
    } else {
        ui_set_label(label_status, "Erro: WiFi ou Config");
        ui_set_face(FACE_BROKEN);
        AudioHandler::playWav("/error_pt.wav");
    }

    vTaskDelay(4000 / portTICK_PERIOD_MS);
    ui_set_face(FACE_NEUTRAL);
    ui_set_label(label_status, "Sistema Pronto");
<<<<<<< HEAD
=======
// --- Evil Portal Task ---
void portal_task(void * param) {
    if (EvilPortal::isRunning()) {
        EvilPortal::stop();
        lv_label_set_text(label_status, "Portal Parado");
    } else {
        // Start "Free Coffee" by default for demo, or cycle/random
        EvilPortal::start("Cafe_Gratis_WiFi", "/evil_portal/03_cafe_gratis.html");
        lv_label_set_text(label_status, "Portal ATIVO");
    }
    vTaskDelete(NULL);
}

static void event_handler_portal_btn(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        xTaskCreate(portal_task, "PortalTask", 4096, NULL, 1, NULL);
    }
}

=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
// --- Upload Task ---
void upload_task(void * param) {
    FaceHandler::setFace(FACE_UPLOAD);
    lv_label_set_text(label_status, "Enviando Cracks...");
    int sent = OnlineCrack::uploadAll();
    if (sent >= 0) {
        char buf[32]; sprintf(buf, "Enviados: %d", sent);
        lv_label_set_text(label_status, buf);
        FaceHandler::setFace(FACE_COOL);
        AudioHandler::playWav("/success_pt.wav");
    } else {
<<<<<<< HEAD
        lv_label_set_text(label_status, "Falha WiFi/Config");
=======
        lv_label_set_text(label_status, "Erro Envio");
>>>>>>> origin/waveshare-s3-amoled-final-polish
        FaceHandler::setFace(FACE_BROKEN);
        AudioHandler::playWav("/error_pt.wav");
    }
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    FaceHandler::setFace(FACE_NEUTRAL);
    lv_label_set_text(label_status, "Sistema Pronto");
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
    vTaskDelete(NULL);
}

static void event_handler_upload_btn(lv_event_t * e) {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
=======
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
>>>>>>> origin/waveshare-s3-amoled-full-review-406
        xTaskCreate(upload_task, "UploadTask", 8192, NULL, 1, NULL);
    }
}

<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
// --- Battery Page ---
void show_battery_details() {
    // Popup or new screen logic could go here
    // For now, simple console dump or update main label extended
    Serial.printf("Bat: %dmV, %dmA\n", power.getBattVoltage(), power.getBattDischargeCurrent());
}

>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
// --- Audio Init ---
esp_err_t audio_init() {
  es8311_handle_t es_handle = es8311_create(0, ES8311_ADDRRES_0);
  if (!es_handle) return ESP_FAIL;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  const es8311_clock_config_t es_clk = { .mclk_inverted = false, .sclk_inverted = false, .mclk_from_mclk_pin = true, .mclk_frequency = 16000 * 256, .sample_frequency = 16000 };
=======
=======
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins

  const es8311_clock_config_t es_clk = {
    .mclk_inverted = false, .sclk_inverted = false, .mclk_from_mclk_pin = true,
    .mclk_frequency = 16000 * 256, .sample_frequency = 16000
  };
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
  const es8311_clock_config_t es_clk = { .mclk_inverted = false, .sclk_inverted = false, .mclk_from_mclk_pin = true, .mclk_frequency = 16000 * 256, .sample_frequency = 16000 };
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  es8311_init(es_handle, &es_clk, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16);
  es8311_voice_volume_set(es_handle, 70, NULL);
  es8311_microphone_gain_set(es_handle, ES8311_MIC_GAIN_18DB);
  es8311_microphone_config(es_handle, false);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  i2s_config_t i2s_config = { .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX), .sample_rate = 16000, .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, .communication_format = I2S_COMM_FORMAT_STAND_I2S, .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, .dma_buf_count = 6, .dma_buf_len = 160, .use_apll = false, .tx_desc_auto_clear = true, .fixed_mclk = 0 };
  i2s_pin_config_t pin_config = { .mck_io_num = MCLKPIN, .bck_io_num = BCLKPIN, .ws_io_num = WSPIN, .data_out_num = DOPIN, .data_in_num = DIPIN };
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
=======
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins

  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
    .sample_rate = 16000, .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 6, .dma_buf_len = 160,
    .use_apll = false, .tx_desc_auto_clear = true, .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .mck_io_num = MCLKPIN,
    .bck_io_num = BCLKPIN, .ws_io_num = WSPIN,
    .data_out_num = DOPIN, .data_in_num = DIPIN
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-fix-v2
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  return ESP_OK;
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-fix-v2
  Serial.println("System Initializing...");

  // Init I2C
  Wire.begin(IIC_SDA, IIC_SCL);

<<<<<<< HEAD
  // Init IO Expander (TCA9554)
=======
  // Init AXP2101 PMU
  bool pmu_result = power.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
  if (!pmu_result) {
    Serial.println("PMU AXP2101 is not online...");
  } else {
    Serial.println("PMU AXP2101 Initialized");

    // Set charging voltage/current if needed (defaults usually safe)
    power.setChargeTargetVoltage(3); // From demo
    power.enableBattDetection();
    power.enableVbusVoltageMeasure();
    power.enableBattVoltageMeasure();
    power.enableSystemVoltageMeasure();

    // IMPORTANT: Enable voltages for Screen/Peripherals
    // The demo doesn't explicitly enable DCDC/ALDOs, implying defaults are ON or controlled via Expander?
    // However, it's safer to ensure they are on if we knew which ones.
    // For now, we trust the demo which relies on Expander for power gates.
  }

  // Init IO Expander (TCA9554)
  // Demo uses 0x20.
>>>>>>> origin/waveshare-s3-amoled-fix-v2
  expander = new ESP_IOExpander_TCA95xx_8bit((i2c_port_t)0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, IIC_SCL, IIC_SDA);
  if(expander) {
      expander->init();
      expander->begin();

<<<<<<< HEAD
      // Configure pins
      expander->pinMode(0, OUTPUT); // LCD_RST
      expander->pinMode(1, OUTPUT); // TOUCH_RST ?
      expander->pinMode(2, OUTPUT); // PWR
      expander->pinMode(6, OUTPUT); // Audio PWR?

      // Reset Sequence
      expander->digitalWrite(0, LOW);
      expander->digitalWrite(1, LOW);
      delay(20);
      expander->digitalWrite(0, HIGH); // Release LCD Reset
      expander->digitalWrite(1, HIGH); // Release Touch Reset
      expander->digitalWrite(2, HIGH); // Enable Power
      expander->digitalWrite(6, HIGH); // Enable Audio Power

      Serial.println("IO Expander Initialized & Reset Sequence Done");
=======
      // Configure pins as per 13_LVGL_Widgets demo
      // It sets 0,1,2,6 to OUTPUT and toggles them.
      // 0: LCD_RST?
      // 1: TOUCH_RST
      // 2: POWER GATE
      // 6: AUDIO/BACKLIGHT?

      expander->pinMode(0, OUTPUT);
      expander->pinMode(1, OUTPUT);
      expander->pinMode(2, OUTPUT);
      expander->pinMode(6, OUTPUT);

      // Reset/Power Cycle Sequence
      expander->digitalWrite(0, LOW);
      expander->digitalWrite(1, LOW);
      expander->digitalWrite(2, LOW);
      expander->digitalWrite(6, LOW);
      delay(50); // Increased delay
      expander->digitalWrite(0, HIGH);
      expander->digitalWrite(1, HIGH);
      expander->digitalWrite(2, HIGH);
      expander->digitalWrite(6, HIGH);

      Serial.println("IO Expander Initialized & Power Sequence Done");
>>>>>>> origin/waveshare-s3-amoled-fix-v2
  } else {
      Serial.println("IO Expander Init Failed");
  }

  // Init Display
<<<<<<< HEAD
  // Note: Reset already done by expander
=======
>>>>>>> origin/waveshare-s3-amoled-fix-v2
  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);
  gfx->Display_Brightness(200);
  Serial.println("Display Initialized");

  // Init Touch
  while (FT3168->begin() == false) {
    Serial.println("FT3168 initialization fail");
    delay(1000);
  }
  Serial.println("Touch Initialized");

  FT3168->IIC_Write_Device_State(FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE,
                                 FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_MONITOR);

  // Init SD Card
  SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("SD Card Mount Failed");
  } else {
    Serial.printf("SD Card Type: %d\n", SD_MMC.cardType());
    Serial.printf("SD Card Size: %lluMB\n", SD_MMC.cardSize() / (1024 * 1024));
  }

  // Init Audio
  audio_init();

  // Init LVGL
  lv_init();
  uint32_t screenWidth = gfx->width();
  uint32_t screenHeight = gfx->height();

  buf1 = (lv_color_t *)heap_caps_malloc(screenWidth * screenHeight / 10 * sizeof(lv_color_t), MALLOC_CAP_DMA);
  buf2 = (lv_color_t *)heap_caps_malloc(screenWidth * screenHeight / 10 * sizeof(lv_color_t), MALLOC_CAP_DMA);

  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, screenWidth * screenHeight / 10);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  // UI
  lv_obj_t *label = lv_label_create(lv_scr_act());
<<<<<<< HEAD
  lv_label_set_text(label, "Hello Waveshare!\nESP32-S3 AMOLED\nAudio & SD Ready");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  // Test Audio Tone (Simple square wave via I2S would require writing buffer)
  // For now just logged initialized.

=======
  lv_label_set_text(label, "Hello Waveshare!\nESP32-S3 AMOLED\nAudio & SD Ready\nPMU: Active");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

>>>>>>> origin/waveshare-s3-amoled-fix-v2
  Serial.println("Setup Done");
}

void loop() {
  lv_timer_handler();
  delay(5);
<<<<<<< HEAD
>>>>>>> origin/waveshare-esp32-s3-amoled-fix
=======
  gui_mutex = xSemaphoreCreateMutex();
  Wire.begin(IIC_SDA, IIC_SCL);

=======
  Wire.begin(IIC_SDA, IIC_SCL);

  // PMU
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
  Wire.begin(IIC_SDA, IIC_SCL);

  // PMU
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
  Wire.begin(IIC_SDA, IIC_SCL);

  // PMU
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
  Wire.begin(IIC_SDA, IIC_SCL);

  // PMU
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
  gui_mutex = xSemaphoreCreateMutex();
  Wire.begin(IIC_SDA, IIC_SCL);

>>>>>>> origin/waveshare-s3-amoled-full-review-406
  power.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
  power.setChargeTargetVoltage(3);
  power.enableBattDetection();
  power.enableVbusVoltageMeasure();
  power.enableBattVoltageMeasure();
  power.enableSystemVoltageMeasure();
  power.enableALDO1(); power.enableALDO2(); power.enableALDO3(); power.enableALDO4();
  power.enableBLDO1(); power.enableBLDO2();

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
  // IO Expander
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
  // IO Expander
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
  // IO Expander
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
  // IO Expander
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  expander = new ESP_IOExpander_TCA95xx_8bit((i2c_port_t)0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, IIC_SCL, IIC_SDA);
  if(expander) {
      expander->init(); expander->begin();
      expander->pinMode(0, OUTPUT); expander->pinMode(1, OUTPUT); expander->pinMode(2, OUTPUT); expander->pinMode(6, OUTPUT);
      expander->digitalWrite(0, LOW); expander->digitalWrite(1, LOW); expander->digitalWrite(2, LOW); expander->digitalWrite(6, LOW);
      delay(50);
      expander->digitalWrite(0, HIGH); expander->digitalWrite(1, HIGH); expander->digitalWrite(2, HIGH); expander->digitalWrite(6, HIGH);
  }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
  // IMU
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
  // IMU
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
  // IMU
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
  // IMU
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
  qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, SensorQMI8658::ACC_ODR_1000Hz, SensorQMI8658::LPF_MODE_0);
  qmi.enableAccelerometer();

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
  // Display
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
  // Display
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
  // Display
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
  // Display
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  gfx->begin();
  gfx->fillScreen(BLACK);
  gfx->Display_Brightness(200);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  FT3168->begin();
  FT3168->IIC_Write_Device_State(FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE, FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_MONITOR);

=======
=======
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
  // Touch
  FT3168->begin();
  FT3168->IIC_Write_Device_State(FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE, FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_MONITOR);

  // SD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
  FT3168->begin();
  FT3168->IIC_Write_Device_State(FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE, FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_MONITOR);

>>>>>>> origin/waveshare-s3-amoled-full-review-406
  SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
  SD_MMC.begin("/sdcard", true);

  Gamification::init();
<<<<<<< HEAD
<<<<<<< HEAD
  EvilPortal::init();
<<<<<<< HEAD
<<<<<<< HEAD
  audio_init();

=======

=======
  SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
  SD_MMC.begin("/sdcard", true);

>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======

>>>>>>> origin/waveshare-s3-amoled-final-polish
=======

>>>>>>> origin/waveshare-s3-amoled-full-plugins
  // Audio
  audio_init();

  // LVGL
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
  EvilPortal::init();
  audio_init();

>>>>>>> origin/waveshare-s3-amoled-full-review-406
  lv_init();
  uint32_t screenWidth = gfx->width();
  uint32_t screenHeight = gfx->height();
  buf1 = (lv_color_t *)heap_caps_malloc(screenWidth * screenHeight / 10 * sizeof(lv_color_t), MALLOC_CAP_DMA);
  buf2 = (lv_color_t *)heap_caps_malloc(screenWidth * screenHeight / 10 * sizeof(lv_color_t), MALLOC_CAP_DMA);
  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, screenWidth * screenHeight / 10);
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth; disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush; disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER; indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  // --- UI Layout ---
  lv_obj_t *scr = lv_scr_act();

<<<<<<< HEAD
=======
  // Dashboard Header
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
  // --- UI Layout ---
  lv_obj_t *scr = lv_scr_act();

  // Dashboard Header
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
  // --- UI Layout ---
  lv_obj_t *scr = lv_scr_act();

  // Dashboard Header (Top)
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
  // --- UI Layout ---
  lv_obj_t *scr = lv_scr_act();

>>>>>>> origin/waveshare-s3-amoled-full-review-406
  lv_obj_t *header_cont = lv_obj_create(scr);
  lv_obj_set_size(header_cont, 360, 50);
  lv_obj_align(header_cont, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_color(header_cont, lv_color_hex(0x222222), 0);

  label_sys = lv_label_create(header_cont);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  lv_label_set_text(label_sys, "Iniciando...");
=======
  lv_label_set_text(label_sys, "Init...");
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
  lv_label_set_text(label_sys, "Init...");
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
  lv_label_set_text(label_sys, "Init...");
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
  lv_label_set_text(label_sys, "Iniciando...");
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  lv_obj_align(label_sys, LV_ALIGN_LEFT_MID, 5, 0);
  lv_obj_set_style_text_color(label_sys, lv_color_white(), 0);

  label_battery = lv_label_create(header_cont);
  lv_label_set_text(label_battery, "Bat: --%");
  lv_obj_align(label_battery, LV_ALIGN_RIGHT_MID, -5, 0);
  lv_obj_set_style_text_color(label_battery, lv_color_white(), 0);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  label_game = lv_label_create(scr);
  lv_label_set_text(label_game, "Nvl 1 | 0 XP");
  lv_obj_align(label_game, LV_ALIGN_TOP_MID, 0, 55);

  label_status = lv_label_create(scr);
  lv_label_set_text(label_status, "Sistema Pronto");
  lv_obj_align(label_status, LV_ALIGN_TOP_MID, 0, 80);
  lv_label_set_long_mode(label_status, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(label_status, 300);
  lv_obj_set_style_text_align(label_status, LV_TEXT_ALIGN_CENTER, 0);

  FaceHandler::init(scr);
  FaceHandler::setFace(FACE_NEUTRAL);

  btn_voice = lv_btn_create(scr);
  lv_obj_align(btn_voice, LV_ALIGN_BOTTOM_LEFT, 10, -80);
  lv_obj_set_size(btn_voice, 110, 60);
<<<<<<< HEAD
=======
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
  // Game Stats
=======
  // Game Stats (Below Header)
>>>>>>> origin/waveshare-s3-amoled-full-plugins
  label_game = lv_label_create(scr);
  lv_label_set_text(label_game, "Lvl 1 | 0 XP");
  lv_obj_align(label_game, LV_ALIGN_TOP_MID, 0, 55);

<<<<<<< HEAD
<<<<<<< HEAD
  // System Status Msg
  label_status = lv_label_create(scr);
  lv_label_set_text(label_status, "Pronto");
  lv_obj_align(label_status, LV_ALIGN_TOP_MID, 0, 80);
=======
  // System Status Msg (Used for Sniffer)
  label_status = lv_label_create(scr);
  lv_label_set_text(label_status, "Pronto");
  lv_obj_align(label_status, LV_ALIGN_TOP_MID, 0, 80);
  // Allow multi-line for list
  lv_label_set_long_mode(label_status, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(label_status, 300);
>>>>>>> origin/waveshare-s3-amoled-final-polish

=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
  // Face
  FaceHandler::init(scr);
  FaceHandler::setFace(FACE_NEUTRAL);

<<<<<<< HEAD
  // Buttons Row 1 (Voice & Upload)
  btn_voice = lv_btn_create(scr);
<<<<<<< HEAD
  lv_obj_align(btn_voice, LV_ALIGN_BOTTOM_LEFT, 20, -80);
  lv_obj_set_size(btn_voice, 140, 60);
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
  lv_obj_align(btn_voice, LV_ALIGN_BOTTOM_LEFT, 10, -80);
  lv_obj_set_size(btn_voice, 110, 60);
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  lv_obj_add_event_cb(btn_voice, event_handler_voice_btn, LV_EVENT_ALL, NULL);
  label_btn = lv_label_create(btn_voice);
  lv_label_set_text(label_btn, "Voz");
  lv_obj_center(label_btn);

  btn_tools = lv_btn_create(scr);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  lv_obj_align(btn_tools, LV_ALIGN_BOTTOM_RIGHT, -10, -80);
  lv_obj_set_size(btn_tools, 110, 60);
=======
  lv_obj_align(btn_tools, LV_ALIGN_BOTTOM_RIGHT, -20, -80);
  lv_obj_set_size(btn_tools, 140, 60);
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
  lv_obj_align(btn_tools, LV_ALIGN_BOTTOM_RIGHT, -10, -80);
  lv_obj_set_size(btn_tools, 110, 60);
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
  lv_obj_align(btn_tools, LV_ALIGN_BOTTOM_RIGHT, -10, -80);
  lv_obj_set_size(btn_tools, 110, 60);
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  lv_obj_add_event_cb(btn_tools, event_handler_upload_btn, LV_EVENT_ALL, NULL);
  lv_obj_t *label_tool = lv_label_create(btn_tools);
  lv_label_set_text(label_tool, "Upload");
  lv_obj_center(label_tool);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
  // New: Sniffer Btn (Center Row 1)
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  btn_sniffer = lv_btn_create(scr);
  lv_obj_align(btn_sniffer, LV_ALIGN_BOTTOM_MID, 0, -80);
  lv_obj_set_size(btn_sniffer, 110, 60);
  lv_obj_add_event_cb(btn_sniffer, event_handler_sniffer_btn, LV_EVENT_ALL, NULL);
  lv_obj_t *label_snif = lv_label_create(btn_sniffer);
  lv_label_set_text(label_snif, "Sniffer");
  lv_obj_center(label_snif);

<<<<<<< HEAD
<<<<<<< HEAD
=======
  // Button Row 2 (Portal)
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  btn_portal = lv_btn_create(scr);
  lv_obj_align(btn_portal, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_set_size(btn_portal, 340, 60);
  lv_obj_set_style_bg_color(btn_portal, lv_color_hex(0xFF0000), 0);
<<<<<<< HEAD
<<<<<<< HEAD
=======
  // Button Row 2 (Portal)
  btn_portal = lv_btn_create(scr);
  lv_obj_align(btn_portal, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_set_size(btn_portal, 200, 60);
  lv_obj_set_style_bg_color(btn_portal, lv_color_hex(0xFF0000), 0); // Red for danger
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  lv_obj_add_event_cb(btn_portal, event_handler_portal_btn, LV_EVENT_ALL, NULL);
  lv_obj_t *label_portal = lv_label_create(btn_portal);
  lv_label_set_text(label_portal, "EVIL PORTAL");
  lv_obj_center(label_portal);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  last_interaction = millis();
=======
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
  // UI
  lv_obj_t *scr = lv_scr_act();
  lv_obj_t *label_title = lv_label_create(scr);
  lv_label_set_text(label_title, "Waveshare Offline");
  lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 10);

  label_status = lv_label_create(scr);
  lv_label_set_text(label_status, "Sistema Pronto");
  lv_obj_align(label_status, LV_ALIGN_TOP_LEFT, 10, 40);

  label_battery = lv_label_create(scr);
  lv_label_set_text(label_battery, "Bat: --%");
  lv_obj_align(label_battery, LV_ALIGN_TOP_RIGHT, -10, 40);

=======
  // Voice Button
>>>>>>> origin/waveshare-s3-amoled-full-plugins
  btn_voice = lv_btn_create(scr);
  lv_obj_align(btn_voice, LV_ALIGN_BOTTOM_MID, 0, -60);
  lv_obj_set_size(btn_voice, 200, 50);
  lv_obj_add_event_cb(btn_voice, event_handler_voice_btn, LV_EVENT_ALL, NULL);
  label_btn = lv_label_create(btn_voice);
  lv_label_set_text(label_btn, "Falar Comando");
  lv_obj_center(label_btn);

<<<<<<< HEAD
=======
  // Instructions
>>>>>>> origin/waveshare-s3-amoled-full-plugins
  label_instr = lv_label_create(scr);
  lv_label_set_text(label_instr, "1: Luz | 2: Status | 3: Bat");
  lv_obj_align(label_instr, LV_ALIGN_BOTTOM_MID, 0, -10);

<<<<<<< HEAD
  // Init Face
  FaceHandler::init(scr);
  FaceHandler::setFace(FACE_NEUTRAL);

>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
  last_interaction = millis();
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  AudioHandler::playWav("/boot_pt.wav");
}

void loop() {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  if (xSemaphoreTake(gui_mutex, portMAX_DELAY)) {
      lv_timer_handler();
      xSemaphoreGive(gui_mutex);
  }

  Gamification::tick();
  EvilPortal::loop();

  if (screen_on && (millis() - last_interaction > 30000)) {
      gfx->Display_Brightness(0);
      screen_on = false;
  } else if (!screen_on && (millis() - last_interaction < 30000)) {
      gfx->Display_Brightness(200);
      screen_on = true;
  }
<<<<<<< HEAD
=======
  lv_timer_handler();
  Gamification::tick();
  EvilPortal::loop(); // DNS Processing
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
  lv_timer_handler();
  Gamification::tick();
  EvilPortal::loop();
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
  lv_timer_handler();
  Gamification::tick();
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406

  static uint32_t last_ui_update = 0;
  if (millis() - last_ui_update > 1000) {
      last_ui_update = millis();

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
      if (xSemaphoreTake(gui_mutex, portMAX_DELAY)) {
          if (EvilPortal::isRunning()) {
              char buf[32]; snprintf(buf, sizeof(buf), "Portal ATIVO\nVitimas: %d", EvilPortal::getCapturedCount());
              lv_label_set_text(label_status, buf);
          } else if (sniffer_active) {
              lv_label_set_text(label_status, WiFiTools::getSnifferText().c_str());
          } else {
              lv_label_set_text(label_sys, WiFiTools::getSystemStats().c_str());
          }

          if (power.isBatteryConnect()) {
              int bat_pct = power.getBatteryPercent();
              char bat_str[32];
              if (power.isCharging()) snprintf(bat_str, sizeof(bat_str), "%d%% %dmV (C)", bat_pct, power.getBattVoltage());
              else snprintf(bat_str, sizeof(bat_str), "%d%% %dmV", bat_pct, power.getBattVoltage());
              lv_label_set_text(label_battery, bat_str);
          }

          String gameStr = Gamification::getLevelStr() + " | " + Gamification::getXPStr();
          lv_label_set_text(label_game, gameStr.c_str());

          xSemaphoreGive(gui_mutex);
      }
  }

  delay(5);
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
      // Update Sys Info
      if (EvilPortal::isRunning()) {
          char buf[32];
          sprintf(buf, "Portal ATIVO\nVitimas: %d", EvilPortal::getCapturedCount());
          lv_label_set_text(label_status, buf);
<<<<<<< HEAD
      } else {
          lv_label_set_text(label_sys, WiFiTools::getSystemStats().c_str());
=======
      } else if (sniffer_active) {
          // Update Wall of Flippers list
          lv_label_set_text(label_status, WiFiTools::getSnifferText().c_str());
      } else {
          lv_label_set_text(label_sys, WiFiTools::getSystemStats().c_str());
          // Standard Status
          // lv_label_set_text(label_status, "Sistema Pronto");
>>>>>>> origin/waveshare-s3-amoled-final-polish
      }

=======
      // Update Dashboard
      lv_label_set_text(label_sys, WiFiTools::getSystemStats().c_str());

      // Update Battery
>>>>>>> origin/waveshare-s3-amoled-full-plugins
      if (power.isBatteryConnect()) {
          int bat_pct = power.getBatteryPercent();
          String bat_str = String(bat_pct) + "% " + String(power.getBattVoltage()) + "mV";
          if (power.isCharging()) bat_str += " (C)";
          lv_label_set_text(label_battery, bat_str.c_str());
      }
<<<<<<< HEAD
      String gameStr = Gamification::getLevelStr() + " | " + Gamification::getXPStr();
=======

      // Update Game
      String gameStr = Gamification::getLevelStr() + " | " + Gamification::getXPStr() + "\n" + Gamification::getAgeStr();
>>>>>>> origin/waveshare-s3-amoled-full-plugins
      lv_label_set_text(label_game, gameStr.c_str());
  }

  delay(5);
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
  lv_timer_handler();
  if (power.isBatteryConnect()) {
      int bat_pct = power.getBatteryPercent();
      String bat_str = "Bat: " + String(bat_pct) + "%";
      if (power.isCharging()) bat_str += " (C)";
      lv_label_set_text(label_battery, bat_str.c_str());
  }
  delay(5);
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-fix-v2
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
}
