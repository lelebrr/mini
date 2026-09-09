#ifndef PWNPET_H
#define PWNPET_H

#include <Arduino.h>
#include <ArduinoJson.h>

struct PetStats {
    int level = 1;
    int xp = 0;
    int hunger = 80;
    int happiness = 50;
    int stage = 0;
    int total_handshakes = 0;
    int total_ble = 0;
    int total_interactions = 0;
    String name = "Lele";
    unsigned long last_feed = 0;
    unsigned long last_save = 0;
    uint32_t evolution_flags = 0;
};

class PwnPet {
public:
    static PetStats stats;
    static const char* savePath;

    static void init();
    static void tick();
    static void feed(int amount);
    static void addHandshake();
    static void addBLE();
    static void addXP(int amount);
    static bool onShake();
    static void setMood(const char* mood);
    static void checkEvolution();
    static void syncRTC();
    static bool load();
    static bool save();
    static int getStage();
    static const char* getStageName();
};

#endif
