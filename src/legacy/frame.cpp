/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * frame.cpp: analisa o frame do parasita
 */

#include "frame.h"

// inicializando
size_t Frame::payloadSize = 255; // por padrão
uint8_t Frame::payload[255];     // por padrão

// coisas de beacon
const uint8_t Frame::beacon[3] = {0x01, 0x00, 0x5e};
const uint8_t Frame::id[3] = {0xde, 0xad, 0xbe};

// IDs de payload de acordo com pwngrid
// https://pwnagotchi.ai/api/grid/#payload-format
const uint8_t Frame::PWNAGOTCHI_IDENTITY = 0x01;
const uint8_t Frame::PWNAGOTCHI_FACE = 0x02;
const uint8_t Frame::PWNAGOTCHI_STATUS = 0x03;
const uint8_t Frame::PWNAGOTCHI_AUTHENTICATION = 0x04;
const uint8_t Frame::PWNAGOTCHI_DEAUTH = 0x05;
const uint8_t Frame::PWNAGOTCHI_HANDSHAKE = 0x06;
const uint8_t Frame::PWNAGOTCHI_ASSOCIATION = 0x07;

/**
 * Cria o frame
 * @param packet Pacote
 */
void Frame::create(const uint8_t *packet) {
  // limpa tudo (ou então o pwnagotchi trava)
  memset(Frame::payload, 0, Frame::payloadSize);

  // cabeçalho para informar ao pwnagotchi que isso é um parasita
  // https://pwnagotchi.ai/api/grid/#advertisement-data
  memcpy(Frame::payload, Frame::beacon, sizeof(Frame::beacon));
  memcpy(Frame::payload + 3, Frame::id, sizeof(Frame::id));
  Frame::payload[6] = 0xef;

  // envia dados
  if (packet != nullptr) {
    memcpy(Frame::payload + 7, packet,
           Frame::payloadSize - 7); // copia dados de pacote para payload
  }
}

/**
 * Analisa o frame
 */
void Frame::parse() {
  DynamicJsonDocument doc(2048);

  // identidade (0x01)
  doc["identity"] = Config::identity;

  // face (0x02)
  doc["face"] = Config::face;

  // status (0x03)
  // não implementado

  // autenticação (0x04)
  // não implementado

  // deauth (0x05)
  // não implementado

  // handshake (0x06)
  // não implementado

  // associação (0x07)
  // não implementado

  // serializa e copia para payload
  // o offset é 7 porque os primeiros 7 bytes são cabeçalho
  serializeJson(doc, Frame::payload + 7, Frame::payloadSize - 7);
}
