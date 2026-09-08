#ifndef TCA9554_H
#define TCA9554_H

#include <Arduino.h>
#include <Wire.h>

/**
 * TCA9554 — driver mínimo do expansor de I/O I2C da Waveshare
 * ESP32-S3-Touch-AMOLED-1.8 (família TCA95xx/PCA9554 8-bit).
 *
 * Motivo de existir: a biblioteca externa ESP32_IO_Expander (lzw655, 1.1.0 —
 * única publicada) usa o driver I2C LEGADO do IDF (i2c_param_config +
 * i2c_driver_install). Com o Arduino core 3.x o Wire usa o driver novo
 * (driver_ng) na MESMA porta I2C0, e o IDF detecta o conflito e chama
 * abort() no boot:
 *   "E (xx) i2c: CONFLICT! driver_ng is not allowed to be used with this
 *    old driver"  ->  abort()  ->  crash loop / tela preta.
 * Este driver fala I2C direto pelo Wire, eliminando o conflito.
 *
 * Registradores (datasheet TCA9554):
 *   0x00 input port | 0x01 output port | 0x02 polarity | 0x03 configuration
 *   (bit de configuration = 1 -> pino é entrada; 0 -> saída)
 *
 * Uso típico neste projeto (mapeamento Waveshare):
 *   P0 = LCD_RST, P1 = TOUCH_RST, P2 = power periféricos,
 *   P6 = rail de áudio, P7 = CS do cartão SD (EXIO7).
 */
class TCA9554 {
public:
    static constexpr uint8_t REG_INPUT    = 0x00;
    static constexpr uint8_t REG_OUTPUT   = 0x01;
    static constexpr uint8_t REG_POLARITY = 0x02;
    static constexpr uint8_t REG_CONFIG   = 0x03;

    bool begin(TwoWire &wire = Wire, uint8_t address = 0x20) {
        _wire = &wire;
        _addr = address;
        _wire->beginTransmission(_addr);
        if (_wire->endTransmission() != 0) {
            _ok = false;
            return false;
        }
        // Restaura os caches a partir do chip (evita gravar às cegas e
        // "pulsar" pinos que estavam em outro estado).
        uint8_t out = 0, cfg = 0;
        if (readReg(REG_OUTPUT, out) && readReg(REG_CONFIG, cfg)) {
            _output = out;
            _config = cfg;
            _ok = true;
        } else {
            _ok = false;
        }
        return _ok;
    }

    bool isReady() const { return _ok; }

    // Aceita as macros do Arduino: OUTPUT (1) -> saída; INPUT/INPUT_PULLUP -> entrada.
    void pinMode(uint8_t pin, uint8_t mode) {
        if (!_ok || pin > 7) return;
        uint8_t cfg = _config;
        if (mode == OUTPUT) cfg &= (uint8_t)~(1 << pin);
        else                cfg |=  (uint8_t)(1 << pin);
        writeRegCached(REG_CONFIG, cfg, _config);
    }

    void digitalWrite(uint8_t pin, uint8_t val) {
        if (!_ok || pin > 7) return;
        uint8_t out = _output;
        if (val) out |=  (uint8_t)(1 << pin);
        else     out &= (uint8_t)~(1 << pin);
        writeRegCached(REG_OUTPUT, out, _output);
    }

    int digitalRead(uint8_t pin) {
        if (!_ok || pin > 7) return -1;
        uint8_t in = 0;
        if (!readReg(REG_INPUT, in)) return -1;
        return (in >> pin) & 0x01;
    }

    // Grava o byte inteiro do registrador de saída (1 transação I2C).
    bool writeOutputPort(uint8_t value) {
        return writeRegCached(REG_OUTPUT, value, _output);
    }

private:
    bool writeReg(uint8_t reg, uint8_t val) {
        _wire->beginTransmission(_addr);
        _wire->write(reg);
        _wire->write(val);
        return _wire->endTransmission() == 0;
    }

    bool readReg(uint8_t reg, uint8_t &val) {
        _wire->beginTransmission(_addr);
        _wire->write(reg);
        if (_wire->endTransmission(false) != 0) return false;
        if (_wire->requestFrom((int)_addr, 1) != 1) return false;
        val = _wire->read();
        return true;
    }

    // Grava só quando o valor muda (cache evita I2C redundante no loop de boot).
    bool writeRegCached(uint8_t reg, uint8_t val, uint8_t &cache) {
        if (val == cache) return true;
        if (!writeReg(reg, val)) return false;
        cache = val;
        return true;
    }

    TwoWire *_wire   = nullptr;
    uint8_t  _addr   = 0x20;
    bool     _ok     = false;
    uint8_t  _output = 0xFF;  // cache: registrador de saída
    uint8_t  _config = 0xFF;  // cache: registrador de configuração (1=entrada)
};

#endif // TCA9554_H
