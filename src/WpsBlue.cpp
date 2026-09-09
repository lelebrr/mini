#include "WpsBlue.h"
#include <SD_MMC.h>
#include <string.h>

// ===========================================================================
// Armazenamento estatico
// ===========================================================================
WpsApEntry     WpsBlue::_aps[WpsBlue::MAX_APS];
portMUX_TYPE   WpsBlue::_mux   = portMUX_INITIALIZER_UNLOCKED;
WpsBlue::AlertCb WpsBlue::_alert = nullptr;

// Ring de quadros crus (preenchido em onFrame, drenado em poll).
#define WPS_RING_SLOTS 6
#define WPS_RING_BYTES 384
struct WpsRawFrame {
    uint8_t  buf[WPS_RING_BYTES];
    uint16_t len;
    int8_t   rssi;
    uint8_t  channel;
};
static WpsRawFrame     s_ring[WPS_RING_SLOTS];
static volatile int    s_ring_head = 0;   // escrito por onFrame
static volatile int    s_ring_tail = 0;   // lido por poll
static portMUX_TYPE    s_ring_mux  = portMUX_INITIALIZER_UNLOCKED;

// Baseline: BSSID -> wps_on
struct WpsBaseEntry { uint8_t bssid[6]; bool wps_on; bool used; };
#define WPS_BASE_MAX 64
static WpsBaseEntry s_base[WPS_BASE_MAX];
static bool         s_base_loaded = false;

// ===========================================================================
// Helpers
// ===========================================================================
static inline uint16_t rd16be(const uint8_t* p) { return (uint16_t)((p[0] << 8) | p[1]); }

static void macToStr(const uint8_t b[6], char* out /*>=18*/) {
    snprintf(out, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
             b[0], b[1], b[2], b[3], b[4], b[5]);
}
static bool macEq(const uint8_t* a, const uint8_t* b) { return memcmp(a, b, 6) == 0; }

// Copia ASCII segura (WSC strings nao sao null-terminated).
static void copyAscii(char* dst, size_t cap, const uint8_t* src, uint16_t n) {
    if (n >= cap) n = cap - 1;
    for (uint16_t i = 0; i < n; i++) {
        uint8_t c = src[i];
        dst[i] = (c >= 0x20 && c < 0x7F) ? (char)c : '.';
    }
    dst[n] = 0;
}

void WpsBlue::setAlertCallback(AlertCb cb) { _alert = cb; }
void WpsBlue::_emit(const char* msg, uint32_t color) {
    Serial.printf("[WpsBlue] %s\n", msg);
    if (_alert) _alert(msg, color);
}

// ===========================================================================
// init
// ===========================================================================
void WpsBlue::init() {
    memset(_aps, 0, sizeof(_aps));
    s_ring_head = s_ring_tail = 0;
    Serial.println("[WpsBlue] Inventario WPS inicializado (BLUE)");
    loadBaseline();
}

// ===========================================================================
// onFrame — contexto da task de Wi-Fi. So faz filtro + copia. SEM parse pesado.
// ===========================================================================
void WpsBlue::onFrame(const uint8_t* payload, uint16_t len, int8_t rssi, uint8_t channel) {
    if (!payload || len < 38) return;
    // Frame Control byte0: beacon=0x80, probe-resp=0x50
    uint8_t fc0 = payload[0];
    if (fc0 != 0x80 && fc0 != 0x50) return;

    // Varre tagged params (offset 36) atras do OUI WPS, de forma barata.
    const uint8_t* ie = payload + 36;
    uint16_t remaining = (len > 36) ? (len - 36) : 0;
    bool found = false;
    while (remaining >= 2) {
        uint8_t tag = ie[0];
        uint8_t tlen = ie[1];
        if ((uint16_t)tlen + 2 > remaining) break;
        if (tag == WPS_IE_ELEMENT_ID && tlen >= 4 &&
            memcmp(ie + 2, WPS_OUI, 3) == 0 && ie[5] == WPS_OUI_TYPE) {
            found = true;
            break;
        }
        ie        += 2 + tlen;
        remaining -= 2 + tlen;
    }
    if (!found) return;

    uint16_t cp = (len > WPS_RING_BYTES) ? WPS_RING_BYTES : len;
    portENTER_CRITICAL(&s_ring_mux);
    int next = (s_ring_head + 1) % WPS_RING_SLOTS;
    if (next != s_ring_tail) {              // ring nao cheio -> enfileira
        memcpy(s_ring[s_ring_head].buf, payload, cp);
        s_ring[s_ring_head].len     = cp;
        s_ring[s_ring_head].rssi    = rssi;
        s_ring[s_ring_head].channel = channel;
        s_ring_head = next;
    }
    portEXIT_CRITICAL(&s_ring_mux);
}

// ===========================================================================
// poll — loop principal. Drena o ring e faz o parse.
// ===========================================================================
void WpsBlue::poll() {
    for (;;) {
        WpsRawFrame f;
        portENTER_CRITICAL(&s_ring_mux);
        if (s_ring_tail == s_ring_head) { portEXIT_CRITICAL(&s_ring_mux); break; }
        f = s_ring[s_ring_tail];
        s_ring_tail = (s_ring_tail + 1) % WPS_RING_SLOTS;
        portEXIT_CRITICAL(&s_ring_mux);

        WpsInfo info; uint8_t bssid[6]; char ssid[33]; int ch = f.channel;
        wpsInfoClear(&info);
        if (!parseMgmtFrame(f.buf, f.len, &info, bssid, ssid, sizeof(ssid), &ch))
            continue;

        WpsApEntry* e = _slotFor(bssid);
        if (!e) continue;
        bool was_present = e->used && e->wps.present;

        if (!e->used) {
            memset(e, 0, sizeof(*e));
            e->used = true;
            memcpy(e->bssid, bssid, 6);
            e->first_seen = millis();
        }
        strncpy(e->ssid, ssid, sizeof(e->ssid) - 1);
        e->ssid[sizeof(e->ssid) - 1] = 0;
        e->channel   = (int8_t)ch;
        e->rssi      = f.rssi;
        e->last_seen = millis();
        e->wps       = info;
        _applyPixieHeuristic(e);
        _checkBaseline(e, was_present, info.present);
    }
}

// ===========================================================================
// parseMgmtFrame
// ===========================================================================
bool WpsBlue::parseMgmtFrame(const uint8_t* frame, uint16_t len,
                             WpsInfo* out, uint8_t bssid_out[6],
                             char* ssid_out, size_t ssid_cap,
                             int* channel_out) {
    if (!frame || len < 38) return false;
    if (bssid_out) memcpy(bssid_out, frame + 16, 6);   // addr3 = BSSID
    if (ssid_out && ssid_cap) ssid_out[0] = 0;

    const uint8_t* ie = frame + 36;                    // apos fixed params (12B)
    uint16_t remaining = len - 36;
    bool wps_ok = false;

    while (remaining >= 2) {
        uint8_t tag = ie[0], tlen = ie[1];
        if ((uint16_t)tlen + 2 > remaining) break;
        const uint8_t* val = ie + 2;
        if (tag == 0x00 && ssid_out && ssid_cap) {           // SSID
            copyAscii(ssid_out, ssid_cap, val, tlen);
        } else if (tag == 0x03 && tlen >= 1 && channel_out) { // DS Param
            *channel_out = val[0];
        } else if (tag == WPS_IE_ELEMENT_ID && tlen >= 4 &&
                   memcmp(val, WPS_OUI, 3) == 0 && val[3] == WPS_OUI_TYPE) {
            if (parseWpsIe(val + 4, tlen - 4, out)) wps_ok = true;
        }
        ie        += 2 + tlen;
        remaining -= 2 + tlen;
    }
    return wps_ok;
}

// ===========================================================================
// parseWpsIe — walk dos TLVs WSC
// ===========================================================================
bool WpsBlue::parseWpsIe(const uint8_t* data, uint16_t len, WpsInfo* w) {
    if (!data || !w) return false;
    wpsInfoClear(w);
    w->present = true;

    uint16_t off = 0;
    while (off + 4 <= len) {
        uint16_t type = rd16be(data + off);
        uint16_t alen = rd16be(data + off + 2);
        off += 4;
        if (off + alen > len) break;
        const uint8_t* v = data + off;

        switch (type) {
            case WPS_ATTR_VERSION:
                if (alen >= 1) w->version = v[0];
                break;
            case WPS_ATTR_WPS_STATE:
                if (alen >= 1) w->wps_state = v[0];
                break;
            case WPS_ATTR_AP_SETUP_LOCKED:
                if (alen >= 1) w->ap_setup_locked = (v[0] == 0x01);
                break;
            case WPS_ATTR_CONFIG_METHODS:
                if (alen >= 2) w->config_methods = rd16be(v);
                break;
            case WPS_ATTR_MANUFACTURER:
                copyAscii(w->manufacturer, sizeof(w->manufacturer), v, alen);
                break;
            case WPS_ATTR_MODEL_NAME:
                copyAscii(w->model_name, sizeof(w->model_name), v, alen);
                break;
            case WPS_ATTR_MODEL_NUMBER:
                copyAscii(w->model_number, sizeof(w->model_number), v, alen);
                break;
            case WPS_ATTR_DEVICE_NAME:
                copyAscii(w->device_name, sizeof(w->device_name), v, alen);
                break;
            case WPS_ATTR_UUID_E:
                if (alen >= 16) { memcpy(w->uuid, v, 16); w->has_uuid = true; }
                break;
            case WPS_ATTR_VENDOR_EXTENSION:
                // WFA OUI(3) + subelementos {id(1),len(1),val}
                if (alen >= 4 && memcmp(v, WPS_WFA_OUI, 3) == 0) {
                    uint16_t so = 3;
                    while (so + 2 <= alen) {
                        uint8_t sid = v[so], slen = v[so + 1];
                        if (so + 2 + slen > alen) break;
                        if (sid == WPS_WFA_SUBELEM_VERSION2 && slen >= 1)
                            w->version2 = true;
                        so += 2 + slen;
                    }
                }
                break;
            default: break;
        }
        off += alen;
    }

    // Categorias derivadas (spec: PIN/PBC/NFC/keypad)
    uint16_t cm = w->config_methods;
    w->cm_pbc    = (cm & WPS_CM_PUSHBUTTON) != 0;
    w->cm_keypad = (cm & WPS_CM_KEYPAD) != 0;
    w->cm_pin    = (cm & (WPS_CM_LABEL | WPS_CM_DISPLAY)) != 0;
    w->cm_nfc    = (cm & (WPS_CM_EXT_NFC | WPS_CM_INT_NFC | WPS_CM_NFC_IF)) != 0;
    return true;
}

// ===========================================================================
// Inventario
// ===========================================================================
WpsApEntry* WpsBlue::_slotFor(const uint8_t bssid[6]) {
    int free_i = -1;
    for (int i = 0; i < MAX_APS; i++) {
        if (_aps[i].used && macEq(_aps[i].bssid, bssid)) return &_aps[i];
        if (!_aps[i].used && free_i < 0) free_i = i;
    }
    if (free_i >= 0) return &_aps[free_i];
    // cheio: recicla o mais antigo (menor last_seen)
    int oldest = 0;
    for (int i = 1; i < MAX_APS; i++)
        if (_aps[i].last_seen < _aps[oldest].last_seen) oldest = i;
    return &_aps[oldest];
}

int WpsBlue::count() {
    int n = 0;
    for (int i = 0; i < MAX_APS; i++) if (_aps[i].used) n++;
    return n;
}
const WpsApEntry* WpsBlue::get(int idx) {
    int n = 0;
    for (int i = 0; i < MAX_APS; i++) {
        if (!_aps[i].used) continue;
        if (n == idx) return &_aps[i];
        n++;
    }
    return nullptr;
}
const WpsApEntry* WpsBlue::findByBssid(const uint8_t bssid[6]) {
    for (int i = 0; i < MAX_APS; i++)
        if (_aps[i].used && macEq(_aps[i].bssid, bssid)) return &_aps[i];
    return nullptr;
}
void WpsBlue::clear() { memset(_aps, 0, sizeof(_aps)); }

WpsHygiene WpsBlue::hygiene(const uint8_t bssid[6]) {
    const WpsApEntry* e = findByBssid(bssid);
    if (!e) return WPS_HYG_OFF;
    return wpsHygieneOf(&e->wps);
}
int WpsBlue::countWpsOn() {
    int n = 0;
    for (int i = 0; i < MAX_APS; i++)
        if (_aps[i].used && wpsHygieneOf(&_aps[i].wps) == WPS_HYG_ON) n++;
    return n;
}
int WpsBlue::countWpsLocked() {
    int n = 0;
    for (int i = 0; i < MAX_APS; i++)
        if (_aps[i].used && wpsHygieneOf(&_aps[i].wps) == WPS_HYG_LOCKED) n++;
    return n;
}

// ===========================================================================
// Heuristica INFO (opcional): OUI + modelo -> nota "legacy chipset,
// historically pixie-vulnerable". SOMENTE INFO. NAO habilita nenhum ataque.
// Nao e um oraculo: apenas sinaliza famílias historicamente citadas.
// ===========================================================================
void WpsBlue::_applyPixieHeuristic(WpsApEntry* e) {
    e->pixie_info = false;
    if (!e->wps.present) return;
    // OUIs historicamente associados a chipsets legados citados em literatura
    // pixie-dust (Ralink/Broadcom/Realtek antigos). Lista curta e conservadora.
    static const uint8_t legacy_ouis[][3] = {
        {0x00,0x0C,0x43}, // Ralink
        {0x00,0x1D,0x0F}, // (exemplo legado)
        {0xC8,0x3A,0x35}, // Tenda/legado
        {0xEC,0x08,0x6B}, // TP-Link legado
    };
    for (auto& o : legacy_ouis) {
        if (memcmp(e->bssid, o, 3) == 0) { e->pixie_info = true; return; }
    }
    // Heuristica leve por modelo/fabricante (apenas marca INFO).
    const char* m = e->wps.model_name;
    if (m[0] && (strstr(m, "RT") == m || strstr(m, "WR"))) e->pixie_info = true;
}

// ===========================================================================
// Baseline
// ===========================================================================
static WpsBaseEntry* baseSlot(const uint8_t bssid[6]) {
    int free_i = -1;
    for (int i = 0; i < WPS_BASE_MAX; i++) {
        if (s_base[i].used && macEq(s_base[i].bssid, bssid)) return &s_base[i];
        if (!s_base[i].used && free_i < 0) free_i = i;
    }
    if (free_i >= 0) return &s_base[free_i];
    return nullptr;
}

void WpsBlue::loadBaseline(const char* path) {
    memset(s_base, 0, sizeof(s_base));
    s_base_loaded = true;
    File f = SD_MMC.open(path, FILE_READ);
    if (!f) { Serial.println("[WpsBlue] baseline: nenhum arquivo (ok)"); return; }
    int n = 0;
    while (f.available()) {
        String line = f.readStringUntil('\n');
        line.trim();
        if (line.length() < 5 || line[0] == '#') continue;
        int p = line.indexOf("\"bssid\"");
        if (p < 0) continue;
        int q = line.indexOf(':', p);
        int a = line.indexOf('"', q + 1);
        int b = line.indexOf('"', a + 1);
        if (a < 0 || b < 0) continue;
        String mac = line.substring(a + 1, b);
        uint8_t bs[6];
        if (sscanf(mac.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                   &bs[0],&bs[1],&bs[2],&bs[3],&bs[4],&bs[5]) != 6) continue;
        bool on = line.indexOf("\"wps_on\":true") >= 0 || line.indexOf("\"wps_on\": true") >= 0;
        WpsBaseEntry* e = baseSlot(bs);
        if (e) { memcpy(e->bssid, bs, 6); e->wps_on = on; e->used = true; n++; }
    }
    f.close();
    Serial.printf("[WpsBlue] baseline: %d AP(s) carregado(s)\n", n);
}

bool WpsBlue::saveBaseline(const char* path) {
    // garante diretorio /sd/wps
    if (!SD_MMC.exists("/wps")) SD_MMC.mkdir("/wps");
    File f = SD_MMC.open(path, FILE_WRITE);
    if (!f) { Serial.println("[WpsBlue] baseline: falha ao escrever"); return false; }
    f.println("# SENTINEL WPS baseline (JSONL). Gerado pelo firmware.");
    char mac[18];
    for (int i = 0; i < WPS_BASE_MAX; i++) {
        if (!s_base[i].used) continue;
        macToStr(s_base[i].bssid, mac);
        f.printf("{\"bssid\":\"%s\",\"wps_on\":%s}\n", mac, s_base[i].wps_on ? "true" : "false");
    }
    f.close();
    return true;
}

void WpsBlue::snapshotBaseline() {
    memset(s_base, 0, sizeof(s_base));
    for (int i = 0; i < MAX_APS; i++) {
        if (!_aps[i].used) continue;
        WpsBaseEntry* e = baseSlot(_aps[i].bssid);
        if (e) { memcpy(e->bssid, _aps[i].bssid, 6); e->wps_on = _aps[i].wps.present; e->used = true; }
    }
    saveBaseline();
    _emit("Baseline WPS atualizado", 0x00C8FF);
}

void WpsBlue::_checkBaseline(WpsApEntry* e, bool /*was_present*/, bool now_present) {
    if (!s_base_loaded) return;
    WpsBaseEntry* b = baseSlot(e->bssid);
    if (!b || !b->used) return;                 // AP nao conhecido -> sem alerta
    if (!b->wps_on && now_present) {            // conhecido e antes SEM WPS
        char mac[18]; macToStr(e->bssid, mac);
        char msg[96];
        snprintf(msg, sizeof(msg), "ALERTA: AP conhecido %s (%s) ativou WPS",
                 mac, e->ssid[0] ? e->ssid : "?");
        _emit(msg, 0xFFB400);
        b->wps_on = true;   // evita repetir o alerta na mesma sessao
    }
}

// ===========================================================================
// Relatorio — secao "WPS inventory"
// ===========================================================================
static void appendEntryMd(String& s, const WpsApEntry* e) {
    char mac[18]; macToStr(e->bssid, mac);
    WpsHygiene h = wpsHygieneOf(&e->wps);
    s += "\n### "; s += (e->ssid[0] ? e->ssid : "(hidden)");
    s += "  `"; s += mac; s += "`\n";
    s += "- hygiene: **"; s += wpsHygieneChip(h); s += "**\n";
    s += "- channel: "; s += String((int)e->channel);
    s += " | rssi: "; s += String((int)e->rssi); s += " dBm\n";
    if (!e->wps.present) { s += "- WPS: not advertised\n"; return; }
    s += "- wps_version: ";
    s += (e->wps.version ? String(e->wps.version >> 4) + "." + String(e->wps.version & 0x0F)
                         : String("?"));
    if (e->wps.version2) s += " (WSC 2.0)";
    s += "\n";
    s += "- config_methods:";
    if (e->wps.cm_pbc)    s += " PBC";
    if (e->wps.cm_pin)    s += " PIN";
    if (e->wps.cm_keypad) s += " Keypad";
    if (e->wps.cm_nfc)    s += " NFC";
    { char hx[10]; snprintf(hx, sizeof(hx), " (0x%04X)", e->wps.config_methods); s += hx; }
    s += "\n";
    s += "- ap_setup_locked: "; s += (e->wps.ap_setup_locked ? "yes" : "no"); s += "\n";
    if (e->wps.manufacturer[0]) { s += "- manufacturer: "; s += e->wps.manufacturer; s += "\n"; }
    if (e->wps.model_name[0])   { s += "- model: "; s += e->wps.model_name;
                                  if (e->wps.model_number[0]) { s += " ("; s += e->wps.model_number; s += ")"; }
                                  s += "\n"; }
    if (e->wps.device_name[0])  { s += "- device_name: "; s += e->wps.device_name; s += "\n"; }
    if (e->wps.has_uuid) {
        s += "- uuid: ";
        char u[3];
        for (int i = 0; i < 16; i++) { snprintf(u, sizeof(u), "%02x", e->wps.uuid[i]); s += u; }
        s += "\n";
    }
    if (e->pixie_info)
        s += "- INFO: legacy chipset, historically pixie-vulnerable (informativo; nenhum ataque habilitado)\n";
}

String WpsBlue::reportSection() {
    String s = "## WPS inventory\n";
    s += "_BLUE — somente observacao passiva. "; 
    s += String(count()); s += " AP(s), ";
    s += String(countWpsOn()); s += " WPS-ON, ";
    s += String(countWpsLocked()); s += " WPS-LOCKED._\n";
    bool any = false;
    for (int i = 0; i < MAX_APS; i++) {
        if (!_aps[i].used) continue;
        appendEntryMd(s, &_aps[i]);
        any = true;
    }
    if (!any) s += "\n_(nenhum AP observado ainda)_\n";
    return s;
}

bool WpsBlue::writeReport(const char* path) {
    if (!SD_MMC.exists("/reports")) SD_MMC.mkdir("/reports");
    File f = SD_MMC.open(path, FILE_WRITE);
    if (!f) { Serial.println("[WpsBlue] relatorio: falha ao escrever"); return false; }
    f.print(reportSection());
    f.close();
    Serial.printf("[WpsBlue] relatorio WPS escrito em %s\n", path);
    return true;
}
