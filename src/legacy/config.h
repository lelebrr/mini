/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * config.h: arquivo de cabeçalho para config.cpp
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "minigotchi.h"

class Config {
private:
  static int time();

public:
  static bool deauth;
  static bool advertise;
  static bool scan;
  static bool spam;
  static const char *ssid;
  static const char *pass;
  static int shortDelay;
  static int longDelay;
  static bool parasite;
  static bool display;
  static std::string screen;
  static int baud;
  static int channel;
  static std::vector<std::string> whitelist;
  static String happy;
  static String sad;
  static String broken;
  static String intense;
  static String looking1;
  static String looking2;
  static String neutral;
  static String sleeping;
  static int epoch;
  static std::string face;
  static std::string identity;
  static std::string name;
  static int ap_ttl;
  static bool associate;
  static int bored_num_epochs;
  static int channels[13];
  static int excited_num_epochs;
  static int hop_recon_time;
  static int max_inactive_scale;
  static int max_interactions;
  static int max_misses_for_recon;
  static int min_recon_time;
  static int min_rssi;
  static int recon_inactive_multiplier;
  static int recon_time;
  static int sad_num_epochs;
  static int sta_ttl;
  static int pwnd_run;
  static int pwnd_tot;
  static std::string session_id;
  static int uptime;
  static wifi_init_config_t wifiCfg;
  static wifi_country_t ctryCfg;
  static int random(int min, int max);
  static bool configured;
  static std::string version;
  static void loadConfig();
  static void saveConfig();
};

#endif
