#ifndef PWN_PET_H
#define PWN_PET_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD_MMC.h>
#include "FS.h"
#include "ConfigManager.h"

// Estágios de evolução do Pet
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
    String        name;
    PetStage      stage;
    int           xp;
    int           level;
    int           hunger;          // 0–100 (0 = faminto, 100 = cheio)
    int           happiness;       // 0–100
    int           handshakes_total;
    unsigned long last_save_ms;
    bool          is_sleeping;
    int           energy_mode;     // 0 = Eco, 1 = Normal, 2 = Turbo
    int           sleep_hours;
};

class PwnPet {
private:
    static PetStats    stats;
    static const char *savePath;

public:
    static void init() {
        savePath = "/pwn_pet_save.json";

        // Valores padrão
        String cfgName = ConfigManager::getInstance()->getString("pet_name");
        stats.name     = cfgName.length() ? cfgName : "PwnBaby";
        stats.stage    = EGG;
        stats.xp       = 0;
        stats.level    = 1;
        stats.hunger   = 100;
        stats.happiness        = 100;
        stats.handshakes_total = 0;
        stats.last_save_ms     = 0;
        stats.is_sleeping      = false;
        stats.energy_mode      = 1; // Normal
        stats.sleep_hours      = 0;

        load();
    }

    static void load() {
        if (!SD_MMC.exists(savePath)) {
            Serial.println("[Pet] Nenhum save encontrado, usando defaults.");
            save();
            return;
        }

        File f = SD_MMC.open(savePath, FILE_READ);
        if (!f) {
            Serial.println("[Pet] Falha ao abrir save, usando defaults.");
            return;
        }

        DynamicJsonDocument doc(2048);
        DeserializationError err = deserializeJson(doc, f);
        f.close();
        if (err) {
            Serial.println("[Pet] Erro ao ler save, usando defaults.");
            return;
        }

        if (doc.containsKey("name"))  stats.name  = doc["name"].as<String>();
        if (doc.containsKey("stage")) stats.stage = (PetStage)doc["stage"].as<int>();
        if (doc.containsKey("xp"))    stats.xp    = doc["xp"].as<int>();
        if (doc.containsKey("level")) stats.level = doc["level"].as<int>();

        if (doc.containsKey("hunger"))      stats.hunger      = doc["hunger"].as<int>();
        if (doc.containsKey("happiness"))   stats.happiness   = doc["happiness"].as<int>();
        if (doc.containsKey("handshakes_total"))
            stats.handshakes_total = doc["handshakes_total"].as<int>();
        if (doc.containsKey("energy_mode"))
            stats.energy_mode = doc["energy_mode"].as<int>();

        Serial.printf("[Pet] Save carregado: %s (Lvl %d)\n",
                      stats.name.c_str(), stats.level);
    }

    static void save() {
        DynamicJsonDocument doc(2048);
        doc["name"]             = stats.name;
        doc["stage"]            = (int)stats.stage;
        doc["xp"]               = stats.xp;
        doc["level"]            = stats.level;
        doc["hunger"]           = stats.hunger;
        doc["happiness"]        = stats.happiness;
        doc["handshakes_total"] = stats.handshakes_total;
        doc["energy_mode"]      = stats.energy_mode;

        File f = SD_MMC.open(savePath, FILE_WRITE);
        if (!f) {
            Serial.println("[Pet] Falha ao salvar /pwn_pet_save.json");
            return;
        }
        serializeJson(doc, f);
        f.close();
        stats.last_save_ms = millis();
    }

    // Tick de jogo – chamada periódica (ex.: a cada segundo)
    static void tick() {
        static unsigned long last_decay_ms = 0;
        unsigned long now = millis();

        // Decaimento de fome/felicidade a cada 60 s
        if (now - last_decay_ms >= 60000) {
            last_decay_ms = now;

            if (!stats.is_sleeping) {
                int hunger_loss = (stats.energy_mode == 0) ? 0 : 1;
                stats.hunger    = max(0, stats.hunger - hunger_loss);
                stats.happiness = max(0, stats.happiness - 1);
            }

            // Auto-save a cada 5 minutos
            if (now - stats.last_save_ms > 300000) {
                save();
            }
        }

        checkEvolution();
    }

    static void feed(int quality) {
        stats.hunger    = min(100, stats.hunger + quality);
        stats.happiness = min(100, stats.happiness + quality / 2);
        addXP(quality);
    }

    static void addXP(int amount) {
        float mult = 1.0f;
        if (stats.energy_mode == 2) mult = 1.5f;
        if (stats.energy_mode == 0) mult = 0.5f;

        stats.xp += (int)(amount * mult);

        while (stats.xp >= stats.level * 100) {
            stats.xp -= stats.level * 100;
            stats.level++;
            stats.happiness = 100;
        }

        checkEvolution();
        save();
    }

    static void addHandshake(bool is_wpa3) {
        stats.handshakes_total++;
        int xp_gain = is_wpa3 ? 100 : 20;
        addXP(xp_gain);
        stats.hunger = min(100, stats.hunger + 2);
        save();
    }

    static void checkEvolution() {
        PetStage old = stats.stage;

        if (stats.level >= 5  && stats.stage == EGG)  stats.stage = BABY;
        if (stats.level >= 10 && stats.stage == BABY) stats.stage = KID;
        if (stats.level >= 20 && stats.stage == KID)  stats.stage = TEEN;
        if (stats.level >= 30 && stats.stage == TEEN) stats.stage = ADULT;
        if (stats.level >= 50 && stats.stage == ADULT) stats.stage = PWNLORD;

        if (stats.handshakes_total >= 10000) stats.stage = PWNGOD;

        if (stats.stage != old) {
            Serial.println("[Pet] Evolução de estágio!");
        }
    }

    static int getHunger() {
        return stats.hunger;
    }

    static PetStats &getStats() {
        return stats;
    }

    static String getMoodStr() {
        if (stats.hunger < 20)      return "FAMINTO";
        if (stats.happiness < 20)   return "TRISTE";
        if (stats.stage == PWNGOD)  return "DIVINO";
        return "FELIZ";
    }
};

// Definições estáticas declaradas em core_singletons.cpp
#endif
