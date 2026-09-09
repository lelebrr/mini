#include "core/PwnAttack.h"
#include "core/ConfigManager.h"
#include "WiFiTools.h"
#include "EvilPortal.h"
#include <WiFi.h>

static bool deauth_enabled = false;
static bool evilportal_enabled = false;
static bool auto_scan = false;
static int deauth_count = 0;

void PwnAttack::init() {
    stats = {};
    scan_active = false;
    deauth_enabled = false;
    evilportal_enabled = false;
    auto_scan = false;
    deauth_count = 0;
    Serial.println("[PwnAttack] Modulo de ataques inicializado");
}

void PwnAttack::start() {
    if (scan_active) return;
    scan_active = true;
    stats.start_time = millis();
    Serial.println("[PwnAttack] Escaneamento iniciado");
}

void PwnAttack::stop() {
    scan_active = false;
    WiFiTools::stopSniffer();
    EvilPortal::stop();
    Serial.println("[PwnAttack] Escaneamento parado");
}

void PwnAttack::tick() {
    if (!scan_active) return;

    // Atualizar contadores
    stats.eapol = WiFiTools::eapol_count;
    stats.packets = WiFiTools::frames_captured;
}

bool PwnAttack::isScanning() {
    return scan_active;
}

int PwnAttack::getHandshakeCount() {
    return stats.handshakes;
}

void PwnAttack::enableDeauth(bool en) {
    deauth_enabled = en;
    WiFiTools::enableDeauth(en);
    Serial.printf("[PwnAttack] Deauth %s\n", en ? "HABILITADO" : "DESABILITADO");
}

void PwnAttack::enableEvilPortal(bool en) {
    evilportal_enabled = en;
    if (en) {
        ConfigManager *cfg = ConfigManager::getInstance();
        String ssid = cfg->getString("atk_portal_ssid", "SENTINEL-LAB");
        EvilPortal::begin(ssid.c_str());
    } else {
        EvilPortal::stop();
    }
    Serial.printf("[PwnAttack] EvilPortal %s\n", en ? "HABILITADO" : "DESABILITADO");
}

void PwnAttack::setAutoScan(bool en) {
    auto_scan = en;
    ConfigManager::getInstance()->set("atk_auto_scan", en);
}

void PwnAttack::cycleTarget() {
    // Cicla entre alvos detectados (placeholder para UI)
    Serial.println("[PwnAttack] Proximo alvo");
}

int PwnAttack::getDeauthCount() {
    return deauth_count;
}
