/*
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * mood.h: arquivo de cabeçalho para mood.cpp
<<<<<<< HEAD
=======
 * Minigotchi: An even smaller Pwnagotchi
 * Copyright (C) 2024 dj1ch
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * mood.h: header files for mood.cpp
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
 */

#ifndef MOOD_H
#define MOOD_H

<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
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
<<<<<<< HEAD
=======
#include "config.h"
#include "display.h"
#include <Arduino.h>
#include <string>

class Mood {
public:
  static Mood &getInstance();
  static void init(String happy, String sad, String broken, String intense,
                   String looking1, String looking2, String neutral,
                   String sleeping);
  Mood(const Mood &) = delete;
  Mood &operator=(const Mood &) = delete;

  String getFull(String face);
  String getCurrentFace();
  String getCurrentMood();
  String getMood(String face);
  String getFace(String mood);
  bool checkMood(String previous, String current);

>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
  String getHappy();
  String getSad();
  String getBroken();
  String getIntense();
  String getLooking1();
  String getLooking2();
  String getNeutral();
  String getSleeping();
<<<<<<< HEAD
<<<<<<< HEAD
};

#endif
=======

private:
  Mood(String happy, String sad, String broken, String intense, String looking1,
       String looking2, String neutral, String sleeping);

  static Mood *instance;
  String happy;
  String sad;
  String broken;
  String intense;
  String looking1;
  String looking2;
  String neutral;
  String sleeping;

  String currentMood;
  String currentFace;
};

#endif // MOOD_H
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
};

#endif
>>>>>>> origin/waveshare-s3-amoled-full-review-406
