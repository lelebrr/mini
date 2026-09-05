#ifndef PWN_RTC_H
#define PWN_RTC_H

#include <Arduino.h>
#include <Wire.h>
#include <time.h>
#include <WiFi.h>
#include "SensorPCF85063.hpp"
#include "pin_config.h"

/**
 * PwnRTC
 * ------
 * Relógio de tempo real usando o chip PCF85063 (já presente na placa Waveshare).
 * Substitui o relógio baseado em millis() e fornece timestamps reais para logs
 * e capturas. Quando conectado à internet (modo STA), sincroniza via NTP e grava
 * a hora certa no PCF85063 (que mantém o horário mesmo desligado, via bateria).
 *
 * Tudo é seguro: se o RTC não for detectado, os métodos degradam para millis().
 */
class PwnRTC {
private:
    static SensorPCF85063 rtc;
    static bool ok;

public:
    static bool init() {
        if (!rtc.begin(Wire, PCF85063_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
            Serial.println("[RTC] PCF85063 não encontrado — usando millis().");
            ok = false;
            return false;
        }
        ok = true;
        RTC_DateTime dt = rtc.getDateTime();
        // Se o RTC nunca foi ajustado (ano inválido), inicia numa data base.
        if (dt.year < 2023 || dt.year > 2099) {
            rtc.setDateTime(2025, 1, 1, 0, 0, 0);
            Serial.println("[RTC] Sem hora válida — ajustado para 2025-01-01.");
        }
        // Alimenta o relógio do sistema (time.h) com a hora do RTC.
        syncSystemFromRTC();
        Serial.println("[RTC] PCF85063 online.");
        return true;
    }

    static bool isOk() { return ok; }

    // "HH:MM"
    static String getClock() {
        if (!ok) {
            unsigned long t = millis() / 1000;
            char b[6]; snprintf(b, sizeof(b), "%02d:%02d",
                                (int)((t / 3600) % 24), (int)((t / 60) % 60));
            return String(b);
        }
        RTC_DateTime d = rtc.getDateTime();
        char b[6]; snprintf(b, sizeof(b), "%02d:%02d", d.hour, d.minute);
        return String(b);
    }

    // "AAAA-MM-DD HH:MM:SS" — usado em nomes de arquivo e logs.
    static String getTimestamp() {
        if (!ok) return String(millis());
        RTC_DateTime d = rtc.getDateTime();
        char b[24];
        snprintf(b, sizeof(b), "%04d-%02d-%02d %02d:%02d:%02d",
                 d.year, d.month, d.day, d.hour, d.minute, d.second);
        return String(b);
    }

    // Versão segura p/ nome de arquivo (sem espaços nem ':').
    static String getFileStamp() {
        if (!ok) return String(millis());
        RTC_DateTime d = rtc.getDateTime();
        char b[24];
        snprintf(b, sizeof(b), "%04d%02d%02d_%02d%02d%02d",
                 d.year, d.month, d.day, d.hour, d.minute, d.second);
        return String(b);
    }

    // Copia a hora do RTC para o relógio do sistema (settimeofday).
    static void syncSystemFromRTC() {
        if (!ok) return;
        RTC_DateTime d = rtc.getDateTime();
        struct tm tmv = {};
        tmv.tm_year = d.year - 1900; tmv.tm_mon = d.month - 1; tmv.tm_mday = d.day;
        tmv.tm_hour = d.hour; tmv.tm_min = d.minute; tmv.tm_sec = d.second;
        time_t t = mktime(&tmv);
        // Evita designated initializer (exige C++20; é seguro com C++17 do toolchain,
        // mas deixa o código portável para toolchains que ainda não padronizaram).
        struct timeval tv;
        tv.tv_sec  = t;
        tv.tv_usec = 0;
        settimeofday(&tv, nullptr);
    }

    // Sincroniza via NTP (precisa de Wi-Fi STA conectado) e grava no PCF85063.
    // tz_offset_hours: fuso (ex.: -3 para BRT). Retorna true se obteve a hora.
    static bool syncNTP(const char *server, int tz_offset_hours) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[RTC] NTP ignorado (sem Wi-Fi STA).");
            return false;
        }
        configTime((long)tz_offset_hours * 3600, 0, server);
        struct tm ti;
        if (!getLocalTime(&ti, 8000)) {
            Serial.println("[RTC] Falha ao obter hora NTP.");
            return false;
        }
        if (ok) {
            rtc.setDateTime(ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday,
                            ti.tm_hour, ti.tm_min, ti.tm_sec);
        }
        Serial.printf("[RTC] NTP OK: %04d-%02d-%02d %02d:%02d\n",
                      ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min);
        return true;
    }

    static void setManual(int y, int mo, int d, int h, int mi, int s) {
        if (ok) rtc.setDateTime(y, mo, d, h, mi, s);
        syncSystemFromRTC();
    }
};

// Definições estáticas em core_singletons.cpp
#endif
