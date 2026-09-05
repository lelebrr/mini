#ifndef FACE_HANDLER_H
#define FACE_HANDLER_H

/**
 * FaceHandler.h — Bicho procedural animado (Mini Lele)
 * ----------------------------------------------------
 * Em vez de rostos ASCII estáticos, desenha um "pet" vivo com olhos e boca
 * vetoriais (primitivas do LVGL) e um motor de animação que combina:
 *   • piscadas em intervalos aleatórios
 *   • olhar para os lados (saccades) com alvos aleatórios
 *   • "respiração"/balanço vertical suave (seno)
 *   • expressão (emoção) que muda a forma dos olhos, a curva da boca e a cor
 *
 * Essas variáveis se combinam de forma praticamente infinita — o bicho nunca
 * fica idêntico dois instantes seguidos, parecendo realmente vivo.
 *
 * LEVE: um lv_timer (~30 FPS) move apenas 5 objetos pequenos; o LVGL redesenha
 * só as regiões que mudaram (partial redraw). Custo de RAM: alguns KB.
 *
 * API mantida: init(parent), setFace(FaceType), getFace() — nada mais no projeto
 * precisa mudar.
 */

#include <Arduino.h>
#include <lvgl.h>
#include <math.h>
#include "esp_random.h"

enum FaceType {
    FACE_HAPPY, FACE_SAD, FACE_NEUTRAL, FACE_LOOK_R, FACE_LOOK_L,
    FACE_LOOK_R_HAPPY, FACE_LOOK_L_HAPPY, FACE_SLEEP, FACE_AWAKE, FACE_BORED,
    FACE_INTENSE, FACE_COOL, FACE_EXCITED, FACE_GRATEFUL, FACE_MOTIVATED,
    FACE_DEMOTIVATED, FACE_LONELY, FACE_ANGRY, FACE_FRIEND, FACE_BROKEN,
    FACE_DEBUG, FACE_UPLOAD
};

class FaceHandler {
private:
    // Objetos LVGL do rosto
    static lv_obj_t *parent;
    static lv_obj_t *eyeL;
    static lv_obj_t *eyeR;
    static lv_obj_t *mouth;    // lv_line (curva)
    static lv_obj_t *mouthO;   // círculo "boca aberta" (surpresa/animado)
    static lv_obj_t *shades;   // óculos escuros (modo COOL / evoluído)
    static lv_timer_t *timer;
    static lv_point_precise_t mpts[3];

    // Geometria base (calculada no init a partir do tamanho do painel)
    static int cx, cy, eyeSp, eyeYoff, eyeWmax, eyeHmax, mouthW, mouthYoff;

    // Estado atual (animado) e alvo (definido pela emoção)
    static float aEyeOpen, tEyeOpen;      // 0..100
    static float aMouth,   tMouth;        // -100 (bravo/triste) .. +100 (sorriso)
    static float aMOpen,   tMOpen;        // 0..100 boca aberta
    static float gazeX, gazeY, gazeTX, gazeTY;
    static float bobAmp, bobPer;          // amplitude/período do balanço
    static lv_color_t curCol, tgtCol;
    static bool  wantShades;
    static FaceType current;
    static bool  enabled;

    // "Espécie" — o formato do bicho muda conforme o estágio de evolução.
    static float skWf, skHf, skSpf, skMwf;   // fatores de largura/altura/espaço/boca
    static bool  oneEye;                       // EGG = ciclope (1 olho grande)
    static int   curSkin;

    static float ez(float a, float b, float k) { return a + (b - a) * k; }
    static int   ri(int max) { return (int)(esp_random() % (max > 0 ? max : 1)); }

    static lv_color_t lerpCol(lv_color_t a, lv_color_t b, float t) {
        // Mistura simples canal a canal (RGB565 -> componentes de 8 bits).
        uint8_t ar = a.red,  ag = a.green,  ab = a.blue;
        uint8_t br = b.red,  bg = b.green,  bb = b.blue;
        lv_color_t r;
        r.red   = (uint8_t)(ar + (br - ar) * t);
        r.green = (uint8_t)(ag + (bg - ag) * t);
        r.blue  = (uint8_t)(ab + (bb - ab) * t);
        return r;
    }

    static lv_obj_t *mkBlob(lv_color_t col) {
        lv_obj_t *o = lv_obj_create(parent);
        lv_obj_remove_style_all(o);
        lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(o, col, 0);
        lv_obj_set_style_radius(o, LV_RADIUS_CIRCLE, 0);
        lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
        return o;
    }

public:
    static void setEnabled(bool e) {           // pausar (ex.: tela apagada)
        enabled = e;
        if (timer) lv_timer_set_period(timer, e ? 33 : 400);
    }

    static void init(lv_obj_t *p) {
        parent = p;
        lv_obj_update_layout(parent);
        int W = lv_obj_get_content_width(parent);
        int H = lv_obj_get_content_height(parent);
        if (W < 40) W = 220;
        if (H < 40) H = 104;

        cx = W / 2;  cy = H / 2;
        eyeSp   = (int)(W * 0.20f);
        eyeYoff = (int)(H * 0.10f);
        eyeWmax = (int)(W * 0.16f);
        eyeHmax = (int)(H * 0.34f);
        mouthW  = (int)(W * 0.34f);
        mouthYoff = (int)(H * 0.24f);

        lv_color_t base = lv_color_hex(0x38ff9e);
        curCol = tgtCol = base;

        eyeL = mkBlob(base);
        eyeR = mkBlob(base);

        mouthO = mkBlob(base);
        lv_obj_add_flag(mouthO, LV_OBJ_FLAG_HIDDEN);

        mouth = lv_line_create(parent);
        lv_obj_remove_style_all(mouth);
        lv_obj_set_style_line_width(mouth, LV_MAX(3, eyeWmax / 5), 0);
        lv_obj_set_style_line_color(mouth, base, 0);
        lv_obj_set_style_line_rounded(mouth, true, 0);

        shades = lv_obj_create(parent);
        lv_obj_remove_style_all(shades);
        lv_obj_set_style_bg_opa(shades, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(shades, lv_color_hex(0x0a0e14), 0);
        lv_obj_set_style_radius(shades, 6, 0);
        lv_obj_set_style_border_width(shades, 2, 0);
        lv_obj_set_style_border_color(shades, lv_color_hex(0xb26bff), 0);
        lv_obj_add_flag(shades, LV_OBJ_FLAG_HIDDEN);

        // Estado inicial
        aEyeOpen = tEyeOpen = 100;
        aMouth = tMouth = 12;
        aMOpen = tMOpen = 0;
        gazeX = gazeY = gazeTX = gazeTY = 0;
        bobAmp = LV_MAX(2, H * 0.03f); bobPer = 2600;
        wantShades = false;
        current = FACE_NEUTRAL;
        enabled = true;
        skWf = skHf = skSpf = skMwf = 1.0f; oneEye = false; curSkin = 2; // KID por padrão

        timer = lv_timer_create(tickCb, 33, nullptr);   // ~30 FPS
        render();
    }

    // Define a EMOÇÃO alvo (o motor continua com os micro-movimentos por cima).
    static void setFace(FaceType type) {
        current = type;
        wantShades = false;
        lv_color_t green = lv_color_hex(0x38ff9e), blue = lv_color_hex(0x5aa9ff),
                   red = lv_color_hex(0xff5a7a), purple = lv_color_hex(0xb26bff),
                   grey = lv_color_hex(0x7a8aa0), white = lv_color_hex(0xe6f0ff),
                   orange = lv_color_hex(0xffa53b);
        switch (type) {
            case FACE_HAPPY: case FACE_GRATEFUL: case FACE_FRIEND:
                tEyeOpen = 42; tMouth = 90; tMOpen = 0; tgtCol = green; bobPer = 2200; break;
            case FACE_EXCITED: case FACE_AWAKE:
                tEyeOpen = 100; tMouth = 60; tMOpen = 80; tgtCol = green; bobPer = 1300; break;
            case FACE_MOTIVATED:
                tEyeOpen = 90; tMouth = 55; tMOpen = 0; tgtCol = purple; bobPer = 1600; break;
            case FACE_SAD: case FACE_DEMOTIVATED: case FACE_LONELY:
                tEyeOpen = 60; tMouth = -70; tMOpen = 0; tgtCol = blue; bobPer = 3600; break;
            case FACE_ANGRY: case FACE_INTENSE:
                tEyeOpen = 45; tMouth = -60; tMOpen = 0; tgtCol = red; bobPer = 1500; break;
            case FACE_BROKEN: case FACE_DEBUG:
                tEyeOpen = 20; tMouth = -20; tMOpen = 30; tgtCol = red; bobPer = 2800; break;
            case FACE_SLEEP: case FACE_BORED:
                tEyeOpen = 10; tMouth = -8; tMOpen = 0; tgtCol = grey; bobPer = 4200; break;
            case FACE_COOL:
                tEyeOpen = 80; tMouth = 40; tMOpen = 0; tgtCol = purple; wantShades = true; bobPer = 2000; break;
            case FACE_UPLOAD:
                tEyeOpen = 100; tMouth = 20; tMOpen = 60; tgtCol = orange; bobPer = 900; break;
            case FACE_LOOK_R: case FACE_LOOK_R_HAPPY:
                tEyeOpen = (type==FACE_LOOK_R_HAPPY)?45:90; tMouth = (type==FACE_LOOK_R_HAPPY)?70:10;
                tMOpen = 0; tgtCol = white; gazeTX = eyeSp*0.5f; break;
            case FACE_LOOK_L: case FACE_LOOK_L_HAPPY:
                tEyeOpen = (type==FACE_LOOK_L_HAPPY)?45:90; tMouth = (type==FACE_LOOK_L_HAPPY)?70:10;
                tMOpen = 0; tgtCol = white; gazeTX = -eyeSp*0.5f; break;
            case FACE_NEUTRAL: default:
                tEyeOpen = 100; tMouth = 12; tMOpen = 0; tgtCol = white; bobPer = 2600; break;
        }
    }

    static FaceType getFace() { return current; }

    // Define a "espécie"/aparência pelo estágio de evolução (0=EGG..6=PWNGOD).
    // O bicho muda de forma conforme cresce, reforçando a sensação de vários pets.
    static void setSkin(uint8_t stage) {
        if (stage == curSkin) return;
        curSkin = stage;
        oneEye = false;
        switch (stage) {
            case 0: /*EGG   */ skWf = 1.9f; skHf = 1.9f; skSpf = 0;    skMwf = 0.55f; oneEye = true;  break; // ciclope fofo
            case 1: /*BABY  */ skWf = 1.5f; skHf = 1.5f; skSpf = 0.85f;skMwf = 0.75f; break;  // olhões redondos
            case 2: /*KID   */ skWf = 1.1f; skHf = 1.15f;skSpf = 1.0f; skMwf = 1.0f;  break;  // padrão
            case 3: /*TEEN  */ skWf = 1.0f; skHf = 1.0f; skSpf = 1.05f;skMwf = 1.05f; break;
            case 4: /*ADULT */ skWf = 1.15f;skHf = 0.72f;skSpf = 1.1f; skMwf = 1.2f;  break;  // olhos amendoados
            case 5: /*PWNLORD*/skWf = 1.2f; skHf = 0.6f; skSpf = 1.15f;skMwf = 1.25f; break;  // olhar afiado
            case 6: /*PWNGOD*/ skWf = 1.1f; skHf = 0.65f;skSpf = 1.1f; skMwf = 1.15f; wantShades = true; break;
            default:          skWf = 1.0f; skHf = 1.0f; skSpf = 1.0f; skMwf = 1.0f;  break;
        }
    }

private:
    static void tickCb(lv_timer_t *) {
        uint32_t now = millis();

        // Suaviza estado -> alvo
        aEyeOpen = ez(aEyeOpen, tEyeOpen, 0.18f);
        aMouth   = ez(aMouth,   tMouth,   0.12f);
        aMOpen   = ez(aMOpen,   tMOpen,   0.15f);
        curCol   = lerpCol(curCol, tgtCol, 0.12f);

        // Piscar (janela triangular suave)
        static uint32_t nextBlink = 0, blinkEnd = 0;
        if (now > nextBlink) { blinkEnd = now + 130; nextBlink = now + 2200 + ri(4000); }
        float blinkF = 1.0f;
        if (now < blinkEnd) { float t = (blinkEnd - now) / 130.0f; blinkF = fabsf(t - 0.5f) * 2.0f; }

        // Olhar em volta (saccade)
        static uint32_t nextSac = 0;
        if (now > nextSac) {
            nextSac = now + 900 + ri(2600);
            if (ri(4) == 0) { gazeTX = 0; gazeTY = 0; }        // volta ao centro
            else { gazeTX = (ri(2) ? 1 : -1) * ri(eyeSp / 2 + 1);
                   gazeTY = (ri(2) ? 1 : -1) * ri(eyeYoff + 1); }
        }
        gazeX = ez(gazeX, gazeTX, 0.12f);
        gazeY = ez(gazeY, gazeTY, 0.12f);

        // Respiração / balanço
        float bob = sinf((now % (uint32_t)bobPer) / bobPer * 6.2831853f) * bobAmp;

        render(blinkF, bob);
    }

    static void render() { render(1.0f, 0.0f); }

    static void render(float blinkF, float bob) {
        if (!eyeL) return;
        int ew  = (int)(eyeWmax * skWf);
        int eh  = LV_MAX(3, (int)(eyeHmax * skHf * (aEyeOpen / 100.0f) * blinkF));
        int esp = (int)(eyeSp * skSpf);
        int mW  = (int)(mouthW * skMwf);
        int by  = cy + (int)bob;
        int ey  = by - eyeYoff - eh / 2 + (int)gazeY;

        lv_obj_set_size(eyeL, ew, eh);
        lv_obj_set_style_bg_color(eyeL, curCol, 0);
        if (oneEye) {                                   // EGG: ciclope (1 olho)
            lv_obj_add_flag(eyeR, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_pos(eyeL, cx - ew / 2 + (int)gazeX, ey);
        } else {
            lv_obj_clear_flag(eyeR, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_size(eyeR, ew, eh);
            lv_obj_set_style_bg_color(eyeR, curCol, 0);
            lv_obj_set_pos(eyeL, cx - esp - ew / 2 + (int)gazeX, ey);
            lv_obj_set_pos(eyeR, cx + esp - ew / 2 + (int)gazeX, ey);
        }

        // Óculos escuros (cobre os olhos) no modo COOL/evoluído
        if (wantShades && !oneEye) {
            lv_obj_clear_flag(shades, LV_OBJ_FLAG_HIDDEN);
            int sw = esp * 2 + ew + 12, sh = eh + 8;
            lv_obj_set_size(shades, sw, sh);
            lv_obj_set_pos(shades, cx - sw / 2 + (int)gazeX / 2, by - eyeYoff - sh / 2);
        } else {
            lv_obj_add_flag(shades, LV_OBJ_FLAG_HIDDEN);
        }

        // Boca: aberta (círculo) quando aMOpen alto, senão linha curva
        int my = by + mouthYoff;
        if (aMOpen > 45) {
            lv_obj_add_flag(mouth, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(mouthO, LV_OBJ_FLAG_HIDDEN);
            int d = LV_MAX(8, (int)(mW * 0.5f * (aMOpen / 100.0f)));
            lv_obj_set_size(mouthO, d, d);
            lv_obj_set_pos(mouthO, cx - d / 2, my - d / 2);
            lv_obj_set_style_bg_color(mouthO, curCol, 0);
        } else {
            lv_obj_add_flag(mouthO, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(mouth, LV_OBJ_FLAG_HIDDEN);
            int curve = (int)(aMouth / 100.0f * (mW * 0.28f)); // + sorriso (∪), - bravo (∩)
            mpts[0].x = cx - mW / 2;          mpts[0].y = my;
            mpts[1].x = cx;                   mpts[1].y = my + curve;
            mpts[2].x = cx + mW / 2;          mpts[2].y = my;
            lv_line_set_points(mouth, mpts, 3);
            lv_obj_set_style_line_color(mouth, curCol, 0);
        }
    }
};

// Definições `inline` (C++17) — sem "multiple definition" em várias TUs.
inline lv_obj_t *FaceHandler::parent = nullptr;
inline lv_obj_t *FaceHandler::eyeL = nullptr;
inline lv_obj_t *FaceHandler::eyeR = nullptr;
inline lv_obj_t *FaceHandler::mouth = nullptr;
inline lv_obj_t *FaceHandler::mouthO = nullptr;
inline lv_obj_t *FaceHandler::shades = nullptr;
inline lv_timer_t *FaceHandler::timer = nullptr;
inline lv_point_precise_t FaceHandler::mpts[3] = {};
inline int FaceHandler::cx = 0, FaceHandler::cy = 0, FaceHandler::eyeSp = 0,
          FaceHandler::eyeYoff = 0, FaceHandler::eyeWmax = 0, FaceHandler::eyeHmax = 0,
          FaceHandler::mouthW = 0, FaceHandler::mouthYoff = 0;
inline float FaceHandler::aEyeOpen = 100, FaceHandler::tEyeOpen = 100,
             FaceHandler::aMouth = 12, FaceHandler::tMouth = 12,
             FaceHandler::aMOpen = 0, FaceHandler::tMOpen = 0,
             FaceHandler::gazeX = 0, FaceHandler::gazeY = 0,
             FaceHandler::gazeTX = 0, FaceHandler::gazeTY = 0,
             FaceHandler::bobAmp = 3, FaceHandler::bobPer = 2600;
inline lv_color_t FaceHandler::curCol = {}, FaceHandler::tgtCol = {};
inline bool FaceHandler::wantShades = false;
inline FaceType FaceHandler::current = FACE_NEUTRAL;
inline bool FaceHandler::enabled = true;
inline float FaceHandler::skWf = 1.0f, FaceHandler::skHf = 1.0f,
             FaceHandler::skSpf = 1.0f, FaceHandler::skMwf = 1.0f;
inline bool FaceHandler::oneEye = false;
inline int FaceHandler::curSkin = 2;

#endif
