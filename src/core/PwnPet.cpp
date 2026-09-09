#include "core/PwnPet.h"
#include "core/ConfigManager.h"
#include "core/PwnPower.h"
#include <SD_MMC.h>
#include <ArduinoJson.h>

extern RTC_DATA_ATTR RTC_SaveData rtc_save;

void PwnPet::init() {
    stats = {};
    stats.name = "Lele";
    stats.last_feed = millis();
    load();
    Serial.println("[PwnPet] Pet inicializado");
}

void PwnPet::tick() {
    // Fome diminui com o tempo
    if (millis() - stats.last_feed > 60000) {
        if (stats.hunger > 0) stats.hunger--;
        if (stats.happiness > 0) stats.happiness--;
    }
    checkEvolution();
}

void PwnPet::feed(int amount) {
    stats.hunger = constrain(stats.hunger + amount, 0, 100);
    stats.happiness = constrain(stats.happiness + 5, 0, 100);
    stats.last_feed = millis();
    addXP(amount * 2);
}

void PwnPet::addHandshake() {
    stats.total_handshakes++;
    stats.total_interactions++;
    addXP(10);
}

void PwnPet::addBLE() {
    stats.total_ble++;
    stats.total_interactions++;
    addXP(3);
}

void PwnPet::addXP(int amount) {
    stats.xp += amount;
    int new_level = 1 + stats.xp / 100;
    if (new_level > (int)stats.level) {
        stats.level = new_level;
        save();
    }
}

bool PwnPet::onShake() {
    stats.total_interactions++;
    stats.happiness = constrain(stats.happiness + 2, 0, 100);
    return true;
}

void PwnPet::setMood(const char* mood) {
    ConfigManager::getInstance()->set("pet_mood", String(mood));
}

void PwnPet::checkEvolution() {
    int new_stage = 0;
    if (stats.total_interactions > 100) new_stage = 1;
    if (stats.total_interactions > 500) new_stage = 2;
    if (stats.total_interactions > 1000) new_stage = 3;
    if (new_stage != stats.stage) {
        stats.stage = new_stage;
        save();
    }
}

void PwnPet::syncRTC() {
    // Salvar stats na RAM RTC
    rtc_save.pet_level = stats.level;
    rtc_save.pet_xp = stats.xp;
    rtc_save.pet_hunger = stats.hunger;
    rtc_save.pet_happiness = stats.happiness;
}

bool PwnPet::load() {
    if (!SD_MMC.begin("/sd", true)) return false;
    File f = SD_MMC.open(savePath);
    if (!f) return false;
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, f);
    f.close();
    if (err) return false;
    stats.level = doc["level"] | 1;
    stats.xp = doc["xp"] | 0;
    stats.hunger = doc["hunger"] | 80;
    stats.happiness = doc["happiness"] | 50;
    stats.stage = doc["stage"] | 0;
    stats.name = doc["name"].as<String>();
    if (stats.name.length() == 0) stats.name = "Lele";
    return true;
}

bool PwnPet::save() {
    if (!SD_MMC.begin("/sd", true)) return false;
    File f = SD_MMC.open(savePath, FILE_WRITE);
    if (!f) return false;
    JsonDocument doc;
    doc["level"] = stats.level;
    doc["xp"] = stats.xp;
    doc["hunger"] = stats.hunger;
    doc["happiness"] = stats.happiness;
    doc["stage"] = stats.stage;
    doc["name"] = stats.name;
    serializeJson(doc, f);
    f.close();
    return true;
}

int PwnPet::getStage() { return stats.stage; }

const char* PwnPet::getStageName() {
    switch (stats.stage) {
        case 0: return "Ovo";
        case 1: return "Filhote";
        case 2: return "Jovem";
        case 3: return "Adulto";
        default: return "Desconhecido";
    }
}
