/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * minigotchi.h: arquivo de cabeçalho para minigotchi.cpp
 */

#ifndef MINIGOTCHI_H
#define MINIGOTCHI_H

#include <Arduino.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

#include "ble.h"
#include "channel.h"
#include "config.h"
#include "deauth.h"
#include "display.h"
#include "frame.h"
#include "mood.h"
#include "parasite.h"
#include "pwnagotchi.h"
#include "webui.h"

class Minigotchi {
private:
  static Mood &mood;
  static WebUI *web;
  static void WebUITask(void *pvParameters);
  static void waitForInput();
  static void info();
  static int addEpoch();
  static void finish();
  static void version();
  static void mem();
  static void cpu();

public:
  static int currentEpoch;
  static void boot();
  static void monStart();
  static void monStop();
  static void epoch();
  static void cycle();
  static void detect();
  static void deauth();
  static void advertise();
  static void spam();
};

#endif
