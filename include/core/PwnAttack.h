#ifndef PWN_ATTACK_H
#define PWN_ATTACK_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "core/PwnPet.h"
#include "core/PwnPower.h" // Power Manager
#include "WiFiTools.h"

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

public:
    static void init() {
        stats.aps_scanned = 0;
        stats.handshakes_captured = 0;
        last_scan_time = millis();
        // Sniffer iniciado apenas se permitido pelo modo de energia
        if (PwnPet::getStats().energy_mode > 0) {
            WiFiTools::startSniffer();
        }
    }

    static bool isRunning() { return scan_active; }

    // Loop principal de ataque
    static void tick() {
        // Otimização 15: Desliga rádio se bateria crítica
        if (PwnPower::isCritical()) {
            if (scan_active) {
                esp_wifi_stop();
                scan_active = false;
                Serial.println("[Attack] Parado por bateria critica");
            }
            return;
        }

        // Otimização 6: Ajusta intensidade baseado no modo de energia
        int energy = PwnPet::getStats().energy_mode;
        int interval = (energy == 2) ? 100 : (energy == 1 ? 500 : 2000); // ms entre scans/ticks

        static unsigned long last_tick = 0;
        if (millis() - last_tick < interval) return;
        last_tick = millis();

        // Lógica de "Sentinela" / Walk Mode
        static int last_ap_count = 0;
        int current_ap_count = WiFiTools::nearby_devices.size();

        // Otimização 12: Cache de PSRAM (Simulado pelo WiFiTools vector)

        if (current_ap_count > last_ap_count + 5) {
            Serial.println("[Attack] Ambiente Rico detectado!");
        }
        last_ap_count = current_ap_count;

        // Otimização 3: Marauder Duty Cycle
        // Se Fome > 90 (Muito faminto), ataca agressivamente
        // Se Fome < 20 (Cheio), apenas escuta passivamente
        if (PwnPet::getHunger() > 80) {
             // Ataque ativo (Deauth broadcast simulado)
             // if (millis() % 10000 == 0) deauthTarget("FF:FF:FF:FF:FF:FF");
        }
    }

    static void deauthTarget(String bssid) {
        Serial.printf("[Attack] Deauthing %s\n", bssid.c_str());
        // Simulação de captura
        // Na prática, injetar frames aqui

        // Se sucesso (simulado):
        PwnPet::addHandshake(false);
    }

    static void evilTwin(String ssid) {
        WiFi.softAP(ssid.c_str());
        // Inicia DNS Server...
    }
};

AttackStats PwnAttack::stats;
bool PwnAttack::scan_active = true;
unsigned long PwnAttack::last_scan_time = 0;

#endif
