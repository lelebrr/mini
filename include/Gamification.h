#ifndef GAMIFICATION_H
#define GAMIFICATION_H

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406

struct GameStats {
    uint32_t xp;
    uint32_t level;
    uint32_t age_seconds;
    uint32_t interactions;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    uint32_t handshakes_total;
    uint32_t missions_completed;
=======
>>>>>>> origin/pwn-tamagotchi-br-release
=======
    uint32_t handshakes_total;
    uint32_t missions_completed;
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
    uint32_t handshakes_total;
    uint32_t missions_completed;
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
    uint32_t handshakes_total;
    uint32_t missions_completed;
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
    uint32_t handshakes_total;
    uint32_t missions_completed;
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
};

class Gamification {
private:
    static GameStats stats;
    static const char* saveFile;
    static uint32_t last_tick;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
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
<<<<<<< HEAD
<<<<<<< HEAD
        trophies.push_back({"hacker_master", "PwnGod", false}); // Reverted to PwnGod
        trophies.push_back({"survivor", "Imortal", false});
    }

<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406

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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
        trophies.push_back({"hacker_master", "PwnGod", false});
        trophies.push_back({"survivor", "Imortal", false});
    }

<<<<<<< HEAD
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
    static void load() {
        if (SD_MMC.exists(saveFile)) {
            File file = SD_MMC.open(saveFile, FILE_READ);
            file.read((uint8_t*)&stats, sizeof(GameStats));
            file.close();
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final

            if (stats.handshakes_total > 0) unlockTrophy("first_blood");
            if (stats.handshakes_total >= 10000) unlockTrophy("hacker_master");
        }
    }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
            Serial.printf("[Game] Dados carregados: Lvl %d, XP %d\n", stats.level, stats.xp);
        }
    }

    /**
     * Salva estatísticas no arquivo binário.
     */
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
    static void save() {
        File file = SD_MMC.open(saveFile, FILE_WRITE);
        if (file) {
            file.write((uint8_t*)&stats, sizeof(GameStats));
            file.close();
        }
    }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
    /**
     * Atualiza o contador de tempo (chamar no loop principal).
     */
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
    /**
     * Atualiza o contador de tempo (chamar no loop principal).
     */
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
=======
    /**
     * Atualiza o contador de tempo (chamar no loop principal).
     */
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
    /**
     * Atualiza o contador de tempo (chamar no loop principal).
     */
>>>>>>> origin/waveshare-s3-amoled-full-review-406
    static void tick() {
        if (millis() - last_tick >= 1000) {
            stats.age_seconds++;
            last_tick = millis();

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
            updateMission("Sobreviva 1h sem USB", 1);

=======
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
            updateMission("Sobreviva 1h sem USB", 1);

            // Overclock dinâmico (Gameplay 8)
            // Lógica implementada no PowerManager baseada em stats.level

<<<<<<< HEAD
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
            if (stats.age_seconds % 60 == 0) save();
        }
    }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
    static void addXP(int amount) {
        stats.xp += amount;
        stats.interactions++;

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
        uint32_t required = stats.level * 500;
        if (stats.xp >= required) {
            stats.xp -= required;
            stats.level++;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
        // Curva simples: Nível * 100 XP necessário para o próximo
        uint32_t required = stats.level * 100;
        if (stats.xp >= required) {
            stats.xp -= required;
            stats.level++;
            Serial.println("[Game] LEVEL UP!");
            // Aqui poderia tocar um som especial
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
            // TODO: Play Sound
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
            // TODO: Play Sound
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
        }
        save();
    }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
    static void registerHandshake() {
        stats.handshakes_total++;
        addXP(100);
        updateMission("Capture 5 Handshakes", 1);

        if (stats.handshakes_total == 1) unlockTrophy("first_blood");
        if (stats.handshakes_total == 10000) unlockTrophy("hacker_master");
    }

    static void triggerBadUSB() {
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
        // Gameplay 9: BadUSB inofensivo se triste
        // Como o ESP32-S3 tem USB nativo:
        // USB.begin();
        // Keyboard.begin();
        // Keyboard.print("I am sad :(");
        // USB.end();
<<<<<<< HEAD
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406

#endif
