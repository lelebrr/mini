#ifndef PWN_PET_H
#define PWN_PET_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD_MMC.h>
#include "FS.h"
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#include "ConfigManager.h"
=======
>>>>>>> origin/pwn-tamagotchi-br-release
=======
#include "ConfigManager.h"
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
#include "ConfigManager.h"
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features

// Estágios de Evolução
enum PetStage {
    EGG = 0,
    BABY,
    KID,
    TEEN,
    ADULT,
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    PWNLORD, // Reverted
    PWNGOD   // Reverted
=======
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
>>>>>>> origin/pwn-tamagotchi-br-release
=======
    PWNLORD, // Reverted
    PWNGOD   // Reverted
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
    PWNLORD, // Reverted
    PWNGOD   // Reverted
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
    PWNLORD,
    PWNGOD // Otimização 30: Forma Secreta
>>>>>>> origin/pwntamagotchi-br-final-90-features
};

struct PetStats {
    String name;
    PetStage stage;
    int xp;
    int level;
    int hunger; // 0-100 (100 = Cheio)
    int happiness; // 0-100
    int handshakes_total;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
    unsigned long last_save;
    bool is_sleeping;
    int energy_mode;
    int sleep_hours;
<<<<<<< HEAD
<<<<<<< HEAD
=======
    int handshakes_today;
    unsigned long birth_date;
    unsigned long last_save;
    bool is_sleeping;
    bool silent_mode;
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
    unsigned long last_save;
    bool is_sleeping;

    // Otimização 6: Modo Energia afeta XP
    int energy_mode; // 0=Eco, 1=Normal, 2=Turbo
    int sleep_hours; // Otimização 23: Siesta Track
>>>>>>> origin/pwntamagotchi-br-final-90-features
};

class PwnPet {
private:
    static PetStats stats;
    static const char* savePath;

public:
    static void init() {
        savePath = "/pwn_pet_save.json";
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
        // Valores Padrão - Nome vem do ConfigManager se disponivel, ou padrao aqui
        stats.name = ConfigManager::getInstance()->getString("pet_name");
        if (stats.name.length() == 0) stats.name = "PwnBaby";

<<<<<<< HEAD
<<<<<<< HEAD
=======
        // Valores Padrão
        stats.name = "PwnBaby";
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
        // Valores Padrão
        stats.name = "PwnBaby";
>>>>>>> origin/pwntamagotchi-br-final-90-features
        stats.stage = EGG;
        stats.xp = 0;
        stats.level = 1;
        stats.hunger = 100;
        stats.happiness = 100;
        stats.handshakes_total = 0;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        stats.is_sleeping = false;
        stats.energy_mode = 1; // Normal
        stats.sleep_hours = 0;
=======
        stats.handshakes_today = 0;
        stats.birth_date = 0; // Será setado via RTC
        stats.is_sleeping = false;
>>>>>>> origin/pwn-tamagotchi-br-release
=======
        stats.is_sleeping = false;
        stats.energy_mode = 1; // Normal
        stats.sleep_hours = 0;
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
        stats.is_sleeping = false;
        stats.energy_mode = 1; // Normal
        stats.sleep_hours = 0;
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
        stats.is_sleeping = false;
        stats.energy_mode = 1; // Normal
        stats.sleep_hours = 0;
>>>>>>> origin/pwntamagotchi-br-final-90-features

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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
            stats.energy_mode = doc["energy_mode"] | 1;

            Serial.printf("[Pet] Carregado: %s (Lvl %d)\n", stats.name.c_str(), stats.level);
        } else {
            Serial.println("[Pet] Novo Save Criado!");
            save();
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
            // ... carregar resto
            Serial.printf("[Pet] Carregado: %s (Lvl %d)\n", stats.name.c_str(), stats.level);
        } else {
            Serial.println("[Pet] Novo Save Criado!");
            save(); // Salva inicial
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
        }
    }

    static void save() {
        DynamicJsonDocument doc(2048);
        doc["name"] = stats.name;
        doc["stage"] = (int)stats.stage;
        doc["xp"] = stats.xp;
        doc["level"] = stats.level;
        doc["handshakes_total"] = stats.handshakes_total;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        doc["energy_mode"] = stats.energy_mode;
=======
>>>>>>> origin/pwn-tamagotchi-br-release
=======
        doc["energy_mode"] = stats.energy_mode;
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
        doc["energy_mode"] = stats.energy_mode;
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
        doc["energy_mode"] = stats.energy_mode;
>>>>>>> origin/pwntamagotchi-br-final-90-features

        File f = SD_MMC.open(savePath, FILE_WRITE);
        serializeJson(doc, f);
        f.close();
        stats.last_save = millis();
    }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    static void tick() {
=======
    // Lógica de Jogo
    static void tick() {
        // Reduz fome/felicidade a cada X tempo
>>>>>>> origin/pwn-tamagotchi-br-release
=======
    static void tick() {
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
    static void tick() {
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
    static void tick() {
>>>>>>> origin/pwntamagotchi-br-final-90-features
        static unsigned long last_decay = 0;
        if (millis() - last_decay > 60000) { // 1 min
            last_decay = millis();
            if (!stats.is_sleeping) {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
                // Otimização 6: Eco mode reduz fome mais devagar
>>>>>>> origin/pwntamagotchi-br-final-90-features
                int hunger_loss = (stats.energy_mode == 0) ? 0 : 1;
                stats.hunger = max(0, stats.hunger - hunger_loss);
                stats.happiness = max(0, stats.happiness - 1);
            }
<<<<<<< HEAD
=======
            // Auto Save a cada 5 min
>>>>>>> origin/pwntamagotchi-br-final-90-features
            if (millis() - stats.last_save > 300000) save();
        }
        checkEvolution();
    }

    static void addXP(int amount) {
<<<<<<< HEAD
=======
        // Otimização 6: XP Boost em Turbo Mode, Penalidade em Eco
>>>>>>> origin/pwntamagotchi-br-final-90-features
        float multiplier = 1.0;
        if (stats.energy_mode == 2) multiplier = 1.5;
        if (stats.energy_mode == 0) multiplier = 0.5;

        stats.xp += (int)(amount * multiplier);

        if (stats.xp > stats.level * 500) {
            stats.level++;
            stats.happiness = 100;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
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
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
        }
        checkEvolution();
    }

    static void addHandshake(bool is_wpa3) {
        stats.handshakes_total++;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        int xp_gain = is_wpa3 ? 100 : 20;
        addXP(xp_gain);
        stats.hunger = min(100, stats.hunger + 2);
=======
        stats.handshakes_today++;
        int xp_gain = is_wpa3 ? 100 : 20; // Karma system
        addXP(xp_gain);
        feed(xp_gain / 5);
>>>>>>> origin/pwn-tamagotchi-br-release
=======
        int xp_gain = is_wpa3 ? 100 : 20;
        addXP(xp_gain);
        stats.hunger = min(100, stats.hunger + 2);
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
        int xp_gain = is_wpa3 ? 100 : 20;
        addXP(xp_gain);
        stats.hunger = min(100, stats.hunger + 2);
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
        int xp_gain = is_wpa3 ? 100 : 20;
        addXP(xp_gain);
        stats.hunger = min(100, stats.hunger + 2); // Handshake alimenta!
>>>>>>> origin/pwntamagotchi-br-final-90-features
        save();
    }

    static void checkEvolution() {
        PetStage old = stats.stage;
        if (stats.level >= 5 && stats.stage == EGG) stats.stage = BABY;
        else if (stats.level >= 10 && stats.stage == BABY) stats.stage = KID;
        else if (stats.level >= 20 && stats.stage == KID) stats.stage = TEEN;
        else if (stats.level >= 30 && stats.stage == TEEN) stats.stage = ADULT;
        else if (stats.level >= 50 && stats.stage == ADULT) stats.stage = PWNLORD;

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        if (stats.handshakes_total >= 10000) stats.stage = PWNGOD;

        if (old != stats.stage) {
=======
        if (stats.handshakes_total >= 10000) stats.stage = PWNGOD; // Secreto

        if (old != stats.stage) {
            // Evento de Evolução
>>>>>>> origin/pwn-tamagotchi-br-release
=======
        if (stats.handshakes_total >= 10000) stats.stage = PWNGOD;

        if (old != stats.stage) {
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
        if (stats.handshakes_total >= 10000) stats.stage = PWNGOD;

        if (old != stats.stage) {
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
        // Otimização 30: Forma Secreta "PwnGod" com 10k handshakes
        if (stats.handshakes_total >= 10000) stats.stage = PWNGOD;

        if (old != stats.stage) {
>>>>>>> origin/pwntamagotchi-br-final-90-features
            Serial.println("[Pet] EVOLUCAO!");
        }
    }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
    static int getHunger() { return stats.hunger; }
    static PetStats& getStats() { return stats; }

    static String getMoodStr() {
        if (stats.hunger < 20) return "FAMINTO";
        if (stats.happiness < 20) return "TRISTE";
        if (stats.stage == PWNGOD) return "DIVINO";
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
    // Getters
    static PetStats& getStats() { return stats; }
    static String getMoodStr() {
        if (stats.hunger < 20) return "FAMINTO";
        if (stats.happiness < 20) return "TRISTE";
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
        return "FELIZ";
    }
};

PetStats PwnPet::stats;
const char* PwnPet::savePath = "/pwn_pet_save.json";

#endif
