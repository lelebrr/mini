#ifndef PWN_UI_H
#define PWN_UI_H

#include <lvgl.h>
#include "PwnPet.h"
#include "PwnPower.h"
#include "FaceHandler.h"
#include "pin_config.h"

class PwnUI {
private:
    static lv_obj_t* scr_main;
    static lv_obj_t* bar_hunger;
    static lv_obj_t* bar_happy;
    static lv_obj_t* label_clock;
    static lv_obj_t* label_stats;
    static lv_obj_t* label_battery;

    static lv_style_t style_base;
    static lv_style_t style_bar_bg;
    static lv_style_t style_bar_indic;

public:
    static void initStyles() {
        lv_style_init(&style_base);
        lv_style_set_bg_color(&style_base, lv_color_hex(0x000000));
        lv_style_set_text_color(&style_base, lv_color_hex(0x00FF00));
        lv_style_set_text_font(&style_base, &lv_font_montserrat_14);

        lv_style_init(&style_bar_bg);
        lv_style_set_bg_color(&style_bar_bg, lv_color_hex(0x202020));
        lv_style_set_border_color(&style_bar_bg, lv_color_hex(0x00FF00));
        lv_style_set_border_width(&style_bar_bg, 1);

        lv_style_init(&style_bar_indic);
        lv_style_set_bg_color(&style_bar_indic, lv_color_hex(0x00AAFF));
    }

    static void init() {
        initStyles();

        scr_main = lv_screen_active();
        lv_obj_add_style(scr_main, &style_base, 0);

        label_clock = lv_label_create(scr_main);
        lv_obj_set_style_text_font(label_clock, &lv_font_montserrat_20, 0);
        lv_label_set_text(label_clock, "00:00");
        lv_obj_align(label_clock, LV_ALIGN_TOP_MID, 0, 10);

        label_battery = lv_label_create(scr_main);
        lv_obj_set_style_text_font(label_battery, &lv_font_montserrat_10, 0);
        lv_label_set_text(label_battery, "---");
        lv_obj_align(label_battery, LV_ALIGN_TOP_RIGHT, -5, 5);

        bar_hunger = lv_bar_create(scr_main);
        lv_obj_add_style(bar_hunger, &style_bar_bg, 0);
        lv_obj_add_style(bar_hunger, &style_bar_indic, LV_PART_INDICATOR);
        lv_obj_set_size(bar_hunger, 100, 12);
        lv_obj_align(bar_hunger, LV_ALIGN_TOP_LEFT, 10, 40);
        lv_bar_set_range(bar_hunger, 0, 100);

        bar_happy = lv_bar_create(scr_main);
        lv_obj_add_style(bar_happy, &style_bar_bg, 0);
        lv_obj_add_style(bar_happy, &style_bar_indic, LV_PART_INDICATOR);
        lv_obj_set_size(bar_happy, 100, 12);
        lv_obj_align(bar_happy, LV_ALIGN_TOP_RIGHT, -10, 40);
        lv_bar_set_range(bar_happy, 0, 100);

        FaceHandler::init(scr_main);

        label_stats = lv_label_create(scr_main);
        lv_obj_set_style_text_font(label_stats, &lv_font_montserrat_10, 0);
        lv_label_set_text(label_stats, "Inicializando...");
        lv_obj_align(label_stats, LV_ALIGN_BOTTOM_MID, 0, -10);
    }

    static void update() {
        PetStats s = PwnPet::getStats();

        lv_bar_set_value(bar_hunger, s.hunger, LV_ANIM_ON);
        lv_bar_set_value(bar_happy, s.happiness, LV_ANIM_ON);

        int pct = PwnPower::getBatteryPercent();
        char bbuf[16];
        snprintf(bbuf, sizeof(bbuf), "%d%%", pct);
        lv_label_set_text(label_battery, bbuf);

        char sbuf[64];
        snprintf(sbuf, sizeof(sbuf), "Lvl %d | HS %d", s.level, s.handshakes_total);
        lv_label_set_text(label_stats, sbuf);

        unsigned long t = millis() / 1000;
        int m = (t / 60) % 60;
        int h = (t / 3600) % 24;
        char cbuf[10];
        snprintf(cbuf, sizeof(cbuf), "%02d:%02d", h, m);
        lv_label_set_text(label_clock, cbuf);

        if (s.stage == PWNGOD) {
            FaceHandler::setFace(FACE_COOL);
        } else if (s.hunger < 20) {
            FaceHandler::setFace(FACE_SAD);
        } else {
            FaceHandler::setFace(FACE_NEUTRAL);
        }
    }
};

inline lv_obj_t* PwnUI::scr_main = nullptr;
inline lv_obj_t* PwnUI::bar_hunger = nullptr;
inline lv_obj_t* PwnUI::bar_happy = nullptr;
inline lv_obj_t* PwnUI::label_clock = nullptr;
inline lv_obj_t* PwnUI::label_stats = nullptr;
inline lv_obj_t* PwnUI::label_battery = nullptr;

inline lv_style_t PwnUI::style_base;
inline lv_style_t PwnUI::style_bar_bg;
inline lv_style_t PwnUI::style_bar_indic;

#endif
