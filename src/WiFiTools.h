#ifndef WIFI_TOOLS_H
#define WIFI_TOOLS_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
<<<<<<< HEAD
<<<<<<< HEAD
#include <SD_MMC.h>
#include "FS.h"
<<<<<<< HEAD
=======
#include <SD_MMC.h>
#include "FS.h"
>>>>>>> origin/waveshare-s3-amoled-manual-upload

// Simple Sniffer Callback
#define MAX_SNIFFED 10
String sniffed_macs[MAX_SNIFFED];
int sniff_idx = 0;
<<<<<<< HEAD
=======
#include <vector>

// Sniffer Globals
#define MAX_SNIFFED 10
struct SniffedDevice {
    String mac;
    int rssi;
    unsigned long last_seen;
};
std::vector<SniffedDevice> nearby_devices;
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======

// Simple Sniffer Callback
// Note: In Arduino/ESP32, promiscuous mode requires care with buffer handling.
// We will store just the last few MACs seen to display on UI.

#define MAX_SNIFFED 10
String sniffed_macs[MAX_SNIFFED];
int sniff_idx = 0;
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload

class WiFiTools {
public:
    static void promiscuous_rx_cb(void* buf, wifi_promiscuous_pkt_type_t type) {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
        wifi_promiscuous_pkt_t* packet = (wifi_promiscuous_pkt_t*)buf;
        // Keep simple: Detect Management Frames (Probe Requests)
        // This is a minimal implementation.
        // In a real sniffer, we parse 802.11 frames.
        // This callback is in ISR context (careful!)
<<<<<<< HEAD
=======
        if (type != WIFI_PKT_MGMT) return; // Only Management Frames

        wifi_promiscuous_pkt_t* packet = (wifi_promiscuous_pkt_t*)buf;
        int len = packet->rx_ctrl.sig_len;
        uint8_t* payload = packet->payload;

        // Frame Control (2 bytes)
        uint8_t frame_type = (payload[0] & 0x0C) >> 2;
        uint8_t frame_subtype = (payload[0] & 0xF0) >> 4;

        // Probe Request (Type 0, Subtype 4)
        if (frame_type == 0 && frame_subtype == 4) {
            // Source Address is at offset 10 (Addr2)
            char macStr[18];
            snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                     payload[10], payload[11], payload[12], payload[13], payload[14], payload[15]);

            String mac = String(macStr);
            int rssi = packet->rx_ctrl.rssi;

            // Add/Update list
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
                SniffedDevice newDev = {mac, rssi, millis()};
                if (nearby_devices.size() >= MAX_SNIFFED) nearby_devices.erase(nearby_devices.begin());
                nearby_devices.push_back(newDev);

                // Log to SD (Append)
                File f = SD_MMC.open("/sniffed_macs.txt", FILE_APPEND);
                if (f) {
                    f.printf("%s,%d,%lu\n", mac.c_str(), rssi, millis());
                    f.close();
                }
            }
        }
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
        wifi_promiscuous_pkt_t* packet = (wifi_promiscuous_pkt_t*)buf;
        wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)packet->rx_ctrl;

        // Filter for Probe Requests (Type 0, Subtype 4)
        // This is a simplified check, actual packet parsing is complex.
        // We will just log existence for "Activity" visual

        // MAC header is at packet->payload
        // uint8_t* mac_addr = packet->payload + 10; // Source address position varies by frame type
        // Just increment a counter for visualization "Activity"
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
    }

    static void startSniffer() {
        WiFi.disconnect();
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
        Serial.println("Sniffer Ativo");
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
    }

    static void stopSniffer() {
        esp_wifi_set_promiscuous(false);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
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
<<<<<<< HEAD
=======
        Serial.println("Sniffer Parado");
    }

    // Save a "Handshake" (Placeholder for EAPOL capture logic which is complex)
    static void saveHandshake(String ssid, String bssid) {
        String filename = "/handshakes/" + ssid + ".pcap";
        if (!SD_MMC.exists("/handshakes")) SD_MMC.mkdir("/handshakes");
        File f = SD_MMC.open(filename, FILE_WRITE);
        if (f) {
            f.println("PCAP DUMMY"); // Real implementation requires libpcap logic
            f.close();
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
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
<<<<<<< HEAD
<<<<<<< HEAD
=======

    static String getSnifferText() {
        String s = "Dispositivos:\n";
        for (auto &dev : nearby_devices) {
            s += dev.mac + " (" + String(dev.rssi) + ")\n";
        }
        if (nearby_devices.empty()) s += "Nenhum...";
        return s;
    }
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
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
>>>>>>> origin/waveshare-s3-amoled-full-plugins
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
};

#endif
