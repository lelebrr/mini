/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * channel.h: arquivo de cabeçalho para channel.cpp
 */

#ifndef CHANNEL_H
#define CHANNEL_H

#include <Arduino.h>
#include <esp_wifi.h>

#include "config.h"

class Channel {
public:
  static int channel;
  static void hop();
};

#endif
