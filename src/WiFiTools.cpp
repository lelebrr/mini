#include "WiFiTools.h"
#include "WpsBlue.h"
#include "EspNowScan.h"
#include "core/ConfigManager.h"
#include <SD_MMC.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Fila de frames de gerenciamento (beacon/probe-resp) p/ inventário Wi-Fi.
// O callback promíscuo roda na task do WiFi: apenas enfileira (barato).
// O parse (SSID/IEs/privacidade) ocorre em pollBeacons(), na task do loop.
#define MGMT_QUEUE 16
struct MgmtFrame {
    uint16_t len;
    int8_t   rssi;
    uint8_t  channel;
    uint8_t  data[160];
};
static MgmtFrame mgmt_queue[MGMT_QUEUE];
static volatile int mgmt_head = 0, mgmt_tail = 0;

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
        // Inventario Wi-Fi: enfileira mgmt p/ parse de beacons em pollBeacons()
        // (o callback roda na task do WiFi: NÃO tocar no vector aqui).
        int mnext = (mgmt_head + 1) % MGMT_QUEUE;
        if (mnext != mgmt_tail) {
            MgmtFrame &mf = mgmt_queue[mgmt_head];
            int mlen = pkt->rx_ctrl.sig_len;
            if (mlen > (int)sizeof(mf.data)) mlen = sizeof(mf.data);
            memcpy(mf.data, pkt->payload, mlen);
            mf.len = (uint16_t)mlen;
            mf.rssi = (int8_t)pkt->rx_ctrl.rssi;
            mf.channel = (uint8_t)pkt->rx_ctrl.channel;
            mgmt_head = mnext;
        }
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
    pollBeacons();   // primeiro converte mgmt pendentes em dispositivos
    // Salto lento de canais (1..13) para o inventário enxergar toda a faixa
    static uint32_t last_hop = 0;
    static uint8_t hop_ch = 1;
    if (sniffing && millis() - last_hop > 400) {
        esp_wifi_set_channel(hop_ch, WIFI_SECOND_CHAN_NONE);
        if (++hop_ch > 13) hop_ch = 1;
        last_hop = millis();
    }
    while (cap_tail != cap_head) {
        if (cap_queue[cap_tail].data) {
            free(cap_queue[cap_tail].data);
            cap_queue[cap_tail].data = nullptr;
        }
        cap_tail = (cap_tail + 1) % CAP_QUEUE;
    }
}

// ---------------------------------------------------------------------------
// Inventario Wi-Fi: parse de beacon/probe-resp enfileirados pelo callback.
// Layout 802.11 mgmt: FC(2) dur(2) addr1(6) addr2(6) addr3(6) seq(2) = 24,
// depois timestamp(8) interval(2) capability(2) -> IEs a partir do offset 36.
// IEs: SSID = tag 0, DS Parameter Set (canal) = tag 3.
// ---------------------------------------------------------------------------
static void wifiUpsertDevice(const uint8_t bssid[6], const String& ssid,
                             int8_t rssi, uint8_t ch, bool privacy) {
    for (size_t i = 0; i < WiFiTools::nearby_devices.size(); i++) {
        SniffedDevice &d = WiFiTools::nearby_devices[i];
        if (memcmp(d.bssid, bssid, 6) == 0) {
            d.rssi = rssi;
            d.channel = ch;
            d.encryption = privacy ? 1 : 0;
            d.last_seen = millis();
            d.packet_count++;
            if (ssid.length() > 0) d.ssid = ssid;
            return;
        }
    }
    if ((int)WiFiTools::nearby_devices.size() >= 64) return;  // limite do inventário
    SniffedDevice d = {};
    memcpy(d.bssid, bssid, 6);
    d.ssid = ssid;
    d.rssi = rssi;
    d.channel = ch;
    d.encryption = privacy ? 1 : 0;   // 0 = aberta, 1 = protegida (privacy bit)
    d.has_eapol = false;
    d.first_seen = d.last_seen = millis();
    d.packet_count = 1;
    WiFiTools::nearby_devices.push_back(d);
}

void WiFiTools::pollBeacons() {
    while (mgmt_tail != mgmt_head) {
        MgmtFrame &mf = mgmt_queue[mgmt_tail];
        mgmt_tail = (mgmt_tail + 1) % MGMT_QUEUE;
        const uint8_t *p = mf.data;
        if (mf.len < 36) continue;
        uint8_t ftype = (p[0] >> 2) & 0x3;
        uint8_t subtype = (p[0] >> 4) & 0xF;
        if (ftype != 0 || (subtype != 8 && subtype != 5)) continue;  // beacon | probe-resp
        const uint8_t *bssid = p + 10;                 // addr2 = transmissor (BSSID AP)
        bool privacy = (p[34] & 0x10) != 0;            // capability bit "privacy"
        String ssid;
        uint8_t ch = mf.channel;
        int pos = 36;
        while (pos + 2 <= (int)mf.len) {
            uint8_t tag = p[pos];
            uint8_t tlen = p[pos + 1];
            if (pos + 2 + tlen > (int)mf.len) break;
            if (tag == 0 && tlen <= 32) {
                for (int i = 0; i < tlen; i++) {
                    char c = (char)p[pos + 2 + i];
                    ssid += (c >= 32 && c < 127) ? c : '.';
                }
            } else if (tag == 3 && tlen == 1) {
                ch = p[pos + 2];
            }
            pos += 2 + tlen;
        }
        wifiUpsertDevice(bssid, ssid, mf.rssi, ch, privacy);
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
