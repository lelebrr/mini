/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * display.cpp: Inicializa display e lida com saída de texto
 */

#include "display.h"

// Define o display eSprite
// Arduino_GFX *gfx_display = new Arduino_SH8601(
//     bus, -1, 0, false, 320, 480); // Ajuste pinos e resolução conforme necessário

/**
 * Inicializa o display
 */
void Display::init() {
  if (Config::display) {
    if (Config::screen == "AMOLED") {
      // Inicialização do display AMOLED seria aqui
      // gfx_display->begin();
      // gfx_display->fillScreen(BLACK);
    }
  }
}

/**
 * Atualiza o display com texto e uma face
 * @param face Face para usar
 * @param text Texto para usar
 */
void Display::updateDisplay(String face, String text) {
  if (Config::display) {
    // Implementação original simplificada
    // gfx_display->fillScreen(BLACK);
    // gfx_display->setCursor(10, 10);
    // gfx_display->println(face);
    // gfx_display->println(text);
  }
}

/**
 * Atualiza o display apenas com uma face
 * @param face Face para usar
 */
void Display::updateDisplay(String face) {
  if (Config::display) {
    // gfx_display->fillScreen(BLACK);
    // gfx_display->setCursor(10, 10);
    // gfx_display->println(face);
  }
}
