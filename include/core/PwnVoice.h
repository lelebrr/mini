#ifndef PWN_VOICE_H
#define PWN_VOICE_H

#include <Arduino.h>
#include "AudioHandler.h"
#include "OfflineVoice.h"
#include "core/PwnPet.h"
#include "core/PwnPower.h"
#include "core/PwnAttack.h"

// Comandos de Voz Mapeados (Offline - Contagem de Sílabas)

class PwnVoice {
private:
    static bool is_listening;

public:
    static void init() {
        is_listening = false;
    }

    static void listen() {
	        if (PwnPower::isCritical()) return;
        is_listening = true;

        // Toca "Ouvindo"
        // AudioHandler::playWav("/voice/listening.wav");

        // Grava 4s
        bool ok = AudioHandler::recordWav("/voice/input.wav", 4, true);

        is_listening = false;

        if (ok) {
            int syllables = OfflineVoice::analyzeCommand("/voice/input.wav");
            processCommand(syllables);
        }
    }

    static void processCommand(int syllables) {
        Serial.printf("[Voice] Silabas: %d\n", syllables);

	        if (PwnPet::getStats().is_sleeping) {
             if (syllables > 4) {
                 PwnPet::getStats().is_sleeping = false;
                 speak("Quem ousa me acordar");
             }
             return;
        }

        switch(syllables) {
	            case 2: // "Ei Lele" / "Status"
	                PwnPet::getStats().hunger += 5; // Atenção = Comida
	                // PwnPet::feed(5); // Usar a função feed se existir e for mais completa
	                // Vou manter a versão do branch de merge, mas adicionando o comentário para clareza.
                speak("Ola amigo");
                break;
            case 3: // "Bateria" / "Comida"
                speak("Estou com fome");
                break;
	            case 4: // "Ataca Vivo" / "Ataque"
	                speak("Iniciando ataque");
	                PwnAttack::start();
                break;
            default:
                speak("Nao entendi");
                break;
        }
    }

	    static void speak(String phrase) {
	        // Otimização: Silent Mode
	        // if (PwnPet::isSilent()) return;
	
	        // Mapeia texto para arquivo WAV
        String file = "/tts/unknown.wav";

        if (phrase.indexOf("Ola") >= 0) file = "/tts/hello.wav";
        else if (phrase.indexOf("fome") >= 0) file = "/tts/hungry.wav";
        else if (phrase.indexOf("ataque") >= 0) file = "/tts/attack.wav";
        else if (phrase.indexOf("acordar") >= 0) file = "/tts/wakeup.wav";

        Serial.printf("[Voice] Falando: %s (%s)\n", phrase.c_str(), file.c_str());
        AudioHandler::playWav(file.c_str());
    }
};

bool PwnVoice::is_listening = false;

#endif
