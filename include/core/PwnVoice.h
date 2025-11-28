#ifndef PWN_VOICE_H
#define PWN_VOICE_H

#include <Arduino.h>
#include "AudioHandler.h"
#include "OfflineVoice.h"
#include "core/PwnPet.h"

// Comandos de Voz Mapeados (Offline - Contagem de Sílabas)
// "Ei Pwn" (2 sílabas) -> Wake Word (Simulado)

class PwnVoice {
public:
    static void init() {
        // Nada específico
    }

    static void listen() {
        // Toca "Ouvindo"
        AudioHandler::playWav("/voice/listening.wav");

        // Grava 4s
        bool ok = AudioHandler::recordWav("/voice/input.wav", 4, true);

        if (ok) {
            int syllables = OfflineVoice::analyzeCommand("/voice/input.wav");
            processCommand(syllables);
        }
    }

    static void processCommand(int syllables) {
        Serial.printf("[Voice] Silabas: %d\n", syllables);
        switch(syllables) {
            case 2: // "Ei Pwn" / "Status"
                PwnPet::feed(5); // Atenção = Comida
                speak("Ola amigo");
                break;
            case 3: // "Bateria" / "Comida"
                speak("Estou com fome");
                break;
            case 4: // "Ataca Vivo"
                speak("Iniciando ataque");
                // PwnAttack::start();
                break;
            default:
                speak("Nao entendi");
                break;
        }
    }

    // TTS Simulado (Frases pré-gravadas)
    static void speak(String phrase) {
        // Mapeia texto para arquivo WAV
        String file = "/tts/unknown.wav";

        if (phrase.indexOf("Ola") >= 0) file = "/tts/hello.wav";
        else if (phrase.indexOf("fome") >= 0) file = "/tts/hungry.wav";
        else if (phrase.indexOf("ataque") >= 0) file = "/tts/attack.wav";
        else if (phrase.indexOf("Mano") >= 0) file = "/tts/slang_mano.wav";

        Serial.printf("[Voice] Falando: %s (%s)\n", phrase.c_str(), file.c_str());
        AudioHandler::playWav(file.c_str());
    }
};

#endif
