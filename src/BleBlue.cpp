#include "BleBlue.h"
#include <SD_MMC.h>

BleDevice BleBlue::_dev[BleBlue::MAX_DEV];
BleBlue::Observer BleBlue::_obs = nullptr;
void BleBlue::setObserver(Observer cb) { _obs = cb; }

// NUS (Nordic UART Service) 6E400001-B5A3-F393-E0A9-E50E24DCCA9E em bytes LE
// (como aparecem no AD, ordem invertida do texto).
static const uint8_t NUS_UUID128_LE[16] = {
    0x9E,0xCA,0xDC,0x24,0x0E,0xE5,0xA9,0xE0,
    0x93,0xF3,0xA3,0xB5,0x01,0x00,0x40,0x6E
};

static inline uint16_t rd16le(const uint8_t* p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static bool macEq(const uint8_t* a, const uint8_t* b) { return memcmp(a, b, 6) == 0; }
static void macToStr(const uint8_t b[6], char* o) {
    snprintf(o, 18, "%02X:%02X:%02X:%02X:%02X:%02X", b[0],b[1],b[2],b[3],b[4],b[5]);
}
static void copyAscii(char* dst, size_t cap, const uint8_t* src, uint16_t n) {
    if (n >= cap) n = cap - 1;
    for (uint16_t i = 0; i < n; i++) { uint8_t c = src[i]; dst[i] = (c>=0x20 && c<0x7F)?(char)c:'.'; }
    dst[n] = 0;
}

void BleBlue::init() {
    memset(_dev, 0, sizeof(_dev));
    Serial.println("[BleBlue] Inventário BLE inicializado (BLUE)");
}

// ===========================================================================
// Parser de AD — à prova de truncar. Cada estrutura: [len][type][data(len-1)].
// ===========================================================================
bool BleBlue::parseAdv(const uint8_t* p, uint16_t len, BleParsed* out) {
    if (!p || !out) return false;
    memset(out, 0, sizeof(*out));
    out->tx_power = 0;

    uint16_t i = 0;
    while (i < len) {
        uint8_t flen = p[i];
        if (flen == 0) break;                 // fim / padding
        if (i + 1 + flen > len) break;         // truncado -> para (nunca lê além)
        uint8_t type = p[i + 1];
        const uint8_t* d = &p[i + 2];
        uint8_t dlen = flen - 1;

        switch (type) {
            case BLE_AD_FLAGS:
                if (dlen >= 1) { out->flags = d[0]; out->has_flags = true; }
                break;
            case BLE_AD_NAME_SHORT:
            case BLE_AD_NAME_COMPLETE:
                copyAscii(out->name, sizeof(out->name), d, dlen);
                out->has_name = true;
                break;
            case BLE_AD_TX_POWER:
                if (dlen >= 1) { out->tx_power = (int8_t)d[0]; out->has_tx_power = true; }
                break;
            case BLE_AD_UUID16_INCOMPLETE:
            case BLE_AD_UUID16_COMPLETE:
                for (uint8_t k = 0; k + 2 <= dlen && out->n_svc < 8; k += 2)
                    out->svc_uuid16[out->n_svc++] = rd16le(d + k);
                break;
            case BLE_AD_SVC_DATA_16:
                if (dlen >= 2 && out->n_svc < 8)
                    out->svc_uuid16[out->n_svc++] = rd16le(d);
                break;
            case BLE_AD_UUID128_INCOMPLETE:
            case BLE_AD_UUID128_COMPLETE:
            case BLE_AD_SVC_DATA_128:
                out->has_uuid128 = true;
                if (dlen >= 16 && memcmp(d, NUS_UUID128_LE, 16) == 0)
                    out->vendor = VND_NUS;     // marca provisória (classify confirma)
                break;
            case BLE_AD_MANUFACTURER:
                if (dlen >= 2) {
                    out->company_id = rd16le(d);
                    out->has_mfr = true;
                    if (out->company_id == BLE_CID_APPLE && dlen >= 3)
                        out->cont_type = d[2];
                }
                break;
            default: break;
        }
        i += 1 + flen;
    }

    // flag HID (serviço 0x1812 presente)
    for (uint8_t k = 0; k < out->n_svc; k++)
        if (out->svc_uuid16[k] == BLE_UUID_HID) out->hid = true;

    out->vendor = classify(out);
    return true;
}

// ===========================================================================
// Classificação de vendor (precedência clara)
// ===========================================================================
BleVendor BleBlue::classify(const BleParsed* p) {
    // 1) Manufacturer-specific (mais específico)
    if (p->has_mfr) {
        switch (p->company_id) {
            case BLE_CID_APPLE:
                switch (p->cont_type) {
                    case APPLE_CONT_IBEACON:   return VND_IBEACON;
                    case APPLE_CONT_PROX_PAIR: return VND_AIRPODS;
                    case APPLE_CONT_FINDMY:    return VND_APPLE_FINDMY;
                    default:                   return VND_APPLE_CONTINUITY;
                }
            case BLE_CID_MICROSOFT: return VND_MS_SWIFTPAIR;
            case BLE_CID_SAMSUNG:   return VND_SAMSUNG;
            case BLE_CID_ESPRESSIF: return VND_ESPRESSIF_BLUFI;
            case BLE_CID_XIAOMI:    return VND_XIAOMI;
            case BLE_CID_TILE:      return VND_TILE;
            default: break;
        }
    }
    // 2) Service UUID / service data
    for (uint8_t k = 0; k < p->n_svc; k++) {
        switch (p->svc_uuid16[k]) {
            case BLE_UUID_FASTPAIR:  return VND_GOOGLE_FASTPAIR;
            case BLE_UUID_EDDYSTONE: return VND_EDDYSTONE;
            case BLE_UUID_TILE:      return VND_TILE;
            case BLE_UUID_XIAOMI:    return VND_XIAOMI;
            case BLE_UUID_IMPROV:    return VND_IMPROV_WIFI;
            case BLE_UUID_BLUFI:     return VND_ESPRESSIF_BLUFI;
            case BLE_UUID_MESH_PROV:
            case BLE_UUID_MESH_PROXY: return VND_MESH;
            default: break;
        }
    }
    // 3) Flipper por prefixo de nome (override de genéricos)
    if (p->has_name && strncmp(p->name, "Flipper", 7) == 0) return VND_FLIPPER;
    // 4) NUS 128-bit
    if (p->vendor == VND_NUS) return VND_NUS;
    // 5) HID genérico
    if (p->hid) return VND_HID_HOGP;
    return VND_UNKNOWN;
}

// ===========================================================================
// onAdv — chamado pelo callback de scan
// ===========================================================================
void BleBlue::onAdv(const uint8_t* payload, uint16_t len,
                    const uint8_t addr[6], uint8_t addr_type, int8_t rssi) {
    BleParsed pa;
    if (!parseAdv(payload, len, &pa)) return;   // AD vazio/ruim: ignora

    BleDevice* e = _slotFor(addr);
    if (!e) return;
    if (!e->used) {
        memset(e, 0, sizeof(*e));
        e->used = true;
        memcpy(e->addr, addr, 6);
        e->first_seen = millis();
    }
    e->addr_type = addr_type;
    e->addr_kind = bleClassifyAddr(addr, addr_type);
    e->rssi = rssi;
    // preserva o nome se este advertising não trouxe um (nome costuma vir no scan-resp)
    if (pa.has_name || !e->ad.has_name) e->ad = pa;
    else { BleParsed keepName = pa; strncpy(keepName.name, e->ad.name, sizeof(keepName.name)-1);
           keepName.name[sizeof(keepName.name)-1]=0; keepName.has_name = e->ad.has_name; e->ad = keepName; }
    e->last_seen = millis();
    e->seen_count++;
    if (_obs) _obs(e, &pa);
}

// ===========================================================================
// Inventário
// ===========================================================================
BleDevice* BleBlue::_slotFor(const uint8_t addr[6]) {
    int free_i = -1;
    for (int i = 0; i < MAX_DEV; i++) {
        if (_dev[i].used && macEq(_dev[i].addr, addr)) return &_dev[i];
        if (!_dev[i].used && free_i < 0) free_i = i;
    }
    if (free_i >= 0) return &_dev[free_i];
    int oldest = 0;                              // recicla o mais antigo
    for (int i = 1; i < MAX_DEV; i++)
        if (_dev[i].last_seen < _dev[oldest].last_seen) oldest = i;
    return &_dev[oldest];
}

int BleBlue::count() { int n=0; for (int i=0;i<MAX_DEV;i++) if (_dev[i].used) n++; return n; }
const BleDevice* BleBlue::get(int idx) {
    int n=0; for (int i=0;i<MAX_DEV;i++){ if(!_dev[i].used) continue; if(n==idx) return &_dev[i]; n++; }
    return nullptr;
}
const BleDevice* BleBlue::findByAddr(const uint8_t addr[6]) {
    for (int i=0;i<MAX_DEV;i++) if (_dev[i].used && macEq(_dev[i].addr, addr)) return &_dev[i];
    return nullptr;
}
void BleBlue::clear() { memset(_dev, 0, sizeof(_dev)); }
int BleBlue::countHid() { int n=0; for(int i=0;i<MAX_DEV;i++) if(_dev[i].used && _dev[i].ad.hid) n++; return n; }
int BleBlue::countVendor(BleVendor v){ int n=0; for(int i=0;i<MAX_DEV;i++) if(_dev[i].used && _dev[i].ad.vendor==v) n++; return n; }

// ===========================================================================
// Relatório
// ===========================================================================
String BleBlue::reportSection() {
    String s = "## BLE inventory\n";
    s += "_BLUE — observação passiva. "; s += String(count()); s += " device(s), ";
    s += String(countHid()); s += " HID._\n";
    char mac[18];
    for (int i = 0; i < MAX_DEV; i++) {
        const BleDevice* e = &_dev[i];
        if (!e->used) continue;
        macToStr(e->addr, mac);
        s += "\n### "; s += (e->ad.has_name ? e->ad.name : "(sem nome)");
        s += "  `"; s += mac; s += "`\n";
        s += "- addr: "; s += bleAddrKindName(e->addr_kind);
        s += " | rssi: "; s += String((int)e->rssi); s += " dBm";
        s += " | visto: "; s += String((unsigned)e->seen_count); s += "x\n";
        s += "- vendor: **"; s += bleVendorName(e->ad.vendor); s += "**";
        if (e->ad.hid) s += " (HID/HOGP)";
        s += "\n";
        if (e->ad.has_mfr) { char h[16]; snprintf(h,sizeof(h),"0x%04X",e->ad.company_id);
                             s += "- company_id: "; s += h;
                             if (e->ad.company_id==BLE_CID_APPLE){ char c[10]; snprintf(c,sizeof(c)," cont=0x%02X",e->ad.cont_type); s+=c; }
                             s += "\n"; }
        if (e->ad.n_svc) { s += "- svc16:"; for(uint8_t k=0;k<e->ad.n_svc;k++){ char u[8]; snprintf(u,sizeof(u)," %04X",e->ad.svc_uuid16[k]); s+=u; } s+="\n"; }
        if (e->ad.has_tx_power) { s += "- tx_power: "; s += String((int)e->ad.tx_power); s += " dBm\n"; }
    }
    if (count() == 0) s += "\n_(nenhum device observado ainda)_\n";
    return s;
}

bool BleBlue::writeReport(const char* path) {
    if (!SD_MMC.exists("/reports")) SD_MMC.mkdir("/reports");
    File f = SD_MMC.open(path, FILE_WRITE);
    if (!f) { Serial.println("[BleBlue] relatório: falha ao escrever"); return false; }
    f.print(reportSection());
    f.close();
    Serial.printf("[BleBlue] relatório BLE em %s\n", path);
    return true;
}
