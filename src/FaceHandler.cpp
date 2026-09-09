#include "FaceHandler.h"
#include "core/PwnPet.h"
#include <lvgl.h>

static lv_obj_t *face_label = nullptr;
static String current_face = "normal";
static int current_skin = 0;
static bool face_enabled = true;

void FaceHandler::init() {
    current_face = "normal";
    current_skin = 0;
    face_enabled = true;
    Serial.println("[FaceHandler] Inicializado");
}

void FaceHandler::setFace(const char* expression) {
    current_face = expression;
    if (face_label) lv_label_set_text(face_label, expression);
}

const char* FaceHandler::getFace() { return current_face.c_str(); }

void FaceHandler::setSkin(int stage) {
    current_skin = stage;
}

void FaceHandler::setEnabled(bool en) { face_enabled = en; }

void FaceHandler::setAnimationSpeed(int speed) {
    // Velocidade da animacao
}

void FaceHandler::setColor(lv_color_t color) {
    if (face_label) lv_obj_set_style_text_color(face_label, color, 0);
}

void FaceHandler::setMood(const char* mood) {
    setFace(mood);
    PwnPet::setMood(mood);
}
