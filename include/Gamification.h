#ifndef GAMIFICATION_H
#define GAMIFICATION_H

#include <Arduino.h>
#include <SD_MMC.h>
#include "FS.h"

struct GameStats {
    uint32_t xp;
    uint32_t level;
    uint32_t age_seconds;
    uint32_t interactions;
};

class Gamification {
private:
    static GameStats stats;
    static const char* saveFile;
    static uint32_t last_tick;

public:
    static void init() {
        saveFile = "/game_stats.bin";
        stats.xp = 0;
        stats.level = 1;
        stats.age_seconds = 0;
        stats.interactions = 0;
        last_tick = millis();
        load();
    }

    static void load() {
        if (!SD_MMC.begin("/sdcard", true, true)) {
            Serial.println("[Game] SD_MMC not available, starting fresh");
            return;
        }
        if (SD_MMC.exists(saveFile)) {
            File file = SD_MMC.open(saveFile, FILE_READ);
            if (file) {
                file.read((uint8_t*)&stats, sizeof(GameStats));
                file.close();
                Serial.printf("[Game] Loaded: Lvl %u, XP %u\n", stats.level, stats.xp);
            }
        }
    }

    static void save() {
        if (!SD_MMC.begin("/sdcard", true, true)) {
            return;
        }
        File file = SD_MMC.open(saveFile, FILE_WRITE);
        if (file) {
            file.write((uint8_t*)&stats, sizeof(GameStats));
            file.close();
        }
    }

    static void tick() {
        if (millis() - last_tick >= 1000) {
            stats.age_seconds++;
            last_tick = millis();
            if (stats.age_seconds % 60 == 0) {
                save();
            }
        }
    }

    static void addXP(int amount) {
        stats.xp += amount;
        stats.interactions++;

        uint32_t required = stats.level * 100;
        if (stats.xp >= required) {
            stats.xp -= required;
            stats.level++;
            Serial.println("[Game] LEVEL UP");
        }
        save();
    }

    static String getLevelStr() {
        return "Lvl " + String(stats.level);
    }

    static String getXPStr() {
        return String(stats.xp) + " XP";
    }

    static String getAgeStr() {
        uint32_t h = stats.age_seconds / 3600;
        uint32_t m = (stats.age_seconds % 3600) / 60;
        return String(h) + "h " + String(m) + "m";
    }

    static GameStats getStats() {
        return stats;
    }
};

inline GameStats Gamification::stats;
inline const char* Gamification::saveFile = "/game_stats.bin";
inline uint32_t Gamification::last_tick = 0;

#endif
