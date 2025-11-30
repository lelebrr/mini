#ifndef PWN_UI_H
#define PWN_UI_H

#include <lvgl.h>
#include "core/PwnPet.h"
#include "core/PwnPower.h"
#include "Gamification.h"
#include "FaceHandler.h"
#include "pin_config.h"

/**
 * PwnUI
 *
 * Camada de UI principal baseada em LVGL.
 * Mostra:
 *  - Relógio
 *  - Barras de Fome / Felicidade
 *  - Bateria
 *  - Stats simples
 *  - Face ASCII via FaceHandler
 */

class PwnUI {
private:
    static lv_obj_t *scr_main;
    static lv_obj_t *bar_hunger;
    static lv_obj_t *bar_happy;
    static lv_obj_t *label_clock;
    static lv_obj_t *label_stats;
    static lv_obj_t *label_battery;

public:
    static void init() {
        scr_main = lv_screen_active();

        // Fundo escuro
        lv_obj_set_style_bg_color(scr_main, lv_color_hex(0x000000), 0);
        lv_obj_set_style_bg_opa(scr_main, LV_OPA_COVER, 0);

        // Relógio
        label_clock = lv_label_create(scr_main);
        lv_obj_set_style_text_font(label_clock, &lv_font_montserrat_20, 0);
        lv_label_set_text(label_clock, "00:00");
        lv_obj_align(label_clock, LV_ALIGN_TOP_MID, 0, 10);

        // Bateria
        label_battery = lv_label_create(scr_main);
        lv_obj_set_style_text_font(label_battery, &lv_font_montserrat_10, 0);
        lv_label_set_text(label_battery, "---");
        lv_obj_align(label_battery, LV_ALIGN_TOP_RIGHT, -5, 5);

        // Barras de status
        bar_hunger = lv_bar_create(scr_main);
        lv_obj_set_size(bar_hunger, 110, 10);
        lv_obj_align(bar_hunger, LV_ALIGN_TOP_LEFT, 10, 40);
        lv_bar_set_range(bar_hunger, 0, 100);
        lv_bar_set_value(bar_hunger, 100, LV_ANIM_OFF);

        bar_happy = lv_bar_create(scr_main);
        lv_obj_set_size(bar_happy, 110, 10);
        lv_obj_align(bar_happy, LV_ALIGN_TOP_RIGHT, -10, 40);
        lv_bar_set_range(bar_happy, 0, 100);
        lv_bar_set_value(bar_happy, 100, LV_ANIM_OFF);

        // Face
        FaceHandler::init(scr_main);

        // Texto de stats
        label_stats = lv_label_create(scr_main);
        lv_obj_set_style_text_font(label_stats, &lv_font_montserrat_10, 0);
        lv_label_set_text(label_stats, "Inicializando...");
        lv_obj_align(label_stats, LV_ALIGN_BOTTOM_MID, 0, -10);
    }

    static void update() {
        PetStats pet = PwnPet::getStats();
        GameStats g  = Gamification::getStats();

        lv_bar_set_value(bar_hunger, pet.hunger, LV_ANIM_ON);
        lv_bar_set_value(bar_happy, pet.happiness, LV_ANIM_ON);

        // Bateria
        String pwr = PwnPower::getPowerStatus();
        lv_label_set_text(label_battery, pwr.c_str());

        // Texto de stats
        String txt = "Lvl " + String(pet.level) +
                     " | XP " + String(g.xp) +
                     " | Hs " + String(pet.handshakes_total);
        lv_label_set_text(label_stats, txt.c_str());

        // Relógio simples baseado em millis (até termos RTC)
        unsigned long t = millis() / 1000;
        int m = (t / 60) % 60;
        int h = (t / 3600) % 24;
        char buf[6];
        snprintf(buf, sizeof(buf), "%02d:%02d", h, m);
        lv_label_set_text(label_clock, buf);

        // Face básica
        if (pet.stage == PWNGOD) {
            FaceHandler::setFace(FACE_COOL);
        } else if (pet.hunger < 20) {
            FaceHandler::setFace(FACE_SAD);
        } else if (pet.happiness > 80) {
            FaceHandler::setFace(FACE_HAPPY);
        } else {
            FaceHandler::setFace(FACE_NEUTRAL);
        }
    }
};

// Definições estáticas declaradas em core_singletons.cpp
#endif
