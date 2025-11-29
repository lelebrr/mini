/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * pwnagotchi.cpp: Implementa lógica de detecção de Pwnagotchi
 */

#include "pwnagotchi.h"

// Variáveis de controle
bool Pwnagotchi::seen = false;
std::vector<String> Pwnagotchi::pwnagotchiList = {};

/**
 * Obtém a primeira instância da classe mood
 */
Mood &Pwnagotchi::mood = Mood::getInstance();

/** nota do desenvolvedor:
 *
 * o frame do pwnagotchi é definido aqui.
 * este é um frame bruto (camada 2)
 *
 */

// inicializando valores
// ...

/**
 * Detecta outros pwnagotchis nas proximidades
 */
void Pwnagotchi::detect() {
  if (Config::scan) {
    if (seen) {
      Serial.println(mood.getHappy() + " Pwnagotchi Found!");
      Display::updateDisplay(mood.getHappy(), "Pwnagotchi Found!");
      delay(Config::shortDelay);
    } else {
      Serial.println(mood.getSad() + " No Pwnagotchis Found...");
      Display::updateDisplay(mood.getSad(), "No Pwnagotchis Found...");
      delay(Config::shortDelay);
    }
  } else {
    // não faz nada se o scan estiver desabilitado
  }
}
