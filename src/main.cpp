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

// --- Global Objects ---
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
  LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */, LCD_SDIO1 /* SDIO1 */,
  LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx = new Arduino_SH8601(bus, LCD_RST /* RST (-1) */,
                                      0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
  std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

ESP_IOExpander *expander = NULL;
XPowersPMU power;

// Touch (FT3168)
void Arduino_IIC_Touch_Interrupt(void);
std::unique_ptr<Arduino_IIC> FT3168(new Arduino_FT3x68(IIC_Bus, FT3168_DEVICE_ADDRESS,
                                                       DRIVEBUS_DEFAULT_VALUE, TP_INT, Arduino_IIC_Touch_Interrupt));

void Arduino_IIC_Touch_Interrupt(void) {
  FT3168->IIC_Interrupt_Flag = true;
}

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
        data->point.x = touchX;
        data->point.y = touchY;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

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
      // Don't return fail yet, maybe just config issue, allow I2S setup
  }

  es8311_voice_volume_set(es_handle, 70, NULL);
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

  // Enable MCLK
  PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[MCLKPIN], PIN_FUNC_GPIO);
  gpio_set_direction((gpio_num_t)MCLKPIN, GPIO_MODE_OUTPUT);

  Serial.println("Audio Initialized (ES8311 + I2S)");
  return ESP_OK;
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  Serial.println("System Initializing...");

  // Init I2C
  Wire.begin(IIC_SDA, IIC_SCL);

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
  expander = new ESP_IOExpander_TCA95xx_8bit((i2c_port_t)0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, IIC_SCL, IIC_SDA);
  if(expander) {
      expander->init();
      expander->begin();

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
  } else {
      Serial.println("IO Expander Init Failed");
  }

  // Init Display
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
  lv_label_set_text(label, "Hello Waveshare!\nESP32-S3 AMOLED\nAudio & SD Ready\nPMU: Active");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  Serial.println("Setup Done");
}

void loop() {
  lv_timer_handler();
  delay(5);
}
