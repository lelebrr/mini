#ifndef ONLINECRACK_H
#define ONLINECRACK_H

#include <Arduino.h>

class OnlineCrack {
public:
    static void init();
    static void uploadHandshake(const char* path);
    static void uploadAll();
    static bool isUploading();
    static void setCredentials(const char* ssid, const char* pass, const char* key);
    static void setAutoUpload(bool en);
};

#endif
