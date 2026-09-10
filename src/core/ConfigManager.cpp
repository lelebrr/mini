#include "core/ConfigManager.h"
#include <SD_MMC.h>
#include <ArduinoJson.h>

static const char* CONFIG_PATH = "/config.json";

ConfigManager* ConfigManager::getInstance() {
    if (!instance) instance = new ConfigManager();
    return instance;
}

int ConfigManager::getInt(const char* key, int def) {
    if (!doc[key].is<int>()) return def;
    return doc[key].as<int>();
}

bool ConfigManager::getBool(const char* key, bool def) {
    if (!doc[key].is<bool>()) return def;
    return doc[key].as<bool>();
}

float ConfigManager::getFloat(const char* key, float def) {
    if (!doc[key].is<float>()) return def;
    return doc[key].as<float>();
}

String ConfigManager::getString(const char* key, const char* def) {
    if (!doc[key].is<const char*>()) return String(def);
    return doc[key].as<String>();
}

bool ConfigManager::has(const char* key) {
    return !doc[key].isNull();
}

void ConfigManager::set(const char* key, int64_t val) { doc[key] = val; }
void ConfigManager::set(const char* key, bool val) { doc[key] = val; }
void ConfigManager::set(const char* key, float val) { doc[key] = val; }
void ConfigManager::set(const char* key, const char* val) { doc[key] = val; }
void ConfigManager::set(const char* key, String val) { doc[key] = val.c_str(); }

bool ConfigManager::load() {
    if (!SD_MMC.begin("/sd", true)) {
        Serial.println("[Config] SD nao disponivel, usando padroes");
        return false;
    }
    File f = SD_MMC.open(CONFIG_PATH);
    if (!f) {
        Serial.println("[Config] config.json nao encontrado, criando padrao");
        save();
        return false;
    }
    DeserializationError err = deserializeJson(doc, f);
    f.close();
    if (err) {
        Serial.printf("[Config] Erro parsing JSON: %s\n", err.c_str());
        return false;
    }
    Serial.println("[Config] Configuracao carregada");
    return true;
}

bool ConfigManager::save() {
    if (!SD_MMC.begin("/sd", true)) return false;
    File f = SD_MMC.open(CONFIG_PATH, FILE_WRITE);
    if (!f) return false;
    serializeJson(doc, f);
    f.close();
    return true;
}

JsonDocument ConfigManager::getJSONRedacted() {
    JsonDocument copy;
    copy.set(doc);
    copy.remove("wifi_password");
    copy.remove("api_key");
    return copy;
}

bool ConfigManager::updateFromJSON(const char* json) {
    JsonDocument newDoc;
    DeserializationError err = deserializeJson(newDoc, json);
    if (err) return false;
    doc.set(newDoc);
    return save();
}
