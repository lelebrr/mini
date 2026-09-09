#ifndef PWNATTACK_H
#define PWNATTACK_H

#include <Arduino.h>

struct AttackStats {
    int handshakes = 0;
    int packets = 0;
    int deauth = 0;
    int eapol = 0;
    int portals = 0;
    unsigned long start_time = 0;
};

class PwnAttack {
public:
    static AttackStats stats;
    static bool scan_active;

    static void init();
    static void start();
    static void stop();
    static void tick();
    static bool isScanning();
    static int getHandshakeCount();
    static void enableDeauth(bool en);
    static void enableEvilPortal(bool en);
    static void setAutoScan(bool en);
    static void cycleTarget();
    static int getDeauthCount();
};

#endif
