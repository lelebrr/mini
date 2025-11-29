/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * frame.h: arquivo de cabeçalho para frame.cpp
 */

#ifndef FRAME_H
#define FRAME_H

#include <ArduinoJson.h>

#include "config.h"

class Frame {
private:
  static const uint8_t beacon[3];
  static const uint8_t id[3];
  static const uint8_t PWNAGOTCHI_IDENTITY;
  static const uint8_t PWNAGOTCHI_FACE;
  static const uint8_t PWNAGOTCHI_STATUS;
  static const uint8_t PWNAGOTCHI_AUTHENTICATION;
  static const uint8_t PWNAGOTCHI_DEAUTH;
  static const uint8_t PWNAGOTCHI_HANDSHAKE;
  static const uint8_t PWNAGOTCHI_ASSOCIATION;

public:
  static size_t payloadSize;
  static uint8_t payload[255];
  static void create(const uint8_t *packet);
  static void parse();
};

#endif
