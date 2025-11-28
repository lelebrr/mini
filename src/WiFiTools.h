#ifndef WIFI_TOOLS_H
#define WIFI_TOOLS_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Simple Sniffer Callback
// Note: In Arduino/ESP32, promiscuous mode requires care with buffer handling.
// We will store just the last few MACs seen to display on UI.

#define MAX_SNIFFED 10
String sniffed_macs[MAX_SNIFFED];
int sniff_idx = 0;

class WiFiTools {
public:
    static void promiscuous_rx_cb(void* buf, wifi_promiscuous_pkt_type_t type) {
        wifi_promiscuous_pkt_t* packet = (wifi_promiscuous_pkt_t*)buf;
        wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)packet->rx_ctrl;

        // Filter for Probe Requests (Type 0, Subtype 4)
        // This is a simplified check, actual packet parsing is complex.
        // We will just log existence for "Activity" visual

        // MAC header is at packet->payload
        // uint8_t* mac_addr = packet->payload + 10; // Source address position varies by frame type
        // Just increment a counter for visualization "Activity"
    }

    static void startSniffer() {
        WiFi.disconnect();
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb);
    }

    static void stopSniffer() {
        esp_wifi_set_promiscuous(false);
    }

    static void scanAndPrint() {
        int n = WiFi.scanNetworks();
        Serial.printf("WiFi Scan: %d redes\n", n);
    }

    // Helper to get system stats for "memtemp"
    static String getSystemStats() {
        // Temp
        float temp = temperatureRead();
        // RAM
        uint32_t free_heap = esp_get_free_heap_size();
        uint32_t total_heap = esp_get_free_heap_size() + 1; // Approx
        // Uptime
        uint32_t uptime = millis() / 1000;

        char buffer[64];
        sprintf(buffer, "Temp: %.1fC | RAM: %dKB\nUp: %ds", temp, free_heap/1024, uptime);
        return String(buffer);
    }
};

#endif
