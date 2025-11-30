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
 *
 * Sniffer simples de Probe Requests para detectar dispositivos próximos.
 * Também fornece funções utilitárias de sistema.
 *
 * Logs:
 *   - /arquivos_cartao_sd/macs_detectados.txt  (MAC, RSSI, timestamp)
 */

struct SniffedDevice {
    String        mac;
    int           rssi;
    unsigned long last_seen;
};

class WiFiTools {
public:
    static std::vector<SniffedDevice> nearby_devices;

    static void startSniffer() {
        // Apenas STA em modo promíscuo para reduzir interferência
        WiFi.mode(WIFI_MODE_STA);
        WiFi.disconnect(true);
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_promiscuous_rx_cb(&wifiSnifferCallback);
    }

    static void stopSniffer() {
        esp_wifi_set_promiscuous(false);
    }

    // Salva um "handshake" fictício em /capturas/SSID.pcap
    static void saveHandshake(const String &ssid, const String &bssid) {
        if (!SD_MMC.exists("/capturas")) {
            SD_MMC.mkdir("/capturas");
        }

        String safeSsid = ssid;
        safeSsid.replace("/", "_");
        safeSsid.replace("\\", "_");

        String path = "/capturas/" + safeSsid + ".pcap";
        File f = SD_MMC.open(path, FILE_WRITE);
        if (!f) {
            Serial.println("[Sniffer] Falha ao criar arquivo de captura.");
            return;
        }

        // Conteúdo simbólico – captura real exigiria um buffer de frames.
        f.println("PCAP DUMMY HEADER - substitua por captura real se desejar.");
        f.printf("# SSID=%s BSSID=%s\n", ssid.c_str(), bssid.c_str());
        f.close();
    }

    static String getSystemStats() {
        float    temp      = temperatureRead();
        uint32_t free_heap = esp_get_free_heap_size();
        uint32_t uptime    = millis() / 1000;

        char buffer[96];
        snprintf(buffer, sizeof(buffer),
                 "Temp: %.0fC RAM: %luKB\nUp: %lus",
                 temp, free_heap / 1024UL, uptime);
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

    // Callback interno chamado pelo driver WiFi em modo promíscuo
    static void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type) {
        if (type != WIFI_PKT_MGMT) return;

        auto *packet  = (wifi_promiscuous_pkt_t *)buf;
        uint8_t *data = packet->payload;

        uint8_t frame_type    = (data[0] & 0x0C) >> 2;
        uint8_t frame_subtype = (data[0] & 0xF0) >> 4;

        // Apenas Probe Requests
        if (frame_type == 0 && frame_subtype == 4) {
            char macStr[18];
            snprintf(macStr, sizeof(macStr),
                     "%02X:%02X:%02X:%02X:%02X:%02X",
                     data[10], data[11], data[12],
                     data[13], data[14], data[15]);

            String mac  = String(macStr);
            int    rssi = packet->rx_ctrl.rssi;

            bool found = false;
            for (auto &dev : nearby_devices) {
                if (dev.mac.equalsIgnoreCase(mac)) {
                    dev.rssi      = rssi;
                    dev.last_seen = millis();
                    found         = true;
                    break;
                }
            }

            if (!found) {
                if (nearby_devices.size() >= 50) {
                    nearby_devices.erase(nearby_devices.begin());
                }

                SniffedDevice dev;
                dev.mac       = mac;
                dev.rssi      = rssi;
                dev.last_seen = millis();
                nearby_devices.push_back(dev);

                // Log persistente
                File f = SD_MMC.open("/arquivos_cartao_sd/macs_detectados.txt", FILE_APPEND);
                if (f) {
                    f.printf("%s,%d,%lu\n", mac.c_str(), rssi, (unsigned long)millis());
                    f.close();
                }
            }
        }
    }

private:
    // Wrapper free-function usado pelo esp_wifi_set_promiscuous_rx_cb
    static void wifiSnifferCallback(void *buf, wifi_promiscuous_pkt_type_t type) {
        WiFiTools::promiscuous_rx_cb(buf, type);
    }
};

// Definição estática declarada em core_singletons.cpp
#endif
