#include "CommandHandler.h"
#include "core/ConfigManager.h"

static void (*cmd_callback)(const char*) = nullptr;

void CommandHandler::init() {
    cmd_callback = nullptr;
    Serial.println("[CommandHandler] Inicializado");
}

void CommandHandler::handle(const char* cmd) {
    Serial.printf("[CommandHandler] Comando: %s\n", cmd);
    if (cmd_callback) cmd_callback(cmd);
}

void CommandHandler::handleJSON(const char* json) {
    Serial.printf("[CommandHandler] JSON: %s\n", json);
    ConfigManager::getInstance()->updateFromJSON(json);
}

void CommandHandler::setCallback(void (*cb)(const char*)) {
    cmd_callback = cb;
}
