#ifndef PWN_PET_H
#define PWN_PET_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD_MMC.h>
#include "FS.h"
#include "ConfigManager.h"

enum PetStage {
    EGG = 0,
    BABY,
    KID,
    TEEN,
    ADULT,
    PWNLORD,
    PWNGOD
};

struct PetStats {
    String name;
    PetStage stage;
    int xp;
    int level;
    int hunger;          // 0-100
    int happiness;       // 0-100
    int handshakes_total;
    unsigned long last_save;
    bool is_sleeping;
    int energy_mode;     // 0=Eco,1=Normal,2=Turbo
    int sleep_hours;
};

class PwnPet {
private:
    static PetStats stats;
    static const char* savePath;

public:
    static void init() {
        savePath = "/pwn_pet_save.json";

        ConfigManager *cfg = ConfigManager::getInstance();
        stats.name = cfg->getString("pet_name");
        if (stats.name.length() == 0) {
            stats.name = "PwnBaby";
        }

        stats.stage = EGG;
        stats.xp = 0;
        stats.level = 1;
        stats.hunger = 100;
        stats.happiness = 100;
        stats.handshakes_total = 0;
        stats.last_save = millis();
        stats.is_sleeping = false;
        stats.energy_mode = 1;
        stats.sleep_hours = 0;

        load();
    }

    static void load() {
        if (!SD_MMC.begin("/sdcard", true, true)) {
            Serial.println("[Pet] SD not ready, using defaults");
            return;
        }

        if (!SD_MMC.exists(savePath)) {
            save();
            return;
        }

        File f = SD_MMC.open(savePath, FILE_READ);
        if (!f) {
            Serial.println("[Pet] Failed to open save, using defaults");
            return;
        }

        DynamicJsonDocument doc(1024);
        DeserializationError err = deserializeJson(doc, f);
        f.close();
        if (err) {
            Serial.println("[Pet] JSON error, using defaults");
            return;
        }

        stats.name = doc["name"] | stats.name;
        stats.stage = (PetStage)(doc["stage"] | (int)stats.stage);
        stats.xp = doc["xp"] | stats.xp;
        stats.level = doc["level"] | stats.level;
        stats.hunger = doc["hunger"] | stats.hunger;
        stats.happiness = doc["happiness"] | stats.happiness;
        stats.handshakes_total = doc["handshakes_total"] | stats.handshakes_total;
        stats.energy_mode = doc["energy_mode"] | stats.energy_mode;

        Serial.printf("[Pet] Loaded: %s (Lvl %d)\n", stats.name.c_str(), stats.level);
    }

    static void save() {
        if (!SD_MMC.begin("/sdcard", true, true)) return;

        DynamicJsonDocument doc(1024);
        doc["name"] = stats.name;
        doc["stage"] = (int)stats.stage;
        doc["xp"] = stats.xp;
        doc["level"] = stats.level;
        doc["hunger"] = stats.hunger;
        doc["happiness"] = stats.happiness;
        doc["handshakes_total"] = stats.handshakes_total;
        doc["energy_mode"] = stats.energy_mode;

        File f = SD_MMC.open(savePath, FILE_WRITE);
        if (f) {
            serializeJson(doc, f);
            f.close();
            stats.last_save = millis();
        }
    }

    static void tick() {
        static unsigned long last_decay = 0;
        unsigned long now = millis();
        if (now - last_decay > 60000) {
            last_decay = now;
            if (!stats.is_sleeping) {
                int hunger_loss = (stats.energy_mode == 0) ? 0 : 1;
                stats.hunger = max(0, stats.hunger - hunger_loss);
                stats.happiness = max(0, stats.happiness - 1);
            }
            if (now - stats.last_save > 300000) {
                save();
            }
        }
        checkEvolution();
    }

    static void feed(int quality) {
        stats.hunger = min(100, stats.hunger + quality);
        stats.happiness = min(100, stats.happiness + quality / 2);
        addXP(quality);
    }

    static void addXP(int amount) {
        float mul = 1.0f;
        if (stats.energy_mode == 2) mul = 1.5f;
        if (stats.energy_mode == 0) mul = 0.5f;

        stats.xp += (int)(amount * mul);
        if (stats.xp > stats.level * 500) {
            stats.level++;
            stats.happiness = 100;
        }
        checkEvolution();
    }

    static void addHandshake(bool is_wpa3) {
        stats.handshakes_total++;
        int xp_gain = is_wpa3 ? 100 : 20;
        addXP(xp_gain);
        stats.hunger = min(100, stats.hunger + 2);
        save();
    }

    static void setSleeping(bool sleeping) {
        stats.is_sleeping = sleeping;
    }

    static void checkEvolution() {
        PetStage old = stats.stage;
        if (stats.level >= 5 && stats.stage == EGG) stats.stage = BABY;
        else if (stats.level >= 10 && stats.stage == BABY) stats.stage = KID;
        else if (stats.level >= 20 && stats.stage == KID) stats.stage = TEEN;
        else if (stats.level >= 30 && stats.stage == TEEN) stats.stage = ADULT;
        else if (stats.level >= 50 && stats.stage == ADULT) stats.stage = PWNLORD;

        if (stats.handshakes_total >= 10000) stats.stage = PWNGOD;

        if (old != stats.stage) {
            Serial.println("[Pet] Evolution!");
        }
    }

    static int getHunger() { return stats.hunger; }
    static PetStats& getStats() { return stats; }

    static String getMoodStr() {
        if (stats.hunger < 20) return "FAMINTO";
        if (stats.happiness < 20) return "TRISTE";
        if (stats.stage == PWNGOD) return "DIVINO";
        return "FELIZ";
    }
};

inline PetStats PwnPet::stats;
inline const char* PwnPet::savePath = "/pwn_pet_save.json";

#endif
