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

struct SniffedDevice {
    String mac;
    int rssi;
    unsigned long last_seen;
};

class WiFiTools {
public:
    static std::vector<SniffedDevice> nearby_devices;

    static void promiscuous_rx_cb(void* buf, wifi_promiscuous_pkt_type_t type) {
        if (type != WIFI_PKT_MGMT) return;

        wifi_promiscuous_pkt_t* packet = (wifi_promiscuous_pkt_t*)buf;
        uint8_t* payload = packet->payload;

        uint8_t frame_type = (payload[0] & 0x0C) >> 2;
        uint8_t frame_subtype = (payload[0] & 0xF0) >> 4;

        // Probe request frames
        if (frame_type == 0 && frame_subtype == 4) {
            char macStr[18];
            snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                     payload[10], payload[11], payload[12],
                     payload[13], payload[14], payload[15]);

            String mac = String(macStr);
            int rssi = packet->rx_ctrl.rssi;

            bool found = false;
            for (auto &dev : nearby_devices) {
                if (dev.mac == mac) {
                    dev.rssi = rssi;
                    dev.last_seen = millis();
                    found = true;
                    break;
                }
            }
            if (!found) {
                SniffedDevice d{mac, rssi, millis()};
                if (nearby_devices.size() >= 50) {
                    nearby_devices.erase(nearby_devices.begin());
                }
                nearby_devices.push_back(d);

                if (SD_MMC.begin("/sdcard", true, true)) {
                    File f = SD_MMC.open("/sniffed_macs.txt", FILE_APPEND);
                    if (f) {
                        f.printf("%s,%d,%lu\n", mac.c_str(), rssi, millis());
                        f.close();
                    }
                }
            }
        }
    }

    static void startSniffer() {
        WiFi.disconnect();
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_promiscuous_rx_cb(&WiFiTools::promiscuous_rx_cb);
        Serial.println("[WiFiTools] Sniffer enabled");
    }

    static void stopSniffer() {
        esp_wifi_set_promiscuous(false);
        Serial.println("[WiFiTools] Sniffer disabled");
    }

    static String getSystemStats() {
        float temp = temperatureRead();
        uint32_t free_heap = esp_get_free_heap_size();
        uint32_t uptime = millis() / 1000;
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "Temp: %.0fC RAM: %dKB\nUp: %lus",
                 temp, free_heap / 1024, uptime);
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

inline std::vector<SniffedDevice> WiFiTools::nearby_devices;

#endif
