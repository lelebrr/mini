#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

/**
 * CommandHandler.h
 * Lógica de processamento de comandos de voz offline.
 *
 * Mapeia contagem de sílabas para ações no dispositivo.
 */

#include <Arduino.h>
#include <XPowersLib.h>
#include "AudioHandler.h"
#include "FaceHandler.h"

// Referências externas para hardware (definidas no main.cpp)
extern XPowersPMU power;
extern Arduino_GFX *gfx;

class CommandHandler {
public:
    /**
     * Processa o comando baseado no número de sílabas detectadas.
     * @param count Número de pulsos de energia (sílabas).
     */
    static void processSyllables(int count) {
        Serial.printf("[Comando] Processando %d silabas\n", count);

        switch (count) {
            case 1:
                // Exemplo: "Luz", "Sim", "Pai", "Som"
                // Ação: Alternar brilho da tela (Lanterna)
                Serial.println("[Comando] Acao: Alternar Luz (1)");
                gfx->Display_Brightness(255);
                FaceHandler::setFace(FACE_COOL);
                AudioHandler::playWav("/success_pt.wav");
                break;

            case 2:
                // Exemplo: "Status", "Ola", "Teste", "Liga"
                // Ação: Reportar status (som de boot) e ficar feliz
                Serial.println("[Comando] Acao: Status (2)");
                FaceHandler::setFace(FACE_HAPPY);
                AudioHandler::playWav("/boot_pt.wav");
                break;

            case 3:
                // Exemplo: "Bateria", "Energia"
                // Ação: Mostrar bateria e ficar motivado
                Serial.println("[Comando] Acao: Bateria (3)");
                FaceHandler::setFace(FACE_MOTIVATED);
                {
                    int pct = power.getBatteryPercent();
                    Serial.printf("[Comando] Bateria: %d%%\n", pct);
                }
                AudioHandler::playWav("/success_pt.wav");
                break;

            default:
                // Ruído ou comando não reconhecido
                Serial.println("[Comando] Erro: Desconhecido");
                FaceHandler::setFace(FACE_SAD);
                AudioHandler::playWav("/error_pt.wav");
                break;
        }
    }
};

#endif
