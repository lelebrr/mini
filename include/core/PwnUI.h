#ifndef PWN_UI_H
#define PWN_UI_H

#include <lvgl.h>
#include "core/PwnPet.h"
#include "FaceHandler.h"

class PwnUI {
private:
    static lv_obj_t* scr_main;
    static lv_obj_t* bar_hunger;
    static lv_obj_t* bar_happy;
    static lv_obj_t* label_clock;
    static lv_obj_t* label_stats;
    static lv_obj_t* img_pet; // Placeholder para Sprite

public:
    static void init() {
        scr_main = lv_scr_act();

        // Estilo Dark Hacker
        lv_obj_set_style_bg_color(scr_main, lv_color_hex(0x000000), 0);

        // Relógio Grande
        label_clock = lv_label_create(scr_main);
        lv_obj_set_style_text_font(label_clock, &lv_font_montserrat_20, 0); // Ideal seria maior
        lv_label_set_text(label_clock, "00:00");
        lv_obj_align(label_clock, LV_ALIGN_TOP_MID, 0, 10);

        // Barras de Status
        bar_hunger = lv_bar_create(scr_main);
        lv_obj_set_size(bar_hunger, 100, 10);
        lv_obj_align(bar_hunger, LV_ALIGN_TOP_LEFT, 10, 40);
        lv_bar_set_range(bar_hunger, 0, 100);
        lv_bar_set_value(bar_hunger, 100, LV_ANIM_ON);

        bar_happy = lv_bar_create(scr_main);
        lv_obj_set_size(bar_happy, 100, 10);
        lv_obj_align(bar_happy, LV_ALIGN_TOP_RIGHT, -10, 40);
        lv_bar_set_range(bar_happy, 0, 100);

        // Pet (Face ASCII por enquanto, Sprite se tivermos img)
        FaceHandler::init(scr_main); // Usa o handler existente

        // Stats Texto
        label_stats = lv_label_create(scr_main);
        lv_label_set_text(label_stats, "Lvl 1 | 0 Redes");
        lv_obj_align(label_stats, LV_ALIGN_BOTTOM_MID, 0, -20);
    }

    static void update() {
        PetStats stats = PwnPet::getStats();

        // Atualiza Barras
        lv_bar_set_value(bar_hunger, stats.hunger, LV_ANIM_ON);
        lv_bar_set_value(bar_happy, stats.happiness, LV_ANIM_ON);

        // Atualiza Texto
        String txt = "Lvl " + String(stats.level) + " | " + String(stats.handshakes_today) + " Pwns";
        lv_label_set_text(label_stats, txt.c_str());

        // Atualiza Relógio (Simulado via millis se não tiver RTC sync)
        unsigned long t = millis() / 1000;
        int m = (t / 60) % 60;
        int h = (t / 3600) % 24;
        char buf[6];
        snprintf(buf, 6, "%02d:%02d", h, m);
        lv_label_set_text(label_clock, buf);

        // Atualiza Face baseado no Humor
        if (stats.hunger < 20) FaceHandler::setFace(FACE_SAD);
        else if (stats.happiness > 80) FaceHandler::setFace(FACE_HAPPY);
        else FaceHandler::setFace(FACE_NEUTRAL);
    }
};

lv_obj_t* PwnUI::scr_main;
lv_obj_t* PwnUI::bar_hunger;
lv_obj_t* PwnUI::bar_happy;
lv_obj_t* PwnUI::label_clock;
lv_obj_t* PwnUI::label_stats;
lv_obj_t* PwnUI::img_pet;

#endif
