#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>
#include <XPowersLib.h>
#include "AudioHandler.h"
#include "FaceHandler.h"

extern XPowersPMU power;      // definido em main.cpp
extern Arduino_GFX *gfx;      // definido em main.cpp

class CommandHandler {
public:
    // Mapeia quantidade de sílabas para ações simples
    static void processSyllables(int count) {
        Serial.printf("[Voice] Silabas detectadas: %d\n", count);

        switch (count) {
            case 1:
                // Ex.: "Luz" – força brilho máximo e face "cool"
                Serial.println("[CMD] 1 -> Luz / Destaque");
                gfx->Display_Brightness(255);
                FaceHandler::setFace(FACE_COOL);
                AudioHandler::playWav("/success_pt.wav");
                break;

            case 2:
                // Ex.: "Status" – som de boot e face feliz
                Serial.println("[CMD] 2 -> Status");
                FaceHandler::setFace(FACE_HAPPY);
                AudioHandler::playWav("/boot_pt.wav");
                break;

            case 3: {
                // Ex.: "Bateria" – mostra no log e face motivada
                Serial.println("[CMD] 3 -> Bateria");
                FaceHandler::setFace(FACE_MOTIVATED);
                int pct = power.getBatteryPercent();
                Serial.printf("[CMD] Bateria: %d%%\n", pct);
                AudioHandler::playWav("/success_pt.wav");
                break;
            }

            default:
                Serial.println("[CMD] Desconhecido / ruido");
                FaceHandler::setFace(FACE_SAD);
                AudioHandler::playWav("/error_pt.wav");
                break;
        }
    }
};

#endif
