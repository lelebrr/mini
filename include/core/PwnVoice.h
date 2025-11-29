#ifndef PWN_VOICE_H
#define PWN_VOICE_H

#include <Arduino.h>
#include "AudioHandler.h"
#include "OfflineVoice.h"
#include "core/PwnPet.h"
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#include "core/PwnPower.h"
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
#include "core/PwnAttack.h"
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
#include "core/PwnAttack.h"
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
#include "core/PwnAttack.h"
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
#include "core/PwnAttack.h"
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/mini-lele-v2-rebrand
=======
#include "core/PwnPower.h"
#include "core/PwnAttack.h"
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
#include "core/PwnPower.h"
#include "core/PwnAttack.h"
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
#include "core/PwnPower.h"
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
#include "core/PwnPower.h"
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized

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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	        if (PwnPower::isCritical()) return;
=======
=======
>>>>>>> origin/mini-lele-v2-rebrand
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
        // Otimização 15: Não ouve se bateria crítica
        if (PwnPower::isCritical()) return;

        // Otimização 18: Wake Word só se tela ligada (implícito se chamado via botão)
        // Se fosse always-on listening, verificaríamos screen state.

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
        if (PwnPower::isCritical()) return;

>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
        if (PwnPower::isCritical()) return;

>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
        if (PwnPower::isCritical()) return;

>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
        if (PwnPower::isCritical()) return;

>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/mini-lele-v2-rebrand
=======
        if (PwnPower::isCritical()) return;

>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
        if (PwnPower::isCritical()) return;

>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
        is_listening = true;

        // Toca "Ouvindo"
        // AudioHandler::playWav("/voice/listening.wav");
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======

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
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized

        // Grava 4s
        bool ok = AudioHandler::recordWav("/voice/input.wav", 4, true);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        is_listening = false;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
        // Otimização 26: DSP off (feito no recordWav auto shutdown)
>>>>>>> origin/mini-lele-v2-rebrand

=======
>>>>>>> origin/pwn-tamagotchi-br-release
=======
        is_listening = false;

>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
        is_listening = false;

>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
        is_listening = false;
        // Otimização 26: DSP off (feito no recordWav auto shutdown)

>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
        is_listening = false;
        // Otimização 26: DSP off (feito no recordWav auto shutdown)

>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
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
<<<<<<< HEAD

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
        if (PwnPet::getStats().is_sleeping) {
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
        if (PwnPet::getStats().is_sleeping) {
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
        if (PwnPet::getStats().is_sleeping) {
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
        if (PwnPet::getStats().is_sleeping) {
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
        // Se Pet estiver dormindo (Modo Zzz), ignora ou acorda?
        if (PwnPet::getStats().is_sleeping) {
             // Acorda se gritar (muitas silabas)?
>>>>>>> origin/mini-lele-v2-rebrand
=======

        if (PwnPet::getStats().is_sleeping) {
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======

        if (PwnPet::getStats().is_sleeping) {
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized

        // Se Pet estiver dormindo (Modo Zzz), ignora ou acorda?
        if (PwnPet::getStats().is_sleeping) {
             // Acorda se gritar (muitas silabas)?
<<<<<<< HEAD
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
            case 2: // "Ei Lele" / "Status"
                PwnPet::getStats().hunger += 5;
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
            case 2: // "Ei Lele" / "Status"
                PwnPet::getStats().hunger += 5;
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
            case 2: // "Ei Lele" / "Status"
                PwnPet::getStats().hunger += 5;
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
            case 2: // "Ei Lele" / "Status"
                PwnPet::getStats().hunger += 5;
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
            case 2: // "Ei Pwn" / "Status"
                PwnPet::feed(5); // Atenção = Comida
>>>>>>> origin/mini-lele-v2-rebrand
=======
        switch(syllables) {
            case 2: // "Ei Pwn" / "Status"
                PwnPet::feed(5); // Atenção = Comida
>>>>>>> origin/pwn-tamagotchi-br-release
=======
            case 2: // "Ei Lele" / "Status"
                PwnPet::getStats().hunger += 5;
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
            case 2: // "Ei Lele" / "Status"
                PwnPet::getStats().hunger += 5;
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
            case 2: // "Ei Pwn" / "Status"
                PwnPet::feed(5); // Atenção = Comida
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
            case 2: // "Ei Pwn" / "Status"
                PwnPet::feed(5); // Atenção = Comida
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
                speak("Ola amigo");
                break;
            case 3: // "Bateria" / "Comida"
                speak("Estou com fome");
                break;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
            case 4: // "Ataca Vivo" / "Ataque"
                speak("Iniciando ataque");
                PwnAttack::start(); // Agora existe
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
            case 4: // "Ataca Vivo" / "Ataque"
                speak("Iniciando ataque");
                PwnAttack::start(); // Agora existe
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
            case 4: // "Ataca Vivo" / "Ataque"
                speak("Iniciando ataque");
                PwnAttack::start(); // Agora existe
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
            case 4: // "Ataca Vivo" / "Ataque"
                speak("Iniciando ataque");
                PwnAttack::start(); // Agora existe
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
            case 4: // "Ataca Vivo"
                speak("Iniciando ataque");
                // PwnAttack::start();
>>>>>>> origin/mini-lele-v2-rebrand
=======
            case 4: // "Ataca Vivo"
                speak("Iniciando ataque");
                // PwnAttack::start();
>>>>>>> origin/pwn-tamagotchi-br-release
=======
            case 4: // "Ataca Vivo" / "Ataque"
                speak("Iniciando ataque");
                PwnAttack::start(); // Agora existe
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
            case 4: // "Ataca Vivo" / "Ataque"
                speak("Iniciando ataque");
                PwnAttack::start(); // Agora existe
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
            case 4: // "Ataca Vivo"
                speak("Iniciando ataque");
                // PwnAttack::start();
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
            case 4: // "Ataca Vivo"
                speak("Iniciando ataque");
                // PwnAttack::start();
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	    static void speak(String phrase) {
	        // Otimização: Silent Mode
	        // if (PwnPet::isSilent()) return;
	
	        // Mapeia texto para arquivo WAV
=======
=======
>>>>>>> origin/mini-lele-v2-rebrand
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
    // TTS Simulado (Frases pré-gravadas)
    static void speak(String phrase) {
        // Otimização: Silent Mode
        // if (PwnPet::isSilent()) return;

        // Mapeia texto para arquivo WAV
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
    static void speak(String phrase) {
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
    static void speak(String phrase) {
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
    static void speak(String phrase) {
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
    static void speak(String phrase) {
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/mini-lele-v2-rebrand
=======
    // TTS Simulado (Frases pré-gravadas)
    static void speak(String phrase) {
        // Mapeia texto para arquivo WAV
>>>>>>> origin/pwn-tamagotchi-br-release
=======
    static void speak(String phrase) {
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
    static void speak(String phrase) {
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
        String file = "/tts/unknown.wav";

        if (phrase.indexOf("Ola") >= 0) file = "/tts/hello.wav";
        else if (phrase.indexOf("fome") >= 0) file = "/tts/hungry.wav";
        else if (phrase.indexOf("ataque") >= 0) file = "/tts/attack.wav";
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        else if (phrase.indexOf("acordar") >= 0) file = "/tts/wakeup.wav";
=======
        else if (phrase.indexOf("Mano") >= 0) file = "/tts/slang_mano.wav";
>>>>>>> origin/pwn-tamagotchi-br-release
=======
        else if (phrase.indexOf("acordar") >= 0) file = "/tts/wakeup.wav";
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
        else if (phrase.indexOf("acordar") >= 0) file = "/tts/wakeup.wav";
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
        else if (phrase.indexOf("acordar") >= 0) file = "/tts/wakeup.wav";
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
        else if (phrase.indexOf("acordar") >= 0) file = "/tts/wakeup.wav";
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized

        Serial.printf("[Voice] Falando: %s (%s)\n", phrase.c_str(), file.c_str());
        AudioHandler::playWav(file.c_str());
    }
};

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
bool PwnVoice::is_listening = false;

=======
>>>>>>> origin/pwn-tamagotchi-br-release
=======
bool PwnVoice::is_listening = false;

>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
bool PwnVoice::is_listening = false;

>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
bool PwnVoice::is_listening = false;

>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
bool PwnVoice::is_listening = false;

>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
#endif
