#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <lvgl.h>
#include <SD_MMC.h>
#include <math.h>
#include "esp_task_wdt.h"

#include <Arduino_GFX_Library.h>
#include "drivers/TCA9554.h"   // expansor TCA9554 próprio (via Wire/driver_ng)
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
#include "FaceHandler.h"
#include "WpsBlue.h"
#include "WpsRed.h"
#include "BleBlue.h"
#include "BleIds.h"
#include "BleGatt.h"
#include "EspNowScan.h"
#include "core/PwnBLE.h"

#ifndef BLACK
#define BLACK 0x0000
#endif

// -----------------------------------------------------------------------------
// Globais de hardware
// -----------------------------------------------------------------------------
TCA9554 expander;   // expansor de IO da placa (P0=LCD_RST, P1=TOUCH_RST, P2=power, P6=áudio, P7=SD CS)
SensorQMI8658   qmi;
static TouchFT3168 touchDev;
static bool     imu_ok = false;

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3);
Arduino_GFX *gfx = new Arduino_SH8601(
    bus, LCD_RST, 0 /*rotation*/, LCD_WIDTH, LCD_HEIGHT);
// NOTA: na GFX Library >= 1.6.x o construtor é
//   (bus, rst, rotation, w, h, col_off1, row_off1, col_off2, row_off2)
// -- NÃO existe mais o parâmetro 'ips'. A chamada antiga passava 'false'
// como largura (w=0) e LCD_HEIGHT (448) como col_offset (truncado p/ 192).

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
    // Driver próprio (drivers/TCA9554.h) falando I2C pelo Wire (driver_ng).
    // A lib externa ESP32_IO_Expander usava o driver I2C LEGADO do IDF
    // (i2c_driver_install), que conflita com o Wire na mesma porta e derruba
    // o firmware com abort() ("CONFLICT! driver_ng...") ainda no boot.
    bool ok = false;
    for (int attempt = 1; attempt <= 3 && !ok; ++attempt) {
        ok = expander.begin(Wire, TCA9554_ADDR);
        if (!ok) { delay(10); }
    }
    if (!ok) {
        Serial.println("[IO] TCA9554 NÃO respondeu no I2C! (SD/semáforo de reset afetados)");
    }
    // Sequência de power-on da Waveshare:
    for (int p : {0, 1, 2, 6, 7}) expander.pinMode(p, OUTPUT);
    expander.digitalWrite(0, LOW);    // LCD em reset
    expander.digitalWrite(1, LOW);    // touch em reset
    expander.digitalWrite(2, HIGH);   // liga power dos periféricos
    expander.digitalWrite(6, HIGH);   // liga rail de áudio
    delay(20);
    expander.digitalWrite(0, HIGH);   // solta LCD_RST
    expander.digitalWrite(1, HIGH);   // solta TOUCH_RST
    expander.digitalWrite(SD_CS_EXIO, HIGH);   // CS do SD em HIGH (demo oficial)
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

// Acesso ao acelerômetro para a UI (Sensores/Nível). Usa a instância global.
bool UI_getAccel(float &x, float &y, float &z) {
    if (!imu_ok) return false;
    if (!qmi.getDataReady()) return false;
    return qmi.getAccelerometer(x, y, z);
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
        BleIds::notifyMotion();   // IDS BLE: sinal de movimento p/ stalker
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

// ===========================================================================
// Console serial WPS (BLUE inventario / RED laboratorio). Consulte docs/WPS.md.
// ===========================================================================
static bool wpsParseMac(const String& in, uint8_t out[6]) {
    return sscanf(in.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                  &out[0],&out[1],&out[2],&out[3],&out[4],&out[5]) == 6;
}

// Varredura BLUE: passa por canais 1..13 coletando IEs WPS de beacons.
static void wpsChannelSweep(int seconds) {
    bool was = WiFiTools::isSniffing();
    if (!was) { WiFiTools::beginNewCapture(); WiFiTools::startSnifferPassive(); }
    uint32_t end = millis() + (uint32_t)seconds * 1000;
    int ch = 1;
    while (millis() < end) {
        esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
        uint32_t t = millis();
        while (millis() - t < 260) { WpsBlue::poll(); EspNowScan::poll(); delay(10); }
        esp_task_wdt_reset();
        if (++ch > 13) ch = 1;
    }
    WpsBlue::poll(); EspNowScan::poll();
    if (!was) WiFiTools::stopSniffer();
}

static void wpsHandleCommand(String line) {
    line.trim();
    if (!line.startsWith("wps")) return;
    String rest = line.substring(3); rest.trim();

    if (rest.startsWith("scan")) {
        int secs = 5; int sp = rest.indexOf(' ');
        if (sp > 0) { int v = rest.substring(sp+1).toInt(); if (v > 0 && v <= 60) secs = v; }
        Serial.printf("[wps] varredura BLUE %d s...\n", secs);
        wpsChannelSweep(secs);
        Serial.printf("[wps] %d AP(s), %d WPS-ON, %d WPS-LOCKED\n",
                      WpsBlue::count(), WpsBlue::countWpsOn(), WpsBlue::countWpsLocked());
    } else if (rest.startsWith("report") || rest.startsWith("list")) {
        Serial.println(WpsBlue::reportSection());
        WpsBlue::writeReport();
    } else if (rest.startsWith("baseline")) {
        WpsBlue::snapshotBaseline();
        Serial.println("[wps] baseline salvo em /sd/wps/baseline.jsonl");
    } else if (rest.startsWith("arm")) {
        Serial.printf("[wps] ARM (GPIO%d) = %s\n", WPS_ARM_GPIO, WpsRed::isArmed() ? "ARMADO" : "desarmado");
    } else if (rest.startsWith("allow")) {
        Serial.printf("[wps] allowlist: %d BSSID(s)\n", WpsRed::loadAllowlist());
    } else if (rest.startsWith("status")) {
        Serial.printf("[wps] RED estado=%s restante=%ds\n", WpsRed::stateStr(), WpsRed::secondsRemaining());
    } else if (rest.startsWith("stop")) {
        WpsRed::abort("USER"); Serial.println("[wps] abort solicitado");
    } else if (rest.startsWith("pbc") || rest.startsWith("pin") || rest.startsWith("vpin")) {
        String mode = rest.startsWith("vpin") ? "vpin" : (rest.startsWith("pbc") ? "pbc" : "pin");
        int sp = rest.indexOf(' ');
        uint8_t bssid[6];
        if (sp < 0 || !wpsParseMac(rest.substring(sp+1), bssid)) {
            Serial.println("[wps] uso: wps <pbc|pin|vpin> AA:BB:CC:DD:EE:FF");
            return;
        }
        if (mode == "pbc")  WpsRed::redPbc(bssid);
        else if (mode == "pin") WpsRed::redPin(bssid);
        else WpsRed::redVendorPinOneShot(bssid);
    } else {
        Serial.println("[wps] cmds: scan [s] | report | baseline | arm | allow | status | stop | pbc/pin/vpin MAC");
    }
}

// Alertas do IDS BLE chegam na task do BT; NAO tocar LVGL de la.
// Bufferiza aqui e a UI atualiza no loop (task principal).
static volatile bool  g_ble_alert = false;
static char           g_ble_alert_msg[96] = {0};
static uint32_t       g_ble_alert_color = 0xFF3232;
static void bleAlertBuffer(const char* msg, uint32_t color) {
    strncpy(g_ble_alert_msg, msg, sizeof(g_ble_alert_msg) - 1);
    g_ble_alert_msg[sizeof(g_ble_alert_msg) - 1] = 0;
    g_ble_alert_color = color;
    g_ble_alert = true;
}

static void bleHandleCommand(String line) {
    line.trim();
    if (!line.startsWith("ble")) return;
    String rest = line.substring(3); rest.trim();
    if (rest.startsWith("scan")) {
        int secs = 4; int sp = rest.indexOf(' ');
        if (sp > 0) { int v = rest.substring(sp+1).toInt(); if (v > 0 && v <= 30) secs = v; }
        Serial.printf("[ble] scan %d s...\n", secs);
        int n = PwnBLE::scan(secs);
        Serial.printf("[ble] %d anuncio(s); inventario: %d device(s), %d HID\n",
                      n, BleBlue::count(), BleBlue::countHid());
    } else if (rest.startsWith("report") || rest.startsWith("list")) {
        Serial.println(BleBlue::reportSection());
        BleBlue::writeReport();
    } else if (rest.startsWith("clear")) {
        BleBlue::clear(); Serial.println("[ble] inventario limpo");
    } else if (rest.startsWith("ids")) {
        Serial.printf("[ble] IDS: %d alerta(s) ate agora\n", BleIds::alertCount());
    } else if (rest.startsWith("gatt")) {
        int sp = rest.indexOf(' '); uint8_t b[6];
        if (sp>0 && sscanf(rest.substring(sp+1).c_str(),"%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",&b[0],&b[1],&b[2],&b[3],&b[4],&b[5])==6) {
            int r = BleGatt::audit(b);
            if (r>=0) Serial.printf("[ble] gatt: %d characteristic(s); relatorio em /reports/gatt-*.md\n", r);
            else if (r==-1) Serial.println("[ble] gatt RECUSADO: alvo fora da allowlist (/allowlist/ble.txt)");
            else Serial.println("[ble] gatt: falha de conexao");
        } else Serial.println("[ble] uso: ble gatt AA:BB:CC:DD:EE:FF (alvo tem de estar na allowlist)");
    } else if (rest.startsWith("foxstop")) {
        BleIds::foxhuntStop(); Serial.println("[ble] foxhunt off");
    } else if (rest.startsWith("fox")) {
        int sp = rest.indexOf(' '); uint8_t b[6];
        if (sp>0 && sscanf(rest.substring(sp+1).c_str(),"%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",&b[0],&b[1],&b[2],&b[3],&b[4],&b[5])==6) {
            BleIds::foxhuntStart(b); Serial.println("[ble] foxhunt on (roda 'ble scan' p/ atualizar RSSI)");
        } else Serial.println("[ble] uso: ble fox AA:BB:CC:DD:EE:FF");
    } else {
        Serial.println("[ble] cmds: scan [s] | report | clear | ids | gatt MAC | fox MAC | foxstop");
    }
}

static void espnowHandleCommand(String line) {
    line.trim();
    if (!line.startsWith("espnow")) return;
    String rest = line.substring(6); rest.trim();
    if (rest.startsWith("scan")) {
        int secs = 6; int sp = rest.indexOf(' ');
        if (sp > 0) { int v = rest.substring(sp+1).toInt(); if (v>0 && v<=60) secs=v; }
        Serial.printf("[espnow] varredura %d s...\n", secs);
        wpsChannelSweep(secs);   // varre canais; alimenta WPS e ESP-NOW
        Serial.printf("[espnow] %d peer(s)\n", EspNowScan::count());
    } else if (rest.startsWith("report") || rest.startsWith("list")) {
        Serial.println(EspNowScan::reportSection());
        EspNowScan::writeReport();
    } else if (rest.startsWith("clear")) {
        EspNowScan::clear(); Serial.println("[espnow] inventario limpo");
    } else {
        Serial.println("[espnow] cmds: scan [s] | report | clear");
    }
}

static void wpsSerialPoll() {
    static String buf;
    while (Serial.available()) {
        char c = (char)Serial.read();
        if (c == '\n' || c == '\r') {
            if (buf.length()) { wpsHandleCommand(buf); bleHandleCommand(buf); espnowHandleCommand(buf); buf = ""; }
        } else if (buf.length() < 96) buf += c;
    }
}

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

    // ---- WPS (BLUE inventario + RED laboratorio) ----
    WpsBlue::init();
    WpsBlue::setAlertCallback([](const char* m, uint32_t c){ PwnUI::showAlert(m, c); });
    WpsRed::init();
    WpsRed::setStatusCallback([](const char* m, uint32_t c){ PwnUI::showToast(m); });
    WpsRed::loadAllowlist();
    BleBlue::init();   // inventario BLE BLUE
    BleIds::init();
    BleBlue::setObserver(BleIds::onEvent);
    BleIds::setAlertCallback(bleAlertBuffer);
    BleGatt::init();   // GATT audit (allowlist)
    EspNowScan::init();   // inventario ESP-NOW (BLUE)

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

    if (cfg->get<bool>("sys_sound_on_boot")) AudioHandler::beep(1760, 70);
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
    WpsBlue::poll();      // parse dos IEs WPS coletados
    EspNowScan::poll();   // parse dos frames ESP-NOW coletados
    // foxhunt BLE: beep com cadencia proporcional ao RSSI do alvo
    static uint32_t last_fox_beep = 0;
    if (BleIds::foxhuntActive()) { int iv = BleIds::foxhuntBeepIntervalMs();
        if (iv > 0 && now - last_fox_beep > (uint32_t)iv) { last_fox_beep = now; AudioHandler::beep(2200, 35); } }
    wpsSerialPoll();      // console WPS via serial
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
        WpsRed::tick();          // maquina de estados RED WPS (timeouts/stops)
        BleIds::tick();          // IDS BLE: decai janelas de spam / movimento
        if (g_ble_alert) { g_ble_alert = false; PwnUI::showAlert(g_ble_alert_msg, g_ble_alert_color); }
        PwnSleep::tick();           // economia de energia
        FaceHandler::setEnabled(!PwnSleep::isScreenOff());  // pausa a animação c/ tela off
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
