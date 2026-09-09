#ifndef BLE_IDS_H
#define BLE_IDS_H
//
// BleIds.h — BLE IDS (BLUE, receive-only). Consome os eventos do BleBlue
// (via observer) e gera alertas: spam por tipo, HID inesperado, stalker
// (Find My/Tile + IMU), assinatura Flipper, baseline. Mais foxhunt (RSSI+beep).
// NÃO transmite, NÃO conecta, NÃO ataca.
//
// Janelas são baseadas em CONTAGEM com decaimento no tick() (~1/s), não em
// tempo decorrido — determinístico e testável.
//
#include "BleCommon.h"

class BleIds {
public:
    typedef void (*AlertCb)(const char* msg, uint32_t color);

    static void init();
    static void setAlertCallback(AlertCb cb);

    // Engancha no BleBlue::setObserver.
    static void onEvent(const BleDevice* e, const BleParsed* p);
    // Chamar ~1x/s: decai janelas de spam, envelhece episódios de movimento.
    static void tick();
    // main chama quando o IMU acusa movimento (usa o checkShake existente).
    static void notifyMotion();

    // Baseline (dispositivos conhecidos ficam fora do stalker).
    static void loadBaseline(const char* path = "/allowlist/ble.txt");
    static bool isKnown(const uint8_t addr[6]);

    // Foxhunt.
    static void foxhuntStart(const uint8_t addr[6]);
    static void foxhuntStop();
    static bool foxhuntActive();
    static int  foxhuntRssi();           // último RSSI do alvo (0 = não visto)
    static int  foxhuntBeepIntervalMs(); // 0 = alvo não visto ainda

    static int  alertCount();

    // Helper puro (testável): RSSI -> intervalo de beep (ms). Mais forte = mais rápido.
    static int  beepIntervalForRssi(int rssi);

private:
    static void _emit(const char* msg, uint32_t color);
};

#endif // BLE_IDS_H
