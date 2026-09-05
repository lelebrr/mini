#ifndef WIFI_TOOLS_H
#define WIFI_TOOLS_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <SD_MMC.h>
#include "FS.h"
#include <vector>
#include "core/PwnPet.h"
#include "Gamification.h"

/**
 * WiFiTools
 * ---------
 * Sniffer promíscuo com:
 *   - Lista de dispositivos próximos (probe requests).
 *   - CAPTURA REAL em .pcap (linktype 105 / IEEE 802.11): beacons + EAPOL.
 *   - Detecção de handshake WPA (frames EAPOL, EtherType 0x888E).
 *
 * Design seguro: o callback promíscuo roda em contexto sensível do driver WiFi,
 * então ele APENAS copia frames de interesse para um buffer em RAM. A gravação
 * no cartão SD (lenta) é feita depois, no loop principal, via flush().
 *
 * Compatível com o modo AP: startSnifferPassive() liga o modo promíscuo SEM
 * derrubar o SoftAP (captura no canal atual do AP). startSnifferScan() faz um
 * scan dedicado com troca de canal (derruba o AP — use só sob demanda).
 */

struct SniffedDevice {
    String        mac;
    int           rssi;
    unsigned long last_seen;
};

// Frame enfileirado para gravação posterior.
struct CapFrame {
    uint16_t len;
    bool     eapol;
    uint32_t ts_sec;
    uint32_t ts_usec;
    uint8_t  data[300];   // snap length (suficiente p/ EAPOL/beacon)
};

// Evento POD de dispositivo (probe request) — copiado no callback SEM heap;
// a conversão para String/vector acontece depois, no loop principal (flush()).
struct DevEvent {
    uint8_t mac[6];
    int8_t  rssi;
};

class WiFiTools {
public:
    static std::vector<SniffedDevice> nearby_devices;

    // ---- Buffer circular de captura (preenchido no callback, drenado no loop) ----
    static const int   CAP_QUEUE = 24;
    static CapFrame    cap_queue[CAP_QUEUE];
    static volatile int cap_head;
    static volatile int cap_tail;
    // Fila de eventos de dispositivos (probe requests, POD — ISR-safe)
    static const int   DEV_QUEUE = 32;
    static DevEvent    dev_queue[DEV_QUEUE];
    static volatile int dev_head;
    static volatile int dev_tail;
    static volatile uint32_t eapol_count;
    static volatile uint32_t frames_captured;
    // Spinlock compartilhado: callback roda na task do driver WiFi (outro core
    // em relação ao loop), então filas/contadores precisam de seção crítica.
    static portMUX_TYPE cap_mux;
    static bool        sniffing;
    static bool        pcap_header_written;
    static String      pcap_path;

    // ---- Controle do sniffer ----
    static void startSnifferPassive() {
        // Não muda o modo do WiFi (funciona junto com o AP no canal atual).
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_promiscuous_rx_cb(&wifiSnifferCallback);
        sniffing = true;
        Serial.println("[Sniffer] Passivo ligado (canal atual, AP preservado).");
    }

    static void startSnifferScan() {
        // Scan dedicado (STA + troca de canal). Derruba o AP.
        WiFi.mode(WIFI_MODE_STA);
        WiFi.disconnect(true);
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_promiscuous_rx_cb(&wifiSnifferCallback);
        sniffing = true;
        Serial.println("[Sniffer] Scan dedicado ligado (com troca de canal).");
    }

    // Compat: mantém o nome antigo (usa modo passivo p/ não quebrar o AP/WebUI).
    static void startSniffer() { startSnifferPassive(); }

    static void stopSniffer() {
        esp_wifi_set_promiscuous(false);
        sniffing = false;
        Serial.println("[Sniffer] Desligado.");
    }

    static bool isSniffing() { return sniffing; }
    static uint32_t getHandshakeCount() { return eapol_count; }

    // Troca de canal (1..13) — usar só no modo scan dedicado.
    static void setChannel(uint8_t ch) {
        esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
    }

    // ---- pcap ----
    static void beginNewCapture() {
        if (!SD_MMC.exists("/capturas")) SD_MMC.mkdir("/capturas");
        pcap_path = "/capturas/cap_" + String(millis()) + ".pcap";
        pcap_header_written = false;
    }

    // Drena o buffer para o SD (chamar no loop principal).
    static void flush() {
        // 1) Eventos de dispositivos: TODO trabalho com heap/String fica AQUI
        //    no loop — nunca no callback do driver WiFi (evita corrupção de heap).
        while (dev_tail != dev_head) {
            DevEvent e = dev_queue[dev_tail];
            dev_tail = (dev_tail + 1) % DEV_QUEUE;
            char macStr[18];
            snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                     e.mac[0], e.mac[1], e.mac[2], e.mac[3], e.mac[4], e.mac[5]);
            updateDevice(macStr, e.rssi);
        }

        // 2) Frames -> pcap no SD.
        while (cap_tail != cap_head) {
            CapFrame &f = cap_queue[cap_tail];
            // Timestamp "segundos" resolvido na drenagem: time() não é ISR-safe.
            f.ts_sec = (uint32_t)time(nullptr);
            writeFrameToPcap(f);
            if (f.eapol) {
                Serial.println("[Sniffer] EAPOL capturado -> handshake!");
                PwnPet::addHandshake(false);
                Gamification::registerHandshake();
            }
            cap_tail = (cap_tail + 1) % CAP_QUEUE;
        }
    }

    // Rotaciona um arquivo se ele passar de `maxBytes` (mantém um .old).
    static void rotateIfBig(const char *path, size_t maxBytes) {
        if (!SD_MMC.exists(path)) return;
        File f = SD_MMC.open(path);
        size_t sz = f ? f.size() : 0;
        if (f) f.close();
        if (sz > maxBytes) {
            String oldp = String(path) + ".old";
            if (SD_MMC.exists(oldp)) SD_MMC.remove(oldp);
            SD_MMC.rename(path, oldp);
        }
    }

    // Persiste um snapshot da lista de dispositivos (chamar ~1x/min), com rotação.
    static void persistDevices() {
        const char *path = "/arquivos_cartao_sd/macs_detectados.txt";
        rotateIfBig(path, 128 * 1024);
        File f = SD_MMC.open(path, FILE_APPEND);
        if (!f) return;
        long ts = (long)time(nullptr);
        for (auto &d : nearby_devices) {
            f.printf("%s,%d,%ld\n", d.mac.c_str(), d.rssi, ts);
        }
        f.close();
    }

    static String getSystemStats() {
        float    temp      = temperatureRead();
        uint32_t free_heap = esp_get_free_heap_size();
        uint32_t uptime    = millis() / 1000;
        char buffer[96];
        snprintf(buffer, sizeof(buffer), "Temp: %.0fC RAM: %luKB\nUp: %lus",
                 temp, free_heap / 1024UL, uptime);
        return String(buffer);
    }

    static String getSnifferText() {
        String s = "Dispositivos:\n";
        for (auto &dev : nearby_devices) s += dev.mac + " (" + String(dev.rssi) + ")\n";
        if (nearby_devices.empty()) s += "Nenhum...";
        return s;
    }

    // ---- Callback promíscuo: roda na TASK do driver WiFi (não em ISR) ----
    // taskENTER_CRITICAL_FROM_ISR não é necessário aqui (não é ISR), e
    // IRAM_ATTR não funciona porque os globais (cap_queue/dev_queue/cap_mux)
    // ficam em DRAM regular e o assembler Xtensa reclama com
    // "dangerous relocation: l32r: literal placed after use". O critical
    // section abaixo (FreeRTOS) já desabilita o preempção no caller.
    static void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type) {
        auto *pkt  = (wifi_promiscuous_pkt_t *)buf;
        uint8_t *d = pkt->payload;
        int len    = pkt->rx_ctrl.sig_len;
        if (len < 24) return;

        uint8_t ftype    = (d[0] & 0x0C) >> 2;   // 0 mgmt, 1 ctrl, 2 data
        uint8_t fsubtype = (d[0] & 0xF0) >> 4;

        // Probe request (mgmt/4): enfileira evento POD — heap/String no flush().
        if (ftype == 0 && fsubtype == 4) {
            enqueueDev(d + 10, pkt->rx_ctrl.rssi);
            return;
        }

        // Beacon (mgmt/8): guarda alguns para o pcap ficar utilizável (SSID).
        if (ftype == 0 && fsubtype == 8) {
            static uint32_t beacon_every = 0;
            if ((beacon_every++ % 20) == 0) enqueue(d, len, false);  // amostragem
            return;
        }

        // Data frame: procura EAPOL (EtherType 0x888E após LLC/SNAP).
        if (ftype == 2) {
            int hdr = 24;
            if (fsubtype & 0x08) hdr += 2;             // QoS Data => +2 (QoS Ctrl)
            if (len >= hdr + 8) {
                if (d[hdr] == 0xAA && d[hdr + 1] == 0xAA && d[hdr + 2] == 0x03 &&
                    d[hdr + 6] == 0x88 && d[hdr + 7] == 0x8E) {
                    enqueue(d, len, true);             // EAPOL!
                }
            }
        }
    }

private:
    static void updateDevice(const char *macStr, int rssi) {
        String mac(macStr);
        for (auto &dev : nearby_devices) {
            if (dev.mac.equalsIgnoreCase(mac)) { dev.rssi = rssi; dev.last_seen = millis(); return; }
        }
        if (nearby_devices.size() >= 50) nearby_devices.erase(nearby_devices.begin());
        nearby_devices.push_back({mac, rssi, millis()});
    }

    // Enfileira evento de dispositivo (POD, sem heap). Roda na task do WiFi.
    static void enqueueDev(const uint8_t *mac, int rssi) {
        int next = (dev_head + 1) % DEV_QUEUE;
        if (next == dev_tail) return;                  // fila cheia -> descarta
        portENTER_CRITICAL(&cap_mux);
        DevEvent &e = dev_queue[dev_head];
        memcpy(e.mac, mac, 6);
        e.rssi = (int8_t)rssi;
        dev_head = next;
        portEXIT_CRITICAL(&cap_mux);
    }

    static void enqueue(uint8_t *d, int len, bool eapol) {
        int next = (cap_head + 1) % CAP_QUEUE;
        if (next == cap_tail) return;                  // fila cheia -> descarta
        portENTER_CRITICAL(&cap_mux);
        CapFrame &f = cap_queue[cap_head];
        f.len   = (len > (int)sizeof(f.data)) ? sizeof(f.data) : len;
        f.eapol = eapol;
        f.ts_usec = micros() % 1000000UL;              // micros() é safe
        memcpy(f.data, d, f.len);
        cap_head = next;
        frames_captured++;
        if (eapol) eapol_count++;
        portEXIT_CRITICAL(&cap_mux);
    }

    // Escreve um frame no arquivo .pcap (cabeçalho global na 1ª vez).
    static void writeFrameToPcap(CapFrame &f) {
        if (pcap_path.length() == 0) beginNewCapture();
        File file = SD_MMC.open(pcap_path, pcap_header_written ? FILE_APPEND : FILE_WRITE);
        if (!file) return;
        if (!pcap_header_written) {
            uint8_t gh[24];
            uint32_t magic = 0xa1b2c3d4; uint16_t vmaj = 2, vmin = 4;
            uint32_t zone = 0, sig = 0, snap = 65535, net = 105; // LINKTYPE_IEEE802_11
            memcpy(gh + 0, &magic, 4); memcpy(gh + 4, &vmaj, 2); memcpy(gh + 6, &vmin, 2);
            memcpy(gh + 8, &zone, 4); memcpy(gh + 12, &sig, 4);
            memcpy(gh + 16, &snap, 4); memcpy(gh + 20, &net, 4);
            file.write(gh, 24);
            pcap_header_written = true;
        }
        uint8_t rh[16];
        uint32_t incl = f.len, orig = f.len;
        memcpy(rh + 0, &f.ts_sec, 4); memcpy(rh + 4, &f.ts_usec, 4);
        memcpy(rh + 8, &incl, 4);     memcpy(rh + 12, &orig, 4);
        file.write(rh, 16);
        file.write(f.data, f.len);
        file.close();
    }

    static void wifiSnifferCallback(void *buf, wifi_promiscuous_pkt_type_t type) {
        WiFiTools::promiscuous_rx_cb(buf, type);
    }
};

// Definições estáticas em core_singletons.cpp
#endif
