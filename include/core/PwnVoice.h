#ifndef PWN_VOICE_H
#define PWN_VOICE_H

#include <Arduino.h>
#include "AudioHandler.h"
#include "OfflineVoice.h"
#include "core/PwnPet.h"
#include "core/PwnPower.h"
#include "core/PwnAttack.h"

/**
 * PwnVoice
 *
 * Camada simples de “voz offline” baseada em:
 *  - Gravação de alguns segundos de áudio (WAV)
 *  - Análise de pulsos de energia (OfflineVoice) => contagem de sílabas
 *  - Mapeamento de número de sílabas para comandos de alto nível.
 */

class PwnVoice {
private:
    static bool is_listening;

    static constexpr const char *LISTEN_WAV = "/arquivos_cartao_sd/voice/listening.wav";
    static constexpr const char *INPUT_WAV  = "/arquivos_cartao_sd/voice/input.wav";

public:
    static void init() {
        is_listening = false;
    }

    static void listen() {
        if (PwnPower::isCritical()) {
            Serial.println("[Voice] Ignorando comando de voz (bateria crítica).");
            return;
        }

        if (is_listening) return;
        is_listening = true;

        // Sinal sonoro de “ouvindo”, se existir
        AudioHandler::playWav(LISTEN_WAV);

        // Grava ~4s de áudio com VAD simples
        bool ok = AudioHandler::recordWav(INPUT_WAV, 4, true);
        is_listening = false;

        if (!ok) {
            Serial.println("[Voice] Nenhuma voz detectada.");
            return;
        }

        int syllables = OfflineVoice::analyzeCommand(INPUT_WAV);
        processCommand(syllables);
    }

    static void processCommand(int syllables) {
        Serial.printf("[Voice] Sílabas detectadas: %d\n", syllables);

        // Se o Pet estiver dormindo, só reage se “gritarem” (muitas sílabas)
        if (PwnPet::getStats().is_sleeping) {
            if (syllables > 4) {
                PwnPet::getStats().is_sleeping = false;
                speak("Quem ousa me acordar");
            }
            return;
        }

        switch (syllables) {
            case 2: // “Ei Pwn” / “Status”
                PwnPet::feed(5);
                speak("Ola amigo");
                break;
            case 3: // “Bateria” / “Comida”
                speak("Estou com fome");
                break;
            case 4: // “Ataca vivo” / “Ataque”
                speak("Iniciando ataque");
                PwnAttack::start();
                break;
            default:
                speak("Nao entendi");
                break;
        }
    }

    // TTS simulado – mapeia frases para arquivos WAV pré-gravados.
    static void speak(const String &phrase) {
        String file = "/tts/unknown.wav";

        if (phrase.indexOf("Ola")      >= 0) file = "/tts/hello.wav";
        else if (phrase.indexOf("fome")     >= 0) file = "/tts/hungry.wav";
        else if (phrase.indexOf("ataque")   >= 0) file = "/tts/attack.wav";
        else if (phrase.indexOf("acordar")  >= 0) file = "/tts/wakeup.wav";

        Serial.printf("[Voice] Falando: %s (%s)\n", phrase.c_str(), file.c_str());
        AudioHandler::playWav(file.c_str());
    }
};

// Definição estática declarada em core_singletons.cpp
#endif
