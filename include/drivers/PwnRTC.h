#ifndef PWNRTCMODULE_H
#define PWNRTCMODULE_H

#include <Arduino.h>
#include <Wire.h>

#define PCF85063_ADDR 0x51

class PwnRTC {
public:
    static bool ok;

    static void init();
    static bool syncNTP(const char* server, int tz);
    static String getTimeString();
    static void setDateTime(int y, int m, int d, int h, int min, int s);
    static unsigned long getEpoch();
    static String formatTimestamp();
    static void readRegisters(uint8_t* data, uint8_t len);
    static void writeRegister(uint8_t reg, uint8_t val);
};

#endif
