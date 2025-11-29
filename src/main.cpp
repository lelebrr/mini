#include <Arduino.h>
#include <lvgl.h>
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
#include <esp_task_wdt.h>

// Core Headers
#include "core/ConfigManager.h" // 1. Config Manager first
#include "core/PwnPower.h"
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
#include <esp_task_wdt.h>

// Core Headers
#include "core/PwnPower.h"
>>>>>>> origin/pwntamagotchi-br-final-90-features
#include "core/PwnPet.h"
#include "core/PwnAttack.h"
#include "core/PwnUI.h"
#include "core/PwnVoice.h"
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
#define IRAM_ATTR_OPT __attribute__((section(".iram1")))

=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
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

TouchLib *touch = NULL;

// LVGL 9 Display Object
lv_display_t * disp;
lv_indev_t * indev;

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
static lv_color_t *buf1;
static lv_color_t *buf2;

// Siesta
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
unsigned long siesta_start = 0;
bool in_siesta = false;

// -------------------------------------------------------------------------
<<<<<<< HEAD
// Display Callback (LVGL 9)
// -------------------------------------------------------------------------
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t * px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t*)px_map, w, h);
    lv_display_flush_ready(disp);
}

// -------------------------------------------------------------------------
// Touch Callback (LVGL 9)
// -------------------------------------------------------------------------
void my_touch_read(lv_indev_t * indev, lv_indev_data_t * data) {
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
    if (touch && touch->read()) {
        TP_Point t = touch->getPoint(0);
        data->point.x = t.x;
        data->point.y = t.y;
<<<<<<< HEAD
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
=======
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
>>>>>>> origin/pwntamagotchi-br-final-90-features
    }
}

// -------------------------------------------------------------------------
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
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
<<<<<<< HEAD
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
=======
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, // Mono
>>>>>>> origin/pwntamagotchi-br-final-90-features
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
        .bck_io_num = BCLKPIN,
        .ws_io_num = WSPIN,
        .data_out_num = DOPIN,
        .data_in_num = DIPIN
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
}

// -------------------------------------------------------------------------
// Task Definitions
// -------------------------------------------------------------------------
void scanTask(void *pvParameters) {
    while(1) {
        if (PwnAttack::isRunning()) {
            PwnAttack::tick();
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
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
    #if CORE_DEBUG_LEVEL > 0
    Serial.begin(115200);
    #endif

    setCpuFrequencyMhz(240);

<<<<<<< HEAD
    Wire.begin(IIC_SDA, IIC_SCL);

    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
    bool sd_ok = SD_MMC.begin("/sdcard", true);

    ConfigManager::getInstance()->load();

    PwnPower::init();

    expander = new ESP_IOExpander_TCA95xx_8bit((i2c_port_t)0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, IIC_SCL, IIC_SDA);
    expander->init();
    expander->begin();
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

    gfx->begin();
    gfx->fillScreen(BLACK);

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

    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touch_read);

    imu.init(Wire);
    imu.enableWakeOnMotion();

    initAudio();

    gui_mutex = xSemaphoreCreateMutex();
    Gamification::init();
    PwnPet::init();

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
}

void loop() {
    unsigned long now = millis();

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
    if (xSemaphoreTake(gui_mutex, 5)) {
        lv_timer_handler();
        PwnUI::update();
        xSemaphoreGive(gui_mutex);
    }

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
    } else if (minute > 3) {
        in_siesta = false;
    }

    if (!in_siesta) {
        PwnPet::tick();
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

    delay(5);
}
