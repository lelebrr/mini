/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * deauth.h: arquivo de cabeçalho para deauth.cpp
 */

#ifndef DEAUTH_H
#define DEAUTH_H

#include <Arduino.h>
#include <esp_wifi.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

#include "config.h"
#include "display.h"
#include "minigotchi.h"
#include "mood.h"
#include "parasite.h"

class Deauth {
private:
  static uint8_t deauthTemp[26];
  static uint8_t deauthFrame[26];
  static uint8_t disassociateFrame[26];
  static uint8_t broadcastAddr[6];
  static Mood &mood;
  static bool running;
  static std::vector<String> whitelist;
  static void add(const std::string &bssids);
  static bool send(uint8_t *buf, uint16_t len, bool sys_seq);
  static bool broadcast(uint8_t *mac);
  static void printMac(uint8_t *mac);
  static String printMacStr(uint8_t *mac);
  static bool select();
  static void start();

public:
  static String randomAP;
  static int randomIndex;
  static void list();
  static void deauth();
};

#endif
