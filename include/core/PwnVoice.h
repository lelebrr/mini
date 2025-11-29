#ifndef PWN_VOICE_H
#define PWN_VOICE_H

#include <Arduino.h>
#include "AudioHandler.h"
#include "OfflineVoice.h"
#include "core/PwnPet.h"
#include "core/PwnPower.h"
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#include "core/PwnAttack.h"
=======
>>>>>>> origin/merge-ready-mini-lele-v2
=======
#include "core/PwnAttack.h"
>>>>>>> origin/mini-lele-v2-complete-verified
=======
#include "core/PwnAttack.h"
>>>>>>> origin/mini-lele-v2-final-verified
=======
#include "core/PwnAttack.h"
>>>>>>> origin/mini-lele-v2-legendary-final-drivers

// Comandos de Voz Mapeados (Offline - Contagem de Sílabas)

class PwnVoice {
private:
    static bool is_listening;

public:
    static void init() {
        is_listening = false;
    }

    static void listen() {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	        if (PwnPower::isCritical()) return;
=======
        // Otimização 15: Não ouve se bateria crítica
        if (PwnPower::isCritical()) return;

        // Otimização 18: Wake Word só se tela ligada (implícito se chamado via botão)
        // Se fosse always-on listening, verificaríamos screen state.

>>>>>>> origin/merge-ready-mini-lele-v2
=======
        if (PwnPower::isCritical()) return;

>>>>>>> origin/mini-lele-v2-complete-verified
=======
        if (PwnPower::isCritical()) return;

>>>>>>> origin/mini-lele-v2-final-verified
=======
        if (PwnPower::isCritical()) return;

>>>>>>> origin/mini-lele-v2-legendary-final-drivers
        is_listening = true;

        // Toca "Ouvindo"
        // AudioHandler::playWav("/voice/listening.wav");

        // Grava 4s
        bool ok = AudioHandler::recordWav("/voice/input.wav", 4, true);

        is_listening = false;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
        // Otimização 26: DSP off (feito no recordWav auto shutdown)
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers

        if (ok) {
            int syllables = OfflineVoice::analyzeCommand("/voice/input.wav");
            processCommand(syllables);
        }
    }

    static void processCommand(int syllables) {
        Serial.printf("[Voice] Silabas: %d\n", syllables);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	        if (PwnPet::getStats().is_sleeping) {
=======
        // Se Pet estiver dormindo (Modo Zzz), ignora ou acorda?
        if (PwnPet::getStats().is_sleeping) {
             // Acorda se gritar (muitas silabas)?
>>>>>>> origin/merge-ready-mini-lele-v2
=======
        if (PwnPet::getStats().is_sleeping) {
>>>>>>> origin/mini-lele-v2-complete-verified
=======
        if (PwnPet::getStats().is_sleeping) {
>>>>>>> origin/mini-lele-v2-final-verified
=======
        if (PwnPet::getStats().is_sleeping) {
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
             if (syllables > 4) {
                 PwnPet::getStats().is_sleeping = false;
                 speak("Quem ousa me acordar");
             }
             return;
        }

        switch(syllables) {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	            case 2: // "Ei Lele" / "Status"
	                PwnPet::getStats().hunger += 5; // Atenção = Comida
	                // PwnPet::feed(5); // Usar a função feed se existir e for mais completa
	                // Vou manter a versão do branch de merge, mas adicionando o comentário para clareza.
=======
            case 2: // "Ei Pwn" / "Status"
                PwnPet::feed(5); // Atenção = Comida
>>>>>>> origin/merge-ready-mini-lele-v2
=======
            case 2: // "Ei Lele" / "Status"
                PwnPet::getStats().hunger += 5;
>>>>>>> origin/mini-lele-v2-complete-verified
=======
            case 2: // "Ei Lele" / "Status"
                PwnPet::getStats().hunger += 5;
>>>>>>> origin/mini-lele-v2-final-verified
=======
            case 2: // "Ei Lele" / "Status"
                PwnPet::getStats().hunger += 5;
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
                speak("Ola amigo");
                break;
            case 3: // "Bateria" / "Comida"
                speak("Estou com fome");
                break;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	            case 4: // "Ataca Vivo" / "Ataque"
	                speak("Iniciando ataque");
	                PwnAttack::start();
=======
            case 4: // "Ataca Vivo"
                speak("Iniciando ataque");
                // PwnAttack::start();
>>>>>>> origin/merge-ready-mini-lele-v2
=======
            case 4: // "Ataca Vivo" / "Ataque"
                speak("Iniciando ataque");
                PwnAttack::start(); // Agora existe
>>>>>>> origin/mini-lele-v2-complete-verified
=======
            case 4: // "Ataca Vivo" / "Ataque"
                speak("Iniciando ataque");
                PwnAttack::start(); // Agora existe
>>>>>>> origin/mini-lele-v2-final-verified
=======
            case 4: // "Ataca Vivo" / "Ataque"
                speak("Iniciando ataque");
                PwnAttack::start(); // Agora existe
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
                break;
            default:
                speak("Nao entendi");
                break;
        }
    }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	    static void speak(String phrase) {
	        // Otimização: Silent Mode
	        // if (PwnPet::isSilent()) return;
	
	        // Mapeia texto para arquivo WAV
=======
    // TTS Simulado (Frases pré-gravadas)
    static void speak(String phrase) {
        // Otimização: Silent Mode
        // if (PwnPet::isSilent()) return;

        // Mapeia texto para arquivo WAV
>>>>>>> origin/merge-ready-mini-lele-v2
=======
    static void speak(String phrase) {
>>>>>>> origin/mini-lele-v2-complete-verified
=======
    static void speak(String phrase) {
>>>>>>> origin/mini-lele-v2-final-verified
=======
    static void speak(String phrase) {
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
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
