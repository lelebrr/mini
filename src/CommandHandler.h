#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>
#include <XPowersLib.h>
#include "AudioHandler.h"
<<<<<<< HEAD
#include "FaceHandler.h"
=======
>>>>>>> origin/waveshare-s3-amoled-offline-voice

extern XPowersPMU power; // From main.cpp
extern Arduino_GFX *gfx; // From main.cpp

class CommandHandler {
public:
    static void processSyllables(int count) {
        Serial.printf("Processando Comando por Silabas: %d\n", count);

        switch (count) {
            case 1:
<<<<<<< HEAD
                // "Luz" -> COOL face
                Serial.println("CMD: [1] Alternar Luz");
                gfx->Display_Brightness(255);
                FaceHandler::setFace(FACE_COOL);
=======
                // "Luz" / "Som" / "Sim"
                // Toggle Brightness
                Serial.println("CMD: [1] Alternar Luz");
                // Check current brightness via a static var or just toggle
                // Since we don't have read-back easily on GFX without tracking:
                gfx->Display_Brightness(255); // Just force High for demo
>>>>>>> origin/waveshare-s3-amoled-offline-voice
                AudioHandler::playWav("/success_pt.wav");
                break;

            case 2:
<<<<<<< HEAD
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
=======
                // "Status" / "Liga"
                Serial.println("CMD: [2] Status");
                AudioHandler::playWav("/boot_pt.wav"); // Play boot status
                break;

            case 3:
                // "Bateria" / "Energia"
                Serial.println("CMD: [3] Bateria");
                {
                    int pct = power.getBatteryPercent();
                    Serial.printf("Bateria: %d%%\n", pct);
                    // In a real scenario, we'd play number WAVs here
>>>>>>> origin/waveshare-s3-amoled-offline-voice
                }
                AudioHandler::playWav("/success_pt.wav");
                break;

            default:
<<<<<<< HEAD
                // Error -> CONFUSED/SAD
                Serial.println("CMD: Desconhecido");
                FaceHandler::setFace(FACE_SAD);
=======
                Serial.println("CMD: Desconhecido / Ruido");
>>>>>>> origin/waveshare-s3-amoled-offline-voice
                AudioHandler::playWav("/error_pt.wav");
                break;
        }
    }
};

#endif
