#ifndef GAMIFICATION_H
#define GAMIFICATION_H

#include <Arduino.h>
#include <SD_MMC.h>
#include "FS.h"
#include <vector>

// Gameplay 14: Missões Diárias
// Gameplay 10: Galeria de Troféus
// Gameplay 9: BadUSB Trigger

struct Mission {
    String description;
    int target_count;
    int current_count;
    bool completed;
    int xp_reward;
};

struct Trophy {
    String id;
    String name;
    bool unlocked;
};

struct GameStats {
    uint32_t xp;
    uint32_t level;
    uint32_t age_seconds;
    uint32_t interactions;
    uint32_t handshakes_total;
    uint32_t missions_completed;
};

class Gamification {
private:
    static GameStats stats;
    static const char* saveFile;
    static uint32_t last_tick;
    static std::vector<Mission> active_missions;
    static std::vector<Trophy> trophies;

public:
    static void init() {
        saveFile = "/game_stats.bin";
        stats = {0, 1, 0, 0, 0, 0};
        last_tick = millis();

        load();

        if (active_missions.empty()) {
            active_missions.push_back({"Capture 5 Handshakes", 5, 0, false, 500});
            active_missions.push_back({"Fale com o Pet", 10, 0, false, 200});
            active_missions.push_back({"Sobreviva 1h sem USB", 60, 0, false, 300});
        }

        trophies.push_back({"first_blood", "Primeira Captura", false});
        trophies.push_back({"hacker_master", "PwnGod", false});
        trophies.push_back({"survivor", "Imortal", false});
    }

    static void load() {
        if (SD_MMC.exists(saveFile)) {
            File file = SD_MMC.open(saveFile, FILE_READ);
            file.read((uint8_t*)&stats, sizeof(GameStats));
            file.close();

            if (stats.handshakes_total > 0) unlockTrophy("first_blood");
            if (stats.handshakes_total >= 10000) unlockTrophy("hacker_master");
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
        if (millis() - last_tick >= 1000) {
            stats.age_seconds++;
            last_tick = millis();

            updateMission("Sobreviva 1h sem USB", 1);

            // Overclock dinâmico (Gameplay 8)
            // Lógica implementada no PowerManager baseada em stats.level

            if (stats.age_seconds % 60 == 0) save();
        }
    }

    static void addXP(int amount) {
        stats.xp += amount;
        stats.interactions++;

        uint32_t required = stats.level * 500;
        if (stats.xp >= required) {
            stats.xp -= required;
            stats.level++;
            // TODO: Play Sound
        }
        save();
    }

    static void registerHandshake() {
        stats.handshakes_total++;
        addXP(100);
        updateMission("Capture 5 Handshakes", 1);

        if (stats.handshakes_total == 1) unlockTrophy("first_blood");
        if (stats.handshakes_total == 10000) unlockTrophy("hacker_master");
    }

    static void triggerBadUSB() {
        // Gameplay 9: BadUSB inofensivo se triste
        // Como o ESP32-S3 tem USB nativo:
        // USB.begin();
        // Keyboard.begin();
        // Keyboard.print("I am sad :(");
        // USB.end();
    }

    static void updateMission(String desc_keyword, int increment) {
        for (auto &m : active_missions) {
            if (m.description.indexOf(desc_keyword) >= 0 && !m.completed) {
                m.current_count += increment;
                if (m.current_count >= m.target_count) {
                    m.completed = true;
                    stats.missions_completed++;
                    addXP(m.xp_reward);
                }
            }
        }
    }

    static void unlockTrophy(String id) {
        for (auto &t : trophies) {
            if (t.id == id && !t.unlocked) {
                t.unlocked = true;
                addXP(1000);
            }
        }
    }

    static GameStats getStats() { return stats; }
    static std::vector<Mission>& getMissions() { return active_missions; }
};

GameStats Gamification::stats;
const char* Gamification::saveFile = "/game_stats.bin";
uint32_t Gamification::last_tick = 0;
std::vector<Mission> Gamification::active_missions;
std::vector<Trophy> Gamification::trophies;

#endif
