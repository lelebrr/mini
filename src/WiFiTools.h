#ifndef WIFI_TOOLS_H
#define WIFI_TOOLS_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <SD_MMC.h>
#include "FS.h"

// Simple Sniffer Callback
#define MAX_SNIFFED 10
String sniffed_macs[MAX_SNIFFED];
int sniff_idx = 0;

class WiFiTools {
public:
    static void promiscuous_rx_cb(void* buf, wifi_promiscuous_pkt_type_t type) {
        wifi_promiscuous_pkt_t* packet = (wifi_promiscuous_pkt_t*)buf;
        // Keep simple: Detect Management Frames (Probe Requests)
        // This is a minimal implementation.
        // In a real sniffer, we parse 802.11 frames.
        // This callback is in ISR context (careful!)
    }

    static void startSniffer() {
        WiFi.disconnect();
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb);
    }

    static void stopSniffer() {
        esp_wifi_set_promiscuous(false);
    }

    // Save a "Handshake" (Simulated/Placeholder)
    static void saveHandshake(String ssid, String bssid) {
        String filename = "/handshakes/" + ssid + "_" + String(millis()) + ".pcap";
        // Ensure dir exists
        if (!SD_MMC.exists("/handshakes")) SD_MMC.mkdir("/handshakes");

        File f = SD_MMC.open(filename, FILE_WRITE);
        if (f) {
            f.println("PCAP HEADER SIMULATED");
            f.printf("SSID: %s\nBSSID: %s\n", ssid.c_str(), bssid.c_str());
            f.close();
            Serial.printf("Handshake salvo: %s\n", filename.c_str());
        }
    }

    static String getSystemStats() {
        float temp = temperatureRead();
        uint32_t free_heap = esp_get_free_heap_size();
        uint32_t uptime = millis() / 1000;
        char buffer[64];
        sprintf(buffer, "Temp: %.0fC RAM: %dKB\nUp: %ds", temp, free_heap/1024, uptime);
        return String(buffer);
    }
};

#endif
