/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * display.h: arquivo de cabeçalho para display.cpp
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
// #include "Arduino_GFX_Library.h" // Comentado para evitar conflito com main.cpp

#include "config.h"

// extern Arduino_GFX *gfx_display;

class Display {
public:
  static void init();
  static void updateDisplay(String face);
  static void updateDisplay(String face, String text);
};

#endif
