#ifndef PWN_IMU_H
#define PWN_IMU_H

#include <Arduino.h>
#include <Wire.h>
#include "SensorLib.h"
#include "pin_config.h"

// Simple wrapper around QMI8658 accelerometer/gyro.
// Only basic accel readings are used for now.
class PwnIMU {
private:
    SensorQMI8658 qmi;
    bool initialized = false;

public:
    bool init(TwoWire &wire) {
        if (!qmi.begin(wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
            Serial.println("[IMU] QMI8658 not found");
            return false;
        }

        qmi.configAccelerometer(
            SensorQMI8658::ACC_RANGE_4G,
            SensorQMI8658::ACC_ODR_1000Hz,
            SensorQMI8658::LPF_MODE_0,
            true
        );
        qmi.enableAccelerometer();

        initialized = true;
        Serial.println("[IMU] QMI8658 Online");
        return true;
    }

    void enableWakeOnMotion() {
        // Optional low power mode; safe no-op if not supported.
        if (!initialized) return;

        qmi.configAccelerometer(
            SensorQMI8658::ACC_RANGE_2G,
            SensorQMI8658::ACC_ODR_LowPower_5Hz,
            SensorQMI8658::LPF_MODE_0,
            true
        );
        qmi.enableAccelerometer();
        Serial.println("[IMU] Wake-on-motion (basic) configured");
    }

    void read(float &accX, float &accY, float &accZ) {
        if (!initialized) return;
        if (qmi.getDataReady()) {
            qmi.getAccelerometer(accX, accY, accZ);
        }
    }
};

#endif
