#ifndef WEBHANDLER_H
#define WEBHANDLER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

class WebHandler {
public:
    static void init();
    static void loop();
    static void stop();
    static bool isRunning();
    static void restart();
    static String getStatusJSON();
    static String getConfigJSON();
    static bool updateConfig(const char* json);
    static void handleUpload();
    static void sendWebSocket(const char* msg);
};

#endif
