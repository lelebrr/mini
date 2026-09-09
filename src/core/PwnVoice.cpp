#include "core/PwnVoice.h"
#include "core/ConfigManager.h"
#include "AudioHandler.h"

static bool voice_enabled = false;

void PwnVoice::init() {
    is_listening = false;
    voice_enabled = true;
    Serial.println("[PwnVoice] Modulo de voz inicializado");
}

void PwnVoice::listen() {
    if (!voice_enabled) return;
    is_listening = true;
    Serial.println("[PwnVoice] Ouvindo...");
}

void PwnVoice::processCommand() {
    // Processamento de comando de voz (placeholder para futuro)
    is_listening = false;
}

void PwnVoice::speak(const char* text) {
    Serial.printf("[PwnVoice] Falando: %s\n", text);
}

void PwnVoice::playWav(const char* path) {
    AudioHandler::playWav(path);
}

void PwnVoice::setEnabled(bool en) { voice_enabled = en; }
bool PwnVoice::isEnabled() { return voice_enabled; }
void PwnVoice::startListening() { listen(); }
void PwnVoice::stopListening() { is_listening = false; }
