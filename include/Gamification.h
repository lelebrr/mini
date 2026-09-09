#include <Arduino.h>
#include <ArduinoJson.h>

#ifndef GAMIFICATION_H
#define GAMIFICATION_H

struct GameStats {
    uint32_t total_handshakes = 0;
    uint32_t total_ble = 0;
    uint32_t total_scans = 0;
    uint32_t total_deauth = 0;
    uint32_t total_portal = 0;
    uint32_t total_voice = 0;
    uint32_t uptime_seconds = 0;
    uint32_t boot_count = 0;
    uint32_t xp = 0;
    uint32_t level = 1;
};

class Gamification {
public:
    static GameStats stats;
    static const char* saveFile;
    static uint32_t last_tick_ms;

    static void init();
    static void tick();
    static void addXP(int amount);
    static void registerHandshake();
    static void registerBLE();
    static void registerDeauth();
    static int getLevel();
    static void save();
    static void load();
    static void reset();
};

#endif
