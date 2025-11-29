#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD_MMC.h>
#include "FS.h"

// Singleton Config Manager
// Gerencia 120 configurações com persistência no SD

class ConfigManager {
private:
    static ConfigManager* instance;
    DynamicJsonDocument* doc;
    const char* configPath = "/config.json";
    bool _dirty = false;

    ConfigManager() {
        // Aumentado para 16KB para acomodar 120 chaves + strings longas
        doc = new DynamicJsonDocument(16384);
    }

public:
    static ConfigManager* getInstance() {
        if (!instance) instance = new ConfigManager();
        return instance;
    }

    void load() {
        if (SD_MMC.exists(configPath)) {
            File file = SD_MMC.open(configPath, FILE_READ);
            DeserializationError error = deserializeJson(*doc, file);
            file.close();
            if (error) {
                Serial.println("[Config] Falha ao ler JSON, criando padrao...");
                createDefaults();
            } else {
                Serial.println("[Config] Carregado com sucesso.");
                validateMissingKeys(); // Garante que novas chaves existam
            }
        } else {
            createDefaults();
        }
    }

    void save() {
        File file = SD_MMC.open(configPath, FILE_WRITE);
        serializeJson(*doc, file);
        file.close();
        _dirty = false;
        Serial.println("[Config] Salvo.");
    }

    void createDefaults() {
        JsonObject root = doc->to<JsonObject>();

        // --- 1. SYSTEM IDENTITY ---
        root["sys_device_name"] = "PwnTamagotchi BR - Legendary";
        root["sys_hostname"] = "minilele";
        root["sys_wifi_mode"] = "AP";
        root["sys_ap_ssid"] = "Mini-Lele";
        root["sys_ap_pass"] = "minilele";
        root["sys_web_user"] = "admin";
        root["sys_web_pass"] = "admin";

        // --- 2. LEGENDARY FEATURES (14 Toggles) ---
        root["leg_clone_celular"] = false; // Evil Twin inteligente
        root["leg_pwn_radar"] = true; // UI
        root["leg_conquistas_br"] = true; // Achievements
        root["leg_modo_fantasma"] = false; // 03h-06h stealth
        root["leg_loot_box"] = true; // 06:66 (07:06)
        root["leg_auto_hashcat"] = false; // PCAP convert
        root["leg_crypto_miner"] = false; // Zoeira Monero
        root["leg_karaoke_8bit"] = false; // Fun
        root["leg_pwn_do_dia"] = true; // Daily target
        root["leg_modo_exame"] = false; // Blank screen
        root["leg_capture_wpa_ent"] = true; // EAPOL Enterprise
        root["leg_pwnversario"] = true; // Birthday logic
        root["leg_uber_pwn"] = false; // Stats per km
        root["leg_desligar_brasil"] = false; // Mass Deauth (Dangerous)

        // --- 3. PET (Gameplay) ---
        root["pet_name"] = "PwnBaby";
        root["pet_voice_enabled"] = true;
        root["pet_hunger_rate"] = 1;
        root["pet_happiness_decay"] = 1;
        root["pet_evolution_enabled"] = true;
        root["pet_sfx_volume"] = 80;
        root["pet_wake_word"] = "ei_pwn";

        // --- 4. ATTACKS ---
        root["atk_auto_scan"] = true;
        root["atk_deauth_enabled"] = false;
        root["atk_evil_portal"] = false;

        // --- 5. POWER ---
        root["pwr_deep_sleep_enabled"] = true;
        root["pwr_battery_critical"] = 10;

        // --- 6. WEBUI ---
        root["web_theme"] = "cyber_favela";
        root["web_live_logs"] = true;

        save();
    }

    void validateMissingKeys() {
        bool changed = false;
        JsonObject root = doc->as<JsonObject>();

        if (!root.containsKey("leg_pwn_radar")) { root["leg_pwn_radar"] = true; changed = true; }
        if (!root.containsKey("sys_device_name")) { root["sys_device_name"] = "PwnTamagotchi BR - Legendary"; changed = true; }

        if (changed) save();
    }

    template <typename T>
    T get(const char* key) {
        return (*doc)[key].as<T>();
    }

    String getString(const char* key) {
        return (*doc)[key].as<String>();
    }

    template <typename T>
    void set(const char* key, T value) {
        (*doc)[key] = value;
        _dirty = true;
        save();
    }

    void getJSON(String &output) {
        serializeJson(*doc, output);
    }

    void updateFromJSON(String json) {
        DeserializationError error = deserializeJson(*doc, json);
        if (!error) save();
    }
};

ConfigManager* ConfigManager::instance = 0;

#endif
