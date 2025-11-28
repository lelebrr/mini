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

// Globals
ESP_IOExpander *expander;
SemaphoreHandle_t gui_mutex;
PwnIMU imu;

// Hardware Objects
Arduino_DataBus *bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3);
Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST, 0 /* rotation */, false /* IPS */);
TouchLib *touch = NULL;

// LVGL Buffers
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

// Otimização 23: Siesta Timer
unsigned long siesta_start = 0;
bool in_siesta = false;

// -------------------------------------------------------------------------
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
    if (touch && touch->read()) {
        TP_Point t = touch->getPoint(0);
        data->point.x = t.x;
        data->point.y = t.y;
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

// -------------------------------------------------------------------------
// Audio Init (ES8311 + I2S)
// -------------------------------------------------------------------------
void initAudio() {
    // 1. Configurar I2S
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, // Mono
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
    }
}

void setup() {
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
}

void loop() {
    unsigned long now = millis();

    // Otimização 24: Monitor de Corrente
    float current = PwnPower::getSystemCurrent(); // Assumindo método disponível ou simulado
    // if (current > 130) PwnPower::setPerformanceMode(0); // Força downclock

    // LVGL
    if (xSemaphoreTake(gui_mutex, 5)) {
        lv_timer_handler();
        PwnUI::update();
        xSemaphoreGive(gui_mutex);
    }

    // Game Logic
    // Otimização 23: Siesta
    int minute = (now / 60000) % 60;
    if (minute == 0 && !in_siesta) {
        in_siesta = true;
        // PwnPower::enterDeepSleep(); // ou Light sleep por 3 min
    } else if (minute > 3) {
        in_siesta = false;
    }

    if (!in_siesta) {
        PwnPet::tick();
        PwnAttack::tick();
    }

    delay(5);
}
