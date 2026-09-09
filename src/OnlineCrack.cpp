#include "OnlineCrack.h"
#include <SD_MMC.h>
#include <WiFi.h>

static bool uploading = false;
static bool auto_upload = false;

void OnlineCrack::init() {
    uploading = false;
    auto_upload = false;
    Serial.println("[OnlineCrack] Inicializado");
}

void OnlineCrack::uploadHandshake(const char* path) {
    Serial.printf("[OnlineCrack] Upload: %s\n", path);
    uploading = true;
    // Upload do handshake para servidor
    uploading = false;
}

void OnlineCrack::uploadAll() {
    uploading = true;
    Serial.println("[OnlineCrack] Upload all");
    uploading = false;
}

bool OnlineCrack::isUploading() { return uploading; }

void OnlineCrack::setCredentials(const char* ssid, const char* pass, const char* key) {
    Serial.printf("[OnlineCrack] Credenciais: %s\n", ssid);
}

void OnlineCrack::setAutoUpload(bool en) { auto_upload = en; }
