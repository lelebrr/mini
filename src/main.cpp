#include <Arduino.h>
#include <Wire.h>
#include <lvgl.h>
#include <SD_MMC.h>
#include <WiFi.h>
#include <driver/i2s.h>

#include <Arduino_GFX_Library.h>
#include <ESP_IOExpander_Library.h>
#include <XPowersLib.h>

#include "pin_config.h"

#include "../lib/es8311/es8311.h"

#include "AudioHandler.h"
#include "OfflineVoice.h"
#include "CommandHandler.h"
#include "FaceHandler.h"
#include "Gamification.h"
#include "WiFiTools.h"
#include "EvilPortal.h"
#include "OnlineCrack.h"

#include <TouchLib.h>

// -----------------------------------------------------------------------------
// Globais de hardware
// -----------------------------------------------------------------------------
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS, LCD_SCLK,
    LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3
);

Arduino_GFX *gfx = new Arduino_SH8601(
    bus,
    LCD_RST,
    0,          // rotation
    false,      // IPS
    LCD_WIDTH,
    LCD_HEIGHT
);

XPowersPMU power;
ESP_IOExpander *expander = nullptr;
TouchLib *touch = nullptr;

// -----------------------------------------------------------------------------
// LVGL
// -----------------------------------------------------------------------------
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_buf1;
static lv_color_t *disp_buf2;

SemaphoreHandle_t gui_mutex;

// Objetos de UI
lv_obj_t *label_status = nullptr;
lv_obj_t *label_battery = nullptr;
lv_obj_t *label_game = nullptr;
lv_obj_t *label_sys = nullptr;
lv_obj_t *label_instr = nullptr;
lv_obj_t *btn_voice = nullptr;
lv_obj_t *label_btn = nullptr;
lv_obj_t *btn_sniffer = nullptr;
lv_obj_t *btn_portal = nullptr;
lv_obj_t *btn_upload = nullptr;

// Estado
bool sniffer_active = false;
bool screen_on = true;
unsigned long last_interaction = 0;

// Externs usados por CommandHandler
extern XPowersPMU power;
extern Arduino_GFX *gfx;

// -----------------------------------------------------------------------------
// LVGL: flush do display
// -----------------------------------------------------------------------------
static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

#if LV_COLOR_16_SWAP
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif
    lv_disp_flush_ready(disp);
}

// -----------------------------------------------------------------------------
// LVGL: leitura do touch
// -----------------------------------------------------------------------------
static void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
    if (touch && touch->read()) {
        TP_Point p = touch->getPoint(0);
        data->state = LV_INDEV_STATE_PR;
        data->point.x = p.x;
        data->point.y = p.y;
        last_interaction = millis();
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

// -----------------------------------------------------------------------------
// Áudio: inicialização do ES8311 + I2S
// -----------------------------------------------------------------------------
static esp_err_t audio_init() {
    es8311_handle_t es = es8311_create(0, ES8311_ADDRRES_0);
    if (!es) {
        Serial.println("[Audio] Falha ao criar handle ES8311");
        return ESP_FAIL;
    }

    es8311_clock_config_t clk_cfg = {};
    clk_cfg.mclk_inverted = false;
    clk_cfg.sclk_inverted = false;
    clk_cfg.mclk_from_mclk_pin = true;
    clk_cfg.mclk_frequency = 16000 * 256;
    clk_cfg.sample_frequency = 16000;

    if (es8311_init(es, &clk_cfg, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16) != ESP_OK) {
        Serial.println("[Audio] Falha ao inicializar ES8311");
    }

    es8311_voice_volume_set(es, 70, nullptr);
    es8311_microphone_gain_set(es, ES8311_MIC_GAIN_18DB);
    es8311_microphone_config(es, false);

    i2s_config_t i2s_cfg = {
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

    i2s_pin_config_t pin_cfg = {
        .mck_io_num = MCLKPIN,
        .bck_io_num = BCLKPIN,
        .ws_io_num = WSPIN,
        .data_out_num = DOPIN,
        .data_in_num = DIPIN
    };

    if (i2s_driver_install(I2S_NUM_0, &i2s_cfg, 0, nullptr) != ESP_OK) {
        Serial.println("[Audio] Falha ao instalar driver I2S");
        return ESP_FAIL;
    }
    if (i2s_set_pin(I2S_NUM_0, &pin_cfg) != ESP_OK) {
        Serial.println("[Audio] Falha ao configurar pinos I2S");
        return ESP_FAIL;
    }

    Serial.println("[Audio] ES8311 + I2S inicializados");
    return ESP_OK;
}

// -----------------------------------------------------------------------------
// Helpers de UI seguros com mutex
// -----------------------------------------------------------------------------
static void ui_set_face(FaceType type) {
    if (!gui_mutex) {
        FaceHandler::setFace(type);
        return;
    }
    if (xSemaphoreTake(gui_mutex, portMAX_DELAY)) {
        FaceHandler::setFace(type);
        xSemaphoreGive(gui_mutex);
    }
}

static void ui_set_label(lv_obj_t *label, const char *text) {
    if (!label) return;
    if (!gui_mutex) {
        lv_label_set_text(label, text);
        return;
    }
    if (xSemaphoreTake(gui_mutex, portMAX_DELAY)) {
        lv_label_set_text(label, text);
        xSemaphoreGive(gui_mutex);
    }
}

// -----------------------------------------------------------------------------
// Tarefa de voz (botão estilo "Siri")
// -----------------------------------------------------------------------------
static void voice_processing_task(void *param) {
    ui_set_face(FACE_LOOK_R);
    AudioHandler::playWav("/listening_pt.wav");

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
        AudioHandler::playWav("/error_pt.wav");
    }

    vTaskDelay(3000 / portTICK_PERIOD_MS);
    ui_set_face(FACE_NEUTRAL);
    ui_set_label(label_btn, "Falar com Mini Lele");
    vTaskDelete(nullptr);
}

// -----------------------------------------------------------------------------
// Tarefa de upload (OnlineCrack)
// -----------------------------------------------------------------------------
static void upload_task(void *param) {
    ui_set_face(FACE_UPLOAD);
    ui_set_label(label_status, "Enviando capturas...");

    int sent = OnlineCrack::uploadAll();
    if (sent >= 0) {
        char buf[48];
        snprintf(buf, sizeof(buf), "Upload OK: %d arquivos", sent);
        ui_set_label(label_status, buf);
        ui_set_face(FACE_COOL);
        AudioHandler::playWav("/success_pt.wav");
        if (sent > 0) {
            Gamification::addXP(50 * sent);
        }
    } else {
        ui_set_label(label_status, "Erro de WiFi/Config");
        ui_set_face(FACE_BROKEN);
        AudioHandler::playWav("/error_pt.wav");
    }

    vTaskDelay(3000 / portTICK_PERIOD_MS);
    ui_set_face(FACE_NEUTRAL);
    ui_set_label(label_status, "Mini Lele pronto");
    vTaskDelete(nullptr);
}

// -----------------------------------------------------------------------------
// Event handlers de botões
// -----------------------------------------------------------------------------
static void event_handler_voice_btn(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        ui_set_label(label_btn, "Ouvindo...");
        xTaskCreate(voice_processing_task, "VoiceTask", 8192, nullptr, 1, nullptr);
    }
}

static void event_handler_sniffer_btn(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    if (!sniffer_active) {
        // Garante que o portal esteja parado
        if (EvilPortal::isRunning()) EvilPortal::stop();
        WiFiTools::startSniffer();
        sniffer_active = true;
        ui_set_label(label_status, "Sniffer ativo");
        lv_label_set_text(lv_obj_get_child(btn_sniffer, 0), "Parar Sniffer");
        ui_set_face(FACE_INTENSE);
    } else {
        WiFiTools::stopSniffer();
        sniffer_active = false;
        lv_label_set_text(lv_obj_get_child(btn_sniffer, 0), "Sniffer");
        ui_set_face(FACE_NEUTRAL);
        ui_set_label(label_status, "Mini Lele pronto");
    }
}

static void event_handler_portal_btn(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    if (!EvilPortal::isRunning()) {
        // Para sniffer antes de ligar o AP
        if (sniffer_active) {
            WiFiTools::stopSniffer();
            sniffer_active = false;
            if (btn_sniffer) {
                lv_label_set_text(lv_obj_get_child(btn_sniffer, 0), "Sniffer");
            }
        }
        EvilPortal::start("MiniLele_AP", "/evil_portal/03_cafe_gratis.html");
        ui_set_label(label_status, "Evil Portal ativo");
        lv_label_set_text(lv_obj_get_child(btn_portal, 0), "Parar Portal");
    } else {
        EvilPortal::stop();
        ui_set_label(label_status, "Portal parado");
        lv_label_set_text(lv_obj_get_child(btn_portal, 0), "Evil Portal");
    }
}

static void event_handler_upload_btn(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        xTaskCreate(upload_task, "UploadTask", 8192, nullptr, 1, nullptr);
    }
}

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    Serial.println("\n[Mini Lele] Boot...");

    gui_mutex = xSemaphoreCreateMutex();

    // I2C
    Wire.begin(IIC_SDA, IIC_SCL);

    // PMU AXP2101
    if (!power.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
        Serial.println("[PMU] AXP2101 nao encontrado");
    } else {
        Serial.println("[PMU] AXP2101 OK");
        power.enableBattDetection();
        power.enableVbusVoltageMeasure();
        power.enableBattVoltageMeasure();
        power.enableSystemVoltageMeasure();
        power.enableALDO1();
        power.enableALDO2();
        power.enableALDO3();
        power.enableALDO4();
        power.enableBLDO1();
        power.enableBLDO2();
    }

    // IO Expander (TCA9554)
    expander = new ESP_IOExpander_TCA95xx_8bit(
        (i2c_port_t)0,
        ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000,
        IIC_SCL,
        IIC_SDA
    );
    if (expander) {
        expander->init();
        expander->begin();
        expander->pinMode(0, OUTPUT); // LCD_RST
        expander->pinMode(1, OUTPUT); // TOUCH_RST
        expander->pinMode(2, OUTPUT); // Power gate
        expander->pinMode(6, OUTPUT); // Audio PA

        expander->digitalWrite(0, LOW);
        expander->digitalWrite(1, LOW);
        expander->digitalWrite(2, LOW);
        expander->digitalWrite(6, LOW);
        delay(50);
        expander->digitalWrite(0, HIGH);
        expander->digitalWrite(1, HIGH);
        expander->digitalWrite(2, HIGH);
        expander->digitalWrite(6, HIGH);
        Serial.println("[IOExpander] Inicializado");
    }

    // Display
    if (!gfx->begin()) {
        Serial.println("[Display] gfx->begin() falhou");
    }
    gfx->fillScreen(BLACK);
    gfx->Display_Brightness(200);

    // Touch (FT3168)
    touch = new TouchLib(Wire, IIC_SDA, IIC_SCL, FT3168_DEVICE_ADDRESS);
    if (!touch->init()) {
        Serial.println("[Touch] Falha ao iniciar FT3168");
    } else {
        Serial.println("[Touch] FT3168 OK");
    }

    // SD Card
    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
    if (!SD_MMC.begin("/sdcard", true)) {
        Serial.println("[SD] Falha ao montar SD");
    } else {
        uint64_t cardSize = SD_MMC.cardSize() / (1024ULL * 1024ULL);
        Serial.printf("[SD] Cartao: %llu MB\n", cardSize);
    }

    // Gamification & EvilPortal
    Gamification::init();
    EvilPortal::init();

    // Audio
    audio_init();

    // LVGL
    lv_init();

    uint32_t screenWidth = gfx->width();
    uint32_t screenHeight = gfx->height();

    uint32_t buf_pixels = (screenWidth * screenHeight) / 10;
    disp_buf1 = (lv_color_t *)heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_DMA);
    disp_buf2 = (lv_color_t *)heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_DMA);

    lv_disp_draw_buf_init(&draw_buf, disp_buf1, disp_buf2, buf_pixels);

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

    // -------------------------------------------------------------------------
    // UI principal
    // -------------------------------------------------------------------------
    lv_obj_t *scr = lv_scr_act();

    // Header
    lv_obj_t *header = lv_obj_create(scr);
    lv_obj_set_size(header, screenWidth - 20, 40);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x222222), 0);

    label_sys = lv_label_create(header);
    lv_label_set_text(label_sys, "Mini Lele inicializando...");
    lv_obj_align(label_sys, LV_ALIGN_LEFT_MID, 5, 0);

    label_battery = lv_label_create(header);
    lv_label_set_text(label_battery, "Bat: --%");
    lv_obj_align(label_battery, LV_ALIGN_RIGHT_MID, -5, 0);

    // Game info
    label_game = lv_label_create(scr);
    lv_label_set_text(label_game, "Lvl 1 | 0 XP");
    lv_obj_align(label_game, LV_ALIGN_TOP_MID, 0, 45);

    // Status
    label_status = lv_label_create(scr);
    lv_label_set_text(label_status, "Mini Lele pronto");
    lv_obj_align(label_status, LV_ALIGN_TOP_MID, 0, 70);
    lv_label_set_long_mode(label_status, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label_status, screenWidth - 40);
    lv_obj_set_style_text_align(label_status, LV_TEXT_ALIGN_CENTER, 0);

    // Face
    FaceHandler::init(scr);
    FaceHandler::setFace(FACE_NEUTRAL);

    // Botões da primeira linha (Sniffer / Voz / Upload)
    btn_sniffer = lv_btn_create(scr);
    lv_obj_set_size(btn_sniffer, 90, 50);
    lv_obj_align(btn_sniffer, LV_ALIGN_BOTTOM_LEFT, 10, -80);
    lv_obj_add_event_cb(btn_sniffer, event_handler_sniffer_btn, LV_EVENT_ALL, nullptr);
    lv_obj_t *lbl_snif = lv_label_create(btn_sniffer);
    lv_label_set_text(lbl_snif, "Sniffer");
    lv_obj_center(lbl_snif);

    btn_voice = lv_btn_create(scr);
    lv_obj_set_size(btn_voice, 110, 50);
    lv_obj_align(btn_voice, LV_ALIGN_BOTTOM_MID, 0, -80);
    lv_obj_add_event_cb(btn_voice, event_handler_voice_btn, LV_EVENT_ALL, nullptr);
    label_btn = lv_label_create(btn_voice);
    lv_label_set_text(label_btn, "Falar com Mini Lele");
    lv_obj_center(label_btn);

    btn_upload = lv_btn_create(scr);
    lv_obj_set_size(btn_upload, 90, 50);
    lv_obj_align(btn_upload, LV_ALIGN_BOTTOM_RIGHT, -10, -80);
    lv_obj_add_event_cb(btn_upload, event_handler_upload_btn, LV_EVENT_ALL, nullptr);
    lv_obj_t *lbl_up = lv_label_create(btn_upload);
    lv_label_set_text(lbl_up, "Upload");
    lv_obj_center(lbl_up);

    // Botão grande de Evil Portal
    btn_portal = lv_btn_create(scr);
    lv_obj_set_size(btn_portal, screenWidth - 40, 50);
    lv_obj_align(btn_portal, LV_ALIGN_BOTTOM_MID, 0, -15);
    lv_obj_set_style_bg_color(btn_portal, lv_color_hex(0xAA0000), 0);
    lv_obj_add_event_cb(btn_portal, event_handler_portal_btn, LV_EVENT_ALL, nullptr);
    lv_obj_t *lbl_portal = lv_label_create(btn_portal);
    lv_label_set_text(lbl_portal, "Evil Portal");
    lv_obj_center(lbl_portal);

    // Instruções
    label_instr = lv_label_create(scr);
    lv_label_set_text(label_instr, "Toque em \"Falar\" e diga: 1=Luz, 2=Status, 3=Bateria");
    lv_obj_align(label_instr, LV_ALIGN_BOTTOM_MID, 0, -140);
    lv_label_set_long_mode(label_instr, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label_instr, screenWidth - 40);
    lv_obj_set_style_text_align(label_instr, LV_TEXT_ALIGN_CENTER, 0);

    last_interaction = millis();
    AudioHandler::playWav("/boot_pt.wav");
}

// -----------------------------------------------------------------------------
// Loop principal
// -----------------------------------------------------------------------------
void loop() {
    // LVGL (thread-safe)
    if (gui_mutex && xSemaphoreTake(gui_mutex, portMAX_DELAY)) {
        lv_timer_handler();
        xSemaphoreGive(gui_mutex);
    } else {
        lv_timer_handler();
    }

    Gamification::tick();
    EvilPortal::loop();

    // Screensaver simples: apaga tela após 30s de inatividade
    unsigned long now = millis();
    if (screen_on && (now - last_interaction > 30000)) {
        gfx->Display_Brightness(0);
        screen_on = false;
    } else if (!screen_on && (now - last_interaction < 30000)) {
        gfx->Display_Brightness(200);
        screen_on = true;
    }

    // Atualização de UI a cada segundo
    static unsigned long last_ui = 0;
    if (now - last_ui > 1000) {
        last_ui = now;

        if (gui_mutex && xSemaphoreTake(gui_mutex, portMAX_DELAY)) {
            // System stats
            lv_label_set_text(label_sys, WiFiTools::getSystemStats().c_str());

            // Bateria
            if (power.isBatteryConnect()) {
                int pct = power.getBatteryPercent();
                char bat[32];
                if (power.isCharging()) {
                    snprintf(bat, sizeof(bat), "Bat: %d%% (C)", pct);
                } else {
                    snprintf(bat, sizeof(bat), "Bat: %d%%", pct);
                }
                lv_label_set_text(label_battery, bat);
            } else {
                lv_label_set_text(label_battery, "Bat: N/A");
            }

            // Gamificação
            String gameStr = Gamification::getLevelStr() + " | " + Gamification::getXPStr();
            lv_label_set_text(label_game, gameStr.c_str());

            // Texto do sniffer / portal
            if (EvilPortal::isRunning()) {
                char buf[48];
                snprintf(buf, sizeof(buf), "Portal ativo\nVitimas: %d",
                         EvilPortal::getCapturedCount());
                lv_label_set_text(label_status, buf);
            } else if (sniffer_active) {
                lv_label_set_text(label_status, WiFiTools::getSnifferText().c_str());
            }

            xSemaphoreGive(gui_mutex);
        }
    }

    delay(5);
}
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
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
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
#include "Gamification.h"
#include "WiFiTools.h"
#include "OnlineCrack.h"
#include "EvilPortal.h"

<<<<<<< HEAD
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
=======
#include "Gamification.h"
#include "WiFiTools.h"
#include "OnlineCrack.h"

// --- Objects ---
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
#include "XPowersLib.h"
#include <SensorQMI8658.hpp>
// WiFi/BLE Removed for Offline Mode (or kept minimal if needed, but per request "offline AI")
// We will remove WiFi includes to save compilation time/space and adhere to "Offline" strictness
#include "AudioHandler.h"
#include "OfflineVoice.h"
#include "CommandHandler.h"

// --- Objects ---
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
#include "XPowersLib.h"
#include <SensorQMI8658.hpp>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
<<<<<<< HEAD

// --- Objetos Globais ---
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
// --- Mutex for Thread Safety ---
SemaphoreHandle_t gui_mutex;

// --- Objects ---
>>>>>>> origin/waveshare-s3-amoled-review-complete
=======
#include "AudioHandler.h"

// --- Objetos Globais ---
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
XPowersPMU power;
SensorQMI8658 qmi;
IMUdata acc;

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
// --- Touch ---
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
// --- Touch (FT3168) ---
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
// --- Touch ---
>>>>>>> origin/waveshare-s3-amoled-review-complete
=======
// --- Touch (FT3168) ---
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-fix-v2
// LVGL Buffers
=======
// --- Buffers LVGL ---
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
// --- Buffers LVGL ---
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf1;
static lv_color_t *buf2;

<<<<<<< HEAD
<<<<<<< HEAD
// --- LVGL Functions ---
=======
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
// --- UI Objects ---
lv_obj_t *label_status;
lv_obj_t *label_wifi;
lv_obj_t *label_battery;
<<<<<<< HEAD

// --- Funções Auxiliares ---

>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
lv_obj_t *btn_voice;
lv_obj_t *label_btn;

// --- Funções Auxiliares ---

>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
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
<<<<<<< HEAD
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
lv_obj_t *label_instr;
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
=======
lv_obj_t *label_instr;
>>>>>>> origin/waveshare-s3-amoled-review-complete
lv_obj_t *label_game;
lv_obj_t *label_sys;
lv_obj_t *btn_tools;
lv_obj_t *btn_portal;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
lv_obj_t *btn_sniffer;

// --- Global States ---
bool sniffer_active = false;
unsigned long last_interaction = 0;
bool screen_on = true;

<<<<<<< HEAD
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
=======
lv_obj_t *label_instr;
lv_obj_t *label_game;
lv_obj_t *label_sys;
lv_obj_t *btn_tools; // New
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
lv_obj_t *label_instr;
>>>>>>> origin/waveshare-s3-amoled-offline-voice

=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
// --- Helper Functions ---
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======

>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
=======

>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-fix-v2
=======
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr

    FT3168->IIC_Interrupt_Flag = true;

    if (touchX > 0 && touchY > 0) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
    FT3168->IIC_Interrupt_Flag = true;
    if (touchX > 0 && touchY > 0) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX; data->point.y = touchY;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
        last_interaction = millis(); // Reset screensaver
>>>>>>> origin/waveshare-s3-amoled-review-complete
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
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
=======
=======
// --- Evento de Voz ---
static void event_handler_voice_btn(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        Serial.println("Botao Voz Clicado");
        lv_label_set_text(label_btn, "Ouvindo...");
        lv_timer_handler(); // Force update UI

        // 1. Play Prompt
        playWav("/listening_pt.wav");

        // 2. Record
        lv_label_set_text(label_btn, "Gravando...");
        lv_timer_handler();
        recordWav("/recording.wav", 4); // 4 seconds

        // 3. Play Ack
        lv_label_set_text(label_btn, "Processando...");
        lv_timer_handler();
        playWav("/success_pt.wav");

        lv_label_set_text(label_btn, "Falar Comando");
    }
}

>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
// --- Inicialização de Áudio ---
esp_err_t audio_init() {
  es8311_handle_t es_handle = es8311_create(0, ES8311_ADDRRES_0);
  if (!es_handle) {
      Serial.println("Erro ao criar ES8311");
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
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
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
      Serial.println("Falha na inicializacao do ES8311");
  }

  es8311_voice_volume_set(es_handle, 70, NULL);
  es8311_microphone_gain_set(es_handle, ES8311_MIC_GAIN_18DB);
  es8311_microphone_config(es_handle, false);

<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
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
<<<<<<< HEAD
<<<<<<< HEAD
=======
    .mck_io_num = MCLKPIN, // MCLK Output
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
    .mck_io_num = MCLKPIN, // MCLK Output
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
    .bck_io_num = BCLKPIN,
    .ws_io_num = WSPIN,
    .data_out_num = DOPIN,
    .data_in_num = DIPIN
  };

  if (i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) != ESP_OK) {
<<<<<<< HEAD
<<<<<<< HEAD
      Serial.println("I2S Driver Install Failed");
=======
      Serial.println("Falha ao instalar driver I2S");
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
      Serial.println("Falha ao instalar driver I2S");
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
      return ESP_FAIL;
  }

  if (i2s_set_pin(I2S_NUM_0, &pin_config) != ESP_OK) {
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
// --- Thread Safe UI Update Helpers ---
>>>>>>> origin/waveshare-s3-amoled-review-complete
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
<<<<<<< HEAD

    // Caminho ajustado: /arquivos_cartao_sd/recording.wav
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
    bool recorded = AudioHandler::recordWav("/recording.wav", 4, true);

    if (recorded) {
        ui_set_face(FACE_INTENSE);
        int syllables = OfflineVoice::analyzeCommand("/recording.wav");
        if (syllables > 0) {
<<<<<<< HEAD
=======
            // Command processing might touch hardware/SD, not UI directly usually
            // but if it does, it needs mutex?
            // CommandHandler uses playWav and FaceHandler::setFace.
            // We need to ensure CommandHandler is safe or wrap calls.
            // For now, CommandHandler calls FaceHandler::setFace which is NOT protected inside logic.
            // Let's protect the call here or inside FaceHandler.
            // BETTER: Update CommandHandler to use the new ui_set_face.
            // Since CommandHandler is a separate class, we should make FaceHandler internal mutex?
            // Or just wrap the call:

            // NOTE: CommandHandler logic mixed UI and Logic. Refactoring needed for perfection.
            // For this iteration, we accept CommandHandler will call FaceHandler.
            // Let's protect the whole block or rely on FaceHandler being fast.
            // Actually, we must protect it.
>>>>>>> origin/waveshare-s3-amoled-review-complete
            if (xSemaphoreTake(gui_mutex, portMAX_DELAY)) {
                 CommandHandler::processSyllables(syllables);
                 xSemaphoreGive(gui_mutex);
            }
<<<<<<< HEAD
=======

>>>>>>> origin/waveshare-s3-amoled-review-complete
            Gamification::addXP(10);
        } else {
            ui_set_face(FACE_BORED);
            AudioHandler::playWav("/error_pt.wav");
        }
    } else {
        ui_set_face(FACE_SAD);
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
// --- Voice Task ---
=======
// --- Voice Task (Offline) ---
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
// --- Voice Task (Offline) ---
>>>>>>> origin/waveshare-s3-amoled-manual-upload
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
<<<<<<< HEAD
            Gamification::addXP(10);
        } else {
<<<<<<< HEAD
=======
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
// --- Voice Task (Offline) ---
void voice_processing_task(void * param) {
    Serial.println("Voice Task: Iniciado (Offline)");

<<<<<<< HEAD
    // Set Listening Face
    FaceHandler::setFace(FACE_LOOK_R);

=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
    // 1. Play Prompt
    AudioHandler::playWav("/listening_pt.wav");

    // 2. Record (Smart VAD)
    bool recorded = AudioHandler::recordWav("/recording.wav", 4, true);

    if (recorded) {
<<<<<<< HEAD
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
=======
            Gamification::addXP(10);
        } else {
>>>>>>> origin/waveshare-s3-amoled-manual-upload
            FaceHandler::setFace(FACE_BORED);
            AudioHandler::playWav("/error_pt.wav");
        }
    } else {
        FaceHandler::setFace(FACE_SAD);
<<<<<<< HEAD
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
        AudioHandler::playWav("/error_pt.wav");
    }

    vTaskDelay(3000 / portTICK_PERIOD_MS);
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
    FaceHandler::setFace(FACE_NEUTRAL);
    lv_label_set_text(label_btn, "Falar Comando");
>>>>>>> origin/waveshare-s3-amoled-manual-upload

=======
        // 3. Analyze Offline
        int syllables = OfflineVoice::analyzeCommand("/recording.wav");
        if (syllables > 0) {
            CommandHandler::processSyllables(syllables);
        } else {
            Serial.println("Voice: Nenhum comando detectado");
            AudioHandler::playWav("/error_pt.wav");
        }
    } else {
        AudioHandler::playWav("/error_pt.wav");
    }

>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
    ui_set_face(FACE_NEUTRAL);
    ui_set_label(label_btn, "Falar Comando");

>>>>>>> origin/waveshare-s3-amoled-review-complete
    vTaskDelete(NULL);
}

static void event_handler_voice_btn(lv_event_t * e) {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
>>>>>>> origin/waveshare-s3-amoled-review-complete
        lv_label_set_text(label_btn, "Ouvindo...");
        xTaskCreate(voice_processing_task, "VoiceTask", 8192, NULL, 1, NULL);
    }
}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
// --- Sniffer Toggle ---
>>>>>>> origin/waveshare-s3-amoled-review-complete
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
<<<<<<< HEAD
=======
// --- Portal Toggle ---
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
=======
// --- Portal Toggle ---
>>>>>>> origin/waveshare-s3-amoled-review-complete
static void event_handler_portal_btn(lv_event_t * e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (!EvilPortal::isRunning()) {
            EvilPortal::start("Cafe_Gratis", "/evil_portal/03_cafe_gratis.html");
            lv_label_set_text(lv_obj_get_child(btn_portal, 0), "Parar Portal");
        } else {
            EvilPortal::stop();
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
            lv_label_set_text(lv_obj_get_child(btn_portal, 0), "EVIL PORTAL");
=======
            lv_label_set_text(lv_obj_get_child(btn_portal, 0), "Evil Portal");
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
            lv_label_set_text(lv_obj_get_child(btn_portal, 0), "EVIL PORTAL");
>>>>>>> origin/waveshare-s3-amoled-full-review-406
=======
            lv_label_set_text(lv_obj_get_child(btn_portal, 0), "Evil Portal");
>>>>>>> origin/waveshare-s3-amoled-review-complete
        }
    }
}

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
void upload_task(void * param) {
    ui_set_face(FACE_UPLOAD);
    ui_set_label(label_status, "Conectando e Enviando...");
=======
// --- Upload Task ---
void upload_task(void * param) {
    ui_set_face(FACE_UPLOAD);
    ui_set_label(label_status, "Enviando Cracks...");
>>>>>>> origin/waveshare-s3-amoled-review-complete

    int sent = OnlineCrack::uploadAll();

    if (sent >= 0) {
<<<<<<< HEAD
        char buf[32]; snprintf(buf, sizeof(buf), "Sucesso! Enviados: %d", sent);
=======
        char buf[32]; snprintf(buf, sizeof(buf), "Enviados: %d", sent);
>>>>>>> origin/waveshare-s3-amoled-review-complete
        ui_set_label(label_status, buf);
        ui_set_face(FACE_COOL);
        AudioHandler::playWav("/success_pt.wav");
    } else {
<<<<<<< HEAD
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
// --- Upload Task ---
void upload_task(void * param) {
    FaceHandler::setFace(FACE_UPLOAD);
    lv_label_set_text(label_status, "Enviando Cracks...");
<<<<<<< HEAD
    int sent = OnlineCrack::uploadAll();
    if (sent >= 0) {
        char buf[32]; sprintf(buf, "Enviados: %d", sent);
=======

    int sent = OnlineCrack::uploadAll();

    if (sent >= 0) {
        char buf[32];
        sprintf(buf, "Enviados: %d", sent);
>>>>>>> origin/waveshare-s3-amoled-manual-upload
        lv_label_set_text(label_status, buf);
        FaceHandler::setFace(FACE_COOL);
        AudioHandler::playWav("/success_pt.wav");
    } else {
<<<<<<< HEAD
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
=======
        lv_label_set_text(label_status, "Falha WiFi/Config");
        FaceHandler::setFace(FACE_BROKEN);
        AudioHandler::playWav("/error_pt.wav");
    }

    vTaskDelay(3000 / portTICK_PERIOD_MS);
    FaceHandler::setFace(FACE_NEUTRAL);
    lv_label_set_text(label_status, "Sistema Pronto");
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
        ui_set_label(label_status, "Falha WiFi/Config");
        ui_set_face(FACE_BROKEN);
        AudioHandler::playWav("/error_pt.wav");
    }
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    ui_set_face(FACE_NEUTRAL);
    ui_set_label(label_status, "Sistema Pronto");
>>>>>>> origin/waveshare-s3-amoled-review-complete
    vTaskDelete(NULL);
}

static void event_handler_upload_btn(lv_event_t * e) {
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
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
>>>>>>> origin/waveshare-s3-amoled-full-review-406
=======
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
>>>>>>> origin/waveshare-s3-amoled-review-complete
        xTaskCreate(upload_task, "UploadTask", 8192, NULL, 1, NULL);
    }
}

<<<<<<< HEAD
<<<<<<< HEAD
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
// --- Audio Init ---
esp_err_t audio_init() {
  es8311_handle_t es_handle = es8311_create(0, ES8311_ADDRRES_0);
  if (!es_handle) return ESP_FAIL;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete

  const es8311_clock_config_t es_clk = {
    .mclk_inverted = false, .sclk_inverted = false, .mclk_from_mclk_pin = true,
    .mclk_frequency = 16000 * 256, .sample_frequency = 16000
  };
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
  es8311_init(es_handle, &es_clk, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16);
  es8311_voice_volume_set(es_handle, 70, NULL);
  es8311_microphone_gain_set(es_handle, ES8311_MIC_GAIN_18DB);
  es8311_microphone_config(es_handle, false);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete

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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
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
=======
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
      Serial.println("Falha ao configurar pinos I2S");
      return ESP_FAIL;
  }

  Serial.println("Audio Inicializado (ES8311 + I2S)");
  return ESP_OK;
}

// --- Tarefa de WiFi ---
void wifi_scan_task(void * parameter) {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    while(1) {
        int n = WiFi.scanNetworks();
        String wifi_list = "Redes WiFi:\n";
        if (n == 0) {
            wifi_list += "Nenhuma rede encontrada";
        } else {
            for (int i = 0; i < n && i < 5; ++i) { // Show top 5
                wifi_list += String(WiFi.SSID(i)) + " (" + String(WiFi.RSSI(i)) + "dBm)\n";
            }
        }

        // Update UI (Need mutex ideally, but for demo simpler)
<<<<<<< HEAD
        lv_label_set_text(label_wifi, wifi_list.c_str());
=======
        if (label_wifi) lv_label_set_text(label_wifi, wifi_list.c_str());
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr

        vTaskDelay(10000 / portTICK_PERIOD_MS); // Scan every 10s
    }
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  Serial.println("Sistema Inicializando...");

  Wire.begin(IIC_SDA, IIC_SCL);

  // 1. PMU Init (AXP2101)
  if (!power.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
    Serial.println("PMU AXP2101 nao encontrado...");
  } else {
    Serial.println("PMU AXP2101 Inicializado");

    // Configurações de Carregamento
    power.setChargeTargetVoltage(3);
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
    power.enableBattDetection();
    power.enableVbusVoltageMeasure();
    power.enableBattVoltageMeasure();
    power.enableSystemVoltageMeasure();

<<<<<<< HEAD
<<<<<<< HEAD
    // IMPORTANT: Enable voltages for Screen/Peripherals
    // The demo doesn't explicitly enable DCDC/ALDOs, implying defaults are ON or controlled via Expander?
    // However, it's safer to ensure they are on if we knew which ones.
    // For now, we trust the demo which relies on Expander for power gates.
  }

  // Init IO Expander (TCA9554)
  // Demo uses 0x20.
>>>>>>> origin/waveshare-s3-amoled-fix-v2
=======
    // Habilitar Rails de Energia (Crucial para Display e Perifericos)
    power.enableALDO1(); // Display logic?
    power.enableALDO2(); // Touch?
    power.enableALDO3(); // Audio?
    power.enableALDO4();
    power.enableBLDO1();
    power.enableBLDO2();

    // Definir voltagens (padroes costumam ser seguros, mas 3.3V eh tipico para perifericos)
    // power.setALDO1Voltage(3300);
    // ...
  }

  // 2. IO Expander Init (TCA9554)
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
    // Habilitar Rails de Energia
    power.enableALDO1();
    power.enableALDO2();
    power.enableALDO3();
    power.enableALDO4();
    power.enableBLDO1();
    power.enableBLDO2();
  }

  // 2. IO Expander Init (TCA9554)
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
  expander = new ESP_IOExpander_TCA95xx_8bit((i2c_port_t)0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, IIC_SCL, IIC_SDA);
  if(expander) {
      expander->init();
      expander->begin();
<<<<<<< HEAD
<<<<<<< HEAD

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

=======
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
      expander->pinMode(0, OUTPUT);
      expander->pinMode(1, OUTPUT);
      expander->pinMode(2, OUTPUT);
      expander->pinMode(6, OUTPUT);

<<<<<<< HEAD
<<<<<<< HEAD
      // Reset/Power Cycle Sequence
=======
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
      expander->digitalWrite(0, LOW);
      expander->digitalWrite(1, LOW);
      expander->digitalWrite(2, LOW);
      expander->digitalWrite(6, LOW);
<<<<<<< HEAD
<<<<<<< HEAD
      delay(50); // Increased delay
=======
      delay(50);
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
      delay(50);
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
      expander->digitalWrite(0, HIGH);
      expander->digitalWrite(1, HIGH);
      expander->digitalWrite(2, HIGH);
      expander->digitalWrite(6, HIGH);
<<<<<<< HEAD
<<<<<<< HEAD

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
=======
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
      Serial.println("IO Expander Inicializado");
  } else {
      Serial.println("Falha no IO Expander");
  }

  // 3. IMU Init
  if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
    Serial.println("Falha ao encontrar QMI8658");
  } else {
    qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, SensorQMI8658::ACC_ODR_1000Hz, SensorQMI8658::LPF_MODE_0);
    qmi.enableAccelerometer();
    Serial.println("IMU QMI8658 Inicializado");
  }

  // 4. Display Init
  if (!gfx->begin()) {
    Serial.println("Falha gfx->begin()");
  }
  gfx->fillScreen(BLACK);
  gfx->Display_Brightness(200);
  Serial.println("Display Inicializado");

  // 5. Touch Init
  while (FT3168->begin() == false) {
    Serial.println("Falha FT3168");
    delay(1000);
  }
  Serial.println("Touch Inicializado");
  FT3168->IIC_Write_Device_State(FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE,
                                 FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_MONITOR);

  // 6. SD Card Init
  SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
  String sd_status = "";
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("Falha ao montar cartao SD");
    sd_status = "Cartao SD: Nao Detectado";
  } else {
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("Cartao SD: %lluMB\n", cardSize);
    sd_status = "Cartao SD: " + String(cardSize) + " MB";
  }

  // 7. Audio Init
  audio_init();

  // 8. Bluetooth Init
  BLEDevice::init("ESP32-S3-AMOLED-BR");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService("12345678-1234-1234-1234-123456789012");
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID("12345678-1234-1234-1234-123456789012");
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Bluetooth Iniciado");

  // 9. LVGL Init
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
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

<<<<<<< HEAD
<<<<<<< HEAD
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
=======
  Wire.begin(IIC_SDA, IIC_SCL);

  // PMU
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
  Wire.begin(IIC_SDA, IIC_SCL);

  // PMU
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
  gui_mutex = xSemaphoreCreateMutex();

  Wire.begin(IIC_SDA, IIC_SCL);

  // PMU
>>>>>>> origin/waveshare-s3-amoled-review-complete
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
=======
  // IO Expander
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
  // IO Expander
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
  // IO Expander
>>>>>>> origin/waveshare-s3-amoled-review-complete
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
=======
  // IMU
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
  // IMU
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
  // IMU
>>>>>>> origin/waveshare-s3-amoled-review-complete
  qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
  qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, SensorQMI8658::ACC_ODR_1000Hz, SensorQMI8658::LPF_MODE_0);
  qmi.enableAccelerometer();

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
  // Display
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
  // Display
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
  // Display
>>>>>>> origin/waveshare-s3-amoled-review-complete
  gfx->begin();
  gfx->fillScreen(BLACK);
  gfx->Display_Brightness(200);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
  // Touch
  FT3168->begin();
  FT3168->IIC_Write_Device_State(FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE, FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_MONITOR);

  // SD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
  SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
  SD_MMC.begin("/sdcard", true);

  Gamification::init();
<<<<<<< HEAD
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
=======

>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
  SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
  SD_MMC.begin("/sdcard", true);

>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
  SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
  if (!SD_MMC.begin("/sdcard", true)) {
      Serial.println("SD Card Failed!");
      // UI fallback?
  }

  Gamification::init();
  EvilPortal::init();

>>>>>>> origin/waveshare-s3-amoled-review-complete
  // Audio
  audio_init();

  // LVGL
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
  // --- UI Layout ---
  lv_obj_t *scr = lv_scr_act();

  // Dashboard Header
<<<<<<< HEAD
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
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
  // --- UI Layout ---
  // Note: No mutex needed here as no tasks are running yet
  lv_obj_t *scr = lv_scr_act();

  // Dashboard Header
>>>>>>> origin/waveshare-s3-amoled-review-complete
  lv_obj_t *header_cont = lv_obj_create(scr);
  lv_obj_set_size(header_cont, 360, 50);
  lv_obj_align(header_cont, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_color(header_cont, lv_color_hex(0x222222), 0);

  label_sys = lv_label_create(header_cont);
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
  lv_label_set_text(label_sys, "Init...");
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
  lv_label_set_text(label_sys, "Init...");
>>>>>>> origin/waveshare-s3-amoled-review-complete
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
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  label_game = lv_label_create(scr);
  lv_label_set_text(label_game, "Nvl 1 | 0 XP");
  lv_obj_align(label_game, LV_ALIGN_TOP_MID, 0, 55);

  label_status = lv_label_create(scr);
  lv_label_set_text(label_status, "Sistema Pronto");
=======
  // Game Stats
  label_game = lv_label_create(scr);
  lv_label_set_text(label_game, "Lvl 1 | 0 XP");
  lv_obj_align(label_game, LV_ALIGN_TOP_MID, 0, 55);

  // Status
  label_status = lv_label_create(scr);
  lv_label_set_text(label_status, "Pronto");
>>>>>>> origin/waveshare-s3-amoled-review-complete
  lv_obj_align(label_status, LV_ALIGN_TOP_MID, 0, 80);
  lv_label_set_long_mode(label_status, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(label_status, 300);
  lv_obj_set_style_text_align(label_status, LV_TEXT_ALIGN_CENTER, 0);

<<<<<<< HEAD
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
=======
  // Game Stats
>>>>>>> origin/waveshare-s3-amoled-manual-upload
  label_game = lv_label_create(scr);
  lv_label_set_text(label_game, "Lvl 1 | 0 XP");
  lv_obj_align(label_game, LV_ALIGN_TOP_MID, 0, 55);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
  // System Status Msg
  label_status = lv_label_create(scr);
  lv_label_set_text(label_status, "Pronto");
  lv_obj_align(label_status, LV_ALIGN_TOP_MID, 0, 80);
<<<<<<< HEAD
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
=======

>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
  // Face
  FaceHandler::init(scr);
  FaceHandler::setFace(FACE_NEUTRAL);

<<<<<<< HEAD
<<<<<<< HEAD
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
=======
  // Voice Button
  btn_voice = lv_btn_create(scr);
  lv_obj_align(btn_voice, LV_ALIGN_BOTTOM_LEFT, 20, -20);
  lv_obj_set_size(btn_voice, 140, 60);
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
  // Buttons
  btn_voice = lv_btn_create(scr);
  lv_obj_align(btn_voice, LV_ALIGN_BOTTOM_LEFT, 10, -80);
  lv_obj_set_size(btn_voice, 110, 60);
>>>>>>> origin/waveshare-s3-amoled-review-complete
  lv_obj_add_event_cb(btn_voice, event_handler_voice_btn, LV_EVENT_ALL, NULL);
  label_btn = lv_label_create(btn_voice);
  lv_label_set_text(label_btn, "Voz");
  lv_obj_center(label_btn);

<<<<<<< HEAD
<<<<<<< HEAD
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
=======
  // Upload Button
  btn_tools = lv_btn_create(scr);
  lv_obj_align(btn_tools, LV_ALIGN_BOTTOM_RIGHT, -20, -20);
  lv_obj_set_size(btn_tools, 140, 60);
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
  btn_tools = lv_btn_create(scr);
  lv_obj_align(btn_tools, LV_ALIGN_BOTTOM_RIGHT, -10, -80);
  lv_obj_set_size(btn_tools, 110, 60);
>>>>>>> origin/waveshare-s3-amoled-review-complete
  lv_obj_add_event_cb(btn_tools, event_handler_upload_btn, LV_EVENT_ALL, NULL);
  lv_obj_t *label_tool = lv_label_create(btn_tools);
  lv_label_set_text(label_tool, "Upload");
  lv_obj_center(label_tool);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
  // New: Sniffer Btn (Center Row 1)
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
  btn_sniffer = lv_btn_create(scr);
  lv_obj_align(btn_sniffer, LV_ALIGN_BOTTOM_MID, 0, -80);
  lv_obj_set_size(btn_sniffer, 110, 60);
  lv_obj_add_event_cb(btn_sniffer, event_handler_sniffer_btn, LV_EVENT_ALL, NULL);
  lv_obj_t *label_snif = lv_label_create(btn_sniffer);
  lv_label_set_text(label_snif, "Sniffer");
  lv_obj_center(label_snif);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
  // Button Row 2 (Portal)
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
  btn_portal = lv_btn_create(scr);
  lv_obj_align(btn_portal, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_set_size(btn_portal, 340, 60);
  lv_obj_set_style_bg_color(btn_portal, lv_color_hex(0xFF0000), 0);
<<<<<<< HEAD
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
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
  lv_obj_add_event_cb(btn_portal, event_handler_portal_btn, LV_EVENT_ALL, NULL);
  lv_obj_t *label_portal = lv_label_create(btn_portal);
  lv_label_set_text(label_portal, "EVIL PORTAL");
  lv_obj_center(label_portal);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  last_interaction = millis();
=======
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
  // UI
  lv_obj_t *scr = lv_scr_act();
  lv_obj_t *label_title = lv_label_create(scr);
  lv_label_set_text(label_title, "Waveshare Offline");
  lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 10);

  label_status = lv_label_create(scr);
<<<<<<< HEAD
  lv_label_set_text(label_status, "Sistema Pronto");
=======
  lv_label_set_text(label_status, "Sistema Pronto (Offline)");
>>>>>>> origin/waveshare-s3-amoled-offline-voice
  lv_obj_align(label_status, LV_ALIGN_TOP_LEFT, 10, 40);

  label_battery = lv_label_create(scr);
  lv_label_set_text(label_battery, "Bat: --%");
  lv_obj_align(label_battery, LV_ALIGN_TOP_RIGHT, -10, 40);

<<<<<<< HEAD
=======
  // Voice Button
>>>>>>> origin/waveshare-s3-amoled-full-plugins
  btn_voice = lv_btn_create(scr);
  lv_obj_align(btn_voice, LV_ALIGN_BOTTOM_MID, 0, -60);
  lv_obj_set_size(btn_voice, 200, 50);
=======
  btn_voice = lv_btn_create(scr);
  lv_obj_align(btn_voice, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_size(btn_voice, 200, 60);
>>>>>>> origin/waveshare-s3-amoled-offline-voice
  lv_obj_add_event_cb(btn_voice, event_handler_voice_btn, LV_EVENT_ALL, NULL);
  label_btn = lv_label_create(btn_voice);
  lv_label_set_text(label_btn, "Falar Comando");
  lv_obj_center(label_btn);

<<<<<<< HEAD
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
=======
  // Instructions
  label_instr = lv_label_create(scr);
  lv_label_set_text(label_instr, "Offline AI Mode");
  lv_obj_align(label_instr, LV_ALIGN_BOTTOM_MID, 0, -90);

>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
  label_instr = lv_label_create(scr);
  lv_label_set_text(label_instr, "Diga: 'Luz' (1), 'Status' (2)\nou 'Bateria' (3)");
  lv_obj_align(label_instr, LV_ALIGN_BOTTOM_MID, 0, -30);

>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
  last_interaction = millis();
>>>>>>> origin/waveshare-s3-amoled-review-complete
  AudioHandler::playWav("/boot_pt.wav");
}

void loop() {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
=======
  // --- LVGL Thread Safety Lock ---
>>>>>>> origin/waveshare-s3-amoled-review-complete
  if (xSemaphoreTake(gui_mutex, portMAX_DELAY)) {
      lv_timer_handler();
      xSemaphoreGive(gui_mutex);
  }

  Gamification::tick();
  EvilPortal::loop();

<<<<<<< HEAD
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
=======
  lv_timer_handler();
  Gamification::tick();
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
  // --- Power Management (Screen Saver) ---
  if (screen_on && (millis() - last_interaction > 30000)) { // 30s timeout
      gfx->Display_Brightness(0);
      screen_on = false;
      Serial.println("Screen Saver: Active");
  } else if (!screen_on && (millis() - last_interaction < 30000)) {
      gfx->Display_Brightness(200);
      screen_on = true;
      Serial.println("Screen Saver: Wake");
  }
>>>>>>> origin/waveshare-s3-amoled-review-complete

  static uint32_t last_ui_update = 0;
  if (millis() - last_ui_update > 1000) {
      last_ui_update = millis();
<<<<<<< HEAD
<<<<<<< HEAD

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
=======

      // Protect string generation and UI update
>>>>>>> origin/waveshare-s3-amoled-review-complete
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
=======
      lv_label_set_text(label_sys, WiFiTools::getSystemStats().c_str());
>>>>>>> origin/waveshare-s3-amoled-manual-upload
      if (power.isBatteryConnect()) {
          int bat_pct = power.getBatteryPercent();
          String bat_str = String(bat_pct) + "% " + String(power.getBattVoltage()) + "mV";
          if (power.isCharging()) bat_str += " (C)";
          lv_label_set_text(label_battery, bat_str.c_str());
      }
<<<<<<< HEAD
<<<<<<< HEAD
      String gameStr = Gamification::getLevelStr() + " | " + Gamification::getXPStr();
=======

      // Update Game
      String gameStr = Gamification::getLevelStr() + " | " + Gamification::getXPStr() + "\n" + Gamification::getAgeStr();
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
      String gameStr = Gamification::getLevelStr() + " | " + Gamification::getXPStr();
>>>>>>> origin/waveshare-s3-amoled-manual-upload
      lv_label_set_text(label_game, gameStr.c_str());
  }

  delay(5);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
  lv_timer_handler();
  if (power.isBatteryConnect()) {
      int bat_pct = power.getBatteryPercent();
      String bat_str = "Bat: " + String(bat_pct) + "%";
      if (power.isCharging()) bat_str += " (C)";
      lv_label_set_text(label_battery, bat_str.c_str());
  }
  delay(5);
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-faces-offline
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-fix-v2
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice
=======
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
  // --- UI Layout (Portuguese) ---
  lv_obj_t *scr = lv_scr_act();

  // Titulo
  lv_obj_t *label_title = lv_label_create(scr);
  lv_label_set_text(label_title, "Waveshare ESP32-S3");
  lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 10);

  // Status Geral
  label_status = lv_label_create(scr);
  lv_label_set_text(label_status, ("Status:\n" + sd_status + "\nAudio: OK\nBluetooth: Ativo").c_str());
  lv_obj_align(label_status, LV_ALIGN_TOP_LEFT, 10, 40);

  // Bateria
  label_battery = lv_label_create(scr);
  lv_label_set_text(label_battery, "Bateria: --%");
  lv_obj_align(label_battery, LV_ALIGN_TOP_RIGHT, -10, 40);

<<<<<<< HEAD
  // WiFi Info
  label_wifi = lv_label_create(scr);
  lv_label_set_text(label_wifi, "Escaneando WiFi...");
  lv_obj_align(label_wifi, LV_ALIGN_CENTER, 0, 40);
=======
  // Botao de Voz
  btn_voice = lv_btn_create(scr);
  lv_obj_align(btn_voice, LV_ALIGN_CENTER, 0, 20);
  lv_obj_set_size(btn_voice, 200, 60);
  lv_obj_add_event_cb(btn_voice, event_handler_voice_btn, LV_EVENT_ALL, NULL);

  label_btn = lv_label_create(btn_voice);
  lv_label_set_text(label_btn, "Falar Comando");
  lv_obj_center(label_btn);

  // WiFi Info
  label_wifi = lv_label_create(scr);
  lv_label_set_text(label_wifi, "Escaneando WiFi...");
  lv_obj_align(label_wifi, LV_ALIGN_BOTTOM_MID, 0, -20);
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr

  // Start WiFi Task
  xTaskCreate(wifi_scan_task, "WiFiScan", 4096, NULL, 1, NULL);

<<<<<<< HEAD
=======
  // Play Boot Sound
  playWav("/boot_pt.wav");

>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
  Serial.println("Setup Completo - UI em Portugues");
}

void loop() {
  lv_timer_handler();

  // Atualizar Bateria
  if (power.isBatteryConnect()) {
      int bat_pct = power.getBatteryPercent();
      String bat_str = "Bateria: " + String(bat_pct) + "%";
      if (power.isCharging()) bat_str += " (C)";
      lv_label_set_text(label_battery, bat_str.c_str());
  } else {
      lv_label_set_text(label_battery, "Bateria: N/A");
  }

  delay(5);
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-ptbr-final
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
=======
>>>>>>> origin/waveshare-s3-amoled-voice-ptbr
}
