#ifndef PWNSLEEP_H
#define PWNSLEEP_H

#include <Arduino.h>

class PwnSleep {
public:
    typedef void (*BrightnessFn)(int);
    
    static BrightnessFn set_brightness;
    static uint32_t last_activity_ms;
    static int full_brightness;
    static int dim_after_s;
    static int off_after_s;
    static int deep_after_s;
    static bool deep_enabled;
    static bool dimmed;
    static bool screen_off;

    static void init(BrightnessFn fn, int brightness, int timeout_s, bool deep, int deep_s);
    static void tick();
    static void notifyActivity();
    static bool isScreenOff();
    static bool isDimmed();
    static void setDimTimeout(int s);
    static void setOffTimeout(int s);
    static void enterDeep();
    static void setDeepEnabled(bool en);
    static void turnScreenOff();
};

#endif

