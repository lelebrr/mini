#ifndef BLE_COMMON_H
#define BLE_COMMON_H
//
// BleCommon.h — modelo de dados e constantes do inventário BLE (BLUE).
//
// SENTINEL / Mini Lele — BLE é BLUE-first: scan + parser de AD à prova de
// truncar + classificação de vendor + inventário. NADA aqui transmite ou ataca.
//
// Nota de stack: o firmware Mini Lele usa Bluedroid (BLEDevice.h). A spec
// SENTINEL-S3 pede NimBLE; o parser abaixo é independente de stack (opera sobre
// os bytes crus do advertising), então serve para os dois.
//
#include <Arduino.h>
#include <stdint.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Tipos de AD (GAP / Assigned Numbers)
// ---------------------------------------------------------------------------
#define BLE_AD_FLAGS               0x01
#define BLE_AD_UUID16_INCOMPLETE   0x02
#define BLE_AD_UUID16_COMPLETE     0x03
#define BLE_AD_UUID128_INCOMPLETE  0x06
#define BLE_AD_UUID128_COMPLETE    0x07
#define BLE_AD_NAME_SHORT          0x08
#define BLE_AD_NAME_COMPLETE       0x09
#define BLE_AD_TX_POWER            0x0A
#define BLE_AD_SVC_DATA_16         0x16
#define BLE_AD_SVC_DATA_32         0x20
#define BLE_AD_SVC_DATA_128        0x21
#define BLE_AD_APPEARANCE          0x19
#define BLE_AD_MANUFACTURER        0xFF

// ---------------------------------------------------------------------------
// Company IDs (Bluetooth SIG) — lidos LE nos bytes do ar, guardados em host order
// ---------------------------------------------------------------------------
#define BLE_CID_APPLE      0x004C
#define BLE_CID_MICROSOFT  0x0006
#define BLE_CID_SAMSUNG    0x0075
#define BLE_CID_GOOGLE     0x00E0
#define BLE_CID_TILE       0x0157
#define BLE_CID_ESPRESSIF  0x02E5
#define BLE_CID_XIAOMI     0x038F
#define BLE_CID_NORDIC     0x0059

// ---------------------------------------------------------------------------
// Service UUIDs 16-bit relevantes
// ---------------------------------------------------------------------------
#define BLE_UUID_HID          0x1812  // HID over GATT (HOGP)
#define BLE_UUID_DEVINFO      0x180A
#define BLE_UUID_MESH_PROV    0x1827
#define BLE_UUID_MESH_PROXY   0x1828
#define BLE_UUID_FASTPAIR     0xFE2C  // Google Fast Pair
#define BLE_UUID_EDDYSTONE    0xFEAA
#define BLE_UUID_TILE         0xFEED
#define BLE_UUID_XIAOMI       0xFE95
#define BLE_UUID_IMPROV       0x4677  // Improv Wi-Fi (ESPHome)
#define BLE_UUID_BLUFI        0xFFFF  // Espressif BluFi (custom)

// Apple Continuity: 1º byte da manufacturer data (após company id 0x004C)
#define APPLE_CONT_IBEACON       0x02
#define APPLE_CONT_AIRPRINT      0x03
#define APPLE_CONT_AIRDROP       0x05
#define APPLE_CONT_HOMEKIT       0x06
#define APPLE_CONT_PROX_PAIR     0x07  // AirPods / proximity pairing
#define APPLE_CONT_HEY_SIRI      0x08
#define APPLE_CONT_AIRPLAY_TGT   0x09
#define APPLE_CONT_AIRPLAY_SRC   0x0A
#define APPLE_CONT_MAGIC_SWITCH  0x0B
#define APPLE_CONT_HANDOFF       0x0C
#define APPLE_CONT_TETHER_TGT    0x0D
#define APPLE_CONT_TETHER_SRC    0x0E
#define APPLE_CONT_NEARBY_ACTION 0x0F
#define APPLE_CONT_NEARBY_INFO   0x10
#define APPLE_CONT_FINDMY        0x12  // offline finding

// ---------------------------------------------------------------------------
// Classificação de vendor
// ---------------------------------------------------------------------------
enum BleVendor {
    VND_UNKNOWN = 0,
    VND_APPLE_CONTINUITY, VND_APPLE_FINDMY, VND_AIRPODS, VND_IBEACON,
    VND_MS_SWIFTPAIR, VND_GOOGLE_FASTPAIR, VND_EDDYSTONE, VND_SAMSUNG,
    VND_TILE, VND_XIAOMI, VND_ESPRESSIF_BLUFI, VND_IMPROV_WIFI, VND_FLIPPER,
    VND_HID_HOGP, VND_MESH, VND_NUS
};

static inline const char* bleVendorName(BleVendor v) {
    switch (v) {
        case VND_APPLE_CONTINUITY: return "Apple Continuity";
        case VND_APPLE_FINDMY:     return "Apple Find My";
        case VND_AIRPODS:          return "AirPods";
        case VND_IBEACON:          return "iBeacon";
        case VND_MS_SWIFTPAIR:     return "MS Swift Pair";
        case VND_GOOGLE_FASTPAIR:  return "Google Fast Pair";
        case VND_EDDYSTONE:        return "Eddystone";
        case VND_SAMSUNG:          return "Samsung";
        case VND_TILE:             return "Tile";
        case VND_XIAOMI:           return "Xiaomi";
        case VND_ESPRESSIF_BLUFI:  return "Espressif BluFi";
        case VND_IMPROV_WIFI:      return "Improv Wi-Fi";
        case VND_FLIPPER:          return "Flipper Zero";
        case VND_HID_HOGP:         return "HID (HOGP)";
        case VND_MESH:             return "BLE Mesh";
        case VND_NUS:              return "Nordic UART";
        default:                   return "?";
    }
}

// Tipo de endereço (espelha esp_ble_addr_type_t)
enum BleAddrKind { BLE_ADDR_PUBLIC = 0, BLE_ADDR_RANDOM_STATIC, BLE_ADDR_RPA,
                   BLE_ADDR_NRPA, BLE_ADDR_UNKNOWN };

// Resultado do parser de um advertising.
struct BleParsed {
    char      name[32];
    bool      has_name;
    int8_t    tx_power;
    bool      has_tx_power;
    uint8_t   flags;
    bool      has_flags;
    uint16_t  svc_uuid16[8];
    uint8_t   n_svc;
    bool      has_uuid128;
    uint16_t  company_id;
    bool      has_mfr;
    uint8_t   cont_type;      // Apple Continuity type (0 se n/a)
    BleVendor vendor;
    bool      hid;            // expõe serviço HID 0x1812
};

// Entrada de inventário por dispositivo.
struct BleDevice {
    uint8_t     addr[6];
    uint8_t     addr_type;    // valor cru do stack
    BleAddrKind addr_kind;    // derivado (public/random-static/RPA/NRPA)
    int8_t      rssi;
    BleParsed   ad;
    uint32_t    first_seen, last_seen, seen_count;
    bool        used;
};

// Classifica o tipo de endereço aleatório pelos 2 bits mais altos do MSB.
static inline BleAddrKind bleClassifyAddr(const uint8_t addr[6], uint8_t addr_type) {
    if (addr_type == 0) return BLE_ADDR_PUBLIC;
    uint8_t top = addr[0] >> 6;         // addr[0] = MSB (big-endian aqui)
    if (top == 0x03) return BLE_ADDR_RANDOM_STATIC;
    if (top == 0x01) return BLE_ADDR_RPA;      // resolvable private
    if (top == 0x00) return BLE_ADDR_NRPA;     // non-resolvable private
    return BLE_ADDR_UNKNOWN;
}
static inline const char* bleAddrKindName(BleAddrKind k) {
    switch (k) {
        case BLE_ADDR_PUBLIC:        return "public";
        case BLE_ADDR_RANDOM_STATIC: return "random-static";
        case BLE_ADDR_RPA:           return "RPA";
        case BLE_ADDR_NRPA:          return "NRPA";
        default:                     return "?";
    }
}

#endif // BLE_COMMON_H
