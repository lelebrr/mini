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
        _ok = false;

        // Auto-detecção: FT3x68 fica em 0x38; GT1151/GT911 responde em
        // 0x14 ou 0x5D dependendo do pino de seleção da placa.
        const uint8_t candidates[] = { addr, 0x38, 0x14, 0x5D, 0x15, 0x5A };
        for (uint8_t a : candidates) {
            _wire->beginTransmission(a);
            if (_wire->endTransmission() == 0) {
                _addr = a;
                _ok = true;
                break;
            }
        }

        // Ninguém respondeu? Varre o barramento inteiro para diagnóstico:
        // mostra todos os dispositivos presentes (PMU, IMU, RTC, touch...).
        if (!_ok) {
            Serial.print("[Touch] Nenhum touch respondeu (cand. 0x38/0x14/0x5D/0x15/0x5A). Scan I2C:");
            for (uint8_t a = 1; a < 127; ++a) {
                _wire->beginTransmission(a);
                if (_wire->endTransmission() == 0) {
                    Serial.printf(" 0x%02X", a);
                }
            }
            Serial.println();
            return false;
        }

        Serial.printf("[Touch] Controlador em 0x%02X.\n", _addr);

        // Coloca o chip em modo normal de operação (registrador 0x00 = 0x00).
        writeReg(0x00, 0x00);
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
