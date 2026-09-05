#ifndef FACE_HANDLER_H
#define FACE_HANDLER_H

/**
 * FaceHandler.h
 * Rostos (personalidade) estilo Pwnagotchi para o Mini Lele.
 *
 * IMPORTANTE (correção de design): a versão anterior usava kaomojis com
 * caracteres Unicode exóticos (⌐ ■ ◕ ‿ …) que NÃO existem na fonte Montserrat
 * do LVGL — apareciam como quadradinhos vazios. Aqui usamos apenas ASCII, que
 * a Montserrat renderiza perfeitamente, num tamanho grande e com cor de acento.
 */

#include <Arduino.h>
#include <lvgl.h>

enum FaceType {
    FACE_HAPPY,
    FACE_SAD,
    FACE_NEUTRAL,
    FACE_LOOK_R,
    FACE_LOOK_L,
    FACE_LOOK_R_HAPPY,
    FACE_LOOK_L_HAPPY,
    FACE_SLEEP,
    FACE_AWAKE,
    FACE_BORED,
    FACE_INTENSE,
    FACE_COOL,
    FACE_EXCITED,
    FACE_GRATEFUL,
    FACE_MOTIVATED,
    FACE_DEMOTIVATED,
    FACE_LONELY,
    FACE_ANGRY,
    FACE_FRIEND,
    FACE_BROKEN,
    FACE_DEBUG,
    FACE_UPLOAD
};

class FaceHandler {
private:
    static lv_obj_t *face_obj;
    static lv_obj_t *parent_screen;
    static FaceType  current;

    // Rostos ASCII (renderizáveis em qualquer fonte).
    static const char *getAscii(FaceType type) {
        switch (type) {
            case FACE_HAPPY:          return "(^_^)";
            case FACE_SAD:            return "(T_T)";
            case FACE_NEUTRAL:        return "(o_o)";
            case FACE_LOOK_R:         return "(o_ )";
            case FACE_LOOK_L:         return "( _o)";
            case FACE_LOOK_R_HAPPY:   return "(^_ )";
            case FACE_LOOK_L_HAPPY:   return "( _^)";
            case FACE_SLEEP:          return "(-_-)";
            case FACE_AWAKE:          return "(O_O)";
            case FACE_BORED:          return "(=_=)";
            case FACE_INTENSE:        return "(>_<)";
            case FACE_COOL:           return "(-_o)";
            case FACE_EXCITED:        return "(*o*)";
            case FACE_GRATEFUL:       return "(^u^)";
            case FACE_MOTIVATED:      return "(oД o)";
            case FACE_DEMOTIVATED:    return "(u_u)";
            case FACE_LONELY:         return "(._.)";
            case FACE_ANGRY:          return "(>_<)";
            case FACE_FRIEND:         return "(^3^)";
            case FACE_BROKEN:         return "(x_x)";
            case FACE_DEBUG:          return "(#_#)";
            case FACE_UPLOAD:         return "(1_0)";
            default:                  return "(-_-)";
        }
    }

    // Cor de acento por humor.
    static lv_color_t getColor(FaceType type) {
        switch (type) {
            case FACE_HAPPY: case FACE_EXCITED: case FACE_GRATEFUL:
            case FACE_FRIEND: case FACE_LOOK_R_HAPPY: case FACE_LOOK_L_HAPPY:
                return lv_color_hex(0x38ff9e);   // verde neon
            case FACE_SAD: case FACE_LONELY: case FACE_DEMOTIVATED:
                return lv_color_hex(0x5aa9ff);   // azul
            case FACE_ANGRY: case FACE_INTENSE: case FACE_BROKEN:
                return lv_color_hex(0xff5a7a);   // vermelho
            case FACE_COOL: case FACE_MOTIVATED:
                return lv_color_hex(0xb26bff);   // roxo
            case FACE_SLEEP: case FACE_BORED:
                return lv_color_hex(0x7a8aa0);   // cinza
            default:
                return lv_color_hex(0xe6f0ff);   // branco suave
        }
    }

public:
    static void init(lv_obj_t *parent) {
        parent_screen = parent;
        face_obj = lv_label_create(parent);
        lv_obj_set_style_text_font(face_obj, &lv_font_montserrat_40, 0);
        lv_obj_set_style_text_color(face_obj, lv_color_hex(0x38ff9e), 0);
        lv_label_set_text(face_obj, "(o_o)");
        lv_obj_center(face_obj);
        current = FACE_NEUTRAL;
    }

    static void setFace(FaceType type) {
        if (!face_obj) return;
        if (type == current) return;   // evita redraw desnecessário
        current = type;
        lv_label_set_text(face_obj, getAscii(type));
        lv_obj_set_style_text_color(face_obj, getColor(type), 0);
    }

    static FaceType getFace() { return current; }
};

// Definições `inline` (C++17): sem "multiple definition" ao incluir em várias TUs.
inline lv_obj_t *FaceHandler::face_obj      = nullptr;
inline lv_obj_t *FaceHandler::parent_screen = nullptr;
inline FaceType  FaceHandler::current       = FACE_NEUTRAL;

#endif
