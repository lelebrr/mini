#ifndef PWN_ATTACK_H
#define PWN_ATTACK_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "core/PwnPet.h"
#include "WiFiTools.h" // Reutilizando a base robusta anterior

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

public:
    static void init() {
        stats.aps_scanned = 0;
        stats.handshakes_captured = 0;
        WiFiTools::startSniffer(); // Inicia modo promíscuo
    }

    // Loop principal de ataque
    static void tick() {
        // Lógica de "Sentinela" / Walk Mode
        // Se detectar muitas redes novas rapidamente -> Avisar Pet

        static int last_ap_count = 0;
        int current_ap_count = WiFiTools::nearby_devices.size();

        if (current_ap_count > last_ap_count + 5) {
            // Muitas redes novas!
            Serial.println("[Attack] Ambiente Rico detectado!");
            // PwnPet::speak("Mano, ta cheio de comida aqui!");
        }
        last_ap_count = current_ap_count;

        // Simulação de captura EAPOL (Placeholder para lógica real libpcap)
        // Em um firmware real, analisariamos o buffer do WiFiTools
    }

    static void deauthTarget(String bssid) {
        // Envia frames de deauth
        Serial.printf("[Attack] Deauthing %s\n", bssid.c_str());
        // Implementação raw frame injection (baseada no legacy deauth.cpp)
        // ...

        // Se sucesso (simulado):
        PwnPet::addHandshake(false); // Ganha XP
    }

    static void evilTwin(String ssid) {
        // Cria AP falso
        WiFi.softAP(ssid.c_str());
        // Inicia DNS Server...
    }
};

AttackStats PwnAttack::stats;
bool PwnAttack::scan_active = false;

#endif
