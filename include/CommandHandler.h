#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <Arduino.h>

class CommandHandler {
public:
    static void init();
    static void handle(const char* cmd);
    static void handleJSON(const char* json);
    static void setCallback(void (*cb)(const char*));
};

#endif
