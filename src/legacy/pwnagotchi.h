/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * pwnagotchi.h: arquivo de cabeçalho para pwnagotchi.cpp
 */

#ifndef PWNAGOTCHI_H
#define PWNAGOTCHI_H

#include <Arduino.h>
#include <esp_wifi.h>

#include <vector>

#include "config.h"
#include "display.h"
#include "mood.h"

class Pwnagotchi {
private:
  static Mood &mood;
  static bool seen;
  static std::vector<String> pwnagotchiList;

public:
  static void detect();
};

#endif
