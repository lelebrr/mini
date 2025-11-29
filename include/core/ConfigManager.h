#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD_MMC.h>
#include "FS.h"

// Singleton Config Manager
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
// Gerencia 120 configurações com persistência no SD
=======
// Gerencia 100 configurações com persistência no SD
>>>>>>> origin/merge-ready-mini-lele-v2
=======
// Gerencia 100 configurações com persistência no SD
>>>>>>> origin/mini-lele-v2-complete-verified
=======
// Gerencia 100 configurações com persistência no SD
>>>>>>> origin/mini-lele-v2-final-verified

class ConfigManager {
private:
    static ConfigManager* instance;
    DynamicJsonDocument* doc;
    const char* configPath = "/config.json";
    bool _dirty = false;

    ConfigManager() {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	        // Aumentado para 16KB para acomodar 120 chaves + strings longas
	        doc = new DynamicJsonDocument(16384);
=======
        // Aloca 8KB para JSON (Suficiente para ~100 chaves)
        doc = new DynamicJsonDocument(8192);
>>>>>>> origin/merge-ready-mini-lele-v2
=======
        // Aloca 8KB para JSON (Suficiente para ~100 chaves)
        doc = new DynamicJsonDocument(8192);
>>>>>>> origin/mini-lele-v2-complete-verified
=======
        // Aloca 8KB para JSON (Suficiente para ~100 chaves)
        doc = new DynamicJsonDocument(8192);
>>>>>>> origin/mini-lele-v2-final-verified
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

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
	        root["pet_tts_speed"] = 1.0;
	        root["pet_wake_word_enabled"] = true;
	        root["pet_mood_dynamic"] = true;
	
	        // --- 4. DISPLAY ---
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
	
	        // --- 5. POWER ---
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
	
	        // --- 6. ATTACKS (Pentest) ---
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
	
	        // --- 7. SYSTEM (cont.) ---
	        root["sys_ntp_server"] = "pool.ntp.org";
	        root["sys_timezone"] = -3; // BRT
	        root["sys_ota_enabled"] = true;
	        root["sys_auto_backup"] = true; // Opt 89
	        root["sys_usb_mode"] = "serial"; // serial, mass_storage
	        root["sys_language"] = "pt_br";
	        root["sys_sound_on_boot"] = true;
	        root["sys_led_notification"] = true;
	        root["sys_haptic_feedback"] = false; // Hardware doesnt support, sound alt
	        root["sys_factory_reset"] = false;
	
	        // --- 8. WEBUI (cont.) ---
	        root["web_theme"] = "cyber_favela";
	        root["web_kiosk_mode"] = false;
	        root["web_live_logs"] = true;
	        root["web_stealth_mode"] = false; // Opt 91
	        root["web_map_heatmap"] = true; // Opt 99
	        root["web_log_level"] = 3;
	
	        // --- 9. VOICE (cont.) ---
	        root["pet_voice_remote"] = false; // Opt 87
	        root["pet_allow_badusb"] = false;
	        root["pet_allow_swear_words"] = false; // Family friendly?
	
	        // --- 10. ATTACKS (cont.) ---
	        root["atk_probe_logging"] = true; // Opt 94
	        root["atk_beacon_spam_timer"] = 5; // Opt 97 (min)
	        root["atk_target_manufacturer"] = "Apple,Samsung";
=======
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
        // --- 1. PET (Gameplay) ---
        root["pet_name"] = "PwnBaby"; // Default mantido
        root["pet_voice_enabled"] = true;
        root["pet_hunger_rate"] = 1; // 1-5
        root["pet_happiness_decay"] = 1;
        root["pet_evolution_enabled"] = true;
        root["pet_sfx_volume"] = 80;
        root["pet_tts_speed"] = 1.0;
        root["pet_wake_word_enabled"] = true;
        root["pet_wake_word"] = "ei_pwn"; // Default mantido
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
        root["sys_ap_ssid"] = "Mini-Lele"; // SYSTEM NAME CHANGED
        root["sys_ap_pass"] = "minilele";
        root["sys_sta_ssid"] = "";
        root["sys_sta_pass"] = "";
        root["sys_web_user"] = "admin";
        root["sys_web_pass"] = "admin";
        root["sys_hostname"] = "minilele";
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
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified

        save();
    }

    void validateMissingKeys() {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	        // Checa se falta alguma chave e adiciona (para upgrades de versão)
	        bool changed = false;
	        JsonObject root = doc->as<JsonObject>();
	
	        if (!root.containsKey("leg_pwn_radar")) { root["leg_pwn_radar"] = true; changed = true; }
	        if (!root.containsKey("sys_device_name")) { root["sys_device_name"] = "PwnTamagotchi BR - Legendary"; changed = true; }
	        if (!root.containsKey("web_theme")) { root["web_theme"] = "cyber_favela"; changed = true; }
	        if (!root.containsKey("sys_web_user")) { root["sys_web_user"] = "admin"; changed = true; }
	        // Adicionar outras verificações críticas aqui (adicionar as chaves novas do main)
	        if (!root.containsKey("pet_tts_speed")) { root["pet_tts_speed"] = 1.0; changed = true; }
	        if (!root.containsKey("pet_wake_word_enabled")) { root["pet_wake_word_enabled"] = true; changed = true; }
	        if (!root.containsKey("pet_mood_dynamic")) { root["pet_mood_dynamic"] = true; changed = true; }
	        if (!root.containsKey("disp_brightness")) { root["disp_brightness"] = 200; changed = true; }
	        if (!root.containsKey("disp_timeout_sec")) { root["disp_timeout_sec"] = 30; changed = true; }
	        if (!root.containsKey("disp_screensaver")) { root["disp_screensaver"] = "matrix"; changed = true; }
	        if (!root.containsKey("disp_theme")) { root["disp_theme"] = "cyber_favela"; changed = true; }
	        if (!root.containsKey("disp_rotation")) { root["disp_rotation"] = 0; changed = true; }
	        if (!root.containsKey("disp_show_fps")) { root["disp_show_fps"] = false; changed = true; }
	        if (!root.containsKey("disp_matrix_color")) { root["disp_matrix_color"] = "#00FF00"; changed = true; }
	        if (!root.containsKey("disp_gamma_correction")) { root["disp_gamma_correction"] = true; changed = true; }
	        if (!root.containsKey("disp_font_size")) { root["disp_font_size"] = "medium"; changed = true; }
	        if (!root.containsKey("disp_animations")) { root["disp_animations"] = true; changed = true; }
	        if (!root.containsKey("pwr_cpu_freq_max")) { root["pwr_cpu_freq_max"] = 240; changed = true; }
	        if (!root.containsKey("pwr_cpu_freq_min")) { root["pwr_cpu_freq_min"] = 80; changed = true; }
	        if (!root.containsKey("pwr_auto_dim")) { root["pwr_auto_dim"] = true; changed = true; }
	        if (!root.containsKey("pwr_zombie_mode")) { root["pwr_zombie_mode"] = true; changed = true; }
	        if (!root.containsKey("pwr_disable_leds")) { root["pwr_disable_leds"] = true; changed = true; }
	        if (!root.containsKey("pwr_wifi_powersave")) { root["pwr_wifi_powersave"] = true; changed = true; }
	        if (!root.containsKey("pwr_peripheral_shutdown")) { root["pwr_peripheral_shutdown"] = true; changed = true; }
	        if (!root.containsKey("pwr_siesta_mode")) { root["pwr_siesta_mode"] = true; changed = true; }
	        if (!root.containsKey("atk_scan_interval")) { root["atk_scan_interval"] = 10; changed = true; }
	        if (!root.containsKey("atk_channel_hop")) { root["atk_channel_hop"] = true; changed = true; }
	        if (!root.containsKey("atk_channels")) { root["atk_channels"] = "1,6,11"; changed = true; }
	        if (!root.containsKey("atk_deauth_reason")) { root["atk_deauth_reason"] = 7; changed = true; }
	        if (!root.containsKey("atk_beacon_spam")) { root["atk_beacon_spam"] = false; changed = true; }
	        if (!root.containsKey("atk_beacon_list")) { root["atk_beacon_list"] = "FBI_Van,Virus_Free,Skynet"; changed = true; }
	        if (!root.containsKey("atk_portal_template")) { root["atk_portal_template"] = "google_login"; changed = true; }
	        if (!root.containsKey("atk_capture_handshakes")) { root["atk_capture_handshakes"] = true; changed = true; }
	        if (!root.containsKey("atk_capture_pmkid")) { root["atk_capture_pmkid"] = true; changed = true; }
	        if (!root.containsKey("atk_whitelist")) { root["atk_whitelist"] = "MyHomeWiFi"; changed = true; }
	        if (!root.containsKey("atk_blacklist")) { root["atk_blacklist"] = "Hospital,Gov"; changed = true; }
	        if (!root.containsKey("atk_random_mac")) { root["atk_random_mac"] = true; changed = true; }
	        if (!root.containsKey("atk_led_feedback")) { root["atk_led_feedback"] = false; changed = true; }
	        if (!root.containsKey("atk_save_pcaps")) { root["atk_save_pcaps"] = true; changed = true; }
	        if (!root.containsKey("atk_sniffer_filter")) { root["atk_sniffer_filter"] = "probe,beacon,auth,assoc"; changed = true; }
	        if (!root.containsKey("atk_aggressive_level")) { root["atk_aggressive_level"] = 1; changed = true; }
	        if (!root.containsKey("atk_karma_enabled")) { root["atk_karma_enabled"] = false; changed = true; }
	        if (!root.containsKey("sys_sta_ssid")) { root["sys_sta_ssid"] = ""; changed = true; }
	        if (!root.containsKey("sys_sta_pass")) { root["sys_sta_pass"] = ""; changed = true; }
	        if (!root.containsKey("sys_ntp_server")) { root["sys_ntp_server"] = "pool.ntp.org"; changed = true; }
	        if (!root.containsKey("sys_timezone")) { root["sys_timezone"] = -3; changed = true; }
	        if (!root.containsKey("web_kiosk_mode")) { root["web_kiosk_mode"] = false; changed = true; }
	        if (!root.containsKey("web_stealth_mode")) { root["web_stealth_mode"] = false; changed = true; }
	        if (!root.containsKey("web_map_heatmap")) { root["web_map_heatmap"] = true; changed = true; }
	        if (!root.containsKey("web_log_level")) { root["web_log_level"] = 3; changed = true; }
	        if (!root.containsKey("sys_ota_enabled")) { root["sys_ota_enabled"] = true; changed = true; }
	        if (!root.containsKey("sys_auto_backup")) { root["sys_auto_backup"] = true; changed = true; }
	        if (!root.containsKey("sys_usb_mode")) { root["sys_usb_mode"] = "serial"; changed = true; }
	        if (!root.containsKey("pet_voice_remote")) { root["pet_voice_remote"] = false; changed = true; }
	        if (!root.containsKey("atk_probe_logging")) { root["atk_probe_logging"] = true; changed = true; }
	        if (!root.containsKey("atk_beacon_spam_timer")) { root["atk_beacon_spam_timer"] = 5; changed = true; }
	        if (!root.containsKey("atk_target_manufacturer")) { root["atk_target_manufacturer"] = "Apple,Samsung"; changed = true; }
	        if (!root.containsKey("sys_language")) { root["sys_language"] = "pt_br"; changed = true; }
	        if (!root.containsKey("sys_sound_on_boot")) { root["sys_sound_on_boot"] = true; changed = true; }
	        if (!root.containsKey("sys_led_notification")) { root["sys_led_notification"] = true; changed = true; }
	        if (!root.containsKey("sys_haptic_feedback")) { root["sys_haptic_feedback"] = false; changed = true; }
	        if (!root.containsKey("pet_allow_badusb")) { root["pet_allow_badusb"] = false; changed = true; }
	        if (!root.containsKey("pet_allow_swear_words")) { root["pet_allow_swear_words"] = false; changed = true; }
	        if (!root.containsKey("sys_factory_reset")) { root["sys_factory_reset"] = false; changed = true; }

        if (changed) save();
   	    // Getters Genéricos
=======
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
        // Checa se falta alguma chave e adiciona (para upgrades de versão)
        bool changed = false;
        JsonObject root = doc->as<JsonObject>();

        if (!root.containsKey("web_theme")) { root["web_theme"] = "cyber_favela"; changed = true; }
        if (!root.containsKey("sys_web_user")) { root["sys_web_user"] = "admin"; changed = true; }
        // Adicionar outras verificações críticas aqui

        if (changed) save();
    }

    // Getters Genéricos
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
    template <typename T>
    T get(const char* key) {
        return (*doc)[key].as<T>();
    }

    String getString(const char* key) {
        return (*doc)[key].as<String>();
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    }	    // Setters
	    template <typename T>
	    void set(const char* key, T value) {
	        (*doc)[key] = value;
	        _dirty = true; // Marca para salvar depois ou salvar imediato
	        save(); // O requisito pede salvamento imediato
	    }
	
	    // Retorna JSON bruto para a WebAPI(String &output) {
        serializeJson(*doc, output);
    }	    // Atualiza do JSON recebido da WebAPI void updateFromJSON(String json) {
=======
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
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
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
        DeserializationError error = deserializeJson(*doc, json);
        if (!error) save();
    }
};

ConfigManager* ConfigManager::instance = 0;

#endif
