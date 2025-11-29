<<<<<<< HEAD
/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * display.h: arquivo de cabeçalho para display.cpp
 */

=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
<<<<<<< HEAD
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
=======
#include <Arduino_GFX_Library.h>

namespace Display {
    void startScreen();
    void updateDisplay(String face);
    void updateDisplay(String face, String text);
}

// Declarações externas
extern Arduino_DataBus *bus;
extern Arduino_GFX *gfx_display;

#endif // DISPLAY_H
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
