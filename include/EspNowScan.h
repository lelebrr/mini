#ifndef ESPNOW_SCAN_H
#define ESPNOW_SCAN_H
//
// EspNowScan.h — inventário ESP-NOW (BLUE, 100% passivo).
//
// ESP-NOW roda como "vendor specific action frame" 802.11 (categoria 0x7F, OUI
// Espressif 18:FE:34). Muitos IoT caseiros (fechaduras/relés/sensores) usam
// ESP-NOW SEM criptografia. Aqui apenas OBSERVAMOS os peers no ar. NÃO
// transmitimos, NÃO fazemos replay (isso é RED/gated e não está implementado).
//
// Alimentado pelo callback promíscuo (WiFiTools) via onFrame(); o parse pesado
// e a atualização do inventário ocorrem em poll() (task do loop).
//
#include <Arduino.h>
#include <stdint.h>

struct EspNowPeer {
    uint8_t  src[6];
    int8_t   rssi;
    uint8_t  channel;
    uint16_t last_len;     // tamanho do payload ESP-NOW
    bool     broadcast;    // destino FF:FF:FF:FF:FF:FF
    uint32_t first_seen, last_seen, count;
    bool     used;
};

class EspNowScan {
public:
    static const int MAX_PEERS = 48;

    static void init();

    // Contexto da task de Wi-Fi: detecta + enfileira (barato). Parse em poll().
    static void onFrame(const uint8_t* payload, uint16_t len, int8_t rssi, uint8_t channel);
    static void poll();   // loop task: drena o ring -> inventário

    // Detector puro (testável): retorna true se é ESP-NOW; preenche src/bcast/paylen.
    static bool isEspNow(const uint8_t* p, uint16_t len,
                         uint8_t src_out[6], bool* bcast_out, uint16_t* paylen_out);

    static int count();
    static const EspNowPeer* get(int i);
    static void clear();

    static String reportSection();
    static bool writeReport(const char* path = "/reports/espnow_inventory.md");

private:
    static EspNowPeer _peers[MAX_PEERS];
    static EspNowPeer* _slotFor(const uint8_t src[6]);
};

#endif // ESPNOW_SCAN_H
