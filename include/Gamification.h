#ifndef GAMIFICATION_H
#define GAMIFICATION_H

#include <Arduino.h>
#include <SD_MMC.h>
#include "FS.h"

/**
 * Gamification
 *
 * Sistema simples de progressão:
 *  - XP / Level
 *  - Tempo ligado (age_seconds)
 *  - Contador de interações e handshakes
 * Persistido em /game_stats.bin no SD.
 */

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
    static GameStats  stats;
    static const char *saveFile;
    static uint32_t   last_tick_ms;

public:
    static void init() {
        saveFile      = "/game_stats.bin";
        stats.xp      = 0;
        stats.level   = 1;
        stats.age_seconds        = 0;
        stats.interactions       = 0;
        stats.handshakes_total   = 0;
        stats.missions_completed = 0;
        last_tick_ms             = millis();
        load();
    }

    static void load() {
        if (!SD_MMC.exists(saveFile)) {
            Serial.println("[Game] Nenhum save de gamificação, usando defaults.");
            save();
            return;
        }

        File file = SD_MMC.open(saveFile, FILE_READ);
        if (!file) {
            Serial.println("[Game] Falha ao abrir /game_stats.bin");
            return;
        }

        if (file.read(reinterpret_cast<uint8_t *>(&stats), sizeof(GameStats)) != sizeof(GameStats)) {
            Serial.println("[Game] Tamanho inesperado em /game_stats.bin, resetando.");
            stats.xp      = 0;
            stats.level   = 1;
            stats.age_seconds        = 0;
            stats.interactions       = 0;
            stats.handshakes_total   = 0;
            stats.missions_completed = 0;
        }
        file.close();

        Serial.printf("[Game] Carregado: Lvl %u, XP %u\n", stats.level, stats.xp);
    }

    static void save() {
        File file = SD_MMC.open(saveFile, FILE_WRITE);
        if (!file) {
            Serial.println("[Game] Falha ao salvar /game_stats.bin");
            return;
        }
        file.write(reinterpret_cast<const uint8_t *>(&stats), sizeof(GameStats));
        file.close();
    }

    // Deve ser chamado a partir do loop principal (a cada ~1s)
    static void tick() {
        uint32_t now = millis();
        if (now - last_tick_ms >= 1000) {
            last_tick_ms = now;
            stats.age_seconds++;

            // Auto-save a cada minuto
            if ((stats.age_seconds % 60) == 0) {
                save();
            }
        }
    }

    static void addXP(uint32_t amount) {
        stats.xp += amount;
        stats.interactions++;

        uint32_t required = stats.level * 100;
        while (stats.xp >= required) {
            stats.xp -= required;
            stats.level++;
            required = stats.level * 100;
            Serial.println("[Game] LEVEL UP!");
        }
        save();
    }

    static void registerHandshake() {
        stats.handshakes_total++;
        stats.interactions++;
        // Recompensa simples de XP
        addXP(100);
        save();
    }

    static GameStats getStats() {
        return stats;
    }

    static String getLevelStr() {
        return "Nvl " + String(stats.level);
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

// Definições estáticas declaradas em core_singletons.cpp
#endif
