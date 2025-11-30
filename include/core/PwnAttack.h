#ifndef PWN_ATTACK_H
#define PWN_ATTACK_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "core/PwnPet.h"
#include "core/PwnPower.h"
#include "Gamification.h"
#include "WiFiTools.h"
#include "EvilPortal.h"

/**
 * PwnAttack
 *
 * Versão simplificada do módulo de ataques:
 *  - Coordena o sniffer (WiFiTools) quando em modo de ataque.
 *  - Pode iniciar um Evil Portal.
 *  - Opcionalmente simula deauth para fins de gameplay (sem foco em uso real).
 */

struct AttackStats {
    int  aps_scanned;
    int  clients_seen;
    int  handshakes_captured;
    bool is_walking;
};

class PwnAttack {
private:
    static AttackStats  stats;
    static bool         scan_active;

public:
    static void init() {
        stats.aps_scanned          = 0;
        stats.clients_seen         = 0;
        stats.handshakes_captured  = 0;
        stats.is_walking           = false;
        scan_active                = false;
    }

    static void start() {
        if (scan_active) return;
        if (PwnPower::isCritical()) {
            Serial.println("[Attack] Não iniciando ataques – bateria crítica.");
            return;
        }

        scan_active = true;
        WiFiTools::startSniffer();
        Serial.println("[Attack] Modo ataque iniciado (sniffer ligado).");
    }

    static void stop() {
        if (!scan_active) return;
        WiFiTools::stopSniffer();
        scan_active = false;
        Serial.println("[Attack] Modo ataque parado.");
    }

    static bool isRunning() {
        return scan_active;
    }

    // Loop leve – atualiza estatísticas e respeita bateria crítica.
    static void tick() {
        if (!scan_active) return;

        if (PwnPower::isCritical()) {
            stop();
            return;
        }

        // Estatísticas simples baseadas na lista do sniffer
        int count = static_cast<int>(WiFiTools::nearby_devices.size());
        stats.aps_scanned = count;

        // Exemplo de “ambiente rico”
        static int last_count = 0;
        if (count > last_count + 5) {
            Serial.println("[Attack] Ambiente rico detectado (muitas novas sondagens).");
        }
        last_count = count;
    }

    // Simulação de um “deauth” – não envia frames reais aqui por simplicidade.
    static void deauthSimulated(const String &bssid) {
        if (!scan_active) return;
        Serial.printf("[Attack] (Simulado) Deauth em %s\n", bssid.c_str());

        // Para gameplay, podemos recompensar de forma simbólica:
        PwnPet::addHandshake(false);
        Gamification::registerHandshake();
        stats.handshakes_captured++;
    }

    // Inicia um Evil Twin usando o módulo EvilPortal.
    static void evilTwin(const String &ssid) {
        Serial.printf("[Attack] Iniciando Evil Portal com SSID '%s'\n", ssid.c_str());
        EvilPortal::start(ssid.c_str(), "/evil_portal/01_wifi_update.html");
    }

    static AttackStats getStats() {
        return stats;
    }
};

// Definições estáticas declaradas em core_singletons.cpp
#endif
