#ifndef WPS_COMMON_H
#define WPS_COMMON_H
//
// WpsCommon.h — modelo de dados e constantes compartilhadas do modulo WPS.
//
// SENTINEL / mini — WPS é BLUE-first (inventario/higiene/baseline/relatorio) e
// RED-limitado (laboratorio, ARM + allowlist + auditoria). NAO ha Reaver,
// NAO ha Pixie Dust, NAO ha brute-force online de PIN, NAO ha DoS de WPS.
// Ver docs/WPS.md para o contrato completo.
//
#include <Arduino.h>
#include <stdint.h>

// ---------------------------------------------------------------------------
// Vendor IE do Wi-Fi Simple Config (WSC / WPS)
//   Element ID 221 (0xDD), OUI 00:50:F2, OUI type 0x04
// ---------------------------------------------------------------------------
#define WPS_IE_ELEMENT_ID   0xDD
static const uint8_t WPS_OUI[3]     = { 0x00, 0x50, 0xF2 };
#define WPS_OUI_TYPE        0x04

// OUI da WFA usado nas Vendor Extensions (subelemento Version2 do WSC 2.0)
static const uint8_t WPS_WFA_OUI[3] = { 0x00, 0x37, 0x2A };

// Atributos WSC (type big-endian, 2 bytes) que fazemos parse
#define WPS_ATTR_VERSION            0x104A  // 1 byte: 0x10=1.0, 0x20=2.0
#define WPS_ATTR_WPS_STATE          0x1044  // 1 byte: 0x01 nao-config, 0x02 config
#define WPS_ATTR_AP_SETUP_LOCKED    0x1057  // 1 byte: 0x01 = locked
#define WPS_ATTR_CONFIG_METHODS     0x1008  // 2 bytes bitmask
#define WPS_ATTR_DEVICE_PW_ID       0x1012  // 2 bytes
#define WPS_ATTR_MANUFACTURER       0x1021  // ASCII
#define WPS_ATTR_MODEL_NAME         0x1023  // ASCII
#define WPS_ATTR_MODEL_NUMBER       0x1024  // ASCII
#define WPS_ATTR_DEVICE_NAME        0x1011  // ASCII
#define WPS_ATTR_UUID_E             0x1047  // 16 bytes
#define WPS_ATTR_VENDOR_EXTENSION   0x1049  // WFA subelements
#define WPS_WFA_SUBELEM_VERSION2    0x00    // dentro da Vendor Extension WFA

// Bits de Config Methods (WSC spec)
#define WPS_CM_USBA         0x0001
#define WPS_CM_ETHERNET     0x0002
#define WPS_CM_LABEL        0x0004
#define WPS_CM_DISPLAY      0x0008
#define WPS_CM_EXT_NFC      0x0010
#define WPS_CM_INT_NFC      0x0020
#define WPS_CM_NFC_IF       0x0040
#define WPS_CM_PUSHBUTTON   0x0080
#define WPS_CM_KEYPAD       0x0100
// WSC 2.0 adiciona bits combinados (mantidos p/ referencia)
#define WPS_CM_PHY_PBC      0x0280
#define WPS_CM_VIRT_PBC     0x0480
#define WPS_CM_PHY_DISPLAY  0x4008
#define WPS_CM_VIRT_DISPLAY 0x2008

// Estado WPS observado por AP para a classificacao de higiene.
enum WpsHygiene {
    WPS_HYG_OFF = 0,   // sem IE WPS -> higiene OK
    WPS_HYG_ON,        // WPS presente e NAO travado -> alerta ambar
    WPS_HYG_LOCKED     // WPS presente e ap_setup_locked -> menos exposto
};

// Informacao WPS extraida do IE de UM AP.
struct WpsInfo {
    bool     present;          // IE WPS encontrado
    uint8_t  version;          // atributo Version base (0x10 / 0x20), 0 se ausente
    bool     version2;         // subelemento Version2 (WSC 2.0) presente
    uint16_t config_methods;   // bitmask cru
    uint8_t  wps_state;        // 0x01 nao-config, 0x02 config, 0 desconhecido
    bool     ap_setup_locked;  // 0x1057 == 1
    // categorias derivadas de config_methods (spec pede PIN/PBC/NFC/keypad)
    bool     cm_pbc;
    bool     cm_pin;           // label | display
    bool     cm_keypad;
    bool     cm_nfc;
    char     manufacturer[33];
    char     model_name[33];
    char     model_number[33];
    char     device_name[33];
    uint8_t  uuid[16];
    bool     has_uuid;
};

static inline void wpsInfoClear(WpsInfo* w) { if (w) memset(w, 0, sizeof(WpsInfo)); }

// Entrada de inventario por AP.
struct WpsApEntry {
    uint8_t  bssid[6];
    char     ssid[33];
    int8_t   channel;
    int8_t   rssi;
    WpsInfo  wps;
    uint32_t first_seen;   // millis()
    uint32_t last_seen;    // millis()
    bool     pixie_info;   // nota INFO "legacy chipset, historically pixie-vulnerable"
    bool     used;         // slot ocupado
};

// Higiene derivada de um WpsInfo.
static inline WpsHygiene wpsHygieneOf(const WpsInfo* w) {
    if (!w || !w->present) return WPS_HYG_OFF;
    return w->ap_setup_locked ? WPS_HYG_LOCKED : WPS_HYG_ON;
}
static inline const char* wpsHygieneChip(WpsHygiene h) {
    switch (h) {
        case WPS_HYG_ON:     return "WPS-ON";
        case WPS_HYG_LOCKED: return "WPS-LOCKED";
        default:             return "WPS-OFF";
    }
}
// Cor sugerida p/ o chip (formato 0xRRGGBB usado por PwnUI).
static inline uint32_t wpsHygieneColor(WpsHygiene h) {
    switch (h) {
        case WPS_HYG_ON:     return 0xFFB400; // ambar: exposto
        case WPS_HYG_LOCKED: return 0x00C8FF; // azul: presente porem travado
        default:             return 0x00C864; // verde: sem WPS
    }
}


// Cores 0xRRGGBB para callbacks de UI (PwnUI::showAlert usa 0xRRGGBB).
#define SENTINEL_BLUE_COLOR   0x0096FFu
#define SENTINEL_RED_COLOR    0xFF3232u
#define SENTINEL_GREEN_COLOR  0x00C864u
#define SENTINEL_AMBER_COLOR  0xFFB400u

#endif // WPS_COMMON_H
