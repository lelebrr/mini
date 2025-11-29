/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * mood.cpp: lida com os "humores" do minigotchi
 */

#include "mood.h"

String Mood::currentFace;
String Mood::happy;
String Mood::sad;
String Mood::broken;
String Mood::intense;
String Mood::looking1;
String Mood::looking2;
String Mood::neutral;
String Mood::sleeping;

/**
 * Define faces de acordo com a configuração pelo menos
 * @param happy Face feliz! (não eu depois de fazer este construtor)
 * @param sad Face triste (eu depois de fazer este construtor)
 * @param broken Face quebrada (também eu depois de fazer este construtor)
 * @param intense Face intensa
 * @param sleeping Face dormindo (eu queria estar dormindo)
 * @param looking1 Face olhando (para esquerda, eu acho?)
 * @param looking2 Face olhando (para direita, eu acho?)
 * @param neutral Face neutra
 */
void Mood::init(String happy, String sad, String broken, String intense,
                String looking1, String looking2, String neutral,
                String sleeping) {
  Mood::happy = happy;
  Mood::sad = sad;
  Mood::broken = broken;
  Mood::intense = intense;
  Mood::looking1 = looking1;
  Mood::looking2 = looking2;
  Mood::neutral = neutral;
  Mood::sleeping = sleeping;
}

/**
 * Retorna a face atual
 */
String Mood::getFace(String face) {
  currentFace = face;
  return currentFace;
}

/**
 * esses procedimentos de checagem serão realmente usados apenas para faces,
 * e não para a lógica real do minigotchi
 */

/**
 * Obtém humor atual baseado na face
 * @param face Face para obter humor
 */
String Mood::getMood(String face) {
  if (face == happy) {
    return "happy";
  } else if (face == sad) {
    return "sad";
  } else if (face == broken) {
    return "broken";
  } else if (face == intense) {
    return "intense";
  } else if (face == looking1) {
    return "looking1";
  } else if (face == looking2) {
    return "looking2";
  } else if (face == neutral) {
    return "neutral";
  } else if (face == sleeping) {
    return "sleeping";
  } else {
    return "neutral"; // fallback
  }
}

/**
 * Obtém face completa (função estranha, apenas repassa?)
 * @param face Face para obter humor
 */
String Mood::getFull(String face) { Mood::getFace(Mood::getMood(face)); return currentFace; }

/**
 * Obtém face baseada no humor
 * @param previous Face anterior
 * @param current Face atual
 */
String Mood::setFace(String previous, String current) {
  if (previous != current) {
    currentFace = current;
    return currentFace;
  }
  return currentFace;
}

/**
 * Getters para cada face
 */
String Mood::getHappy() { return Mood::happy; }
String Mood::getSad() { return Mood::sad; }
String Mood::getBroken() { return Mood::broken; }
String Mood::getIntense() { return Mood::intense; }
String Mood::getLooking1() { return Mood::looking1; }
String Mood::getLooking2() { return Mood::looking2; }
String Mood::getNeutral() { return Mood::neutral; }
String Mood::getSleeping() { return Mood::sleeping; }
