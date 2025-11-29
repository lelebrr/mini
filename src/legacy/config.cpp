/*
 * ARQUIVO LEGADO - MANTIDO PARA REFERÊNCIA
 */

/**
 * config.cpp: configuração para minigotchi
 */

#include "config.h"

/** nota do desenvolvedor:
 *
 * este é o equivalente exato do 'config.toml' para o pwnagotchi
 * variáveis são definidas aqui e serão usadas pelo minigotchi
 * tudo que pode ser desabilitado/habilitado pode ser feito aqui
 *
 */

// define se os recursos serão usados
bool Config::deauth = true;
bool Config::advertise = true;
bool Config::scan = true;
bool Config::spam = true;

// define ssid e senha do ponto de acesso
const char *Config::ssid = "minigotchi";
const char *Config::pass = "dj1ch-minigotchi";

// define atrasos universais
int Config::shortDelay = 500;
int Config::longDelay = 5000;

// Define se está rodando em modo parasita onde conecta diretamente a um Pwnagotchi
bool Config::parasite = false;

// configuração de tela
bool Config::display = true;
std::string Config::screen = "AMOLED";

// define taxa de transmissão
int Config::baud = 115200;

// define canal inicial
int Config::channel = 1;

// define whitelist
std::vector<std::string> Config::whitelist = {"SSID", "SSID", "SSID"};

// define faces
String Config::happy = "(^-^)";
String Config::sad = "(;-;)";
String Config::broken = "(X-X)";
String Config::intense = "(>-<)";
String Config::looking1 = "(0-o)";
String Config::looking2 = "(o-0)";
String Config::neutral = "('-')";
String Config::sleeping = "(-.-)";

// configuração json
int Config::epoch = Minigotchi::currentEpoch;
std::string Config::face = "(^-^)";
std::string Config::identity =
    "b9210077f7c14c0651aa338c55e820e93f90110ef679648001b1cecdbffc0090";
std::string Config::name = "minigotchi";
int Config::ap_ttl = Config::random(30, 600);
bool Config::associate = true;
int Config::bored_num_epochs = Config::random(5, 30);

// define canais
int Config::channels[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

// veja https://github.com/evilsocket/pwnagotchi/blob/master/pwnagotchi/ai/gym.py
int Config::excited_num_epochs = Config::random(5, 30);
int Config::hop_recon_time = Config::random(5, 60);
int Config::max_inactive_scale = Config::random(3, 10);
int Config::max_interactions = Config::random(1, 25);
int Config::max_misses_for_recon = Config::random(3, 10);
int Config::min_recon_time = Config::random(1, 30);
int Config::min_rssi = Config::random(-200, -50);
int Config::recon_inactive_multiplier = Config::random(1, 3);
int Config::recon_time = Config::random(5, 60);
int Config::sad_num_epochs = Config::random(5, 30);
int Config::sta_ttl = Config::random(60, 300);
int Config::pwnd_run = 0;
int Config::pwnd_tot = 0;
std::string Config::session_id = "84:f3:eb:58:95:bd";
int Config::uptime = Config::time();

// configurações wifi
wifi_init_config_t Config::wifiCfg = WIFI_INIT_CONFIG_DEFAULT();
wifi_country_t Config::ctryCfg = {.cc = "US", .schan = 1, .nchan = 13};

// flag configurada que apenas o WebUI muda
bool Config::configured = false;

// define versão (por favor não mude, isso não deve ser alterado)
std::string Config::version = "3.5.2-beta";

/**
 * Carrega valores de configuração do NVS
 */
void Config::loadConfig() {
  nvs_handle_t cfgHandle;
  esp_err_t err = nvs_open("storage", NVS_READWRITE, &cfgHandle);
  if (err == ESP_OK) {
    // carrega Config::configured
    uint8_t configured = 0;
    err = nvs_get_u8(cfgHandle, "configured", &configured);
    if (err == ESP_OK) {
      Config::configured = (configured == 1);
    }

    // carrega Config::whitelist
    size_t required_size = 0;
    err = nvs_get_str(cfgHandle, "whitelist", NULL, &required_size);
    if (err == ESP_OK && required_size > 0) {
      char *whitelistStr = (char *)malloc(required_size);
      err = nvs_get_str(cfgHandle, "whitelist", whitelistStr, &required_size);
      if (err == ESP_OK) {
        // converte de volta para vetor
        Config::whitelist.clear();
        std::stringstream ss(whitelistStr);
        std::string item;
        while (std::getline(ss, item, ',')) {
          Config::whitelist.push_back(item);
        }
      }
      free(whitelistStr);
    }

    nvs_close(cfgHandle);
  }
}

/**
 * Salva configuração no NVS
 */
void Config::saveConfig() {
  nvs_handle_t cfgHandle;
  esp_err_t err = nvs_open("storage", NVS_READWRITE, &cfgHandle);
  if (err == ESP_OK) {
    // salva Config::configured
    uint8_t configured = Config::configured ? 1 : 0;
    err = nvs_set_u8(cfgHandle, "configured", configured);
    ESP_ERROR_CHECK(err);

    // salva Config::whitelist
    std::string whitelistStr;
    for (size_t i = 0; i < Config::whitelist.size(); ++i) {
      whitelistStr += Config::whitelist[i];
      if (i < Config::whitelist.size() - 1) {
        whitelistStr += ",";
      }
    }
    err = nvs_set_str(cfgHandle, "whitelist", whitelistStr.c_str());
    ESP_ERROR_CHECK(err);

    err = nvs_commit(cfgHandle);
    ESP_ERROR_CHECK(err);
    nvs_close(cfgHandle);
  }
}

/** nota do desenvolvedor:
 *
 * estes servem para fornecer valores válidos para os dados do quadro para serem quase
 * idênticos aos de um pwnagotchi. eles devem estar dentro de um certo intervalo para serem válidos.
 *
 */

/**
 * Gera um inteiro aleatório baseado em um intervalo de números
 * @param min Número mais baixo
 * @param max Número mais alto
 */
int Config::random(int min, int max) { return min + rand() % (max - min + 1); }

/**
 * Verifica tempo ligado atual (uptime)
 */
int Config::time() { return millis() / 1000; }
