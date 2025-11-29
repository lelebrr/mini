#ifndef WIFI_TOOLS_H
#define WIFI_TOOLS_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <vector>
#include "core/PwnPet.h"
#include "core/Gamification.h"

// Estrutura para dispositivos encontrados
struct WiFiDevice {
    String mac;
    String vendor; // Placeholder
    int rssi;
    unsigned long last_seen;
};

class WiFiTools {
public:
    static std::vector<WiFiDevice> nearby_devices;

    static void startSniffer() {
        WiFi.mode(WIFI_MODE_APSTA); // APSTA permite injeção + WebUI
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb);
    }

    static void promiscuous_rx_cb(void* buf, wifi_promiscuous_pkt_type_t type) {
        wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
        uint8_t* frame = pkt->payload;
        int len = pkt->rx_ctrl.sig_len;

        // Frame Control (Bytes 0-1)
        uint8_t frame_type = (frame[0] & 0x0C) >> 2;
        uint8_t frame_subtype = (frame[0] & 0xF0) >> 4;

        // --- 1. Management Frames (Beacons / Probes) ---
        if (frame_type == 0) {
            // Probe Request (Subtype 4)
            if (frame_subtype == 4) {
                // Source Addr está no offset 10
                char macStr[18];
                snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                    frame[10], frame[11], frame[12], frame[13], frame[14], frame[15]);

                // Add/Update List
                bool found = false;
                for (auto &dev : nearby_devices) {
                    if (dev.mac.equalsIgnoreCase(macStr)) {
                        dev.rssi = pkt->rx_ctrl.rssi;
                        dev.last_seen = millis();
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    nearby_devices.push_back({String(macStr), "Unknown", pkt->rx_ctrl.rssi, millis()});
                    // Limita tamanho
                    if (nearby_devices.size() > 50) nearby_devices.erase(nearby_devices.begin());
                }
            }
        }

        // --- 2. Data Frames (EAPOL Handshakes) ---
        if (frame_type == 2) {
            // Check for EAPOL (0x888e in LLC/SNAP header usually around offset 30+ depending on QoS)
            // Simplificado: Check payload patterns se possivel
            // Para EAPOL Handshake, precisamos ver se EtherType é 0x888e

            // Offset varia se tem QoS (2 bytes)
            int header_len = 24;
            uint16_t frame_ctrl = frame[0] | (frame[1] << 8);
            if ((frame_ctrl & 0x0080) != 0) header_len += 2; // QoS present

            // Verifica LLC (8 bytes) -> EtherType
            if (len > header_len + 8) {
                uint8_t* llc = frame + header_len;
                // EAPOL EtherType = 0x888E
                if (llc[6] == 0x88 && llc[7] == 0x8E) {
                    // EAPOL Detectado!
                    // Verificar Key Type (1/4, 2/4, 3/4, 4/4) no payload EAPOL
                    // ... logica complexa de parsing ...

                    Serial.println("[Sniffer] EAPOL Frame Detected!");

                    // Alimentar Pet
                    PwnPet::addHandshake(false);
                    Gamification::registerHandshake();

                    // TODO: Salvar PCAP
                }
            }
        }
    }
};

std::vector<WiFiDevice> WiFiTools::nearby_devices;

#endif
