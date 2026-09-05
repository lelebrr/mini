#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD_MMC.h>
#include "FS.h"

/**
 * ConfigManager
 *
 * Gerencia configurações persistentes em /config.json no cartão SD.
 * Mantém os valores em memória (DynamicJsonDocument) e oferece:
 *  - load() / save()
 *  - get<T>(chave), getString(chave), set(chave, valor)
 *  - getJSON / updateFromJSON para integração com WebUI.
 *
 * O objetivo é ter uma base estável e simples, sem heranças de merges
 * antigos de outros firmwares.
 */
class ConfigManager {
private:
    static ConfigManager *instance;
    JsonDocument doc;                       // ArduinoJson 7: documento elástico
    const char *configPath = "/config.json";

    ConfigManager() {}

    // Preenche um JsonObject com os valores padrão
    static void fillDefaults(JsonObject root) {
        // --- PET (Gameplay) ---
        root["pet_name"]              = "PwnBaby";
        root["pet_voice_enabled"]     = true;
        root["pet_hunger_rate"]       = 1;      // 1–5
        root["pet_happiness_decay"]   = 1;
        root["pet_evolution_enabled"] = true;
        root["pet_sfx_volume"]        = 80;
        root["pet_tts_speed"]         = 1.0;
        root["pet_wake_word_enabled"] = true;
        root["pet_wake_word"]         = "ei_pwn";
        root["pet_mood_dynamic"]      = true;
        root["pet_voice_remote"]      = false;
        root["pet_allow_badusb"]      = false;
        root["pet_allow_swear_words"] = false;

        // --- DISPLAY ---
        root["disp_brightness"]        = 200;         // 0–255
        root["disp_timeout_sec"]       = 30;
        root["disp_screensaver"]       = "matrix";    // matrix, sleep, blank
        root["disp_theme"]             = "cyber_favela";
        root["disp_rotation"]          = 0;
        root["disp_show_fps"]          = false;
        root["disp_matrix_color"]      = "#00FF00";
        root["disp_gamma_correction"]  = true;
        root["disp_font_size"]         = "medium";
        root["disp_animations"]        = true;

        // --- POWER ---
        root["pwr_deep_sleep_enabled"]   = true;
        root["pwr_cpu_freq_max"]         = 240;
        root["pwr_cpu_freq_min"]         = 80;
        root["pwr_auto_dim"]             = true;
        root["pwr_battery_critical"]     = 10;   // %
        root["pwr_zombie_mode"]          = true; // <3%
        root["pwr_disable_leds"]         = true;
        root["pwr_wifi_powersave"]       = true;
        root["pwr_peripheral_shutdown"]  = true;
        root["pwr_siesta_mode"]          = true;
        // --- BATERIA / CARGA (LiPo PL502030 250mAh) ---
        root["pwr_battery_capacity_mah"] = 250;   // capacidade da célula
        root["pwr_charge_current_ma"]    = 100;   // 100/125/150/200 (<=125 ideal p/ 250mAh)
        root["pwr_charge_target_mv"]     = 4200;  // tensão de corte (padrão LiPo)
        root["pwr_low_warn_pct"]         = 10;    // aviso de bateria baixa
        root["pwr_low_shutdown_pct"]     = 5;     // desliga para proteger a célula
        root["pwr_deep_sleep_after_sec"] = 300;   // deep sleep após X s ocioso (na bateria)

        // --- ATTACKS / WIFI ---
        root["atk_auto_scan"]         = true;
        root["atk_scan_interval"]     = 10;          // s
        root["atk_channel_hop"]       = true;
        root["atk_channels"]          = "1,6,11";
        root["atk_deauth_enabled"]    = false;       // seguro por padrão
        root["atk_deauth_reason"]     = 7;
        root["atk_beacon_spam"]       = false;
        root["atk_beacon_list"]       = "FBI_Van,Virus_Free,Skynet";
        root["atk_evil_portal"]       = false;
        root["atk_portal_template"]   = "01_wifi_update.html";
        root["atk_capture_handshakes"]= true;
        root["atk_capture_pmkid"]     = true;
        root["atk_whitelist"]         = "MyHomeWiFi";
        root["atk_blacklist"]         = "Hospital,Gov";
        root["atk_random_mac"]        = true;
        root["atk_led_feedback"]      = false;
        root["atk_save_pcaps"]        = true;
        root["atk_sniffer_filter"]    = "probe,beacon,auth,assoc";
        root["atk_aggressive_level"]  = 1;           // 1–3
        root["atk_karma_enabled"]     = false;
        root["atk_probe_logging"]     = true;
        root["atk_beacon_spam_timer"] = 5;           // min
        root["atk_target_manufacturer"]= "Apple,Samsung";
        root["atk_ble_scan"]          = false;       // varredura BLE periódica
        root["atk_ble_interval_sec"]  = 30;          // intervalo da varredura BLE

        // --- SYSTEM / WEB ---
        root["sys_device_name"]   = "Mini Lele";
        root["sys_hostname"]      = "minilele";
        root["sys_wifi_mode"]     = "AP";      // AP, STA, AP_STA
        root["sys_ap_ssid"]       = "Mini-Lele";
        root["sys_ap_pass"]       = "minilele";
        root["sys_sta_ssid"]      = "";
        root["sys_sta_pass"]      = "";
        root["sys_web_user"]      = "admin";
        root["sys_web_pass"]      = "admin";
        root["sys_ntp_server"]    = "pool.ntp.org";
        root["sys_timezone"]      = -3;        // BRT
        root["sys_ota_enabled"]   = true;
        root["sys_auto_backup"]   = true;
        root["sys_usb_mode"]      = "serial";  // serial, mass_storage
        root["sys_language"]      = "pt_br";
        root["sys_sound_on_boot"] = true;
        root["sys_led_notification"] = true;
        root["sys_haptic_feedback"]  = false;
        root["sys_factory_reset"]    = false;
        root["sys_watchdog"]         = true;   // reinício automático se travar

        root["web_theme"]        = "cyber_favela";
        root["web_kiosk_mode"]   = false;
        root["web_live_logs"]    = true;
        root["web_stealth_mode"] = false;
        root["web_map_heatmap"]  = true;
        root["web_log_level"]    = 3;
    }

    void validateMissingKeys() {
        // Usa um documento temporário com os defaults e garante que
        // qualquer chave nova seja adicionada ao JSON existente.
        JsonDocument defaults;
        JsonObject defRoot = defaults.to<JsonObject>();
        fillDefaults(defRoot);

        JsonObject root = doc.as<JsonObject>();
        bool changed = false;

        for (JsonPair kv : defRoot) {
            const char *key = kv.key().c_str();
            if (root[key].isNull()) {
                root[key] = kv.value();
                changed = true;
            }
        }

        if (changed) {
            save();
        }
    }

public:
    static ConfigManager *getInstance() {
        if (!instance) {
            instance = new ConfigManager();
        }
        return instance;
    }

    void load() {
        if (SD_MMC.exists(configPath)) {
            File file = SD_MMC.open(configPath, FILE_READ);
            DeserializationError error = deserializeJson(doc, file);
            file.close();

            if (error) {
                Serial.println("[Config] Erro ao ler /config.json, recriando com defaults.");
                doc.clear();
                JsonObject root = doc.to<JsonObject>();
                fillDefaults(root);
                save();
            } else {
                Serial.println("[Config] Config carregada de /config.json.");
                validateMissingKeys();
            }
        } else {
            Serial.println("[Config] /config.json não existe, criando com defaults.");
            doc.clear();
            JsonObject root = doc.to<JsonObject>();
            fillDefaults(root);
            save();
        }
    }

    void save() {
        File file = SD_MMC.open(configPath, FILE_WRITE);
        if (!file) {
            Serial.println("[Config] Falha ao abrir /config.json para escrita.");
            return;
        }
        serializeJson(doc, file);
        file.close();
        Serial.println("[Config] Config salva.");
    }

    template <typename T>
    T get(const char *key) const {
        JsonVariant v = doc[key];
        if (v.isNull()) {
            return T();
        }
        return v.as<T>();
    }

    String getString(const char *key) const {
        JsonVariant v = doc[key];
        if (v.isNull()) {
            return String();
        }
        return v.as<String>();
    }

    template <typename T>
    void set(const char *key, T value) {
        doc[key] = value;
        save();
    }

    void getJSON(String &output) const {
        serializeJson(doc, output);
    }

    // Versão para a WebUI: mascara campos sensíveis (senhas) para não trafegar
    // credenciais em texto puro pela rede. O valor "********" é ignorado no
    // updateFromJSON (mantém a senha atual, a menos que o usuário digite outra).
    void getJSONRedacted(String &output) const {
        JsonDocument copy;
        copy.set(doc);  // cópia profunda
        const char *secret_keys[] = {
            "sys_web_pass", "sys_ap_pass", "sys_sta_pass"
        };
        for (const char *k : secret_keys) {
            if (!copy[k].isNull()) copy[k] = "********";
        }
        serializeJson(copy, output);
    }

    void updateFromJSON(const String &json) {
        JsonDocument incoming;
        DeserializationError err = deserializeJson(incoming, json);
        if (err) {
            Serial.println("[Config] JSON inválido recebido via WebUI.");
            return;
        }

        JsonObject root    = doc.as<JsonObject>();
        JsonObject updates = incoming.as<JsonObject>();

        for (JsonPair kv : updates) {
            // Ignora o placeholder de senha mascarada vindo da WebUI.
            if (kv.value().is<const char *>() &&
                String(kv.value().as<const char *>()) == "********") {
                continue;
            }
            root[kv.key().c_str()] = kv.value();
        }

        save();
    }
};

// Instância estática declarada em core_singletons.cpp
#endif
