#ifndef BLE_BLUE_H
#define BLE_BLUE_H
//
// BleBlue.h — BLE BLUE (defensivo): parser de AD, classificação de vendor e
// inventário. Alimentado pelo payload cru do scan (PwnBLE / Bluedroid).
// NÃO transmite, NÃO conecta, NÃO ataca.
//
#include "BleCommon.h"

class BleBlue {
public:
    static const int MAX_DEV = 96;

    static void init();

    // Alimentado pelo callback de scan (contexto do host BLE).
    static void onAdv(const uint8_t* payload, uint16_t len,
                      const uint8_t addr[6], uint8_t addr_type, int8_t rssi);

    // Observer opcional (BleIds engancha aqui) — chamado a cada advertising.
    typedef void (*Observer)(const BleDevice* dev, const BleParsed* parsed);
    static void setObserver(Observer cb);

    // ---- Parser puro (à prova de truncar) — usado também nos testes ----------
    static bool parseAdv(const uint8_t* payload, uint16_t len, BleParsed* out);
    // Classificação de vendor a partir de um BleParsed já preenchido.
    static BleVendor classify(const BleParsed* p);

    // ---- Inventário ----------------------------------------------------------
    static int count();
    static const BleDevice* get(int i);
    static const BleDevice* findByAddr(const uint8_t addr[6]);
    static void clear();
    static int countHid();
    static int countVendor(BleVendor v);

    // ---- Relatório -----------------------------------------------------------
    static String reportSection();
    static bool writeReport(const char* path = "/reports/ble_inventory.md");

private:
    static BleDevice _dev[MAX_DEV];
    static Observer _obs;
    static BleDevice* _slotFor(const uint8_t addr[6]);
};

#endif // BLE_BLUE_H
