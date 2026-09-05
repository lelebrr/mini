#include <Arduino.h>
#include <Wire.h>
#include <lvgl.h>
#include <SD_MMC.h>
#include <math.h>
#include "esp_task_wdt.h"

#include <Arduino_GFX_Library.h>
#include <ESP_IOExpander_Library.h>
#include <SensorQMI8658.hpp>
#include "esp_heap_caps.h"

#include "pin_config.h"
#include "drivers/TouchFT3168.h"
#include "drivers/PwnRTC.h"

// Módulos do Mini Lele (arquitetura v2.0, totalmente integrada)
#include "core/ConfigManager.h"
#include "core/PwnPower.h"
#include "core/PwnPet.h"
#include "core/PwnUI.h"
#include "core/PwnAttack.h"
#include "core/PwnVoice.h"
#include "core/PwnSleep.h"
#include "core/PwnBLE.h"
#include "Gamification.h"
#include "WiFiTools.h"
#include "AudioHandler.h"
#include "EvilPortal.h"
#include "web/WebHandler.h"

// -----------------------------------------------------------------------------
// Globais de hardware
// -----------------------------------------------------------------------------
ESP_IOExpander *expander = nullptr;
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
        PwnSleep::notifyActivity();      // toque acorda a tela
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// Callback de brilho para o PwnSleep (desacopla o gfx do módulo de sono).
static void setBrightnessCb(int b) {
    if (gfx) static_cast<Arduino_OLED *>(gfx)->setBrightness((uint8_t)constrain(b, 0, 255));
}

// -----------------------------------------------------------------------------
// Inicializações de hardware
// -----------------------------------------------------------------------------
static void initIOExpander() {
    Serial.println("[IO] Inicializando expansor TCA9554...");
    expander = new ESP_IOExpander_TCA95xx_8bit(
        (i2c_port_t)0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, IIC_SCL, IIC_SDA);
    // init()/begin() da API C++ retornam void — o que dá para validar é a
    // alocação; sem o expansor o SD/touch não funcionam, então reinicia.
    if (!expander) {
        Serial.println("[IO] FALHA ao alocar expansor TCA9554! Reiniciando...");
        delay(3000);
        ESP.restart();
    }
    expander->init();
    expander->begin();
    for (int p : {0, 1, 2, 6, 7}) expander->pinMode(p, OUTPUT);
    expander->digitalWrite(0, LOW);
    expander->digitalWrite(1, LOW);
    expander->digitalWrite(2, HIGH);
    expander->digitalWrite(6, HIGH);
    delay(20);
    expander->digitalWrite(0, HIGH);
    expander->digitalWrite(1, HIGH);
    expander->digitalWrite(SD_CS_EXIO, HIGH);   // CS do SD em HIGH (demo oficial)
    delay(50);
    Serial.println("[IO] Expansor pronto (SD CS = HIGH).");
}

static void initDisplay() {
    Serial.println("[DISPLAY] Inicializando SH8601...");
    if (!gfx->begin()) { Serial.println("[DISPLAY] gfx->begin() falhou!"); return; }
    gfx->fillScreen(BLACK);
    static_cast<Arduino_OLED *>(gfx)->setBrightness(200);
    Serial.printf("[DISPLAY] %d x %d\n", gfx->width(), gfx->height());
}

static void initSD() {
    Serial.println("[SD] Montando cartão (SDMMC 1-bit)...");
    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_D0);
    if (!SD_MMC.begin("/sdcard", true)) {
        Serial.println("[SD] Falha ao montar. Config/saves só em RAM.");
        return;
    }
    if (SD_MMC.cardType() == CARD_NONE) { Serial.println("[SD] Nenhum cartão."); return; }
    Serial.printf("[SD] OK — %lluMB\n", SD_MMC.cardSize() / (1024ULL * 1024ULL));
    const char *dirs[] = {"/arquivos_cartao_sd", "/arquivos_cartao_sd/voice",
                          "/arquivos_cartao_sd/tts", "/arquivos_cartao_sd/evil_portal",
                          "/capturas", "/fila_envio", "/tts"};
    for (const char *d : dirs) if (!SD_MMC.exists(d)) SD_MMC.mkdir(d);
}

static void initIMU() {
    Serial.println("[IMU] Inicializando QMI8658...");
    if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
        Serial.println("[IMU] QMI8658 não encontrado."); imu_ok = false; return;
    }
    qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G,
                            SensorQMI8658::ACC_ODR_1000Hz, SensorQMI8658::LPF_MODE_0);
    qmi.enableAccelerometer();
    imu_ok = true;
    Serial.println("[IMU] pronto.");
}

static void initLVGL() {
    Serial.println("[LVGL] Inicializando...");
    lv_init();
    // Buffer maior em PSRAM (1/4 da tela) => UI bem mais fluida (temos 8MB).
    // NOTA: a heap de PSRAM no ESP32-S3 NÃO é registrada com MALLOC_CAP_DMA
    // (ver esp_psram.c do IDF) — pedir SPIRAM|DMA sempre retorna NULL.
    uint32_t buffer_pixels = (LCD_WIDTH * LCD_HEIGHT) / 4;
    buf1 = (lv_color_t *)heap_caps_malloc(buffer_pixels * sizeof(lv_color_t),
                                          MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    buf2 = (lv_color_t *)heap_caps_malloc(buffer_pixels * sizeof(lv_color_t),
                                          MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!buf1 || !buf2) {
        // Fallback: buffers menores se a PSRAM estiver apertada.
        if (buf1) { heap_caps_free(buf1); buf1 = nullptr; }
        if (buf2) { heap_caps_free(buf2); buf2 = nullptr; }
        buffer_pixels = (LCD_WIDTH * LCD_HEIGHT) / 10;
        buf1 = (lv_color_t *)heap_caps_malloc(buffer_pixels * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        buf2 = (lv_color_t *)heap_caps_malloc(buffer_pixels * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    }
    if (!buf1 || !buf2) {
        // Última tentativa: RAM interna (buffers bem menores).
        if (buf1) { heap_caps_free(buf1); buf1 = nullptr; }
        if (buf2) { heap_caps_free(buf2); buf2 = nullptr; }
        buffer_pixels = (LCD_WIDTH * LCD_HEIGHT) / 20;
        buf1 = (lv_color_t *)heap_caps_malloc(buffer_pixels * sizeof(lv_color_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
        buf2 = (lv_color_t *)heap_caps_malloc(buffer_pixels * sizeof(lv_color_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    }
    if (!buf1 || !buf2) {
        Serial.println("[LVGL] Falha ao alocar buffers! Reiniciando...");
        delay(3000);
        ESP.restart();
    }

    lv_display = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_flush_cb(lv_display, my_disp_flush);
    lv_display_set_buffers(lv_display, buf1, buf2,
                           buffer_pixels * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_indev = lv_indev_create();
    lv_indev_set_type(lv_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(lv_indev, my_touch_read);
    Serial.println("[LVGL] pronto.");
}

// Detecção de "chacoalhada" pelo acelerômetro (alimenta/interage com o pet).
static void checkShake() {
    if (!imu_ok) return;
    float ax, ay, az;
    if (!qmi.getDataReady() || !qmi.getAccelerometer(ax, ay, az)) return;
    float mag = sqrtf(ax * ax + ay * ay + az * az);   // ~1.0 em repouso
    if (mag > 1.9f) {                                  // movimento brusco
        if (PwnPet::onShake()) FaceHandler::setFace(FACE_EXCITED);
        PwnSleep::notifyActivity();
    }
}

// Botão BOOT (GPIO0): clique curto = próxima tela/acorda; longo = deep sleep.
static void checkButton() {
    static uint32_t pressed_at = 0;
    static bool was_down = false;
    bool down = (digitalRead(PIN_BOOT) == LOW);
    if (down && !was_down) { pressed_at = millis(); was_down = true; }
    else if (!down && was_down) {
        uint32_t held = millis() - pressed_at;
        was_down = false;
        PwnSleep::notifyActivity();
        if (held > 1200) PwnSleep::enterDeep();   // longo
        else             PwnUI::nextTile();       // curto
    }
}

// -----------------------------------------------------------------------------
// setup / loop
// -----------------------------------------------------------------------------
#ifndef LVGL_SMOKE_TEST
void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("\n=== Mini Lele v2.1 — boot ===");

    pinMode(PIN_BOOT, INPUT_PULLUP);
    Wire.begin(IIC_SDA, IIC_SCL);
    Wire.setClock(400000);

    initIOExpander();
    PwnPower::init();
    initDisplay();
    touchDev.begin(Wire);
    initIMU();
    initSD();
    initLVGL();

    // ---- Aplicação ----
    ConfigManager *cfg = ConfigManager::getInstance();
    cfg->load();

    if (gfx) static_cast<Arduino_OLED *>(gfx)->setBrightness(
        (uint8_t)constrain(cfg->get<int>("disp_brightness"), 0, 255));
    PwnPower::setBatteryCapacity(cfg->get<int>("pwr_battery_capacity_mah"));
    PwnPower::setChargeCurrentMa(cfg->get<int>("pwr_charge_current_ma"));
    PwnPower::setPerformanceMode(cfg->get<int>("pwr_cpu_freq_max") >= 240 ? 2 : 1);

    PwnRTC::init();
    Gamification::init();
    PwnPet::init();
    AudioHandler::init();
    PwnUI::init();
    PwnAttack::init();
    PwnVoice::init();

    PwnSleep::init(setBrightnessCb,
                   cfg->get<int>("disp_brightness"),
                   cfg->get<int>("disp_timeout_sec"),
                   cfg->get<bool>("pwr_deep_sleep_enabled"),
                   cfg->get<int>("pwr_deep_sleep_after_sec"));

    WebHandler::init();

    // Sincroniza hora via NTP se estiver conectado (modo STA/AP_STA).
    if (WiFi.status() == WL_CONNECTED)
        PwnRTC::syncNTP(cfg->getString("sys_ntp_server").c_str(), cfg->get<int>("sys_timezone"));

    // Sniffer passivo automático (não derruba o AP/WebUI).
    if (cfg->get<bool>("atk_auto_scan")) { WiFiTools::beginNewCapture(); WiFiTools::startSnifferPassive(); }

    // Watchdog de tarefa (reinício automático se travar).
    if (cfg->get<bool>("sys_watchdog")) {
        esp_task_wdt_config_t wdt = {};
        wdt.timeout_ms = 20000; wdt.idle_core_mask = 0; wdt.trigger_panic = true;
        if (esp_task_wdt_init(&wdt) != ESP_OK) esp_task_wdt_reconfigure(&wdt);
        esp_task_wdt_add(NULL);
    }

    if (cfg->get<bool>("sys_sound_on_boot")) AudioHandler::playWav("/boot_pt.wav");
    Serial.println("[MAIN] Setup completo. Mini Lele vivo!");
}

void loop() {
    static uint32_t last_tick_ms = 0, last_sec_ms = 0, last_ble_ms = 0, last_persist_ms = 0;
    uint32_t now = millis();

    // LVGL
    uint32_t elapsed = now - last_tick_ms;
    if (elapsed >= 5) { lv_tick_inc(elapsed); last_tick_ms = now; lv_timer_handler(); }

    // Captura Wi-Fi (drena o buffer do sniffer para o SD)
    WiFiTools::flush();
    // DNS do portal cativo da WebUI
    WebHandler::loop();
    // Portal cativo do ataque (se ativo)
    EvilPortal::loop();

    // Interação física + botão
    checkShake();
    checkButton();

    // Tarefas por segundo
    if (now - last_sec_ms >= 1000) {
        last_sec_ms = now;
        PwnPet::tick();
        PwnPet::syncRTC();          // espelha stats na RAM RTC
        Gamification::tick();
        PwnPower::monitor();
        PwnAttack::tick();
        PwnSleep::tick();           // economia de energia
        if (!PwnSleep::isScreenOff()) PwnUI::update();  // não desenha com tela off
    }

    // Varredura BLE periódica (opcional)
    ConfigManager *cfg = ConfigManager::getInstance();
    if (cfg->get<bool>("atk_ble_scan")) {
        int iv = cfg->get<int>("atk_ble_interval_sec"); if (iv < 10) iv = 10;
        if (now - last_ble_ms > (uint32_t)iv * 1000) {
            last_ble_ms = now;
            int n = PwnBLE::scan(3);
            if (n > 0) PwnPet::feed(1);   // BLE também é "comida"
        }
    }

    // Persiste lista de dispositivos a cada 60 s (com rotação de log)
    if (now - last_persist_ms > 60000) { last_persist_ms = now; WiFiTools::persistDevices(); }

    esp_task_wdt_reset();   // alimenta o watchdog
    delay(2);
}
#endif // LVGL_SMOKE_TEST
