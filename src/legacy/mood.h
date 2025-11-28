/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * mood.h: arquivo de cabeçalho para mood.cpp
 */

#ifndef MOOD_H
#define MOOD_H

#include <Arduino.h>

class Mood {
private:
  static String currentFace;
  static String happy;
  static String sad;
  static String broken;
  static String intense;
  static String looking1;
  static String looking2;
  static String neutral;
  static String sleeping;

public:
  static void init(String happy, String sad, String broken, String intense,
                   String looking1, String looking2, String neutral,
                   String sleeping);
  static Mood &getInstance() {
    static Mood instance;
    return instance;
  }
  String getFace(String face);
  String getFull(String face);
  String getMood(String face);
  String setFace(String previous, String current);
  String getHappy();
  String getSad();
  String getBroken();
  String getIntense();
  String getLooking1();
  String getLooking2();
  String getNeutral();
  String getSleeping();
};

#endif
