#include <Arduino.h>
#include <lvgl.h>
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include <ESP_IOExpander_Library.h>
#include "pin_config.h"
#include <Wire.h>
#include <SD_MMC.h>
#include "es8311.h"
#include <driver/i2s.h>
#include "XPowersLib.h"
#include <SensorQMI8658.hpp>
#include "AudioHandler.h"
#include "OfflineVoice.h"
#include "CommandHandler.h"
#include "FaceHandler.h"
#include "Gamification.h"
#include "WiFiTools.h"
#include "OnlineCrack.h"
#include "EvilPortal.h"

// --- Mutex for Thread Safety ---
SemaphoreHandle_t gui_mutex;

// --- Objects ---
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
  LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */, LCD_SDIO1 /* SDIO1 */,
  LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST /* RST (-1) */,
                                      0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
  std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

ESP_IOExpander *expander = NULL;
XPowersPMU power;
SensorQMI8658 qmi;
IMUdata acc;

// --- Touch ---
void Arduino_IIC_Touch_Interrupt(void);
std::unique_ptr<Arduino_IIC> FT3168(new Arduino_FT3x68(IIC_Bus, FT3168_DEVICE_ADDRESS,
                                                       DRIVEBUS_DEFAULT_VALUE, TP_INT, Arduino_IIC_Touch_Interrupt));

void Arduino_IIC_Touch_Interrupt(void) {
  FT3168->IIC_Interrupt_Flag = true;
}

// --- UI Objects ---
lv_obj_t *label_status;
lv_obj_t *label_battery;
lv_obj_t *btn_voice;
lv_obj_t *label_btn;
lv_obj_t *label_instr;
lv_obj_t *label_game;
lv_obj_t *label_sys;
lv_obj_t *btn_tools;
lv_obj_t *btn_portal;
lv_obj_t *btn_sniffer;

// --- Global States ---
bool sniffer_active = false;
unsigned long last_interaction = 0;
bool screen_on = true;

// --- Helper Functions ---
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif
  lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    int32_t touchX = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
    int32_t touchY = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
    FT3168->IIC_Interrupt_Flag = true;
    if (touchX > 0 && touchY > 0) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX; data->point.y = touchY;
        last_interaction = millis(); // Reset screensaver
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

// --- Thread Safe UI Update Helpers ---
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
    bool recorded = AudioHandler::recordWav("/recording.wav", 4, true);

    if (recorded) {
        ui_set_face(FACE_INTENSE);
        int syllables = OfflineVoice::analyzeCommand("/recording.wav");
        if (syllables > 0) {
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
    ui_set_label(label_btn, "Falar Comando");

    vTaskDelete(NULL);
}

static void event_handler_voice_btn(lv_event_t * e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_label_set_text(label_btn, "Ouvindo...");
        xTaskCreate(voice_processing_task, "VoiceTask", 8192, NULL, 1, NULL);
    }
}

// --- Sniffer Toggle ---
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

// --- Portal Toggle ---
static void event_handler_portal_btn(lv_event_t * e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if (!EvilPortal::isRunning()) {
            EvilPortal::start("Cafe_Gratis", "/evil_portal/03_cafe_gratis.html");
            lv_label_set_text(lv_obj_get_child(btn_portal, 0), "Parar Portal");
        } else {
            EvilPortal::stop();
            lv_label_set_text(lv_obj_get_child(btn_portal, 0), "Evil Portal");
        }
    }
}

// --- Upload Task ---
void upload_task(void * param) {
    ui_set_face(FACE_UPLOAD);
    ui_set_label(label_status, "Enviando Cracks...");

    int sent = OnlineCrack::uploadAll();

    if (sent >= 0) {
        char buf[32]; snprintf(buf, sizeof(buf), "Enviados: %d", sent);
        ui_set_label(label_status, buf);
        ui_set_face(FACE_COOL);
        AudioHandler::playWav("/success_pt.wav");
    } else {
        ui_set_label(label_status, "Falha WiFi/Config");
        ui_set_face(FACE_BROKEN);
        AudioHandler::playWav("/error_pt.wav");
    }
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    ui_set_face(FACE_NEUTRAL);
    ui_set_label(label_status, "Sistema Pronto");
    vTaskDelete(NULL);
}

static void event_handler_upload_btn(lv_event_t * e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        xTaskCreate(upload_task, "UploadTask", 8192, NULL, 1, NULL);
    }
}

// --- Audio Init ---
esp_err_t audio_init() {
  es8311_handle_t es_handle = es8311_create(0, ES8311_ADDRRES_0);
  if (!es_handle) return ESP_FAIL;

  const es8311_clock_config_t es_clk = {
    .mclk_inverted = false, .sclk_inverted = false, .mclk_from_mclk_pin = true,
    .mclk_frequency = 16000 * 256, .sample_frequency = 16000
  };
  es8311_init(es_handle, &es_clk, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16);
  es8311_voice_volume_set(es_handle, 70, NULL);
  es8311_microphone_gain_set(es_handle, ES8311_MIC_GAIN_18DB);
  es8311_microphone_config(es_handle, false);

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

  return ESP_OK;
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  gui_mutex = xSemaphoreCreateMutex();

  Wire.begin(IIC_SDA, IIC_SCL);

  // PMU
  power.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
  power.setChargeTargetVoltage(3);
  power.enableBattDetection();
  power.enableVbusVoltageMeasure();
  power.enableBattVoltageMeasure();
  power.enableSystemVoltageMeasure();
  power.enableALDO1(); power.enableALDO2(); power.enableALDO3(); power.enableALDO4();
  power.enableBLDO1(); power.enableBLDO2();

  // IO Expander
  expander = new ESP_IOExpander_TCA95xx_8bit((i2c_port_t)0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, IIC_SCL, IIC_SDA);
  if(expander) {
      expander->init(); expander->begin();
      expander->pinMode(0, OUTPUT); expander->pinMode(1, OUTPUT); expander->pinMode(2, OUTPUT); expander->pinMode(6, OUTPUT);
      expander->digitalWrite(0, LOW); expander->digitalWrite(1, LOW); expander->digitalWrite(2, LOW); expander->digitalWrite(6, LOW);
      delay(50);
      expander->digitalWrite(0, HIGH); expander->digitalWrite(1, HIGH); expander->digitalWrite(2, HIGH); expander->digitalWrite(6, HIGH);
  }

  // IMU
  qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
  qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, SensorQMI8658::ACC_ODR_1000Hz, SensorQMI8658::LPF_MODE_0);
  qmi.enableAccelerometer();

  // Display
  gfx->begin();
  gfx->fillScreen(BLACK);
  gfx->Display_Brightness(200);

  // Touch
  FT3168->begin();
  FT3168->IIC_Write_Device_State(FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE, FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_MONITOR);

  // SD
  SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
  if (!SD_MMC.begin("/sdcard", true)) {
      Serial.println("SD Card Failed!");
      // UI fallback?
  }

  Gamification::init();
  EvilPortal::init();

  // Audio
  audio_init();

  // LVGL
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

  // --- UI Layout ---
  // Note: No mutex needed here as no tasks are running yet
  lv_obj_t *scr = lv_scr_act();

  // Dashboard Header
  lv_obj_t *header_cont = lv_obj_create(scr);
  lv_obj_set_size(header_cont, 360, 50);
  lv_obj_align(header_cont, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_color(header_cont, lv_color_hex(0x222222), 0);

  label_sys = lv_label_create(header_cont);
  lv_label_set_text(label_sys, "Init...");
  lv_obj_align(label_sys, LV_ALIGN_LEFT_MID, 5, 0);
  lv_obj_set_style_text_color(label_sys, lv_color_white(), 0);

  label_battery = lv_label_create(header_cont);
  lv_label_set_text(label_battery, "Bat: --%");
  lv_obj_align(label_battery, LV_ALIGN_RIGHT_MID, -5, 0);
  lv_obj_set_style_text_color(label_battery, lv_color_white(), 0);

  // Game Stats
  label_game = lv_label_create(scr);
  lv_label_set_text(label_game, "Lvl 1 | 0 XP");
  lv_obj_align(label_game, LV_ALIGN_TOP_MID, 0, 55);

  // Status
  label_status = lv_label_create(scr);
  lv_label_set_text(label_status, "Pronto");
  lv_obj_align(label_status, LV_ALIGN_TOP_MID, 0, 80);
  lv_label_set_long_mode(label_status, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(label_status, 300);
  lv_obj_set_style_text_align(label_status, LV_TEXT_ALIGN_CENTER, 0);

  // Face
  FaceHandler::init(scr);
  FaceHandler::setFace(FACE_NEUTRAL);

  // Buttons
  btn_voice = lv_btn_create(scr);
  lv_obj_align(btn_voice, LV_ALIGN_BOTTOM_LEFT, 10, -80);
  lv_obj_set_size(btn_voice, 110, 60);
  lv_obj_add_event_cb(btn_voice, event_handler_voice_btn, LV_EVENT_ALL, NULL);
  label_btn = lv_label_create(btn_voice);
  lv_label_set_text(label_btn, "Voz");
  lv_obj_center(label_btn);

  btn_tools = lv_btn_create(scr);
  lv_obj_align(btn_tools, LV_ALIGN_BOTTOM_RIGHT, -10, -80);
  lv_obj_set_size(btn_tools, 110, 60);
  lv_obj_add_event_cb(btn_tools, event_handler_upload_btn, LV_EVENT_ALL, NULL);
  lv_obj_t *label_tool = lv_label_create(btn_tools);
  lv_label_set_text(label_tool, "Upload");
  lv_obj_center(label_tool);

  btn_sniffer = lv_btn_create(scr);
  lv_obj_align(btn_sniffer, LV_ALIGN_BOTTOM_MID, 0, -80);
  lv_obj_set_size(btn_sniffer, 110, 60);
  lv_obj_add_event_cb(btn_sniffer, event_handler_sniffer_btn, LV_EVENT_ALL, NULL);
  lv_obj_t *label_snif = lv_label_create(btn_sniffer);
  lv_label_set_text(label_snif, "Sniffer");
  lv_obj_center(label_snif);

  btn_portal = lv_btn_create(scr);
  lv_obj_align(btn_portal, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_set_size(btn_portal, 340, 60);
  lv_obj_set_style_bg_color(btn_portal, lv_color_hex(0xFF0000), 0);
  lv_obj_add_event_cb(btn_portal, event_handler_portal_btn, LV_EVENT_ALL, NULL);
  lv_obj_t *label_portal = lv_label_create(btn_portal);
  lv_label_set_text(label_portal, "EVIL PORTAL");
  lv_obj_center(label_portal);

  last_interaction = millis();
  AudioHandler::playWav("/boot_pt.wav");
}

void loop() {
  // --- LVGL Thread Safety Lock ---
  if (xSemaphoreTake(gui_mutex, portMAX_DELAY)) {
      lv_timer_handler();
      xSemaphoreGive(gui_mutex);
  }

  Gamification::tick();
  EvilPortal::loop();

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

  static uint32_t last_ui_update = 0;
  if (millis() - last_ui_update > 1000) {
      last_ui_update = millis();

      // Protect string generation and UI update
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
}
