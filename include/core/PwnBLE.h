#ifndef PWNBLE_H
#define PWNBLE_H

#include <Arduino.h>

class PwnBLE {
public:
    static int last_count;
    static int total_seen;

    static void init();
    static void deinit();
    static int scan(int timeout_sec);
    static int getDeviceCount();
    static String getDeviceName(int index);
    static String getDeviceAddress(int index);
    static int getDeviceRSSI(int index);
    static void clear();
};

#endif
