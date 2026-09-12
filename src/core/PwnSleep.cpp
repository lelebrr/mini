#include "core/PwnSleep.h"
#include "core/ConfigManager.h"

void PwnSleep::init(BrightnessFn fn, int brightness, int timeout_s, bool deep, int deep_s) {
    set_brightness = fn;
    full_brightness = brightness;
    dim_after_s = timeout_s;
    off_after_s = timeout_s * 2;
    deep_after_s = deep_s;
    deep_enabled = deep;
    last_activity_ms = millis();
    dimmed = false;
    screen_off = false;
    Serial.println("[PwnSleep] Controle de sono inicializado");
}

void PwnSleep::tick() {
    uint32_t elapsed = (millis() - last_activity_ms) / 1000;
    if (screen_off) return;

    if (elapsed > off_after_s) {
        if (!screen_off) {
            screen_off = true;
            if (set_brightness) set_brightness(0);
            Serial.println("[PwnSleep] Tela desligada");
        }
    } else if (elapsed > dim_after_s) {
        if (!dimmed) {
            dimmed = true;
            if (set_brightness) set_brightness(full_brightness / 4);
            Serial.println("[PwnSleep] Brilho reduzido");
        }
    }
}

void PwnSleep::notifyActivity() {
    last_activity_ms = millis();
    if (screen_off) {
        screen_off = false;
        if (set_brightness) set_brightness(full_brightness);
    }
    if (dimmed) {
        dimmed = false;
        if (set_brightness) set_brightness(full_brightness);
    }
}

bool PwnSleep::isScreenOff() { return screen_off; }
bool PwnSleep::isDimmed() { return dimmed; }

void PwnSleep::setDimTimeout(int s) {
    dim_after_s = s;
    ConfigManager::getInstance()->set("disp_timeout_sec", (int64_t)s);
}

void PwnSleep::setOffTimeout(int s) {
    off_after_s = s;
}

void PwnSleep::enterDeep() {
    if (deep_enabled) {
        Serial.println("[PwnSleep] Deep sleep");
        ESP.deepSleep(deep_after_s * 1000000ULL);
    }
}

void PwnSleep::setDeepEnabled(bool en) {
    deep_enabled = en;
    ConfigManager::getInstance()->set("pwr_deep_sleep_enabled", en);
}

void PwnSleep::turnScreenOff() {
    // Apaga a tela imediatamente (backlight 0), sem esperar o timeout
    if (!screen_off) {
        screen_off = true;
        dimmed = false;
        if (set_brightness) set_brightness(0);
        Serial.println("[PwnSleep] Tela desligada pelo botao");
    }
}
