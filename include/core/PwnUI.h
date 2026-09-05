#ifndef PWN_UI_H
#define PWN_UI_H

#include <lvgl.h>
#include "core/PwnPet.h"
#include "core/PwnPower.h"
#include "Gamification.h"
#include "WiFiTools.h"
#include "FaceHandler.h"
#include "pin_config.h"

/**
 * PwnUI
 * -----
 * Interface principal no AMOLED (368x448), redesenhada: tema escuro com acentos
 * neon, cartão do rosto, medidores de fome/felicidade, barra de status superior
 * (relógio + bateria/carga) e chips de estatísticas na base.
 */

// Paleta
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
    static lv_obj_t *bar_hunger;
    static lv_obj_t *bar_happy;
    static lv_obj_t *label_clock;
    static lv_obj_t *label_stats;
    static lv_obj_t *label_battery;
    static lv_obj_t *label_name;
    static lv_obj_t *label_mood;

    // Aplica um visual de "cartão" a um objeto.
    static void styleCard(lv_obj_t *o, lv_color_t border) {
        lv_obj_set_style_bg_color(o, UI_PANEL, 0);
        lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(o, 14, 0);
        lv_obj_set_style_border_color(o, border, 0);
        lv_obj_set_style_border_width(o, 1, 0);
        lv_obj_set_style_border_opa(o, LV_OPA_60, 0);
        lv_obj_set_style_pad_all(o, 6, 0);
        lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
    }

    static lv_obj_t *makeBar(lv_obj_t *parent, int y, lv_color_t fill) {
        lv_obj_t *bar = lv_bar_create(parent);
        lv_obj_set_size(bar, 300, 14);
        lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, y);
        lv_bar_set_range(bar, 0, 100);
        lv_bar_set_value(bar, 100, LV_ANIM_OFF);
        lv_obj_set_style_radius(bar, 7, 0);
        lv_obj_set_style_bg_color(bar, UI_PANEL2, LV_PART_MAIN);
        lv_obj_set_style_bg_color(bar, fill, LV_PART_INDICATOR);
        lv_obj_set_style_radius(bar, 7, LV_PART_INDICATOR);
        return bar;
    }

    static lv_obj_t *makeCaption(lv_obj_t *parent, const char *txt, int y) {
        lv_obj_t *l = lv_label_create(parent);
        lv_obj_set_style_text_font(l, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(l, UI_DIM, 0);
        lv_label_set_text(l, txt);
        lv_obj_align(l, LV_ALIGN_TOP_LEFT, 34, y);
        return l;
    }

public:
    static void init() {
        scr_main = lv_screen_active();
        lv_obj_set_style_bg_color(scr_main, UI_BG, 0);
        lv_obj_set_style_bg_opa(scr_main, LV_OPA_COVER, 0);
        lv_obj_clear_flag(scr_main, LV_OBJ_FLAG_SCROLLABLE);

        // ---- Barra superior ----
        lv_obj_t *topbar = lv_obj_create(scr_main);
        lv_obj_set_size(topbar, 348, 40);
        lv_obj_align(topbar, LV_ALIGN_TOP_MID, 0, 8);
        styleCard(topbar, UI_PURPLE);

        label_clock = lv_label_create(topbar);
        lv_obj_set_style_text_font(label_clock, &lv_font_montserrat_20, 0);
        lv_obj_set_style_text_color(label_clock, UI_TEXT, 0);
        lv_label_set_text(label_clock, "00:00");
        lv_obj_align(label_clock, LV_ALIGN_LEFT_MID, 4, 0);

        label_battery = lv_label_create(topbar);
        lv_obj_set_style_text_font(label_battery, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(label_battery, UI_GREEN, 0);
        lv_label_set_text(label_battery, "-- %");
        lv_obj_align(label_battery, LV_ALIGN_RIGHT_MID, -4, 0);

        // ---- Cartão do rosto ----
        lv_obj_t *facePanel = lv_obj_create(scr_main);
        lv_obj_set_size(facePanel, 240, 130);
        lv_obj_align(facePanel, LV_ALIGN_TOP_MID, 0, 62);
        styleCard(facePanel, UI_GREEN);
        FaceHandler::init(facePanel);

        // Nome + humor
        label_name = lv_label_create(scr_main);
        lv_obj_set_style_text_font(label_name, &lv_font_montserrat_20, 0);
        lv_obj_set_style_text_color(label_name, UI_TEXT, 0);
        lv_label_set_text(label_name, "PwnBaby");
        lv_obj_align(label_name, LV_ALIGN_TOP_MID, 0, 200);

        label_mood = lv_label_create(scr_main);
        lv_obj_set_style_text_font(label_mood, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(label_mood, UI_PURPLE, 0);
        lv_label_set_text(label_mood, "FELIZ");
        lv_obj_align(label_mood, LV_ALIGN_TOP_MID, 0, 226);

        // ---- Medidores ----
        makeCaption(scr_main, "FOME", 250);
        bar_hunger = makeBar(scr_main, 268, UI_ORANGE);
        makeCaption(scr_main, "FELICIDADE", 292);
        bar_happy  = makeBar(scr_main, 310, UI_GREEN);

        // ---- Chips de status (base) ----
        lv_obj_t *statPanel = lv_obj_create(scr_main);
        lv_obj_set_size(statPanel, 348, 56);
        lv_obj_align(statPanel, LV_ALIGN_BOTTOM_MID, 0, -10);
        styleCard(statPanel, UI_PURPLE);

        label_stats = lv_label_create(statPanel);
        lv_obj_set_style_text_font(label_stats, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(label_stats, UI_TEXT, 0);
        lv_label_set_long_mode(label_stats, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(label_stats, 330);
        lv_obj_set_style_text_align(label_stats, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_text(label_stats, "Inicializando...");
        lv_obj_center(label_stats);
    }

    static void update() {
        if (!scr_main) return;
        PetStats pet = PwnPet::getStats();
        GameStats g  = Gamification::getStats();

        lv_bar_set_value(bar_hunger, pet.hunger, LV_ANIM_ON);
        lv_bar_set_value(bar_happy, pet.happiness, LV_ANIM_ON);

        // Nome + humor
        lv_label_set_text(label_name, pet.name.c_str());
        lv_label_set_text(label_mood, PwnPet::getMoodStr().c_str());

        // Bateria + carga
        int  batt = PwnPower::getBatteryPercent();
        bool chg  = PwnPower::isCharging();
        bool usb  = PwnPower::isVbusIn();
        lv_label_set_text_fmt(label_battery, "%s %d%%",
                              chg ? LV_SYMBOL_CHARGE : (usb ? LV_SYMBOL_USB : LV_SYMBOL_BATTERY_FULL),
                              batt);
        lv_color_t bcol = (batt < 15 && !usb) ? UI_RED : (chg ? UI_GREEN : UI_TEXT);
        lv_obj_set_style_text_color(label_battery, bcol, 0);

        // Chips
        lv_label_set_text_fmt(label_stats,
            "Lvl %d   XP %u   HS %d   Dispositivos %d",
            pet.level, (unsigned)g.xp, pet.handshakes_total,
            (int)WiFiTools::nearby_devices.size());

        // Relógio (millis até termos RTC)
        unsigned long t = millis() / 1000;
        lv_label_set_text_fmt(label_clock, "%02d:%02d",
                              (int)((t / 3600) % 24), (int)((t / 60) % 60));

        // Rosto conforme estado
        if (pet.stage == PWNGOD)        FaceHandler::setFace(FACE_COOL);
        else if (pet.is_sleeping)       FaceHandler::setFace(FACE_SLEEP);
        else if (chg)                   FaceHandler::setFace(FACE_GRATEFUL);
        else if (pet.hunger < 20)       FaceHandler::setFace(FACE_SAD);
        else if (pet.happiness > 80)    FaceHandler::setFace(FACE_HAPPY);
        else                            FaceHandler::setFace(FACE_NEUTRAL);
    }
};

// Definições estáticas declaradas em core_singletons.cpp
#endif
