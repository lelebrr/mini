#include "drivers/PwnRTC.h"
#include <WiFi.h>
#include <time.h>

void PwnRTC::init() {
    ok = false;
    Wire.beginTransmission(PCF85063_ADDR);
    if (Wire.endTransmission() == 0) {
        ok = true;
        Serial.println("[PwnRTC] PCF85063 detectado");
    } else {
        Serial.println("[PwnRTC] PCF85063 nao encontrado");
    }
}

bool PwnRTC::syncNTP(const char* server, int tz) {
    if (WiFi.status() != WL_CONNECTED) return false;
    configTime(tz * 3600, 0, server);
    Serial.printf("[PwnRTC] NTP sync: %s (tz=%d)\n", server, tz);
    return true;
}

String PwnRTC::getTimeString() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[32];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
    return String(buf);
}

void PwnRTC::setDateTime(int y, int m, int d, int h, int min, int s) {
    if (!ok) return;
    // Converter para BCD e escrever no PCF85063
    Wire.beginTransmission(PCF85063_ADDR);
    Wire.write(0x04);  // Segundos
    Wire.write(((s / 10) << 4) | (s % 10));
    Wire.write(((min / 10) << 4) | (min % 10));
    Wire.write(((h / 10) << 4) | (h % 10));
    Wire.write(((d / 10) << 4) | (d % 10));
    Wire.write(0);  // Dia da semana
    Wire.write(((m / 10) << 4) | (m % 10));
    Wire.write((((y - 2000) / 10) << 4) | ((y - 2000) % 10));
    Wire.endTransmission();
}

unsigned long PwnRTC::getEpoch() {
    return time(nullptr);
}

String PwnRTC::formatTimestamp() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[32];
    snprintf(buf, sizeof(buf), "%04d%02d%02d-%02d%02d%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
    return String(buf);
}

void PwnRTC::readRegisters(uint8_t* data, uint8_t len) {
    if (!ok) return;
    Wire.beginTransmission(PCF85063_ADDR);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom(PCF85063_ADDR, len);
    for (uint8_t i = 0; i < len && Wire.available(); i++) data[i] = Wire.read();
}

void PwnRTC::writeRegister(uint8_t reg, uint8_t val) {
    if (!ok) return;
    Wire.beginTransmission(PCF85063_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}
