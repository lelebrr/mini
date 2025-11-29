#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD_MMC.h>
#include "FS.h"

// Singleton Config Manager
// Gerencia 100 configurações com persistência no SD

class ConfigManager {
private:
    static ConfigManager* instance;
    DynamicJsonDocument* doc;
    const char* configPath = "/config.json";
    bool _dirty = false;

    ConfigManager() {
        // Aloca 8KB para JSON (Suficiente para ~100 chaves)
        doc = new DynamicJsonDocument(8192);
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

        // --- 1. PET (Gameplay) ---
        root["pet_name"] = "PwnBaby";
        root["pet_voice_enabled"] = true;
        root["pet_hunger_rate"] = 1; // 1-5
        root["pet_happiness_decay"] = 1;
        root["pet_evolution_enabled"] = true;
        root["pet_sfx_volume"] = 80;
        root["pet_tts_speed"] = 1.0;
        root["pet_wake_word_enabled"] = true;
        root["pet_wake_word"] = "ei_pwn";
        root["pet_mood_dynamic"] = true;

        // --- 2. DISPLAY ---
        root["disp_brightness"] = 200; // 0-255
        root["disp_timeout_sec"] = 30;
        root["disp_screensaver"] = "matrix"; // matrix, sleep, blank
        root["disp_theme"] = "cyber_favela";
        root["disp_rotation"] = 0;
        root["disp_show_fps"] = false;
        root["disp_matrix_color"] = "#00FF00";
        root["disp_gamma_correction"] = true;
        root["disp_font_size"] = "medium";
        root["disp_animations"] = true;

        // --- 3. POWER ---
        root["pwr_deep_sleep_enabled"] = true;
        root["pwr_cpu_freq_max"] = 240;
        root["pwr_cpu_freq_min"] = 80;
        root["pwr_auto_dim"] = true;
        root["pwr_battery_critical"] = 10; // %
        root["pwr_zombie_mode"] = true; // <3%
        root["pwr_disable_leds"] = true;
        root["pwr_wifi_powersave"] = true;
        root["pwr_peripheral_shutdown"] = true;
        root["pwr_siesta_mode"] = true; // Soneca forçada

        // --- 4. ATTACKS (Pentest) ---
        root["atk_auto_scan"] = true;
        root["atk_scan_interval"] = 10; // sec
        root["atk_channel_hop"] = true;
        root["atk_channels"] = "1,6,11";
        root["atk_deauth_enabled"] = false; // Safe default
        root["atk_deauth_reason"] = 7;
        root["atk_beacon_spam"] = false;
        root["atk_beacon_list"] = "FBI_Van,Virus_Free,Skynet";
        root["atk_evil_portal"] = false;
        root["atk_portal_template"] = "google_login";
        root["atk_capture_handshakes"] = true;
        root["atk_capture_pmkid"] = true;
        root["atk_whitelist"] = "MyHomeWiFi";
        root["atk_blacklist"] = "Hospital,Gov";
        root["atk_random_mac"] = true;
        root["atk_led_feedback"] = false; // Stealth
        root["atk_save_pcaps"] = true;
        root["atk_sniffer_filter"] = "probe,beacon,auth,assoc";
        root["atk_aggressive_level"] = 1; // 1-3
        root["atk_karma_enabled"] = false;

        // --- 5. SYSTEM ---
        root["sys_wifi_mode"] = "AP"; // AP, STA, AP_STA
        root["sys_ap_ssid"] = "PwnTamagotchi-BR";
        root["sys_ap_pass"] = "pwnme123";
        root["sys_sta_ssid"] = "";
        root["sys_sta_pass"] = "";
        root["sys_web_user"] = "admin";
        root["sys_web_pass"] = "admin";
        root["sys_hostname"] = "ptchi";
        root["sys_ntp_server"] = "pool.ntp.org";
        root["sys_timezone"] = -3; // BRT

        // --- EXTRA 20 (81-100) ---
        root["web_theme"] = "cyber_favela";
        root["web_kiosk_mode"] = false;
        root["web_live_logs"] = true;
        root["web_stealth_mode"] = false; // Opt 91
        root["web_map_heatmap"] = true; // Opt 99
        root["web_log_level"] = 3;
        root["sys_ota_enabled"] = true;
        root["sys_auto_backup"] = true; // Opt 89
        root["sys_usb_mode"] = "serial"; // serial, mass_storage
        root["pet_voice_remote"] = false; // Opt 87
        root["atk_probe_logging"] = true; // Opt 94
        root["atk_beacon_spam_timer"] = 5; // Opt 97 (min)
        root["atk_target_manufacturer"] = "Apple,Samsung";
        root["sys_language"] = "pt_br";
        root["sys_sound_on_boot"] = true;
        root["sys_led_notification"] = true;
        root["sys_haptic_feedback"] = false; // Hardware doesnt support, sound alt
        root["pet_allow_badusb"] = false;
        root["pet_allow_swear_words"] = false; // Family friendly?
        root["sys_factory_reset"] = false;

        save();
    }

    void validateMissingKeys() {
        // Checa se falta alguma chave e adiciona (para upgrades de versão)
        bool changed = false;
        JsonObject root = doc->as<JsonObject>();

        if (!root.containsKey("web_theme")) { root["web_theme"] = "cyber_favela"; changed = true; }
        if (!root.containsKey("sys_web_user")) { root["sys_web_user"] = "admin"; changed = true; }
        // Adicionar outras verificações críticas aqui

        if (changed) save();
    }

    // Getters Genéricos
    template <typename T>
    T get(const char* key) {
        return (*doc)[key].as<T>();
    }

    String getString(const char* key) {
        return (*doc)[key].as<String>();
    }

    // Setters
    template <typename T>
    void set(const char* key, T value) {
        (*doc)[key] = value;
        _dirty = true; // Marca para salvar depois ou salvar imediato
        save(); // O requisito pede salvamento imediato
    }

    // Retorna JSON bruto para a WebAPI
    void getJSON(String &output) {
        serializeJson(*doc, output);
    }

    // Atualiza do JSON recebido da WebAPI
    void updateFromJSON(String json) {
        DeserializationError error = deserializeJson(*doc, json);
        if (!error) save();
    }
};

ConfigManager* ConfigManager::instance = 0;

#endif
