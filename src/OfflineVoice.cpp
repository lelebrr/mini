#include "OfflineVoice.h"
#include <SD_MMC.h>

static String last_command = "";
static int syllable_count = 0;
static bool has_command = false;
static int threshold = 50;

void OfflineVoice::init() {
    last_command = "";
    syllable_count = 0;
    has_command = false;
    Serial.println("[OfflineVoice] Inicializado");
}

void OfflineVoice::startListening() {
    Serial.println("[OfflineVoice] Ouvindo...");
}

void OfflineVoice::stopListening() {
    Serial.println("[OfflineVoice] Parou de ouvir");
}

String OfflineVoice::getLastCommand() { return last_command; }
int OfflineVoice::getSyllableCount() { return syllable_count; }
bool OfflineVoice::hasCommand() { return has_command; }
void OfflineVoice::setThreshold(int level) { threshold = level; }
