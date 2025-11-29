/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * ble.h: Arquivo de cabeçalho para ble.cpp
 */

#ifndef BLE_H
#define BLE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "config.h"
#include "display.h"
#include "mood.h"

class Ble {
private:
  static BLEAdvertising *pAdvertising;
  static int random(int min, int max);
  static Mood &mood;

public:
  static int deviceType;
  static int delaySeconds;
  static int advType;
  static void init();
  static void start();
  static void stop();
  static void spam();
};

#endif
