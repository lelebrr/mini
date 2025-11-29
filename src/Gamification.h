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
        // Defaults
        stats.xp = 0;
        stats.level = 1;
        stats.age_seconds = 0;
        stats.interactions = 0;
        last_tick = millis();
        load();
    }

    static void load() {
        if (SD_MMC.exists(saveFile)) {
            File file = SD_MMC.open(saveFile, FILE_READ);
            file.read((uint8_t*)&stats, sizeof(GameStats));
            file.close();
            Serial.printf("GameStats Loaded: Lvl %d, XP %d\n", stats.level, stats.xp);
        }
    }

    static void save() {
        File file = SD_MMC.open(saveFile, FILE_WRITE);
        if (file) {
            file.write((uint8_t*)&stats, sizeof(GameStats));
            file.close();
        }
    }

    static void tick() {
        // Called every loop, but updates every second
        if (millis() - last_tick >= 1000) {
            stats.age_seconds++;
            last_tick = millis();

            // Auto-save every minute to reduce SD wear
            if (stats.age_seconds % 60 == 0) {
                save();
            }
        }
    }

    static void addXP(int amount) {
        stats.xp += amount;
        stats.interactions++;

        // Simple Level Curve: Lvl * 100 XP required
        uint32_t required = stats.level * 100;
        if (stats.xp >= required) {
            stats.xp -= required;
            stats.level++;
            Serial.println("LEVEL UP!");
            // Return true if level up?
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
};

GameStats Gamification::stats;
const char* Gamification::saveFile = "/game_stats.bin";
uint32_t Gamification::last_tick = 0;

#endif
