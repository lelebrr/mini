#ifndef TCA9554_H
#define TCA9554_H

#include <Arduino.h>
#include <Wire.h>

#define TCA9554_ADDR 0x20

class TCA9554 {
public:
    bool begin(TwoWire &w = Wire, uint8_t addr = TCA9554_ADDR);
    void pinMode(uint8_t pin, uint8_t mode);
    void digitalWrite(uint8_t pin, uint8_t val);
    uint8_t digitalRead(uint8_t pin);
    uint8_t read8();
    void write8(uint8_t val);

private:
    TwoWire* wire;
    uint8_t _addr;
    uint8_t _output;
    uint8_t _input;
    uint8_t _config;
};

#endif
