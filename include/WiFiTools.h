#ifndef WIFI_TOOLS_H
#define WIFI_TOOLS_H

/**
 * WiFiTools.h
 * Ferramentas WiFi com caminhos traduzidos.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <SD_MMC.h>
#include "FS.h"
#include <vector>
#include "core/PwnPet.h"
#include "core/Gamification.h"

#define MAX_SNIFFED 50 // Aumentado para 50
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
                    if (nearby_devices.size() > MAX_SNIFFED) nearby_devices.erase(nearby_devices.begin());

                    // Log em /arquivos_cartao_sd/macs_detectados.txt
                    File f = SD_MMC.open("/arquivos_cartao_sd/macs_detectados.txt", FILE_APPEND);
                    if (f) {
                        f.printf("%s,%d,%lu\n", macStr, pkt->rx_ctrl.rssi, millis());
                        f.close();
                    }
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
    static void startSniffer() {
        WiFi.mode(WIFI_MODE_APSTA); // APSTA permite injeção + WebUI
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb);
    }

    static void stopSniffer() {
        esp_wifi_set_promiscuous(false);
    }

    static void saveHandshake(String ssid, String bssid) {
        // Caminho: /arquivos_cartao_sd/capturas/
        String filename = "/arquivos_cartao_sd/capturas/" + ssid + ".pcap";
        if (!SD_MMC.exists("/arquivos_cartao_sd/capturas")) SD_MMC.mkdir("/arquivos_cartao_sd/capturas");

        File f = SD_MMC.open(filename, FILE_WRITE);
        if (f) {
            f.println("PCAP DUMMY HEADER");
            f.close();
        }
    }

    static String getSystemStats() {
        float temp = temperatureRead();
        uint32_t free_heap = esp_get_free_heap_size();
        uint32_t uptime = millis() / 1000;
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "Temp: %.0fC RAM: %dKB\nUp: %ds", temp, free_heap/1024, uptime);
        return String(buffer);
    }

    static String getSnifferText() {
        String s = "Dispositivos:\n";
        for (auto &dev : nearby_devices) {
            s += dev.mac + " (" + String(dev.rssi) + ")\n";
        }
        if (nearby_devices.empty()) s += "Nenhum...";
        return s;
    }
};

std::vector<WiFiDevice> WiFiTools::nearby_devices;

#endif