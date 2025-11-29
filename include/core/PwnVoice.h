#ifndef PWN_VOICE_H
#define PWN_VOICE_H

#include <Arduino.h>
#include "AudioHandler.h"
#include "OfflineVoice.h"
#include "core/PwnPet.h"
#include "core/PwnPower.h"

// Comandos de Voz Mapeados (Offline - Contagem de Sílabas)

class PwnVoice {
private:
    static bool is_listening;

public:
    static void init() {
        is_listening = false;
    }

    static void listen() {
        // Otimização 15: Não ouve se bateria crítica
        if (PwnPower::isCritical()) return;

        // Otimização 18: Wake Word só se tela ligada (implícito se chamado via botão)
        // Se fosse always-on listening, verificaríamos screen state.

        is_listening = true;

        // Toca "Ouvindo"
        // AudioHandler::playWav("/voice/listening.wav");

        // Grava 4s
        bool ok = AudioHandler::recordWav("/voice/input.wav", 4, true);

        is_listening = false;
        // Otimização 26: DSP off (feito no recordWav auto shutdown)

        if (ok) {
            int syllables = OfflineVoice::analyzeCommand("/voice/input.wav");
            processCommand(syllables);
        }
    }

    static void processCommand(int syllables) {
        Serial.printf("[Voice] Silabas: %d\n", syllables);

        // Se Pet estiver dormindo (Modo Zzz), ignora ou acorda?
        if (PwnPet::getStats().is_sleeping) {
             // Acorda se gritar (muitas silabas)?
             if (syllables > 4) {
                 PwnPet::getStats().is_sleeping = false;
                 speak("Quem ousa me acordar");
             }
             return;
        }

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
