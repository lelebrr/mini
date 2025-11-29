/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * parasite.cpp: Lida com a funcionalidade do modo parasita
 */

#include "parasite.h"

int Parasite::channel = 0;

/**
 * Envia o status de desautenticação para o Pwnagotchi
 * @param status Status de deauth
 * @param ap AP selecionado (opcional)
 * @param channel Canal (opcional)
 */
void Parasite::sendDeauthStatus(DeauthStatus status, const char *ap,
                                int channel) {
  if (Config::parasite) {
    DynamicJsonDocument doc(2048);
    doc["type"] = "deauth";
    doc["status"] = status;
    if (ap != nullptr) {
      doc["ap"] = ap;
    }
    if (channel > 0) {
      doc["channel"] = channel;
    }
    String output;
    serializeJson(doc, output);
    Serial.println(output);
  }
}

/**
 * Lê dados do Pwnagotchi
 */
void Parasite::readData() {
  if (Config::parasite && Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, input);

    if (doc.containsKey("channel")) {
      Parasite::channel = doc["channel"];
    }
  }
}

/**
 * Envia nome do parasita para o Pwnagotchi
 */
void Parasite::sendName() {
  if (Config::parasite) {
    DynamicJsonDocument doc(2048);
    doc["type"] = "name";
    doc["name"] = Config::name;
    String output;
    serializeJson(doc, output);
    Serial.println(output);
  }
}
