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
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

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
            FaceHandler::setFace(FACE_BORED);
            AudioHandler::playWav("/error_pt.wav");
        }
    } else {
        FaceHandler::setFace(FACE_SAD);
        AudioHandler::playWav("/error_pt.wav");
    }

    // Return to default after a while?
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    FaceHandler::setFace(FACE_NEUTRAL);
    lv_label_set_text(label_btn, "Falar Comando");

    vTaskDelete(NULL);
}

static void event_handler_voice_btn(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        lv_label_set_text(label_btn, "Ouvindo...");
        xTaskCreate(voice_processing_task, "VoiceTask", 8192, NULL, 1, NULL);
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
  SD_MMC.begin("/sdcard", true);

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

  btn_voice = lv_btn_create(scr);
  lv_obj_align(btn_voice, LV_ALIGN_BOTTOM_MID, 0, -60);
  lv_obj_set_size(btn_voice, 200, 50);
  lv_obj_add_event_cb(btn_voice, event_handler_voice_btn, LV_EVENT_ALL, NULL);
  label_btn = lv_label_create(btn_voice);
  lv_label_set_text(label_btn, "Falar Comando");
  lv_obj_center(label_btn);

  label_instr = lv_label_create(scr);
  lv_label_set_text(label_instr, "1: Luz | 2: Status | 3: Bat");
  lv_obj_align(label_instr, LV_ALIGN_BOTTOM_MID, 0, -10);

  // Init Face
  FaceHandler::init(scr);
  FaceHandler::setFace(FACE_NEUTRAL);

  AudioHandler::playWav("/boot_pt.wav");
}

void loop() {
  lv_timer_handler();
  if (power.isBatteryConnect()) {
      int bat_pct = power.getBatteryPercent();
      String bat_str = "Bat: " + String(bat_pct) + "%";
      if (power.isCharging()) bat_str += " (C)";
      lv_label_set_text(label_battery, bat_str.c_str());
  }
  delay(5);
}
