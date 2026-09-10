#include "FaceHandler.h"
#include "core/PwnPet.h"
#include "AudioHandler.h"
#include <lvgl.h>

// Cores temáticas para a Face
#define COLOR_CYBER_BLUE   lv_color_hex(0x00F0FF)
#define COLOR_CYBER_GREEN  lv_color_hex(0x00FF88)
#define COLOR_CYBER_AMBER  lv_color_hex(0xFFB300)
#define COLOR_CYBER_RED    lv_color_hex(0xFF3366)
#define COLOR_CYBER_PURPLE lv_color_hex(0xBD00FF)
#define COLOR_CYBER_DARK   lv_color_hex(0x080D1A)
#define COLOR_PUPIL_DARK   lv_color_hex(0x04060C)
#define COLOR_BLUSH        lv_color_hex(0xFF3388)
#define COLOR_WHITE        lv_color_hex(0xFFFFFF)

static lv_obj_t *face_box = nullptr;
static lv_obj_t *eye_l = nullptr;
static lv_obj_t *eye_r = nullptr;
static lv_obj_t *pupil_l = nullptr;
static lv_obj_t *pupil_r = nullptr;
static lv_obj_t *highlight_l = nullptr;
static lv_obj_t *highlight_r = nullptr;
static lv_obj_t *eyebrow_l = nullptr;
static lv_obj_t *eyebrow_r = nullptr;
static lv_obj_t *blush_l = nullptr;
static lv_obj_t *blush_r = nullptr;
static lv_obj_t *mouth_lbl = nullptr;
static lv_obj_t *reaction_lbl = nullptr;
static lv_obj_t *sunglasses_bar = nullptr;
static lv_obj_t *laser_scan_bar = nullptr;

// Elementos de Evolução / Skins
static lv_obj_t *antenna_l = nullptr;
static lv_obj_t *antenna_tip_l = nullptr;
static lv_obj_t *antenna_r = nullptr;
static lv_obj_t *antenna_tip_r = nullptr;
static lv_obj_t *reticle_ring = nullptr;
static lv_obj_t *egg_shell = nullptr;
static lv_obj_t *egg_crack1 = nullptr;
static lv_obj_t *egg_crack2 = nullptr;
static lv_obj_t *level_up_banner = nullptr;

static String current_face = FACE_NORMAL;
static int current_skin = 1; // 0: Ovo, 1: Filhote, 2: Jovem, 3: Adulto
static bool face_enabled = true;
static uint32_t last_blink_time = 0;
static uint32_t blink_interval = 4000;
static uint32_t last_saccade_time = 0;

// Callbacks de animação LVGL
static void anim_eye_height_cb(void *var, int32_t v) {
    if (!var) return;
    lv_obj_set_height((lv_obj_t *)var, v);
}

static void anim_pupil_x_cb(void *var, int32_t v) {
    if (!var) return;
    lv_obj_set_x((lv_obj_t *)var, v);
}

static void anim_bounce_y_cb(void *var, int32_t v) {
    if (!var) return;
    lv_obj_set_y((lv_obj_t *)var, v);
}

static void anim_wobble_x_cb(void *var, int32_t v) {
    if (!var) return;
    lv_obj_set_x((lv_obj_t *)var, v);
}

static void anim_antenna_pulse_cb(void *var, int32_t v) {
    if (!var) return;
    lv_obj_set_style_opa((lv_obj_t *)var, (uint8_t)v, 0);
}

static void anim_scan_x_cb(void *var, int32_t v) {
    if (!var) return;
    lv_obj_set_x((lv_obj_t *)var, v);
}

static void anim_reaction_y_cb(void *var, int32_t v) {
    if (!var) return;
    lv_obj_set_y((lv_obj_t *)var, v);
}

static void anim_level_up_cb(void *var, int32_t v) {
    if (!var) return;
    lv_obj_set_y((lv_obj_t *)var, v);
    if (v >= 60) {
        lv_obj_add_flag((lv_obj_t *)var, LV_OBJ_FLAG_HIDDEN);
    }
}

static void on_face_clicked(lv_event_t *e) {
    (void)e;
    // Toque carinhoso no Pet: pisca, adiciona felicidade e toca beep suave
    FaceHandler::wink();
    FaceHandler::triggerBounce();
    PwnPet::feed(1);
    AudioHandler::beep(2400, 30);
    if (reaction_lbl) {
        lv_label_set_text(reaction_lbl, "♥");
        lv_obj_set_style_text_color(reaction_lbl, COLOR_CYBER_RED, 0);
        
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, reaction_lbl);
        lv_anim_set_values(&a, 10, -15);
        lv_anim_set_duration(&a, 600);
        lv_anim_set_exec_cb(&a, anim_reaction_y_cb);
        lv_anim_start(&a);
    }
}

void FaceHandler::init() {
    current_face = FACE_NORMAL;
    current_skin = 0;
    face_enabled = true;
    last_blink_time = millis();
    blink_interval = 3500 + (random(0, 2000));
    Serial.println("[FaceHandler] Inicializado com motor gráfico animado.");
}

lv_obj_t* FaceHandler::createFaceWidget(lv_obj_t *parent, int width, int height) {
    // Container principal da Face
    face_box = lv_obj_create(parent);
    lv_obj_set_size(face_box, width, height);
    lv_obj_set_style_bg_color(face_box, COLOR_CYBER_DARK, 0);
    lv_obj_set_style_bg_opa(face_box, LV_OPA_90, 0);
    lv_obj_set_style_border_color(face_box, COLOR_CYBER_BLUE, 0);
    lv_obj_set_style_border_width(face_box, 2, 0);
    lv_obj_set_style_border_opa(face_box, LV_OPA_60, 0);
    lv_obj_set_style_radius(face_box, 20, 0);
    lv_obj_set_style_pad_all(face_box, 0, 0);
    lv_obj_clear_flag(face_box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(face_box, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(face_box, on_face_clicked, LV_EVENT_CLICKED, NULL);

    int eye_w = 54;
    int eye_h = 58;
    int eye_spacing = 50;

    // Sobrancelha Esquerda
    eyebrow_l = lv_obj_create(face_box);
    lv_obj_set_size(eyebrow_l, 46, 6);
    lv_obj_set_style_bg_color(eyebrow_l, COLOR_CYBER_BLUE, 0);
    lv_obj_set_style_border_width(eyebrow_l, 0, 0);
    lv_obj_set_style_radius(eyebrow_l, 3, 0);
    lv_obj_align(eyebrow_l, LV_ALIGN_CENTER, -eye_spacing, -38);

    // Sobrancelha Direita
    eyebrow_r = lv_obj_create(face_box);
    lv_obj_set_size(eyebrow_r, 46, 6);
    lv_obj_set_style_bg_color(eyebrow_r, COLOR_CYBER_BLUE, 0);
    lv_obj_set_style_border_width(eyebrow_r, 0, 0);
    lv_obj_set_style_radius(eyebrow_r, 3, 0);
    lv_obj_align(eyebrow_r, LV_ALIGN_CENTER, eye_spacing, -38);

    // Olho Esquerdo
    eye_l = lv_obj_create(face_box);
    lv_obj_set_size(eye_l, eye_w, eye_h);
    lv_obj_set_style_bg_color(eye_l, COLOR_CYBER_BLUE, 0);
    lv_obj_set_style_border_width(eye_l, 0, 0);
    lv_obj_set_style_radius(eye_l, 24, 0);
    lv_obj_align(eye_l, LV_ALIGN_CENTER, -eye_spacing, -5);
    lv_obj_clear_flag(eye_l, LV_OBJ_FLAG_SCROLLABLE);

    // Pupila Esquerda
    pupil_l = lv_obj_create(eye_l);
    lv_obj_set_size(pupil_l, 26, 30);
    lv_obj_set_style_bg_color(pupil_l, COLOR_PUPIL_DARK, 0);
    lv_obj_set_style_border_width(pupil_l, 0, 0);
    lv_obj_set_style_radius(pupil_l, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(pupil_l, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(pupil_l, LV_OBJ_FLAG_SCROLLABLE);

    // Brilho na pupila esquerda
    highlight_l = lv_obj_create(pupil_l);
    lv_obj_set_size(highlight_l, 8, 8);
    lv_obj_set_style_bg_color(highlight_l, COLOR_WHITE, 0);
    lv_obj_set_style_border_width(highlight_l, 0, 0);
    lv_obj_set_style_radius(highlight_l, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(highlight_l, LV_ALIGN_TOP_RIGHT, -2, 2);

    // Olho Direito
    eye_r = lv_obj_create(face_box);
    lv_obj_set_size(eye_r, eye_w, eye_h);
    lv_obj_set_style_bg_color(eye_r, COLOR_CYBER_BLUE, 0);
    lv_obj_set_style_border_width(eye_r, 0, 0);
    lv_obj_set_style_radius(eye_r, 24, 0);
    lv_obj_align(eye_r, LV_ALIGN_CENTER, eye_spacing, -5);
    lv_obj_clear_flag(eye_r, LV_OBJ_FLAG_SCROLLABLE);

    // Pupila Direita
    pupil_r = lv_obj_create(eye_r);
    lv_obj_set_size(pupil_r, 26, 30);
    lv_obj_set_style_bg_color(pupil_r, COLOR_PUPIL_DARK, 0);
    lv_obj_set_style_border_width(pupil_r, 0, 0);
    lv_obj_set_style_radius(pupil_r, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(pupil_r, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(pupil_r, LV_OBJ_FLAG_SCROLLABLE);

    // Brilho na pupila direita
    highlight_r = lv_obj_create(pupil_r);
    lv_obj_set_size(highlight_r, 8, 8);
    lv_obj_set_style_bg_color(highlight_r, COLOR_WHITE, 0);
    lv_obj_set_style_border_width(highlight_r, 0, 0);
    lv_obj_set_style_radius(highlight_r, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(highlight_r, LV_ALIGN_TOP_RIGHT, -2, 2);

    // Bochechas rosadas
    blush_l = lv_obj_create(face_box);
    lv_obj_set_size(blush_l, 22, 10);
    lv_obj_set_style_bg_color(blush_l, COLOR_BLUSH, 0);
    lv_obj_set_style_bg_opa(blush_l, LV_OPA_60, 0);
    lv_obj_set_style_border_width(blush_l, 0, 0);
    lv_obj_set_style_radius(blush_l, 5, 0);
    lv_obj_align(blush_l, LV_ALIGN_CENTER, -eye_spacing - 16, 26);
    lv_obj_add_flag(blush_l, LV_OBJ_FLAG_HIDDEN);

    blush_r = lv_obj_create(face_box);
    lv_obj_set_size(blush_r, 22, 10);
    lv_obj_set_style_bg_color(blush_r, COLOR_BLUSH, 0);
    lv_obj_set_style_bg_opa(blush_r, LV_OPA_60, 0);
    lv_obj_set_style_border_width(blush_r, 0, 0);
    lv_obj_set_style_radius(blush_r, 5, 0);
    lv_obj_align(blush_r, LV_ALIGN_CENTER, eye_spacing + 16, 26);
    lv_obj_add_flag(blush_r, LV_OBJ_FLAG_HIDDEN);

    // Boca
    mouth_lbl = lv_label_create(face_box);
    lv_label_set_text(mouth_lbl, "---");
    lv_obj_set_style_text_color(mouth_lbl, COLOR_CYBER_BLUE, 0);
    lv_obj_align(mouth_lbl, LV_ALIGN_CENTER, 0, 36);

    // Reação flutuante (emoji / partículas)
    reaction_lbl = lv_label_create(face_box);
    lv_label_set_text(reaction_lbl, "");
    lv_obj_align(reaction_lbl, LV_ALIGN_TOP_RIGHT, -12, 10);

    // Óculos de sol (Cyberpunk Shades)
    sunglasses_bar = lv_obj_create(face_box);
    lv_obj_set_size(sunglasses_bar, 190, 32);
    lv_obj_set_style_bg_color(sunglasses_bar, lv_color_hex(0x111622), 0);
    lv_obj_set_style_border_color(sunglasses_bar, COLOR_CYBER_PURPLE, 0);
    lv_obj_set_style_border_width(sunglasses_bar, 2, 0);
    lv_obj_set_style_radius(sunglasses_bar, 6, 0);
    lv_obj_align(sunglasses_bar, LV_ALIGN_CENTER, 0, -6);
    lv_obj_add_flag(sunglasses_bar, LV_OBJ_FLAG_HIDDEN);

    // Feixe de laser de varredura
    laser_scan_bar = lv_obj_create(face_box);
    lv_obj_set_size(laser_scan_bar, 4, 66);
    lv_obj_set_style_bg_color(laser_scan_bar, COLOR_CYBER_GREEN, 0);
    lv_obj_set_style_border_width(laser_scan_bar, 0, 0);
    lv_obj_set_style_radius(laser_scan_bar, 2, 0);
    lv_obj_align(laser_scan_bar, LV_ALIGN_CENTER, -80, -5);
    lv_obj_add_flag(laser_scan_bar, LV_OBJ_FLAG_HIDDEN);

    // Antenas Cibernéticas (Estágios 2 e 3)
    antenna_l = lv_obj_create(face_box);
    lv_obj_set_size(antenna_l, 4, 22);
    lv_obj_set_style_bg_color(antenna_l, COLOR_CYBER_BLUE, 0);
    lv_obj_set_style_border_width(antenna_l, 0, 0);
    lv_obj_set_style_radius(antenna_l, 2, 0);
    lv_obj_align(antenna_l, LV_ALIGN_TOP_LEFT, 50, -12);
    lv_obj_add_flag(antenna_l, LV_OBJ_FLAG_HIDDEN);

    antenna_tip_l = lv_obj_create(face_box);
    lv_obj_set_size(antenna_tip_l, 10, 10);
    lv_obj_set_style_bg_color(antenna_tip_l, COLOR_CYBER_AMBER, 0);
    lv_obj_set_style_border_width(antenna_tip_l, 0, 0);
    lv_obj_set_style_radius(antenna_tip_l, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(antenna_tip_l, LV_ALIGN_TOP_LEFT, 47, -18);
    lv_obj_add_flag(antenna_tip_l, LV_OBJ_FLAG_HIDDEN);

    antenna_r = lv_obj_create(face_box);
    lv_obj_set_size(antenna_r, 4, 22);
    lv_obj_set_style_bg_color(antenna_r, COLOR_CYBER_BLUE, 0);
    lv_obj_set_style_border_width(antenna_r, 0, 0);
    lv_obj_set_style_radius(antenna_r, 2, 0);
    lv_obj_align(antenna_r, LV_ALIGN_TOP_RIGHT, -50, -12);
    lv_obj_add_flag(antenna_r, LV_OBJ_FLAG_HIDDEN);

    antenna_tip_r = lv_obj_create(face_box);
    lv_obj_set_size(antenna_tip_r, 10, 10);
    lv_obj_set_style_tip:
    lv_obj_set_style_bg_color(antenna_tip_r, COLOR_CYBER_RED, 0);
    lv_obj_set_style_border_width(antenna_tip_r, 0, 0);
    lv_obj_set_style_radius(antenna_tip_r, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(antenna_tip_r, LV_ALIGN_TOP_RIGHT, -47, -18);
    lv_obj_add_flag(antenna_tip_r, LV_OBJ_FLAG_HIDDEN);

    // Retículo Militar de Mira (Estágio 3 - Adulto)
    reticle_ring = lv_obj_create(face_box);
    lv_obj_set_size(reticle_ring, 68, 68);
    lv_obj_set_style_bg_opa(reticle_ring, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_color(reticle_ring, COLOR_CYBER_RED, 0);
    lv_obj_set_style_border_width(reticle_ring, 2, 0);
    lv_obj_set_style_radius(reticle_ring, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(reticle_ring, LV_ALIGN_CENTER, eye_spacing, -5);
    lv_obj_add_flag(reticle_ring, LV_OBJ_FLAG_HIDDEN);

    // Casca de Ovo Cibernético (Estágio 0 - Ovo)
    egg_shell = lv_obj_create(face_box);
    lv_obj_set_size(egg_shell, 150, 110);
    lv_obj_set_style_bg_color(egg_shell, lv_color_hex(0x182436), 0);
    lv_obj_set_style_border_color(egg_shell, COLOR_CYBER_BLUE, 0);
    lv_obj_set_style_border_width(egg_shell, 2, 0);
    lv_obj_set_style_radius(egg_shell, 50, 0);
    lv_obj_center(egg_shell);
    lv_obj_add_flag(egg_shell, LV_OBJ_FLAG_HIDDEN);

    egg_crack1 = lv_obj_create(egg_shell);
    lv_obj_set_size(egg_crack1, 80, 4);
    lv_obj_set_style_bg_color(egg_crack1, COLOR_CYBER_BLUE, 0);
    lv_obj_set_style_border_width(egg_crack1, 0, 0);
    lv_obj_center(egg_crack1);

    egg_crack2 = lv_label_create(egg_shell);
    lv_label_set_text(egg_crack2, "[ CHOCANDO... ]");
    lv_obj_set_style_text_color(egg_crack2, COLOR_CYBER_BLUE, 0);
    lv_obj_set_style_text_font(egg_crack2, &lv_font_montserrat_12, 0);
    lv_obj_align(egg_crack2, LV_ALIGN_BOTTOM_MID, 0, -8);

    // Banner de Level Up
    level_up_banner = lv_label_create(face_box);
    lv_label_set_text(level_up_banner, "★ LEVEL UP! ★");
    lv_obj_set_style_text_color(level_up_banner, COLOR_CYBER_AMBER, 0);
    lv_obj_set_style_text_font(level_up_banner, &lv_font_montserrat_16, 0);
    lv_obj_set_style_bg_color(level_up_banner, lv_color_hex(0x281B00), 0);
    lv_obj_set_style_border_color(level_up_banner, COLOR_CYBER_AMBER, 0);
    lv_obj_set_style_border_width(level_up_banner, 1, 0);
    lv_obj_set_style_pad_hor(level_up_banner, 14, 0);
    lv_obj_set_style_pad_ver(level_up_banner, 4, 0);
    lv_obj_set_style_radius(level_up_banner, 10, 0);
    lv_obj_align(level_up_banner, LV_ALIGN_TOP_MID, 0, 8);
    lv_obj_add_flag(level_up_banner, LV_OBJ_FLAG_HIDDEN);

    setSkin(current_skin);
    setFace(current_face.c_str());
    return face_box;
}

void FaceHandler::setFace(const char* expression) {
    current_face = expression ? expression : FACE_NORMAL;
    if (!face_box) return;

    // Reset de visibilidade
    if (sunglasses_bar) lv_obj_add_flag(sunglasses_bar, LV_OBJ_FLAG_HIDDEN);
    if (laser_scan_bar) lv_obj_add_flag(laser_scan_bar, LV_OBJ_FLAG_HIDDEN);
    if (blush_l) lv_obj_add_flag(blush_l, LV_OBJ_FLAG_HIDDEN);
    if (blush_r) lv_obj_add_flag(blush_r, LV_OBJ_FLAG_HIDDEN);

    lv_color_t theme_color = COLOR_CYBER_BLUE;

    if (current_face == FACE_HAPPY || current_face == FACE_LOVE) {
        theme_color = (current_face == FACE_LOVE) ? COLOR_BLUSH : COLOR_CYBER_GREEN;
        lv_obj_set_height(eye_l, 50);
        lv_obj_set_height(eye_r, 50);
        if (blush_l) lv_obj_clear_flag(blush_l, LV_OBJ_FLAG_HIDDEN);
        if (blush_r) lv_obj_clear_flag(blush_r, LV_OBJ_FLAG_HIDDEN);
        if (mouth_lbl) lv_label_set_text(mouth_lbl, "\\__/");
        if (reaction_lbl) lv_label_set_text(reaction_lbl, current_face == FACE_LOVE ? "♥" : "^_^");
    } 
    else if (current_face == FACE_EXCITED) {
        theme_color = COLOR_CYBER_AMBER;
        lv_obj_set_height(eye_l, 62);
        lv_obj_set_height(eye_r, 62);
        if (mouth_lbl) lv_label_set_text(mouth_lbl, "[ O ]");
        if (reaction_lbl) lv_label_set_text(reaction_lbl, "★");
        triggerBounce();
    }
    else if (current_face == FACE_SLEEPY) {
        theme_color = lv_color_hex(0x607D8B);
        lv_obj_set_height(eye_l, 8);
        lv_obj_set_height(eye_r, 8);
        if (mouth_lbl) lv_label_set_text(mouth_lbl, "---");
        if (reaction_lbl) lv_label_set_text(reaction_lbl, "Zzz");
    }
    else if (current_face == FACE_ANGRY) {
        theme_color = COLOR_CYBER_RED;
        lv_obj_set_height(eye_l, 44);
        lv_obj_set_height(eye_r, 44);
        if (mouth_lbl) lv_label_set_text(mouth_lbl, "/--\\");
        if (reaction_lbl) lv_label_set_text(reaction_lbl, "!");
    }
    else if (current_face == FACE_COOL) {
        theme_color = COLOR_CYBER_PURPLE;
        if (sunglasses_bar) lv_obj_clear_flag(sunglasses_bar, LV_OBJ_FLAG_HIDDEN);
        if (mouth_lbl) lv_label_set_text(mouth_lbl, "---");
        if (reaction_lbl) lv_label_set_text(reaction_lbl, "B)");
    }
    else if (current_face == FACE_SCANNING) {
        theme_color = COLOR_CYBER_GREEN;
        if (laser_scan_bar) {
            lv_obj_clear_flag(laser_scan_bar, LV_OBJ_FLAG_HIDDEN);
            lv_anim_t scan_anim;
            lv_anim_init(&scan_anim);
            lv_anim_set_var(&scan_anim, laser_scan_bar);
            lv_anim_set_values(&scan_anim, -80, 80);
            lv_anim_set_duration(&scan_anim, 800);
            lv_anim_set_playback_duration(&scan_anim, 800);
            lv_anim_set_repeat_count(&scan_anim, LV_ANIM_REPEAT_INFINITE);
            lv_anim_set_exec_cb(&scan_anim, anim_scan_x_cb);
            lv_anim_start(&scan_anim);
        }
        if (mouth_lbl) lv_label_set_text(mouth_lbl, "====");
        if (reaction_lbl) lv_label_set_text(reaction_lbl, "SCAN");
    }
    else if (current_face == FACE_HUNGRY) {
        theme_color = COLOR_CYBER_AMBER;
        lv_obj_set_height(eye_l, 46);
        lv_obj_set_height(eye_r, 46);
        if (mouth_lbl) lv_label_set_text(mouth_lbl, "( o )");
        if (reaction_lbl) lv_label_set_text(reaction_lbl, "FOOM!");
    }
    else { // NORMAL
        theme_color = COLOR_CYBER_BLUE;
        lv_obj_set_height(eye_l, 58);
        lv_obj_set_height(eye_r, 58);
        if (mouth_lbl) lv_label_set_text(mouth_lbl, "---");
        if (reaction_lbl) lv_label_set_text(reaction_lbl, "");
    }

    setColor(theme_color);
}

const char* FaceHandler::getFace() { 
    return current_face.c_str(); 
}

const char* FaceHandler::getFaceAscii() {
    if (current_face == FACE_HAPPY) return "( ^ _ ^ )";
    if (current_face == FACE_LOVE) return "( ♥ _ ♥ )";
    if (current_face == FACE_EXCITED) return "( ★ _ ★ )";
    if (current_face == FACE_SLEEPY) return "( - _ - ) zZ";
    if (current_face == FACE_ANGRY) return "( > _ < )";
    if (current_face == FACE_COOL) return "(⌐■ _ ■)";
    if (current_face == FACE_SCANNING) return "[ o _ o ]";
    if (current_face == FACE_HUNGRY) return "( o _ o )";
    return "( • _ • )";
}

void FaceHandler::setSkin(int stage) {
    current_skin = stage;
}

void FaceHandler::setEnabled(bool en) { 
    face_enabled = en; 
}

void FaceHandler::setAnimationSpeed(int speed) {
    (void)speed;
}

void FaceHandler::setColor(lv_color_t color) {
    if (!face_box) return;
    if (eye_l) lv_obj_set_style_bg_color(eye_l, color, 0);
    if (eye_r) lv_obj_set_style_bg_color(eye_r, color, 0);
    if (eyebrow_l) lv_obj_set_style_bg_color(eyebrow_l, color, 0);
    if (eyebrow_r) lv_obj_set_style_bg_color(eyebrow_r, color, 0);
    if (mouth_lbl) lv_obj_set_style_text_color(mouth_lbl, color, 0);
    if (face_box) lv_obj_set_style_border_color(face_box, color, 0);
}

void FaceHandler::setMood(const char* mood) {
    setFace(mood);
    PwnPet::setMood(mood);
}

void FaceHandler::blink() {
    if (!face_box || !eye_l || !eye_r || current_face == FACE_SLEEPY || current_face == FACE_COOL) return;

    int normal_h = (current_face == FACE_EXCITED) ? 62 : 54;
    lv_anim_t al, ar;
    lv_anim_init(&al);
    lv_anim_set_var(&al, eye_l);
    lv_anim_set_values(&al, normal_h, 4);
    lv_anim_set_duration(&al, 110);
    lv_anim_set_playback_duration(&al, 110);
    lv_anim_set_exec_cb(&al, anim_eye_height_cb);
    lv_anim_start(&al);

    lv_anim_init(&ar);
    lv_anim_set_var(&ar, eye_r);
    lv_anim_set_values(&ar, normal_h, 4);
    lv_anim_set_duration(&ar, 110);
    lv_anim_set_playback_duration(&ar, 110);
    lv_anim_set_exec_cb(&ar, anim_eye_height_cb);
    lv_anim_start(&ar);
}

void FaceHandler::wink() {
    if (!face_box || !eye_r) return;
    int normal_h = (current_face == FACE_EXCITED) ? 62 : 54;
    lv_anim_t ar;
    lv_anim_init(&ar);
    lv_anim_set_var(&ar, eye_r);
    lv_anim_set_values(&ar, normal_h, 4);
    lv_anim_set_duration(&ar, 150);
    lv_anim_set_playback_duration(&ar, 150);
    lv_anim_set_exec_cb(&ar, anim_eye_height_cb);
    lv_anim_start(&ar);
}

void FaceHandler::triggerBounce() {
    if (!face_box) return;
    int orig_y = lv_obj_get_y(face_box);
    lv_anim_t ab;
    lv_anim_init(&ab);
    lv_anim_set_var(&ab, face_box);
    lv_anim_set_values(&ab, orig_y, orig_y - 12);
    lv_anim_set_duration(&ab, 180);
    lv_anim_set_playback_duration(&ab, 180);
    lv_anim_set_exec_cb(&ab, anim_bounce_y_cb);
    lv_anim_start(&ab);
}

void FaceHandler::cycleExpression() {
    const char* faces[] = {
        FACE_NORMAL, FACE_HAPPY, FACE_COOL, FACE_SCANNING,
        FACE_EXCITED, FACE_HUNGRY, FACE_LOVE, FACE_SLEEPY
    };
    int total = sizeof(faces) / sizeof(faces[0]);
    static int idx = 0;
    idx = (idx + 1) % total;
    setFace(faces[idx]);
    AudioHandler::beep(1800 + idx * 80, 25);
}

void FaceHandler::tick() {
    if (!face_enabled || !face_box) return;
    uint32_t now = millis();

    // Piscar natural a cada intervalo aleatório
    if (now - last_blink_time > blink_interval) {
        last_blink_time = now;
        blink_interval = 3000 + random(0, 3000);
        blink();
    }

    // Saccade suave (olhar pros lados)
    if (now - last_saccade_time > 6000 && pupil_l && pupil_r && current_face != FACE_SLEEPY) {
        last_saccade_time = now;
        int target_x = (random(0, 3) - 1) * 8; // -8 (esquerda), 0 (centro), +8 (direita)

        lv_anim_t sl, sr;
        lv_anim_init(&sl);
        lv_anim_set_var(&sl, pupil_l);
        lv_anim_set_values(&sl, lv_obj_get_x(pupil_l), target_x);
        lv_anim_set_duration(&sl, 250);
        lv_anim_set_exec_cb(&sl, anim_pupil_x_cb);
        lv_anim_start(&sl);

        lv_anim_init(&sr);
        lv_anim_set_var(&sr, pupil_r);
        lv_anim_set_values(&sr, lv_obj_get_x(pupil_r), target_x);
        lv_anim_set_duration(&sr, 250);
        lv_anim_set_exec_cb(&sr, anim_pupil_x_cb);
        lv_anim_start(&sr);
    }
}

