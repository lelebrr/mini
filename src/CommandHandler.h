#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>
#include <XPowersLib.h>
#include "AudioHandler.h"
#include "FaceHandler.h"

extern XPowersPMU power; // From main.cpp
extern Arduino_GFX *gfx; // From main.cpp

class CommandHandler {
public:
    static void processSyllables(int count) {
        Serial.printf("Processando Comando por Silabas: %d\n", count);

        switch (count) {
            case 1:
                // "Luz" -> COOL face
                Serial.println("CMD: [1] Alternar Luz");
                gfx->Display_Brightness(255);
                FaceHandler::setFace(FACE_COOL);
                AudioHandler::playWav("/success_pt.wav");
                break;

            case 2:
                // "Status" -> HAPPY
                Serial.println("CMD: [2] Status");
                FaceHandler::setFace(FACE_HAPPY);
                AudioHandler::playWav("/boot_pt.wav");
                break;

            case 3:
                // "Bateria" -> MOTIVATED
                Serial.println("CMD: [3] Bateria");
                FaceHandler::setFace(FACE_MOTIVATED);
                {
                    int pct = power.getBatteryPercent();
                    Serial.printf("Bateria: %d%%\n", pct);
                }
                AudioHandler::playWav("/success_pt.wav");
                break;

            default:
                // Error -> CONFUSED/SAD
                Serial.println("CMD: Desconhecido");
                FaceHandler::setFace(FACE_SAD);
                AudioHandler::playWav("/error_pt.wav");
                break;
        }
    }
};

#endif
