#include <Arduino.h>
#include <lvgl.h>
#include <esp_task_wdt.h>

// Core Headers
#include "core/PwnPower.h"
#include "core/PwnPet.h"
#include "core/PwnAttack.h"
#include "core/PwnUI.h"
#include "core/PwnVoice.h"
#include "core/Gamification.h"

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

// Otimização Block 4: IRAM Attributes
#define IRAM_ATTR_OPT __attribute__((section(".iram1")))

// Globals
ESP_IOExpander *expander;
SemaphoreHandle_t gui_mutex;
PwnIMU imu;

// Hardware Objects
// Configuração para SH8601 368x448 (Waveshare Standard)
// Se usuário realmente tiver ST7701S RGB, a pinagem e driver seriam drasticamente diferentes.
// Assume-se que o usuário quer o driver SH8601 que roda na placa citada.
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0, false, LCD_WIDTH, LCD_HEIGHT);

TouchLib *touch = NULL;

// LVGL 9 Display Object
lv_display_t * disp;
lv_indev_t * indev;

// Double Buffer PSRAM (Block 4: Opt 1)
#define BUFFER_SIZE (LCD_WIDTH * LCD_HEIGHT / 10) // 1/10th screen buffer
static lv_color_t *buf1;
static lv_color_t *buf2;

// Siesta
unsigned long siesta_start = 0;
bool in_siesta = false;

// -------------------------------------------------------------------------
// Display Callback (LVGL 9)
// -------------------------------------------------------------------------
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t * px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    // DMA2D Simulation via GFX (Block 4: Opt 4)
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t*)px_map, w, h);

    lv_display_flush_ready(disp);
}

// -------------------------------------------------------------------------
// Touch Callback (LVGL 9)
// -------------------------------------------------------------------------
void my_touch_read(lv_indev_t * indev, lv_indev_data_t * data) {
    // Block 4: Opt 17 (Interrupt check would happen here if wired)
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
        .bck_io_num = BCLKPIN,
        .ws_io_num = WSPIN,
        .data_out_num = DOPIN,
        .data_in_num = DIPIN
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_zero_dma_buffer(I2S_NUM_0);

    es8311_handle_t es_dev = es8311_create(0, ES8311_ADDRRES_0);
    if (es_dev) {
         es8311_clock_config_t cfg = {0};
         cfg.sample_frequency = 16000;
         es8311_init(es_dev, &cfg, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16);
         es8311_voice_volume_set(es_dev, 60, NULL);
         es8311_microphone_config(es_dev, false);
         es8311_voice_mute(es_dev, false);
    }
}

// -------------------------------------------------------------------------
// Task Definitions
// -------------------------------------------------------------------------
// Block 4: Opt 9 (Core 0 Pinned Task for WiFi)
void scanTask(void *pvParameters) {
    while(1) {
        if (PwnAttack::isRunning()) {
            // Priority logic inside PwnAttack
            PwnAttack::tick();
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void setup() {
    // Opt 26: Serial off in runtime
    #if CORE_DEBUG_LEVEL > 0
    Serial.begin(115200);
    #endif

    setCpuFrequencyMhz(240); // Boot turbo (Opt 7 handles scaling later)

    Wire.begin(IIC_SDA, IIC_SCL);
    PwnPower::init();

    expander = new ESP_IOExpander_TCA95xx_8bit((i2c_port_t)0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, IIC_SCL, IIC_SDA);
    expander->init();
    expander->begin();
    expander->pinMode(0, OUTPUT); // LCD RST
    expander->pinMode(6, OUTPUT); // Audio PA
    expander->digitalWrite(0, LOW); delay(10);
    expander->digitalWrite(0, HIGH); delay(50);
    expander->digitalWrite(6, HIGH);

    gfx->begin();
    gfx->fillScreen(BLACK);

    Wire.beginTransmission(GT1151_DEVICE_ADDRESS);
    if (Wire.endTransmission() == 0) {
        touch = new TouchLib(Wire, IIC_SDA, IIC_SCL, GT1151_DEVICE_ADDRESS);
    } else {
        touch = new TouchLib(Wire, IIC_SDA, IIC_SCL, FT3168_DEVICE_ADDRESS);
    }
    touch->init();

    lv_init();

    // Allocate Buffers in PSRAM
    buf1 = (lv_color_t*)heap_caps_malloc(BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    buf2 = (lv_color_t*)heap_caps_malloc(BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);

    // Create Display
    disp = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, buf1, buf2, BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);

    // Create Input
    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touch_read);

    imu.init(Wire);
    imu.enableWakeOnMotion();

    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
    SD_MMC.begin("/sdcard", true);

    initAudio();

    gui_mutex = xSemaphoreCreateMutex();
    Gamification::init();
    PwnPet::init();
    PwnAttack::init();
    PwnUI::init();

    // Create Scan Task Pinned to Core 0
    xTaskCreatePinnedToCore(scanTask, "WiFiScan", 4096, NULL, 1, NULL, 0);

    // Watchdog Init (Opt 30)
    // esp_task_wdt_init(10, true);
    // esp_task_wdt_add(NULL);
}

void loop() {
    unsigned long now = millis();

    // Opt 24: Monitor Current
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

    // esp_task_wdt_reset();
    delay(5);
}
