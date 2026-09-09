#include "drivers/TCA9554.h"

#define TCA9554_REG_INPUT  0x00
#define TCA9554_REG_OUTPUT 0x01
#define TCA9554_REG_CONFIG 0x03

bool TCA9554::begin(TwoWire &w, uint8_t addr) {
    wire = &w;
    _addr = addr;
    _output = 0xFF;
    _config = 0x00;
    wire->beginTransmission(_addr);
    if (wire->endTransmission() != 0) return false;
    write8(_output);
    // Configurar todos como saída
    wire->beginTransmission(_addr);
    wire->write(TCA9554_REG_CONFIG);
    wire->write(_config);
    return wire->endTransmission() == 0;
}

void TCA9554::pinMode(uint8_t pin, uint8_t mode) {
    if (mode == INPUT) _config |= (1 << pin);
    else _config &= ~(1 << pin);
    wire->beginTransmission(_addr);
    wire->write(TCA9554_REG_CONFIG);
    wire->write(_config);
    wire->endTransmission();
}

void TCA9554::digitalWrite(uint8_t pin, uint8_t val) {
    if (val) _output |= (1 << pin);
    else _output &= ~(1 << pin);
    wire->beginTransmission(_addr);
    wire->write(TCA9554_REG_OUTPUT);
    wire->write(_output);
    wire->endTransmission();
}

uint8_t TCA9554::digitalRead(uint8_t pin) {
    wire->beginTransmission(_addr);
    wire->write(TCA9554_REG_INPUT);
    wire->endTransmission();
    wire->requestFrom(_addr, (uint8_t)1);
    _input = wire->read();
    return (_input >> pin) & 0x01;
}

uint8_t TCA9554::read8() {
    wire->beginTransmission(_addr);
    wire->write(TCA9554_REG_INPUT);
    wire->endTransmission();
    wire->requestFrom(_addr, (uint8_t)1);
    return wire->read();
}

void TCA9554::write8(uint8_t val) {
    _output = val;
    wire->beginTransmission(_addr);
    wire->write(TCA9554_REG_OUTPUT);
    wire->write(_output);
    wire->endTransmission();
}
