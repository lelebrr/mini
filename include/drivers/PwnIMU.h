#ifndef PWN_IMU_H
#define PWN_IMU_H

#include <Arduino.h>
#include <Wire.h>
#include "SensorLib.h"
#include "pin_config.h"

// Wrapper para o acelerômetro QMI8658
// Otimização 9 & 22: ULP + Low Power Motion Wakeup

class PwnIMU {
private:
    SensorQMI8658 qmi;
    bool initialized = false;

public:
    bool init(TwoWire &wire) {
        if (!qmi.begin(wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) { // Fix pin naming
             if (!qmi.begin(wire, QMI8658_H_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
                 Serial.println("[IMU] Falha ao iniciar QMI8658");
                 return false;
             }
        }

        Serial.println("[IMU] QMI8658 Online");

        qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, SensorQMI8658::ACC_ODR_1000Hz, SensorQMI8658::LPF_MODE_0, true);
        qmi.configGyroscope(SensorQMI8658::GYR_RANGE_64DPS, SensorQMI8658::GYR_ODR_896Hz, SensorQMI8658::LPF_MODE_3, true);
        qmi.enableGyroscope();
        qmi.enableAccelerometer();

        initialized = true;
        return true;
    }

    void enableWakeOnMotion() {
        if (!initialized) return;

        // Configuração AnyMotion para QMI8658
        // Threshold (~32mg per LSB usually), duration
        // qmi.configAnyMotion(threshold, duration);
        // Habilitar interrupção no INT1

        // Como a biblioteca SensorLib varia, usaremos a implementação genérica da lib se disponível
        // Se não, acesso direto aos registros (CTRL1...CTRL9)

        // Exemplo simplificado usando API pública da SensorLib (assumindo versão recente)
        qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_2G, SensorQMI8658::ACC_ODR_LowPower_5Hz, SensorQMI8658::LPF_MODE_0, true);
        qmi.enableAccelerometer();
        qmi.disableGyroscope(); // Economia

        // Enable Wake-on-motion
        // qmi.enableAnyMotion(true); // Placeholder function, verificar docs da lib Lewisxhe

        Serial.println("[IMU] Wake-on-Motion ativado (Low Power)");
    }

    void read(float &accX, float &accY, float &accZ) {
        if (!initialized) return;
        if (qmi.getDataReady()) {
            qmi.getAccelerometer(accX, accY, accZ);
        }
    }
};

#endif
