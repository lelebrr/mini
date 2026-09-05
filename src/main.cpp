#include <Arduino.h>
#include <Wire.h>
#include <lvgl.h>
#include <SD_MMC.h>

#include <Arduino_GFX_Library.h>
#include <ESP_IOExpander_Library.h>
#include <SensorQMI8658.hpp>
#include "esp_heap_caps.h"

#include "pin_config.h"
#include "drivers/TouchFT3168.h"

// Módulos do Mini Lele (arquitetura v2.0, agora totalmente integrada)
#include "core/ConfigManager.h"
#include "core/PwnPower.h"
#include "core/PwnPet.h"
#include "core/PwnUI.h"
#include "core/PwnAttack.h"
#include "core/PwnVoice.h"
#include "Gamification.h"
#include "WiFiTools.h"
#include "AudioHandler.h"
#include "EvilPortal.h"
#include "web/WebHandler.h"

// -----------------------------------------------------------------------------
// Globais de hardware
// -----------------------------------------------------------------------------
ESP_IOExpander *expander = nullptr;   // usado por AudioHandler (extern)
SensorQMI8658   qmi;
static TouchFT3168 touchDev;
static bool     imu_ok = false;

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3);

Arduino_GFX *gfx = new Arduino_SH8601(
    bus, LCD_RST, 0 /*rotation*/, false /*IPS*/, LCD_WIDTH, LCD_HEIGHT);

// -----------------------------------------------------------------------------
// LVGL
// -----------------------------------------------------------------------------
static lv_display_t *lv_display = nullptr;
static lv_indev_t   *lv_indev   = nullptr;
static lv_color_t   *buf1 = nullptr;
static lv_color_t   *buf2 = nullptr;

static void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
#if (LV_COLOR_16_SWAP != 0)
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
#else
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
#endif
    lv_display_flush_ready(disp);
}

static void my_touch_read(lv_indev_t *indev, lv_indev_data_t *data) {
    (void)indev;
    TouchFT3168::Point p = touchDev.read();
    if (p.touched) {
        data->point.x = p.x;
        data->point.y = p.y;
        data->state   = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// -----------------------------------------------------------------------------
// Inicializações de hardware
// -----------------------------------------------------------------------------
static void initIOExpander() {
    Serial.println("[IO] Inicializando expansor TCA9554...");
    expander = new ESP_IOExpander_TCA95xx_8bit(
        (i2c_port_t)0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, IIC_SCL, IIC_SDA);
    expander->init();
    expander->begin();

    // P0=LCD_RST, P1=TOUCH_RST, P2=periféricos, P6=áudio, P7=SD CS
    for (int p : {0, 1, 2, 6, 7}) expander->pinMode(p, OUTPUT);

    // Sequência de reset dos periféricos
    expander->digitalWrite(0, LOW);
    expander->digitalWrite(1, LOW);
    expander->digitalWrite(2, HIGH);   // liga periféricos
    expander->digitalWrite(6, HIGH);
    delay(20);
    expander->digitalWrite(0, HIGH);   // solta reset do display
    expander->digitalWrite(1, HIGH);   // solta reset do touch

    // Chip-select do cartão SD deve ficar em HIGH (ver demo oficial 14).
    expander->digitalWrite(SD_CS_EXIO, HIGH);
    delay(50);
    Serial.println("[IO] Expansor pronto (SD CS = HIGH).");
}

static void initDisplay() {
    Serial.println("[DISPLAY] Inicializando SH8601...");
    if (!gfx->begin()) {
        Serial.println("[DISPLAY] gfx->begin() falhou!");
        return;
    }
    gfx->fillScreen(BLACK);
    gfx->Display_Brightness(200);
    Serial.printf("[DISPLAY] %d x %d\n", gfx->width(), gfx->height());
}

static void initSD() {
    Serial.println("[SD] Montando cartão (SDMMC 1-bit)...");
    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_D0);
    if (!SD_MMC.begin("/sdcard", true)) {   // true = modo 1 bit
        Serial.println("[SD] Falha ao montar. Config/saves ficarão só em RAM.");
        return;
    }
    uint8_t type = SD_MMC.cardType();
    if (type == CARD_NONE) {
        Serial.println("[SD] Nenhum cartão detectado.");
        return;
    }
    Serial.printf("[SD] OK — %lluMB\n", SD_MMC.cardSize() / (1024ULL * 1024ULL));

    // Garante a estrutura de pastas usada pelo firmware.
    const char *dirs[] = {"/arquivos_cartao_sd", "/arquivos_cartao_sd/voice",
                          "/arquivos_cartao_sd/tts", "/arquivos_cartao_sd/evil_portal",
                          "/capturas", "/fila_envio", "/tts"};
    for (const char *d : dirs) if (!SD_MMC.exists(d)) SD_MMC.mkdir(d);
}

static void initIMU() {
    Serial.println("[IMU] Inicializando QMI8658...");
    if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
        Serial.println("[IMU] QMI8658 não encontrado.");
        imu_ok = false;
        return;
    }
    qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G,
                            SensorQMI8658::ACC_ODR_1000Hz,
                            SensorQMI8658::LPF_MODE_0);
    qmi.enableAccelerometer();
    imu_ok = true;
    Serial.println("[IMU] pronto.");
}

static void initLVGL() {
    Serial.println("[LVGL] Inicializando...");
    lv_init();

    uint32_t buffer_pixels = (LCD_WIDTH * LCD_HEIGHT) / 10;
    buf1 = (lv_color_t *)heap_caps_malloc(buffer_pixels * sizeof(lv_color_t),
                                          MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    buf2 = (lv_color_t *)heap_caps_malloc(buffer_pixels * sizeof(lv_color_t),
                                          MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    if (!buf1 || !buf2) {
        Serial.println("[LVGL] Falha ao alocar buffers em PSRAM!");
        while (true) delay(1000);
    }

    lv_display = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_flush_cb(lv_display, my_disp_flush);
    lv_display_set_buffers(lv_display, buf1, buf2,
                           buffer_pixels * sizeof(lv_color_t),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_indev = lv_indev_create();
    lv_indev_set_type(lv_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(lv_indev, my_touch_read);
    Serial.println("[LVGL] pronto.");
}

// -----------------------------------------------------------------------------
// setup / loop
// -----------------------------------------------------------------------------
#ifndef LVGL_SMOKE_TEST
void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("\n=== Mini Lele v2.0 — boot ===");

    Wire.begin(IIC_SDA, IIC_SCL);
    Wire.setClock(400000);

    initIOExpander();
    PwnPower::init();       // rails + carregamento (AXP2101) antes do display
    initDisplay();
    touchDev.begin(Wire);   // FT3168 (driver interno, sem TouchLib)
    initIMU();
    initSD();
    initLVGL();

    // ---- Camada de aplicação ----
    ConfigManager *cfg = ConfigManager::getInstance();
    cfg->load();

    // Aplica preferências salvas
    gfx->Display_Brightness(cfg->get<int>("disp_brightness"));
    PwnPower::setPerformanceMode(cfg->get<int>("pwr_cpu_freq_max") >= 240 ? 2 : 1);
    PwnPower::setChargeCurrentMa(cfg->get<int>("pwr_charge_current_ma"));

    Gamification::init();
    PwnPet::init();
    AudioHandler::init();   // I2S + ES8311 (seguro: degrada se falhar)
    PwnUI::init();
    PwnAttack::init();
    PwnVoice::init();

    WebHandler::init();     // sobe o AP + WebUI (padrão)

    if (cfg->get<bool>("sys_sound_on_boot")) AudioHandler::playWav("/boot_pt.wav");

    Serial.println("[MAIN] Setup completo. Mini Lele vivo!");
}

void loop() {
    static uint32_t last_tick_ms = 0;
    static uint32_t last_sec_ms  = 0;
    uint32_t now = millis();

    // LVGL
    uint32_t elapsed = now - last_tick_ms;
    if (elapsed >= 5) {
        lv_tick_inc(elapsed);
        last_tick_ms = now;
        lv_timer_handler();
    }

    // Tarefas por segundo
    if (now - last_sec_ms >= 1000) {
        last_sec_ms = now;
        PwnPet::tick();
        Gamification::tick();
        PwnPower::monitor();     // monitora bateria/carga USB-C
        PwnAttack::tick();
        PwnUI::update();
    }

    // Portal cativo (só quando ativo)
    EvilPortal::loop();

    delay(2);
}
#endif // LVGL_SMOKE_TEST
