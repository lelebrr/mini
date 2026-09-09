#include "AudioHandler.h"
#include "pin_config.h"
#include <Wire.h>

void AudioHandler::init() {
    audio_ok = false;
    // Habilitar amplificador no GPIO46
    pinMode(PA_EN, OUTPUT);
    digitalWrite(PA_EN, HIGH);
    // Tenta comunicação I2C com ES8311
    Wire.beginTransmission(0x18);
    if (Wire.endTransmission() == 0) {
        audio_ok = true;
        Serial.println("[Audio] ES8311 detectado");
    } else {
        Serial.println("[Audio] ES8311 nao encontrado, beep via delay");
    }
}

void AudioHandler::beep(int freq, int dur_ms) {
    Serial.printf("[Audio] Beep %dHz %dms\n", freq, dur_ms);
}

void AudioHandler::playWav(const char* path) {
    Serial.printf("[Audio] Tocando: %s\n", path);
}

void AudioHandler::recordWav(const char* path, int duration_ms) {
    Serial.printf("[Audio] Gravando: %s (%dms)\n", path, duration_ms);
}

void AudioHandler::setVolume(int vol) {
    Serial.printf("[Audio] Volume: %d\n", vol);
}

int AudioHandler::getVolume() { return 50; }

void AudioHandler::setAmpPower(bool on) {
    pinMode(PA_EN, OUTPUT);
    digitalWrite(PA_EN, on ? HIGH : LOW);
}
