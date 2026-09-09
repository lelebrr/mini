#ifndef TOUCHFT3168_H
#define TOUCHFT3168_H

#include <Arduino.h>
#include <Wire.h>

class TouchFT3168 {
public:
    struct Point {
        int x = 0;
        int y = 0;
        bool touched = false;
    };

    bool begin(TwoWire &w = Wire, uint8_t addr = 0x38, int irq_pin = -1);
    Point read();
    bool isTouched();
    uint8_t getGesture();

private:
    TwoWire* wire;
    uint8_t _addr;
    int _irq_pin;
};

#endif
