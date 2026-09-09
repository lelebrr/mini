#ifndef AUDIOHANDLER_H
#define AUDIOHANDLER_H

#include <Arduino.h>

class AudioHandler {
public:
    static bool audio_ok;

    static void init();
    static void beep(int freq, int dur_ms);
    static void playWav(const char* path);
    static void recordWav(const char* path, int duration_ms);
    static void setVolume(int vol);
    static int getVolume();
    static void setAmpPower(bool on);
};

#endif
