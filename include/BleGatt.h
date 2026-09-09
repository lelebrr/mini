#ifndef BLE_GATT_H
#define BLE_GATT_H
//
// BleGatt.h — GATT audit BLUE, SÓ allowlist. Conecta como central a um alvo
// autorizado, enumera serviços/characteristics, marca WRITE exposta e gera
// relatório gatt-<MAC>.md. Read-only: NÃO escreve em characteristic, NÃO ataca.
//
// O corpo real (Bluedroid BLEClient) compila no firmware (ARDUINO). No host de
// teste, a lógica pura (allowlist, props, relatório) é compilada e testada; a
// conexão em si não roda no host.
//
#include "BleCommon.h"

// Bits de propriedade de characteristic (BLE spec)
#define GATT_PROP_BROADCAST   0x01
#define GATT_PROP_READ        0x02
#define GATT_PROP_WRITE_NR    0x04
#define GATT_PROP_WRITE       0x08
#define GATT_PROP_NOTIFY      0x10
#define GATT_PROP_INDICATE    0x20
#define GATT_PROP_SIGNED_WR   0x40

struct GattChar {
    char    svc_uuid[40];
    char    chr_uuid[40];
    uint8_t props;
    bool    open_write;   // heurística: writable enumerável -> revisar
};

class BleGatt {
public:
    static const int MAX_CHARS = 64;

    static void init();

    // Allowlist (mesma /allowlist/ble.txt = endereços autorizados).
    static int  loadAllowlist(const char* path = "/allowlist/ble.txt");
    static bool isAllowed(const uint8_t addr[6]);

    // Auditoria real (central Bluedroid). Retorna nº de characteristics,
    // -1 recusado (fora da allowlist), -2 falha de conexão.
    static int  audit(const uint8_t addr[6]);

    // Acesso aos resultados da última auditoria.
    static int  count();
    static const GattChar* get(int i);

    // ---- Puros (testáveis no host) ------------------------------------------
    static bool isOpenWrite(uint8_t props);
    static void propsString(uint8_t props, char* out, size_t cap);  // "R W Wnr N I"
    static String formatReport(const uint8_t addr[6], const GattChar* chars, int n);

private:
    static GattChar _chars[MAX_CHARS];
    static int      _n;
    static void _auditLog(const uint8_t addr[6], const char* event, const char* detail);
    static bool _writeReport(const uint8_t addr[6]);
};

#endif // BLE_GATT_H
