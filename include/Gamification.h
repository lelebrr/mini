#ifndef GAMIFICATION_H
#define GAMIFICATION_H

/**
 * Gamification.h
 * Sistema de RPG/Progresso para o dispositivo.
 *
 * Funcionalidades:
 * - Rastreia Nível, XP, Idade (tempo ligado) e Interações.
 * - Salva progresso persistentemente no Cartão SD (/game_stats.bin).
 * - Lógica de Level Up.
 */

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
    /**
     * Inicializa e carrega os dados salvos.
     */
    static void init() {
        saveFile = "/game_stats.bin";
        // Valores padrão
        stats.xp = 0;
        stats.level = 1;
        stats.age_seconds = 0;
        stats.interactions = 0;
        last_tick = millis();
        load();
    }

    /**
     * Carrega estatísticas do arquivo binário.
     */
    static void load() {
        if (SD_MMC.exists(saveFile)) {
            File file = SD_MMC.open(saveFile, FILE_READ);
            file.read((uint8_t*)&stats, sizeof(GameStats));
            file.close();
            Serial.printf("[Game] Dados carregados: Lvl %d, XP %d\n", stats.level, stats.xp);
        }
    }

    /**
     * Salva estatísticas no arquivo binário.
     */
    static void save() {
        File file = SD_MMC.open(saveFile, FILE_WRITE);
        if (file) {
            file.write((uint8_t*)&stats, sizeof(GameStats));
            file.close();
        }
    }

    /**
     * Atualiza o contador de tempo (chamar no loop principal).
     */
    static void tick() {
        if (millis() - last_tick >= 1000) {
            stats.age_seconds++;
            last_tick = millis();

            // Auto-salvar a cada minuto para não desgastar o SD
            if (stats.age_seconds % 60 == 0) {
                save();
            }
        }
    }

    /**
     * Adiciona experiência e verifica level up.
     * @param amount Quantidade de XP.
     */
    static void addXP(int amount) {
        stats.xp += amount;
        stats.interactions++;

        // Curva simples: Nível * 100 XP necessário para o próximo
        uint32_t required = stats.level * 100;
        if (stats.xp >= required) {
            stats.xp -= required;
            stats.level++;
            Serial.println("[Game] LEVEL UP!");
            // Aqui poderia tocar um som especial
        }
        save();
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

// Inicialização estática
GameStats Gamification::stats;
const char* Gamification::saveFile = "/game_stats.bin";
uint32_t Gamification::last_tick = 0;

#endif
