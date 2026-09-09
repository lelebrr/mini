#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD_MMC.h>

class ConfigManager {
public:
    static ConfigManager* getInstance();
    static ConfigManager* instance;

    template<typename T>
    T get(const char* key) { return doc[key].as<T>(); }

    int getInt(const char* key, int def = 0);
    bool getBool(const char* key, bool def = false);
    float getFloat(const char* key, float def = 0.0f);
    String getString(const char* key, const char* def = "");
    bool has(const char* key);
    void set(const char* key, int64_t val);
    void set(const char* key, bool val);
    void set(const char* key, float val);
    void set(const char* key, const char* val);
    void set(const char* key, String val);
    bool load();
    bool save();
    JsonDocument getJSONRedacted();
    bool updateFromJSON(const char* json);

private:
    ConfigManager() {}
    JsonDocument doc;
};

#endif
