#ifndef WIFITOOLS_H
#define WIFITOOLS_H

#include <Arduino.h>
#include <vector>
#include "esp_wifi.h"

struct SniffedDevice {
    uint8_t bssid[6];
    int channel;
    int rssi;
    String ssid;
    uint8_t encryption;
    bool has_eapol;
    unsigned long first_seen;
    unsigned long last_seen;
    int packet_count;
};

struct CapFrame {
    uint8_t* data;
    uint16_t len;
    wifi_promiscuous_pkt_type_t type;
    uint32_t timestamp;
};

struct DevEvent {
    uint8_t mac[6];
    uint8_t type;
    int rssi;
};

class WiFiTools {
public:
    static const int CAP_QUEUE = 64;
    static const int DEV_QUEUE = 32;
    
    static std::vector<SniffedDevice> nearby_devices;
    static CapFrame cap_queue[CAP_QUEUE];
    static volatile int cap_head;
    static volatile int cap_tail;
    static DevEvent dev_queue[DEV_QUEUE];
    static volatile int dev_head;
    static volatile int dev_tail;
    static volatile uint32_t eapol_count;
    static volatile uint32_t frames_captured;
    static portMUX_TYPE cap_mux;
    static bool sniffing;
    static bool pcap_header_written;
    static String pcap_path;

    static void beginNewCapture();
    static void startSnifferPassive();
    static void stopSniffer();
    static void flush();
    static void persistDevices();
    static int getAPCount();
    static int getDeviceCount();
    static SniffedDevice getAP(int i);
    static bool isSniffing();
    static void enableDeauth(bool en);
    static int getDeauthCount();
};

#endif
