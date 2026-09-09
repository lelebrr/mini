#include "drivers/TouchFT3168.h"

#define FT3168_REG_MODE  0x00
#define FT3168_REG_TOUCH 0x02

bool TouchFT3168::begin(TwoWire &w, uint8_t addr, int irq_pin) {
    wire = &w;
    _addr = addr;
    _irq_pin = irq_pin;
    wire->beginTransmission(_addr);
    if (wire->endTransmission() != 0) return false;
    // Reset
    wire->beginTransmission(_addr);
    wire->write(0x00);
    wire->write(0x00);
    wire->endTransmission();
    return true;
}

TouchFT3168::Point TouchFT3168::read() {
    Point p;
    // Mapa FocalTech (FT3168/FT6x36) a partir de 0x02:
    //   d[0]=TD_STATUS (nº de toques em [3:0])
    //   d[1]=P1_XH (event flag [7:6], X[11:8] em [3:0])   d[2]=P1_XL (X[7:0])
    //   d[3]=P1_YH (touch id [7:4], Y[11:8] em [3:0])      d[4]=P1_YL (Y[7:0])
    wire->beginTransmission(_addr);
    wire->write(FT3168_REG_TOUCH);
    if (wire->endTransmission() != 0) return p;   // NAK -> sem toque
    wire->requestFrom(_addr, (uint8_t)5);
    if (wire->available() >= 5) {
        uint8_t d[5];
        for (int i = 0; i < 5; i++) d[i] = wire->read();
        uint8_t points = d[0] & 0x0F;             // nº de dedos
        uint8_t event  = (d[1] >> 6) & 0x03;      // 0=down,1=up,2=contact,3=none
        p.touched = (points > 0) && (event != 0x01);
        p.x = ((uint16_t)(d[1] & 0x0F) << 8) | d[2];
        p.y = ((uint16_t)(d[3] & 0x0F) << 8) | d[4];
    }
    return p;
}

bool TouchFT3168::isTouched() {
    wire->beginTransmission(_addr);
    wire->write(FT3168_REG_TOUCH);            // 0x02 = TD_STATUS
    if (wire->endTransmission() != 0) return false;
    wire->requestFrom(_addr, (uint8_t)1);
    if (wire->available()) {
        uint8_t d = wire->read();
        return (d & 0x0F) > 0;                // nº de toques > 0
    }
    return false;
}

uint8_t TouchFT3168::getGesture() {
    wire->beginTransmission(_addr);
    wire->write(0x01);
    wire->endTransmission();
    wire->requestFrom(_addr, (uint8_t)1);
    if (wire->available()) return wire->read();
    return 0;
}
