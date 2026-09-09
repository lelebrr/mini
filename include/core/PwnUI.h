#ifndef PWNUI_H
#define PWNUI_H

#include <Arduino.h>
#include <lvgl.h>

class PwnUI {
public:
    static void init();
    static void update();
    static void nextTile();
    static void prevTile();
    static int getCurrentTile();
    static void setTile(int tile);
    static void showNotification(const char* msg, uint32_t color = 0x00FF00);
    static void showAlert(const char* msg, uint32_t color = 0xFF0000);
    static void showToast(const char* msg);
    static void updateStatusBar();
    static void updateBattery(int pct, bool charging);
    static void updateWiFi(bool connected, int ap_count);
    static void updateBLE(int count);
    static void updateClock(const char* time_str);
    static void updatePetStats();
};

#endif
