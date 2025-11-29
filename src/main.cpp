#include <Arduino.h>
#include <lvgl.h>
#include <esp_task_wdt.h>

// Core Headers
#include "core/ConfigManager.h" // 1. Config Manager first
#include "core/PwnPower.h"
#include "core/PwnPet.h"
#include "core/PwnAttack.h"
#include "core/PwnUI.h"
#include "core/PwnVoice.h"
#include "core/Gamification.h"
#include "web/WebHandler.h" // 2. Web Handler

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

#define IRAM_ATTR_OPT __attribute__((section(".iram1")))

// Globals
ESP_IOExpander *expander;
SemaphoreHandle_t gui_mutex;
PwnIMU imu;

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
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
#define BUFFER_SIZE (LCD_WIDTH * LCD_HEIGHT)
static lv_color_t *buf1;
static lv_color_t *buf2;

<<<<<<< HEAD
=======
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
#define BUFFER_SIZE (LCD_WIDTH * LCD_HEIGHT / 10)
static lv_color_t *buf1;
static lv_color_t *buf2;

// Siesta
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
unsigned long siesta_start = 0;
bool in_siesta = false;

// -------------------------------------------------------------------------
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
    if (touch && touch->read()) {
        TP_Point t = touch->getPoint(0);
        data->point.x = t.x;
        data->point.y = t.y;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// -------------------------------------------------------------------------
// Audio Init
// -------------------------------------------------------------------------
void initAudio() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
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
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_LRCK,
        .data_out_num = I2S_DOUT,
        .data_in_num = I2S_DIN
<<<<<<< HEAD
=======
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
        .bck_io_num = BCLKPIN,
        .ws_io_num = WSPIN,
        .data_out_num = DOPIN,
        .data_in_num = DIPIN
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
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_zero_dma_buffer(I2S_NUM_0);

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

    // Liga o Amplificador (GPIO 46)
    pinMode(PA_EN, OUTPUT);
    digitalWrite(PA_EN, HIGH);
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
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
    }
}

void setup() {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    Serial.begin(115200);
    setCpuFrequencyMhz(240);

    // I2C 400kHz
    Wire.begin(IIC_SDA, IIC_SCL, 400000);

    PwnPower::init();

=======
    // Serial nativa (USB CDC) para debug
    Serial.begin(115200);
    // Nota: Nao esperar Serial aqui, senao trava sem USB conectado em boot

    setCpuFrequencyMhz(240);

    // I2C Init (Shared Bus)
    Wire.begin(IIC_SDA, IIC_SCL);

    // PMU Init (Critical for voltage rails)
    PwnPower::init();

    // IO Expander (TCA9554)
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
    expander = new ESP_IOExpander_TCA95xx_8bit((i2c_port_t)0, 0x20, IIC_SCL, IIC_SDA);
    expander->init();
    expander->begin();
    expander->pinMode(0, OUTPUT); // LCD RST
    expander->pinMode(1, OUTPUT); // Touch RST

    // Reset Sequence
<<<<<<< HEAD
    expander->digitalWrite(0, LOW); delay(20);
    expander->digitalWrite(0, HIGH); delay(100);

    expander->digitalWrite(1, LOW); delay(20);
    expander->digitalWrite(1, HIGH); delay(100);

    // SD Card
    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_D0);
    if (!SD_MMC.begin("/sdcard", true, true)) {
=======
    expander->digitalWrite(0, LOW); delay(10);
    expander->digitalWrite(0, HIGH); delay(50);

    expander->digitalWrite(1, LOW); delay(10);
    expander->digitalWrite(1, HIGH); delay(50);

    // SD Card (SDMMC 1-bit mode)
    // GPIOs 2 e 1 usados para CLK e CMD
    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_D0);
    if (!SD_MMC.begin("/sdcard", true, true)) { // true=1bit, true=format_if_fail
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
        Serial.println("SD MMC Init Failed!");
    } else {
        ConfigManager::getInstance()->load();
    }
<<<<<<< HEAD
=======
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
    #if CORE_DEBUG_LEVEL > 0
    Serial.begin(115200);
    #endif

    setCpuFrequencyMhz(240);

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
>>>>>>> main
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified

    gfx->begin();
    gfx->fillScreen(BLACK);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    touch = new TouchLib(Wire, IIC_SDA, IIC_SCL, FT3168_ADDR);
    if (!touch->init()) {
=======

    // Init GFX
    gfx->begin();
    gfx->fillScreen(BLACK);

    // Init Touch (FT3168 or GT1151 fallback)
    touch = new TouchLib(Wire, IIC_SDA, IIC_SCL, FT3168_ADDR);
    if (!touch->init()) {
         // Try GT1151 address
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
         delete touch;
         touch = new TouchLib(Wire, IIC_SDA, IIC_SCL, 0x14);
         touch->init();
    }
<<<<<<< HEAD
=======
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
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
>>>>>>> main
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers

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

    Serial.println("Mini Lele (v2.0) Iniciado!");
}

void loop() {
    unsigned long now = millis();

    float current = PwnPower::getSystemCurrent();
    if (current > 130) PwnPower::setPerformanceMode(0);

    if (xSemaphoreTake(gui_mutex, 5)) {
        lv_timer_handler();
        PwnUI::update();
        xSemaphoreGive(gui_mutex);
    }

    int minute = (now / 60000) % 60;
    if (minute == 0 && !in_siesta) {
        in_siesta = true;
    } else if (minute > 3) {
        in_siesta = false;
    }

    if (!in_siesta) {
        PwnPet::tick();
        Gamification::tick();
    }

    delay(5);
}
