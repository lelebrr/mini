#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD_MMC.h>
#include "FS.h"

// Simple singleton configuration manager backed by /config.json on SD.
class ConfigManager {
private:
    static ConfigManager* instance;
    DynamicJsonDocument doc;
    const char* configPath = "/config.json";

    ConfigManager() : doc(8192) {
    }

public:
    static ConfigManager* getInstance() {
        if (!instance) {
            instance = new ConfigManager();
        }
        return instance;
    }

    void load() {
        if (!SD_MMC.begin("/sdcard", true, true)) {
            Serial.println("[Config] SD_MMC not initialized, using defaults in RAM");
            createDefaults();
            return;
        }

        if (SD_MMC.exists(configPath)) {
            File file = SD_MMC.open(configPath, FILE_READ);
            if (!file) {
                Serial.println("[Config] Failed to open config file, using defaults");
                createDefaults();
                return;
            }

            DeserializationError err = deserializeJson(doc, file);
            file.close();
            if (err) {
                Serial.println("[Config] JSON error, recreating defaults");
                createDefaults();
            } else {
                validateMissingKeys();
                Serial.println("[Config] Loaded");
            }
        } else {
            Serial.println("[Config] No config file, creating defaults");
            createDefaults();
        }
    }

    void save() {
        if (!SD_MMC.begin("/sdcard", true, true)) {
            Serial.println("[Config] SD_MMC not available, skipping save");
            return;
        }

        File file = SD_MMC.open(configPath, FILE_WRITE);
        if (!file) {
            Serial.println("[Config] Failed to open config for write");
            return;
        }
        serializeJson(doc, file);
        file.close();
        Serial.println("[Config] Saved");
    }

    void createDefaults() {
        doc.clear();
        JsonObject root = doc.to<JsonObject>();

        // System / WiFi / Web
        root["sys_device_name"] = "PwnTamagotchi BR";
        root["sys_hostname"]    = "ptchi";
        root["sys_wifi_mode"]   = "AP";          // AP, STA, AP_STA
        root["sys_ap_ssid"]     = "Mini-Lele";
        root["sys_ap_pass"]     = "minilele";
        root["sys_sta_ssid"]    = "";
        root["sys_sta_pass"]    = "";
        root["sys_web_user"]    = "admin";
        root["sys_web_pass"]    = "admin";
        root["sys_ntp_server"]  = "pool.ntp.org";
        root["sys_timezone"]    = -3;            // BRT
        root["sys_ota_enabled"] = true;
        root["sys_language"]    = "pt_br";

        // Pet
        root["pet_name"]             = "PwnBaby";
        root["pet_voice_enabled"]    = true;
        root["pet_hunger_rate"]      = 1;
        root["pet_happiness_decay"]  = 1;
        root["pet_evolution_enabled"]= true;
        root["pet_sfx_volume"]       = 80;
        root["pet_wake_word"]        = "ei_pwn";
        root["pet_wake_word_enabled"]= true;
        root["pet_mood_dynamic"]     = true;

        // Display
        root["disp_brightness"]      = 200;      // 0-255
        root["disp_timeout_sec"]     = 30;
        root["disp_screensaver"]     = "matrix";
        root["disp_theme"]           = "cyber_favela";
        root["disp_rotation"]        = 0;
        root["disp_show_fps"]        = false;

        // Power
        root["pwr_deep_sleep_enabled"] = true;
        root["pwr_cpu_freq_max"]       = 240;
        root["pwr_cpu_freq_min"]       = 80;
        root["pwr_auto_dim"]           = true;
        root["pwr_battery_critical"]   = 10;

        // Attacks (minimal set)
        root["atk_auto_scan"]          = true;
        root["atk_scan_interval"]      = 10;
        root["atk_channel_hop"]        = true;
        root["atk_channels"]           = "1,6,11";
        root["atk_deauth_enabled"]     = false;
        root["atk_evil_portal"]        = false;

        // Web UI
        root["web_theme"]        = "cyber_favela";
        root["web_live_logs"]    = true;

        save();
    }

    void validateMissingKeys() {
        bool changed = false;
        JsonObject root = doc.as<JsonObject>();

        auto ensure = [&](const char* key, JsonVariantConst defVal) {
            if (!root.containsKey(key)) {
                root[key] = defVal;
                changed = true;
            }
        };

        ensure("sys_device_name",  "PwnTamagotchi BR");
        ensure("sys_hostname",     "ptchi");
        ensure("sys_wifi_mode",    "AP");
        ensure("sys_ap_ssid",      "Mini-Lele");
        ensure("sys_ap_pass",      "minilele");
        ensure("sys_web_user",     "admin");
        ensure("sys_web_pass",     "admin");
        ensure("sys_ntp_server",   "pool.ntp.org");
        ensure("sys_timezone",     -3);
        ensure("sys_ota_enabled",  true);
        ensure("sys_language",     "pt_br");

        ensure("pet_name",             "PwnBaby");
        ensure("pet_voice_enabled",    true);
        ensure("pet_hunger_rate",      1);
        ensure("pet_happiness_decay",  1);
        ensure("pet_evolution_enabled",true);
        ensure("pet_sfx_volume",       80);
        ensure("pet_wake_word",        "ei_pwn");
        ensure("pet_wake_word_enabled",true);
        ensure("pet_mood_dynamic",     true);

        ensure("disp_brightness",      200);
        ensure("disp_timeout_sec",     30);
        ensure("disp_screensaver",     "matrix");
        ensure("disp_theme",           "cyber_favela");
        ensure("disp_rotation",        0);
        ensure("disp_show_fps",        false);

        ensure("pwr_deep_sleep_enabled", true);
        ensure("pwr_cpu_freq_max",       240);
        ensure("pwr_cpu_freq_min",       80);
        ensure("pwr_auto_dim",           true);
        ensure("pwr_battery_critical",   10);

        ensure("atk_auto_scan",      true);
        ensure("atk_scan_interval",  10);
        ensure("atk_channel_hop",    true);
        ensure("atk_channels",       "1,6,11");
        ensure("atk_deauth_enabled", false);
        ensure("atk_evil_portal",    false);

        ensure("web_theme",      "cyber_favela");
        ensure("web_live_logs",  true);

        if (changed) {
            save();
        }
    }

    template <typename T>
    T get(const char* key) {
        return doc[key].as<T>();
    }

    String getString(const char* key) {
        return doc[key].as<String>();
    }

    template <typename T>
    void set(const char* key, T value) {
        doc[key] = value;
        save();
    }

    void getJSON(String &output) {
        serializeJson(doc, output);
    }

    void updateFromJSON(const String &json) {
        DynamicJsonDocument incoming(doc.capacity());
        DeserializationError err = deserializeJson(incoming, json);
        if (err) {
            Serial.println("[Config] JSON update error");
            return;
        }
        JsonObject src = incoming.as<JsonObject>();
        JsonObject dst = doc.as<JsonObject>();
        for (JsonPairConst kv : src) {
            dst[kv.key()] = kv.value();
        }
        save();
    }
};

inline ConfigManager* ConfigManager::instance = nullptr;

#endif
