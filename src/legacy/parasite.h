/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * parasite.h: arquivo de cabeçalho para parasite.cpp
 */

#ifndef PARASITE_H
#define PARASITE_H

#include <ArduinoJson.h>

#include "config.h"

// Enumeração para Status de Deauth
enum DeauthStatus {
  START_SCAN,
  NO_APS,
  SKIPPING_WHITELIST,
  SKIPPING_UNENCRYPTED,
  PICKED_AP,
  DEAUTH_SCAN_ERROR,
  START_DEAUTH
};

class Parasite {
public:
  static int channel;
  static void sendDeauthStatus(DeauthStatus status, const char *ap = nullptr,
                               int channel = 0);
  static void readData();
  static void sendName();
};

#endif
