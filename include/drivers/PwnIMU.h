#ifndef PWN_IMU_H
#define PWN_IMU_H

#include <Arduino.h>
#include <Wire.h>
#include "SensorLib.h"
#include "pin_config.h"

// Wrapper simples para o acelerômetro/giroscópio QMI8658.

class PwnIMU {
private:
    SensorQMI8658 qmi;
    bool initialized = false;

public:
    bool init(TwoWire &wire) {
        // Usa o barramento I2C principal configurado em pin_config.h
        if (!qmi.begin(wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
            if (!qmi.begin(wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
                Serial.println("[IMU] Falha ao iniciar QMI8658");
                return false;
            }
        }

        Serial.println("[IMU] QMI8658 Online");

        qmi.configAccelerometer(
            SensorQMI8658::ACC_RANGE_4G,
            SensorQMI8658::ACC_ODR_1000Hz,
            SensorQMI8658::LPF_MODE_0,
            true
        );
        qmi.configGyroscope(
            SensorQMI8658::GYR_RANGE_64DPS,
            SensorQMI8658::GYR_ODR_896Hz,
            SensorQMI8658::LPF_MODE_3,
            true
        );
        qmi.enableGyroscope();
        qmi.enableAccelerometer();

        initialized = true;
        return true;
    }

    void enableWakeOnMotion() {
        if (!initialized) return;

        // Configuração simplificada de low power:
        qmi.configAccelerometer(
            SensorQMI8658::ACC_RANGE_2G,
            SensorQMI8658::ACC_ODR_LowPower_5Hz,
            SensorQMI8658::LPF_MODE_0,
            true
        );
        qmi.enableAccelerometer();
        qmi.disableGyroscope();

        Serial.println("[IMU] Wake-on-Motion (low power) configurado.");
    }

    void read(float &accX, float &accY, float &accZ) {
        if (!initialized) return;
        if (qmi.getDataReady()) {
            qmi.getAccelerometer(accX, accY, accZ);
        }
    }
};

#endif
