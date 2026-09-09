#ifndef OFFLINEVOICE_H
#define OFFLINEVOICE_H

#include <Arduino.h>

class OfflineVoice {
public:
    static void init();
    static void startListening();
    static void stopListening();
    static String getLastCommand();
    static int getSyllableCount();
    static bool hasCommand();
    static void setThreshold(int level);
};

#endif
