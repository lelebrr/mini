#ifndef PWN_UI_H
#define PWN_UI_H

#include <lvgl.h>
#include "core/PwnPet.h"
#include "core/PwnPower.h"
#include "FaceHandler.h"
#include "pin_config.h"

// Estilo Cyberpunk BR
static lv_style_t style_base;
static lv_style_t style_bar_bg;
static lv_style_t style_bar_indic;
static lv_style_t style_neon_text;

class PwnUI {
private:
    static lv_obj_t* scr_main;
    static lv_obj_t* bar_hunger;
    static lv_obj_t* bar_happy;
    static lv_obj_t* label_clock;
    static lv_obj_t* label_stats;
    static lv_obj_t* label_battery;
    static lv_obj_t* matrix_bg; // Placeholder para Matrix Rain

public:
    static void initStyles() {
        lv_style_init(&style_base);
        lv_style_set_bg_color(&style_base, lv_color_hex(0x000000));
        lv_style_set_text_color(&style_base, lv_color_hex(0x00FF00)); // Verde Matrix
        lv_style_set_text_font(&style_base, &lv_font_montserrat_14);

        lv_style_init(&style_neon_text);
        lv_style_set_text_color(&style_neon_text, lv_color_hex(0xFF00FF)); // Roxo Neon

        lv_style_init(&style_bar_bg);
        lv_style_set_bg_color(&style_bar_bg, lv_color_hex(0x202020));
        lv_style_set_border_color(&style_bar_bg, lv_color_hex(0x00FF00));
        lv_style_set_border_width(&style_bar_bg, 1);

        lv_style_init(&style_bar_indic);
        lv_style_set_bg_color(&style_bar_indic, lv_color_hex(0x800080)); // Roxo
        lv_style_set_bg_grad_color(&style_bar_indic, lv_color_hex(0xFF00FF), LV_GRAD_DIR_HOR);
    }

    static void init() {
        initStyles();
        scr_main = lv_scr_act();
        lv_obj_add_style(scr_main, &style_base, 0);

        // Matrix Rain (Simulado com texto simples por enquanto)
        matrix_bg = lv_label_create(scr_main);
        lv_label_set_text(matrix_bg, "101010101010101\n010101010101010\n101010101010101");
        lv_obj_set_style_text_color(matrix_bg, lv_color_hex(0x003300), 0); // Verde escuro
        lv_obj_center(matrix_bg);

        // Relógio Grande (Fonte Pixel se possível, usando Montserrat por padrão)
        label_clock = lv_label_create(scr_main);
        lv_obj_set_style_text_font(label_clock, &lv_font_montserrat_20, 0);
        lv_label_set_text(label_clock, "00:00");
        lv_obj_align(label_clock, LV_ALIGN_TOP_MID, 0, 10);

        // Bateria
        label_battery = lv_label_create(scr_main);
        lv_obj_set_style_text_font(label_battery, &lv_font_montserrat_10, 0);
        lv_label_set_text(label_battery, "---");
        lv_obj_align(label_battery, LV_ALIGN_TOP_RIGHT, -5, 5);

        // Barras "Hacker Battery"
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

        // Pet Face
        FaceHandler::init(scr_main);

        // Stats Texto
        label_stats = lv_label_create(scr_main);
        lv_obj_set_style_text_font(label_stats, &lv_font_montserrat_10, 0);
        lv_label_set_text(label_stats, "Iniciando Systema...");
        lv_obj_align(label_stats, LV_ALIGN_BOTTOM_MID, 0, -10);
    }

    static void update() {
        PetStats stats = PwnPet::getStats();

        lv_bar_set_value(bar_hunger, stats.hunger, LV_ANIM_ON);
        lv_bar_set_value(bar_happy, stats.happiness, LV_ANIM_ON);

        // Otimização 30: Pisca vermelho se crítico
        if (stats.hunger < 20) {
             lv_obj_set_style_bg_color(scr_main, lv_color_hex(0x220000), 0);
        } else {
             lv_obj_set_style_bg_color(scr_main, lv_color_hex(0x000000), 0);
        }

        String pwr = PwnPower::getPowerStatus();
        lv_label_set_text(label_battery, pwr.c_str());

        String txt = "Lvl " + String(stats.level) + " | " + String(stats.handshakes_today) + " Pwns";
        if (stats.energy_mode == 0) txt += " [ECO]";
        if (stats.energy_mode == 2) txt += " [ATK]";

        lv_label_set_text(label_stats, txt.c_str());

        // Relógio
        unsigned long t = millis() / 1000; // Placeholder para RTC
        int m = (t / 60) % 60;
        int h = (t / 3600) % 24;
        char buf[10];
        snprintf(buf, 10, "%02d:%02d", h, m);
        lv_label_set_text(label_clock, buf);

        // Face Logic (Expansível)
        if (stats.stage == PWNGOD) FaceHandler::setFace(FACE_COOL);
        else if (stats.hunger < 20) FaceHandler::setFace(FACE_SAD);
        else FaceHandler::setFace(FACE_NEUTRAL);
    }
};

lv_obj_t* PwnUI::scr_main;
lv_obj_t* PwnUI::bar_hunger;
lv_obj_t* PwnUI::bar_happy;
lv_obj_t* PwnUI::label_clock;
lv_obj_t* PwnUI::label_stats;
lv_obj_t* PwnUI::label_battery;
lv_obj_t* PwnUI::matrix_bg;

#endif
