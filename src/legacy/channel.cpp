/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * channel.cpp: lida com saltos de canal
 */

#include "channel.h"

// Define o canal inicial
int Channel::channel = 1;

/**
 * Salta canais
 */
void Channel::hop() {
  if (Config::scan) {
    if (Channel::channel < 14) {
      Channel::channel++;
    } else {
      Channel::channel = 1;
    }
    esp_wifi_set_channel(Channel::channel, WIFI_SECOND_CHAN_NONE);
  }
}
