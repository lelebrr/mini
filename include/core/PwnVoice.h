#ifndef PWN_VOICE_H
#define PWN_VOICE_H

#include <Arduino.h>
#include "AudioHandler.h"
#include "OfflineVoice.h"
#include "PwnPet.h"
#include "PwnPower.h"
#include "PwnAttack.h"

class PwnVoice {
private:
    static bool listening;

public:
    static void init() {
        listening = false;
    }

    static void listen() {
        if (PwnPower::isCritical()) return;
        listening = true;

        bool ok = AudioHandler::recordWav("/voice_input.wav", 4, true);
        listening = false;

        if (ok) {
            int syllables = OfflineVoice::analyzeCommand("/voice_input.wav");
            processCommand(syllables);
        }
    }

    static void processCommand(int syllables) {
        PetStats s = PwnPet::getStats();
        if (s.is_sleeping) {
            if (syllables > 4) {
                PwnPet::setSleeping(false);
                speak("acordar");
            }
            return;
        }

        switch (syllables) {
            case 2:
                PwnPet::feed(5);
                speak("Ola");
                break;
            case 3:
                speak("fome");
                break;
            case 4:
                speak("ataque");
                PwnAttack::start();
                break;
            default:
                speak("nao entendi");
                break;
        }
    }

    static void speak(const String &phrase) {
        String file = "/tts_unknown.wav";
        if (phrase.indexOf("Ola") >= 0) file = "/tts_hello.wav";
        else if (phrase.indexOf("fome") >= 0) file = "/tts_hungry.wav";
        else if (phrase.indexOf("ataque") >= 0) file = "/tts_attack.wav";
        else if (phrase.indexOf("acordar") >= 0) file = "/tts_wakeup.wav";

        AudioHandler::playWav(file.c_str());
    }
};

inline bool PwnVoice::listening = false;

#endif
