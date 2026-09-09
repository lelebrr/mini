#include "Gamification.h"
#include "core/ConfigManager.h"
#include <SD_MMC.h>
#include <ArduinoJson.h>

void Gamification::init() {
    stats = {};
    last_tick_ms = millis();
    load();
    stats.boot_count++;
    save();
    Serial.println("[Gamification] Sistema de gamificacao inicializado");
}

void Gamification::tick() {
    stats.uptime_seconds++;
    if (stats.uptime_seconds % 60 == 0) save();
}

void Gamification::addXP(int amount) {
    stats.xp += amount;
    int new_level = 1 + stats.xp / 100;
    if (new_level > stats.level) {
        stats.level = new_level;
        Serial.printf("[Gamification] Level up! Nivel %d\n", stats.level);
    }
}

void Gamification::registerHandshake() { stats.total_handshakes++; addXP(10); }
void Gamification::registerBLE() { stats.total_ble++; addXP(3); }
void Gamification::registerDeauth() { stats.total_deauth++; addXP(5); }

int Gamification::getLevel() { return stats.level; }

void Gamification::save() {
    if (!SD_MMC.begin("/sd", true)) return;
    File f = SD_MMC.open(saveFile, FILE_WRITE);
    if (!f) return;
    JsonDocument doc;
    doc["xp"] = stats.xp;
    doc["level"] = stats.level;
    doc["handshakes"] = stats.total_handshakes;
    doc["ble"] = stats.total_ble;
    doc["boots"] = stats.boot_count;
    serializeJson(doc, f);
    f.close();
}

void Gamification::load() {
    if (!SD_MMC.begin("/sd", true)) return;
    File f = SD_MMC.open(saveFile);
    if (!f) return;
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, f);
    f.close();
    if (err) return;
    stats.xp = doc["xp"] | 0;
    stats.level = doc["level"] | 1;
    stats.total_handshakes = doc["handshakes"] | 0;
    stats.total_ble = doc["ble"] | 0;
    stats.boot_count = doc["boots"] | 0;
}

void Gamification::reset() {
    stats = {};
    save();
}
