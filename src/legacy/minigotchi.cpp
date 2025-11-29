/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * minigotchi.cpp: lida com informações de uso do sistema, etc
 */

#include "minigotchi.h"

/** nota do desenvolvedor:
 *
 * as funções aqui fornecem informações sobre o minigotchi, como memória,
 * temperatura, etc. tudo o que isso realmente faz é imprimir informações no terminal
 * serial destinado a ser executado na inicialização.
 *
 */

// este código é bem bagunçado mas deixa o minigotchi.ino menos
// poluído!!!

// inicializando valores
Mood &Minigotchi::mood = Mood::getInstance();
WebUI *Minigotchi::web = nullptr;

// valor atual de época (epoch)
int Minigotchi::currentEpoch = 0;

/**
 * Tarefa WebUI para freeRTOS
 */
void Minigotchi::WebUITask(void *pvParameters) {
  // configura servidor web
  WebUI *web = new WebUI();

  // espera até que algo legal aconteça (isso não é legal, confie em mim)
  while (!Config::configured) {
    taskYIELD(); // espera
  }

  // limpa quando terminar
  delete web;
  web = nullptr;

  // deleta tarefa
  vTaskDelete(NULL);
}

/**
 * Espera pelo WebUI para obter entrada de que a configuração está pronta
 */
void Minigotchi::waitForInput() {
  // no núcleo um
  if (!Config::configured) {
    xTaskCreatePinnedToCore(WebUITask, "WebUI Task", 8192, NULL, 1, NULL, 1);
  }

  // espera até estar pronto
  while (!Config::configured) {
    delay(1);
  }
}

/**
 * Incrementa época atual em um
 */
int Minigotchi::addEpoch() {
  Minigotchi::currentEpoch++;
  return Minigotchi::currentEpoch;
}

/**
 * Mostra época atual do Minigotchi
 */
void Minigotchi::epoch() {
  Minigotchi::addEpoch();
  Parasite::readData();
  Serial.print(mood.getNeutral() + " Current Epoch: ");
  Serial.println(Minigotchi::currentEpoch);
  Serial.println(" ");
  Display::updateDisplay(mood.getNeutral(),
                         "Current Epoch: " + Minigotchi::currentEpoch);
}

/**
 * Coisas para fazer na inicialização
 */
void Minigotchi::boot() {
  // configura humores
  Mood::init(Config::happy, Config::sad, Config::broken, Config::intense,
             Config::looking1, Config::looking2, Config::neutral,
             Config::sleeping);

  Serial.println("DEBUG: Iniciando Display::startScreen()...");
  Display::startScreen();
  Serial.println("DEBUG: Display::startScreen() completado!");
  Serial.println(" ");
  Serial.println(mood.getHappy() +
                 " Hi, I'm Minigotchi, your pwnagotchi's best friend!");
  Display::updateDisplay(mood.getHappy(), "Hi, I'm Minigotchi");
  Serial.println(" ");
  Serial.println(mood.getNeutral() +
                 " You can edit my configuration parameters in config.cpp!");
  Serial.println(" ");
  delay(Config::shortDelay);
  Display::updateDisplay(mood.getNeutral(), "Edit my config.cpp!");
  delay(Config::shortDelay);
  Serial.println(mood.getIntense() + " Starting now...");
  Serial.println(" ");
  Display::updateDisplay(mood.getIntense(), "Starting  now");
  delay(Config::shortDelay);
  Serial.println("################################################");
  Serial.println("#                BOOTUP PROCESS                #");
  Serial.println("################################################");
  Serial.println(" ");
  ESP_ERROR_CHECK(esp_wifi_init(&Config::wifiCfg));
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }

  Config::loadConfig();
  ESP_ERROR_CHECK(err);
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_country(&Config::ctryCfg));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());

  // espera pela configuração webui - TEMPORARIAMENTE DESABILITADO
  // if (!Config::configured) {
  //   waitForInput();
  // }

  Deauth::list();
  Channel::init(Config::channel);
  Minigotchi::info();
  Parasite::sendName();
  Minigotchi::finish();
}

/**
 * Mostra info/stats atuais do Minigotchi
 */
void Minigotchi::info() {
  delay(Config::shortDelay);
  Serial.println(" ");
  Serial.println(mood.getNeutral() + " Current Minigotchi Stats: ");
  Display::updateDisplay(mood.getNeutral(), "Current Minigotchi Stats:");
  version();
  mem();
  cpu();
  Serial.println(" ");
  delay(Config::shortDelay);
}

/**
 * Isso é impresso depois que tudo é feito no processo de inicialização
 */
void Minigotchi::finish() {
  Serial.println("################################################");
  Serial.println(" ");
  Serial.println(mood.getNeutral() + " Started successfully!");
  Serial.println(" ");
  Display::updateDisplay(mood.getNeutral(), "Started sucessfully");
  delay(Config::shortDelay);
}

/**
 * Mostra versão atual do Minigotchi
 */
void Minigotchi::version() {
  Serial.print(mood.getNeutral() + " Version: ");
  Serial.println(Config::version.c_str());
  Display::updateDisplay(mood.getNeutral(),
                         "Version: " + (String)Config::version.c_str());
  delay(Config::shortDelay);
}

/**
 * Mostra uso de memória atual do Minigotchi
 */
void Minigotchi::mem() {
  Serial.print(mood.getNeutral() + " Heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  Display::updateDisplay(mood.getNeutral(),
                         "Heap: " + (String)ESP.getFreeHeap() + " bytes");
  delay(Config::shortDelay);
}

/**
 * Mostra Frequência de CPU atual do Minigotchi
 */
void Minigotchi::cpu() {
  Serial.print(mood.getNeutral() + " CPU Frequency: ");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(" MHz");
  Display::updateDisplay(mood.getNeutral(),
                         "CPU Frequency: " + (String)ESP.getCpuFreqMHz() +
                             " MHz");
  delay(Config::shortDelay);
}

/** nota do desenvolvedor:
 *
 * essas funções são muito parecidas com monstart e monstop do pwnagotchi que
 * iniciam e param o modo monitor. legal!
 *
 * obviamente eu gosto de manter as coisas consistentes com o pwnagotchi, essa é uma
 * dessas coisas.
 *
 */

/** nota do desenvolvedor:
 *
 * para evitar problemas colocamos o minigotchi de volta no modo cliente que é o
 * "padrão"
 *
 */

/**
 * Coloca Minigotchi em modo promiscuo
 */
void Minigotchi::monStart() {
  // desconecta do WiFi se estivermos conectados
  WiFi.disconnect();

  // reverte para modo estação
  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true);
}

/**
 * Tira Minigotchi do modo promiscuo
 */
void Minigotchi::monStop() {
  esp_wifi_set_promiscuous(false);

  // reverte para modo estação
  WiFi.mode(WIFI_STA);
}

/** nota do desenvolvedor:
 *
 * quando o minigotchi não está ciclando, detectando um pwnagotchi, ou desautenticando,
 * ele está anunciando sua própria presença, daí a razão de haver um constante
 * Frame::stop(); e Frame::start(); em cada função
 *
 * quando se trata de qualquer um desses recursos, você não pode simplesmente chamar algo e
 * esperar que rode normalmente ex: chamar Deauth::deauth(); porque você vai
 * receber o erro:
 *
 * (X-X) No access point selected. Use select() first.
 * ('-') Told you so!
 *
 * a placa ainda está ocupada em modo monitor em um certo canal(anunciando), e
 * os AP's que estamos procurando podem estar em outros canais, por isso precisamos chamar
 * Frame::stop(); para parar isso e então podemos fazer o que queremos...
 *
 */

/**
 * Ciclagem de canal
 */
void Minigotchi::cycle() {
  Parasite::readData();
  Channel::cycle();
}

/**
 * Detecção de Pwnagotchi
 */
void Minigotchi::detect() {
  Parasite::readData();
  Pwnagotchi::detect();
}

/**
 * Desautenticação
 */
void Minigotchi::deauth() {
  Parasite::readData();
  Deauth::deauth();
}

/**
 * Publicidade (Advertising)
 */
void Minigotchi::advertise() {
  Parasite::readData();
  Frame::advertise();
}

/**
 * Spam BLE
 */
void Minigotchi::spam() {
  Parasite::readData();
  Ble::spam();
}
