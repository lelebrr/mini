/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * platformio_init.cpp: Inicialização específica para o ambiente PlatformIO
 */

#include "platformio.h"
#include <Arduino.h>
// #include "XPowersLib.h" // Comentado para evitar conflito com main.cpp

void platformio_init() {
    Serial.println("[PlatformIO] Inicializando configurações específicas...");

    // Inicialização do PMU movida para main.cpp para controle centralizado
    // XPowersPMU power;
    // ...
}
