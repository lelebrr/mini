#include "web/WebHandler.h"
#include "core/ConfigManager.h"
#include "web/WebAssets.h"
#include <SD_MMC.h>
#include <DNSServer.h>

static AsyncWebServer* server = nullptr;
static DNSServer* dns = nullptr;
static bool running = false;

void WebHandler::init() {
    running = false;
    Serial.println("[WebHandler] Inicializado");
}

void WebHandler::loop() {
    if (running && dns) dns->processNextRequest();
}

void WebHandler::stop() {
    if (server) { server->end(); delete server; server = nullptr; }
    if (dns) { dns->stop(); delete dns; dns = nullptr; }
    running = false;
}

bool WebHandler::isRunning() { return running; }

void WebHandler::restart() {
    stop();
    init();
}

String WebHandler::getStatusJSON() {
    return "{\"status\":\"ok\",\"uptime\":0}";
}

String WebHandler::getConfigJSON() {
    ConfigManager *cfg = ConfigManager::getInstance();
    return cfg->getJSONRedacted().as<String>();
}

bool WebHandler::updateConfig(const char* json) {
    return ConfigManager::getInstance()->updateFromJSON(json);
}

void WebHandler::handleUpload() {
    // Upload de arquivos
}

void WebHandler::sendWebSocket(const char* msg) {
    // WebSocket broadcast
}
