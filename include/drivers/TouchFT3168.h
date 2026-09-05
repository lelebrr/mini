#ifndef TOUCH_FT3168_H
#define TOUCH_FT3168_H

#include <Arduino.h>
#include <Wire.h>
#include "pin_config.h"

/**
 * TouchFT3168
 * -----------
 * Driver mínimo e autocontido para o controlador capacitivo FT3168 (FocalTech)
 * da placa Waveshare ESP32-S3-Touch-AMOLED-1.8, falando I2C diretamente.
 *
 * Motivo: as bibliotecas genéricas de touch (ex.: TouchLib) NÃO suportam o
 * FT3168, e o Arduino_DriveBus da Waveshare é pesado. O protocolo do FT3168 é
 * simples o bastante para lermos aqui, eliminando uma dependência externa e
 * garantindo que o projeto compile sem downloads adicionais.
 *
 * Mapa de registradores (compatível família FT3x68/FT6x36):
 *   0x02  -> número de toques ativos (bits 0..3)
 *   0x03  -> [bits 7..6 event flag][bits 3..0 = X high]
 *   0x04  -> X low
 *   0x05  -> [bits 3..0 = Y high]
 *   0x06  -> Y low
 */
class TouchFT3168 {
public:
    struct Point {
        int  x = 0;
        int  y = 0;
        bool touched = false;
    };

    bool begin(TwoWire &wire = Wire, uint8_t addr = FT3168_DEVICE_ADDRESS) {
        _wire = &wire;
        _addr = addr;

        // Confirma presença no barramento I2C.
        _wire->beginTransmission(_addr);
        if (_wire->endTransmission() != 0) {
            Serial.println("[Touch] FT3168 não respondeu no I2C.");
            _ok = false;
            return false;
        }

        // Coloca o chip em modo normal de operação (registrador 0x00 = 0x00).
        writeReg(0x00, 0x00);
        _ok = true;
        Serial.println("[Touch] FT3168 pronto.");
        return true;
    }

    bool isReady() const { return _ok; }

    // Lê um único ponto de toque. Retorna true se há dedo na tela.
    Point read() {
        Point p;
        if (!_ok) return p;

        uint8_t buf[7];
        if (!readRegs(0x00, buf, sizeof(buf))) return p;

        uint8_t touches = buf[0x02] & 0x0F;
        if (touches == 0 || touches > 5) return p;

        int rawX = ((buf[0x03] & 0x0F) << 8) | buf[0x04];
        int rawY = ((buf[0x05] & 0x0F) << 8) | buf[0x06];

        // Limita à resolução do painel (368 x 448).
        p.x = constrain(rawX, 0, LCD_WIDTH  - 1);
        p.y = constrain(rawY, 0, LCD_HEIGHT - 1);
        p.touched = true;
        return p;
    }

private:
    TwoWire *_wire = nullptr;
    uint8_t  _addr = FT3168_DEVICE_ADDRESS;
    bool     _ok   = false;

    void writeReg(uint8_t reg, uint8_t val) {
        _wire->beginTransmission(_addr);
        _wire->write(reg);
        _wire->write(val);
        _wire->endTransmission();
    }

    bool readRegs(uint8_t reg, uint8_t *out, size_t len) {
        _wire->beginTransmission(_addr);
        _wire->write(reg);
        if (_wire->endTransmission(false) != 0) return false;

        size_t got = _wire->requestFrom((int)_addr, (int)len);
        if (got < len) return false;
        for (size_t i = 0; i < len; i++) out[i] = _wire->read();
        return true;
    }
};

#endif // TOUCH_FT3168_H
