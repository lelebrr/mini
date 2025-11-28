#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
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
