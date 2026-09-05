#ifndef PWN_UI_H
#define PWN_UI_H

#include <lvgl.h>
#include "core/PwnPet.h"
#include "core/PwnPower.h"
#include "core/PwnBLE.h"
#include "Gamification.h"
#include "WiFiTools.h"
#include "FaceHandler.h"
#include "drivers/PwnRTC.h"
#include "pin_config.h"

/**
 * PwnUI
 * -----
 * Interface no AMOLED (368x448), tema escuro neon, com NAVEGAÇÃO POR GESTOS:
 * um `tileview` com 3 telas deslizáveis (arraste na horizontal):
 *   [0] Pet     — rosto, nome, humor, fome/felicidade
 *   [1] Wi-Fi   — dispositivos próximos, handshakes, status do sniffer
 *   [2] Energia — bateria/carga, capacidade, autonomia, BLE, uptime
 *
 * Barra superior fixa: relógio (RTC) + bateria/carga.
 * Anti-burn-in: a área grande é deslocada alguns pixels periodicamente (o preto
 * do AMOLED = pixel apagado, então a folga nas bordas não incomoda).
 */

#define UI_BG        lv_color_hex(0x0A0E14)
#define UI_PANEL     lv_color_hex(0x121821)
#define UI_PANEL2    lv_color_hex(0x1A2230)
#define UI_GREEN     lv_color_hex(0x38FF9E)
#define UI_PURPLE    lv_color_hex(0xB26BFF)
#define UI_ORANGE    lv_color_hex(0xFFA53B)
#define UI_RED       lv_color_hex(0xFF5A7A)
#define UI_TEXT      lv_color_hex(0xE6F0FF)
#define UI_DIM       lv_color_hex(0x7A8AA0)

class PwnUI {
private:
    static lv_obj_t *scr_main;
    static lv_obj_t *tv;
    static lv_obj_t *bar_hunger;
    static lv_obj_t *bar_happy;
    static lv_obj_t *label_clock;
    static lv_obj_t *label_battery;
    static lv_obj_t *label_name;
    static lv_obj_t *label_mood;
    static lv_obj_t *label_stats;   // tile Pet: chips
    static lv_obj_t *label_wifi;    // tile Wi-Fi
    static lv_obj_t *label_power;   // tile Energia

    static void styleCard(lv_obj_t *o, lv_color_t border) {
        lv_obj_set_style_bg_color(o, UI_PANEL, 0);
        lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(o, 14, 0);
        lv_obj_set_style_border_color(o, border, 0);
        lv_obj_set_style_border_width(o, 1, 0);
        lv_obj_set_style_border_opa(o, LV_OPA_50, 0);
        lv_obj_set_style_pad_all(o, 8, 0);
        lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
    }
    static lv_obj_t *bodyLabel(lv_obj_t *parent, const lv_font_t *font, lv_color_t col) {
        lv_obj_t *l = lv_label_create(parent);
        lv_obj_set_style_text_font(l, font, 0);
        lv_obj_set_style_text_color(l, col, 0);
        lv_label_set_long_mode(l, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(l, LCD_WIDTH - 40);
        return l;
    }

public:
    static void init() {
        scr_main = lv_screen_active();
        lv_obj_set_style_bg_color(scr_main, UI_BG, 0);
        lv_obj_set_style_bg_opa(scr_main, LV_OPA_COVER, 0);
        lv_obj_clear_flag(scr_main, LV_OBJ_FLAG_SCROLLABLE);

        // ---- Barra superior fixa ----
        lv_obj_t *topbar = lv_obj_create(scr_main);
        lv_obj_set_size(topbar, LCD_WIDTH - 16, 40);
        lv_obj_align(topbar, LV_ALIGN_TOP_MID, 0, 4);
        styleCard(topbar, UI_PURPLE);

        label_clock = lv_label_create(topbar);
        lv_obj_set_style_text_font(label_clock, &lv_font_montserrat_20, 0);
        lv_obj_set_style_text_color(label_clock, UI_TEXT, 0);
        lv_label_set_text(label_clock, "00:00");
        lv_obj_align(label_clock, LV_ALIGN_LEFT_MID, 2, 0);

        label_battery = lv_label_create(topbar);
        lv_obj_set_style_text_font(label_battery, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(label_battery, UI_GREEN, 0);
        lv_label_set_text(label_battery, "-- %");
        lv_obj_align(label_battery, LV_ALIGN_RIGHT_MID, -2, 0);

        // ---- Tileview (telas deslizáveis) ----
        tv = lv_tileview_create(scr_main);
        lv_obj_set_pos(tv, 2, 48);
        lv_obj_set_size(tv, LCD_WIDTH - 4, LCD_HEIGHT - 52);
        lv_obj_set_style_bg_opa(tv, LV_OPA_TRANSP, 0);

        buildTilePet(lv_tileview_add_tile(tv, 0, 0, LV_DIR_HOR));
        buildTileWifi(lv_tileview_add_tile(tv, 1, 0, LV_DIR_HOR));
        buildTilePower(lv_tileview_add_tile(tv, 2, 0, LV_DIR_HOR));
    }

    // Navegação por botão físico: vai para a próxima tela.
    static void nextTile() {
        static int cur = 0;
        cur = (cur + 1) % 3;
        lv_tileview_set_tile_by_index(tv, cur, 0, LV_ANIM_ON);
    }

    static void update() {
        if (!scr_main) return;
        PetStats pet = PwnPet::getStats();
        GameStats g  = Gamification::getStats();

        // Barra superior
        lv_label_set_text(label_clock, PwnRTC::getClock().c_str());
        int  batt = PwnPower::getBatteryPercent();
        bool chg  = PwnPower::isCharging();
        bool usb  = PwnPower::isVbusIn();
        lv_label_set_text_fmt(label_battery, "%s %d%%",
            chg ? LV_SYMBOL_CHARGE : (usb ? LV_SYMBOL_USB : LV_SYMBOL_BATTERY_FULL), batt);
        lv_obj_set_style_text_color(label_battery,
            (batt < 15 && !usb) ? UI_RED : (chg ? UI_GREEN : UI_TEXT), 0);

        // Tile Pet
        lv_bar_set_value(bar_hunger, pet.hunger, LV_ANIM_ON);
        lv_bar_set_value(bar_happy, pet.happiness, LV_ANIM_ON);
        lv_label_set_text(label_name, pet.name.c_str());
        lv_label_set_text(label_mood, PwnPet::getMoodStr().c_str());
        const char *stages[] = {"EGG","BABY","KID","TEEN","ADULT","PWNLORD","PWNGOD"};
        int stageIdx = (int)pet.stage;   // bounds-check: save corrompido não pode OOB
        if (stageIdx < 0 || stageIdx > PWNGOD) stageIdx = PWNGOD;
        lv_label_set_text_fmt(label_stats, "Lvl %d  •  XP %u  •  %s",
                              pet.level, (unsigned)g.xp, stages[stageIdx]);

        FaceHandler::setSkin((uint8_t)pet.stage);   // "espécie" muda com a evolução
        if (pet.stage == PWNGOD)      FaceHandler::setFace(FACE_COOL);
        else if (pet.is_sleeping)     FaceHandler::setFace(FACE_SLEEP);
        else if (chg)                 FaceHandler::setFace(FACE_GRATEFUL);
        else if (pet.hunger < 20)     FaceHandler::setFace(FACE_SAD);
        else if (pet.happiness > 80)  FaceHandler::setFace(FACE_HAPPY);
        else                          FaceHandler::setFace(FACE_NEUTRAL);

        // Tile Wi-Fi
        String w = "Dispositivos: " + String((int)WiFiTools::nearby_devices.size()) + "\n";
        w += "Handshakes: " + String((unsigned)WiFiTools::getHandshakeCount()) + "\n";
        w += String("Sniffer: ") + (WiFiTools::isSniffing() ? "ATIVO" : "parado") + "\n\n";
        int shown = 0;
        for (auto &d : WiFiTools::nearby_devices) {
            w += d.mac + "  " + String(d.rssi) + "dBm\n";
            if (++shown >= 8) break;
        }
        if (WiFiTools::nearby_devices.empty()) w += "(procurando...)";
        lv_label_set_text(label_wifi, w.c_str());

        // Tile Energia
        String p = "Bateria: " + String(batt) + "%  (" +
                   String(PwnPower::getBatteryVoltage(), 2) + " V)\n";
        p += "Estado: " + PwnPower::getChargeStatusStr() + "\n";
        p += "Capacidade: " + String(PwnPower::getBatteryCapacity()) + " mAh\n";
        if (!usb) p += "Autonomia: ~" + String(PwnPower::getEstimatedHours(), 1) + " h\n";
        p += "BLE visto: " + String(PwnBLE::getLastCount()) + "\n";
        p += "RAM livre: " + String(ESP.getFreeHeap() / 1024) + " KB\n";
        uint32_t up = millis() / 1000;
        p += "Uptime: " + String(up / 3600) + "h " + String((up / 60) % 60) + "m";
        lv_label_set_text(label_power, p.c_str());

        antiBurnIn();
    }

private:
    static void buildTilePet(lv_obj_t *t) {
        lv_obj_clear_flag(t, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_t *facePanel = lv_obj_create(t);
        lv_obj_set_size(facePanel, 240, 120);
        lv_obj_align(facePanel, LV_ALIGN_TOP_MID, 0, 6);
        styleCard(facePanel, UI_GREEN);
        FaceHandler::init(facePanel);

        label_name = lv_label_create(t);
        lv_obj_set_style_text_font(label_name, &lv_font_montserrat_20, 0);
        lv_obj_set_style_text_color(label_name, UI_TEXT, 0);
        lv_label_set_text(label_name, "PwnBaby");
        lv_obj_align(label_name, LV_ALIGN_TOP_MID, 0, 134);

        label_mood = lv_label_create(t);
        lv_obj_set_style_text_font(label_mood, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(label_mood, UI_PURPLE, 0);
        lv_label_set_text(label_mood, "FELIZ");
        lv_obj_align(label_mood, LV_ALIGN_TOP_MID, 0, 160);

        makeCap(t, "FOME", 188);
        bar_hunger = makeBar(t, 206, UI_ORANGE);
        makeCap(t, "FELICIDADE", 232);
        bar_happy  = makeBar(t, 250, UI_GREEN);

        label_stats = lv_label_create(t);
        lv_obj_set_style_text_font(label_stats, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(label_stats, UI_TEXT, 0);
        lv_obj_align(label_stats, LV_ALIGN_TOP_MID, 0, 288);
        lv_label_set_text(label_stats, "Lvl 1");
    }
    static void buildTileWifi(lv_obj_t *t) {
        lv_obj_clear_flag(t, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_t *card = lv_obj_create(t);
        lv_obj_set_size(card, LCD_WIDTH - 16, LCD_HEIGHT - 70);
        lv_obj_align(card, LV_ALIGN_TOP_MID, 0, 4);
        styleCard(card, UI_GREEN);
        label_wifi = bodyLabel(card, &lv_font_montserrat_14, UI_TEXT);
        lv_obj_align(label_wifi, LV_ALIGN_TOP_LEFT, 0, 0);
        lv_label_set_text(label_wifi, "Wi-Fi...");
    }
    static void buildTilePower(lv_obj_t *t) {
        lv_obj_clear_flag(t, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_t *card = lv_obj_create(t);
        lv_obj_set_size(card, LCD_WIDTH - 16, LCD_HEIGHT - 70);
        lv_obj_align(card, LV_ALIGN_TOP_MID, 0, 4);
        styleCard(card, UI_PURPLE);
        label_power = bodyLabel(card, &lv_font_montserrat_16, UI_TEXT);
        lv_obj_align(label_power, LV_ALIGN_TOP_LEFT, 0, 0);
        lv_label_set_text(label_power, "Energia...");
    }
    static lv_obj_t *makeBar(lv_obj_t *p, int y, lv_color_t fill) {
        lv_obj_t *bar = lv_bar_create(p);
        lv_obj_set_size(bar, LCD_WIDTH - 60, 14);
        lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, y);
        lv_bar_set_range(bar, 0, 100);
        lv_bar_set_value(bar, 100, LV_ANIM_OFF);
        lv_obj_set_style_radius(bar, 7, 0);
        lv_obj_set_style_bg_color(bar, UI_PANEL2, LV_PART_MAIN);
        lv_obj_set_style_bg_color(bar, fill, LV_PART_INDICATOR);
        lv_obj_set_style_radius(bar, 7, LV_PART_INDICATOR);
        return bar;
    }
    static void makeCap(lv_obj_t *p, const char *txt, int y) {
        lv_obj_t *l = lv_label_create(p);
        lv_obj_set_style_text_font(l, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(l, UI_DIM, 0);
        lv_label_set_text(l, txt);
        lv_obj_align(l, LV_ALIGN_TOP_LEFT, 26, y);
    }
    // Desloca a área grande alguns pixels a cada ~40s (anti-burn-in do AMOLED).
    static void antiBurnIn() {
        static uint32_t last = 0;
        static int step = 0;
        if (millis() - last < 40000) return;
        last = millis();
        const int dx[] = {0, 3, 3, 0};
        const int dy[] = {0, 0, 3, 3};
        step = (step + 1) % 4;
        lv_obj_set_pos(tv, 2 + dx[step], 48 + dy[step]);
    }
};

// Definições estáticas em core_singletons.cpp
#endif
