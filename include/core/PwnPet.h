#ifndef PWN_PET_H
#define PWN_PET_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD_MMC.h>
#include "FS.h"

// Estágios de Evolução
enum PetStage {
    EGG = 0,
    BABY,
    KID,
    TEEN,
    ADULT,
    PWNLORD,
    PWNGOD // Secreto
};

// Humor
enum PetMood {
    HAPPY,
    BORED,
    ANGRY,
    SLEEPY,
    HUNGRY,
    EVIL,
    COOL
};

struct PetStats {
    String name;
    PetStage stage;
    int xp;
    int level;
    int hunger; // 0-100 (100 = Cheio)
    int happiness; // 0-100
    int handshakes_total;
    int handshakes_today;
    unsigned long birth_date;
    unsigned long last_save;
    bool is_sleeping;
    bool silent_mode;
};

class PwnPet {
private:
    static PetStats stats;
    static const char* savePath;

public:
    static void init() {
        savePath = "/pwn_pet_save.json";
        // Valores Padrão
        stats.name = "PwnBaby";
        stats.stage = EGG;
        stats.xp = 0;
        stats.level = 1;
        stats.hunger = 100;
        stats.happiness = 100;
        stats.handshakes_total = 0;
        stats.handshakes_today = 0;
        stats.birth_date = 0; // Será setado via RTC
        stats.is_sleeping = false;

        load();
    }

    static void load() {
        if (SD_MMC.exists(savePath)) {
            File f = SD_MMC.open(savePath, FILE_READ);
            DynamicJsonDocument doc(2048);
            deserializeJson(doc, f);
            f.close();

            stats.name = doc["name"].as<String>();
            stats.stage = (PetStage)doc["stage"].as<int>();
            stats.xp = doc["xp"];
            stats.level = doc["level"];
            stats.handshakes_total = doc["handshakes_total"];
            // ... carregar resto
            Serial.printf("[Pet] Carregado: %s (Lvl %d)\n", stats.name.c_str(), stats.level);
        } else {
            Serial.println("[Pet] Novo Save Criado!");
            save(); // Salva inicial
        }
    }

    static void save() {
        DynamicJsonDocument doc(2048);
        doc["name"] = stats.name;
        doc["stage"] = (int)stats.stage;
        doc["xp"] = stats.xp;
        doc["level"] = stats.level;
        doc["handshakes_total"] = stats.handshakes_total;

        File f = SD_MMC.open(savePath, FILE_WRITE);
        serializeJson(doc, f);
        f.close();
        stats.last_save = millis();
    }

    // Lógica de Jogo
    static void tick() {
        // Reduz fome/felicidade a cada X tempo
        static unsigned long last_decay = 0;
        if (millis() - last_decay > 60000) { // 1 min
            last_decay = millis();
            if (!stats.is_sleeping) {
                stats.hunger = max(0, stats.hunger - 1);
                stats.happiness = max(0, stats.happiness - 1);
            }

            // Auto Save a cada 5 min
            if (millis() - stats.last_save > 300000) save();
        }

        checkEvolution();
    }

    static void feed(int quality) {
        stats.hunger = min(100, stats.hunger + quality);
        stats.happiness = min(100, stats.happiness + (quality/2));
        addXP(quality);
    }

    static void addXP(int amount) {
        stats.xp += amount;
        // Level Up
        if (stats.xp > stats.level * 500) {
            stats.level++;
            stats.happiness = 100;
            // Tocar som de Level UP
        }
        checkEvolution();
    }

    static void addHandshake(bool is_wpa3) {
        stats.handshakes_total++;
        stats.handshakes_today++;
        int xp_gain = is_wpa3 ? 100 : 20; // Karma system
        addXP(xp_gain);
        feed(xp_gain / 5);
        save();
    }

    static void checkEvolution() {
        PetStage old = stats.stage;
        if (stats.level >= 5 && stats.stage == EGG) stats.stage = BABY;
        else if (stats.level >= 10 && stats.stage == BABY) stats.stage = KID;
        else if (stats.level >= 20 && stats.stage == KID) stats.stage = TEEN;
        else if (stats.level >= 30 && stats.stage == TEEN) stats.stage = ADULT;
        else if (stats.level >= 50 && stats.stage == ADULT) stats.stage = PWNLORD;

        if (stats.handshakes_total >= 10000) stats.stage = PWNGOD; // Secreto

        if (old != stats.stage) {
            // Evento de Evolução
            Serial.println("[Pet] EVOLUCAO!");
        }
    }

    // Getters
    static PetStats& getStats() { return stats; }
    static String getMoodStr() {
        if (stats.hunger < 20) return "FAMINTO";
        if (stats.happiness < 20) return "TRISTE";
        return "FELIZ";
    }
};

PetStats PwnPet::stats;
const char* PwnPet::savePath = "/pwn_pet_save.json";

#endif
