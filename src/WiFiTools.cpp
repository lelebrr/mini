#include "WiFiTools.h"
#include "WpsBlue.h"
#include "EspNowScan.h"
#include "core/ConfigManager.h"
#include <SD_MMC.h>
#include <WiFi.h>
#include <esp_wifi.h>

static void wifi_promisc_cb(void* buf, wifi_promiscuous_pkt_type_t type) {
    if (!WiFiTools::sniffing) return;
    wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
    // WPS BLUE: alimenta o parser de IE WPS com quadros de gerenciamento
    // (beacon/probe-resp). Apenas enfileira; o parse ocorre em WpsBlue::poll().
    if (type == WIFI_PKT_MGMT) {
        WpsBlue::onFrame(pkt->payload, pkt->rx_ctrl.sig_len,
                         (int8_t)pkt->rx_ctrl.rssi, (uint8_t)pkt->rx_ctrl.channel);
        EspNowScan::onFrame(pkt->payload, pkt->rx_ctrl.sig_len,
                            (int8_t)pkt->rx_ctrl.rssi, (uint8_t)pkt->rx_ctrl.channel);
    }
    int next = (WiFiTools::cap_head + 1) % WiFiTools::CAP_QUEUE;
    if (next == WiFiTools::cap_tail) return;

    int len = pkt->rx_ctrl.sig_len;
    if (len > 256) len = 256;
    WiFiTools::cap_queue[WiFiTools::cap_head].data = (uint8_t*)malloc(len);
    if (!WiFiTools::cap_queue[WiFiTools::cap_head].data) return;
    memcpy(WiFiTools::cap_queue[WiFiTools::cap_head].data, pkt->payload, len);
    WiFiTools::cap_queue[WiFiTools::cap_head].len = len;
    WiFiTools::cap_queue[WiFiTools::cap_head].type = type;
    WiFiTools::cap_queue[WiFiTools::cap_head].timestamp = millis();
    WiFiTools::cap_head = next;
    WiFiTools::frames_captured = WiFiTools::frames_captured + 1;
}

void WiFiTools::beginNewCapture() {
    nearby_devices.clear();
    cap_head = 0;
    cap_tail = 0;
    frames_captured = 0;
    eapol_count = 0;
    pcap_header_written = false;
    Serial.println("[WiFiTools] Nova captura iniciada");
}

void WiFiTools::startSnifferPassive() {
    if (sniffing) return;
    sniffing = true;
    WiFi.mode(WIFI_STA);
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&wifi_promisc_cb);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    Serial.println("[WiFiTools] Sniffer passivo iniciado");
}

void WiFiTools::stopSniffer() {
    sniffing = false;
    esp_wifi_set_promiscuous(false);
    esp_wifi_set_promiscuous_rx_cb(nullptr);
    Serial.println("[WiFiTools] Sniffer parado");
}

void WiFiTools::flush() {
    while (cap_tail != cap_head) {
        if (cap_queue[cap_tail].data) {
            free(cap_queue[cap_tail].data);
            cap_queue[cap_tail].data = nullptr;
        }
        cap_tail = (cap_tail + 1) % CAP_QUEUE;
    }
}

void WiFiTools::persistDevices() {
    if (!SD_MMC.begin("/sd", true)) return;
    File f = SD_MMC.open("/devices.json", FILE_WRITE);
    if (!f) return;
    // Salvar lista de dispositivos
    f.print("{\"devices\":[");
    for (size_t i = 0; i < nearby_devices.size(); i++) {
        if (i > 0) f.print(",");
        char buf[128];
        snprintf(buf, sizeof(buf), "{\"ssid\":\"%s\",\"rssi\":%d}",
                 nearby_devices[i].ssid.c_str(), nearby_devices[i].rssi);
        f.print(buf);
    }
    f.print("]}");
    f.close();
}

int WiFiTools::getAPCount() { return nearby_devices.size(); }
int WiFiTools::getDeviceCount() { return nearby_devices.size(); }
SniffedDevice WiFiTools::getAP(int i) {
    if (i >= 0 && i < (int)nearby_devices.size()) return nearby_devices[i];
    return {};
}
bool WiFiTools::isSniffing() { return sniffing; }

void WiFiTools::enableDeauth(bool en) {
    Serial.printf("[WiFiTools] Deauth %s\n", en ? "ON" : "OFF");
}

int WiFiTools::getDeauthCount() { return 0; }
