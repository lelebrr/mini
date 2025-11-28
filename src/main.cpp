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
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// --- Objetos Globais ---
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

// --- Touch (FT3168) ---
void Arduino_IIC_Touch_Interrupt(void);
std::unique_ptr<Arduino_IIC> FT3168(new Arduino_FT3x68(IIC_Bus, FT3168_DEVICE_ADDRESS,
                                                       DRIVEBUS_DEFAULT_VALUE, TP_INT, Arduino_IIC_Touch_Interrupt));

void Arduino_IIC_Touch_Interrupt(void) {
  FT3168->IIC_Interrupt_Flag = true;
}

// --- Buffers LVGL ---
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf1;
static lv_color_t *buf2;

// --- UI Objects ---
lv_obj_t *label_status;
lv_obj_t *label_wifi;
lv_obj_t *label_battery;

// --- Funções Auxiliares ---

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

// --- Inicialização de Áudio ---
esp_err_t audio_init() {
  es8311_handle_t es_handle = es8311_create(0, ES8311_ADDRRES_0);
  if (!es_handle) {
      Serial.println("Erro ao criar ES8311");
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
      Serial.println("Falha na inicializacao do ES8311");
  }

  es8311_voice_volume_set(es_handle, 70, NULL);
  es8311_microphone_gain_set(es_handle, ES8311_MIC_GAIN_18DB);
  es8311_microphone_config(es_handle, false);

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
    .mck_io_num = MCLKPIN, // MCLK Output
    .bck_io_num = BCLKPIN,
    .ws_io_num = WSPIN,
    .data_out_num = DOPIN,
    .data_in_num = DIPIN
  };

  if (i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) != ESP_OK) {
      Serial.println("Falha ao instalar driver I2S");
      return ESP_FAIL;
  }

  if (i2s_set_pin(I2S_NUM_0, &pin_config) != ESP_OK) {
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
        lv_label_set_text(label_wifi, wifi_list.c_str());

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
    power.enableBattDetection();
    power.enableVbusVoltageMeasure();
    power.enableBattVoltageMeasure();
    power.enableSystemVoltageMeasure();

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
  expander = new ESP_IOExpander_TCA95xx_8bit((i2c_port_t)0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, IIC_SCL, IIC_SDA);
  if(expander) {
      expander->init();
      expander->begin();
      expander->pinMode(0, OUTPUT);
      expander->pinMode(1, OUTPUT);
      expander->pinMode(2, OUTPUT);
      expander->pinMode(6, OUTPUT);

      expander->digitalWrite(0, LOW);
      expander->digitalWrite(1, LOW);
      expander->digitalWrite(2, LOW);
      expander->digitalWrite(6, LOW);
      delay(50);
      expander->digitalWrite(0, HIGH);
      expander->digitalWrite(1, HIGH);
      expander->digitalWrite(2, HIGH);
      expander->digitalWrite(6, HIGH);
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

  // WiFi Info
  label_wifi = lv_label_create(scr);
  lv_label_set_text(label_wifi, "Escaneando WiFi...");
  lv_obj_align(label_wifi, LV_ALIGN_CENTER, 0, 40);

  // Start WiFi Task
  xTaskCreate(wifi_scan_task, "WiFiScan", 4096, NULL, 1, NULL);

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
}
