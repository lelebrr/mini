#ifndef PWNVOICE_H
#define PWNVOICE_H

#include <Arduino.h>

class PwnVoice {
public:
    static bool is_listening;

    static void init();
    static void listen();
    static void processCommand();
    static void speak(const char* text);
    static void playWav(const char* path);
    static void setEnabled(bool en);
    static bool isEnabled();
    static void startListening();
    static void stopListening();
};

#endif
