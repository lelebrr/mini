#ifndef FACE_HANDLER_H
#define FACE_HANDLER_H

/**
 * FaceHandler.h
 * Gerenciador de "Faces" (Personalidade) estilo Pwnagotchi.
 *
 * Funcionalidades:
 * - Exibe rostos ASCII baseados no estado do sistema.
 * - Suporta carregamento de imagens PNG (implementação futura/comentada).
 * - Integração com LVGL para exibição na UI.
 */

#include <Arduino.h>
#include <lvgl.h>
#include <SD_MMC.h>
#include "FS.h"

// Enumeração das faces padrão
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
    static lv_obj_t* face_obj; // Objeto LVGL (Label ou Imagem)
    static bool is_image;
    static lv_obj_t* parent_screen;

    // Mapeamento ASCII para cada face
    static const char* getAscii(FaceType type) {
        switch(type) {
            case FACE_HAPPY: return "(^‿^)";
            case FACE_SAD: return "(;_;)";
            case FACE_NEUTRAL: return "(O_O)";
            case FACE_LOOK_R: return "( ⚆_⚆)";
            case FACE_LOOK_L: return "(☉_⚆)";
            case FACE_LOOK_R_HAPPY: return "( ◕‿◕)";
            case FACE_LOOK_L_HAPPY: return "(◕‿◕ )";
            case FACE_SLEEP: return "(⇀‿‿↼)";
            case FACE_AWAKE: return "(◕‿‿◕)";
            case FACE_BORED: return "(-__-)";
            case FACE_INTENSE: return "(°▃▃°)";
            case FACE_COOL: return "(⌐■_■)";
            case FACE_EXCITED: return "(ᵔ◡◡ᵔ)";
            case FACE_GRATEFUL: return "(^▿▿^)";
            case FACE_MOTIVATED: return "(☼▿▿☼)";
            case FACE_DEMOTIVATED: return "(≖__≖)";
            case FACE_LONELY: return "(ب__ب)";
            case FACE_ANGRY: return "(-_-')";
            case FACE_FRIEND: return "(♥▿▿♥)";
            case FACE_BROKEN: return "(X_X)";
            case FACE_DEBUG: return "(#_#)";
            case FACE_UPLOAD: return "(1__0)";
            default: return "(?)";
        }
    }

    // Caminhos para arquivos de imagem (se existirem)
    static const char* getFilename(FaceType type) {
        switch(type) {
            case FACE_HAPPY: return "/custom-faces/HAPPY.png";
            case FACE_SAD: return "/custom-faces/SAD.png";
            // ... Mapear outros conforme necessário
            default: return NULL;
        }
    }

public:
    /**
     * Inicializa o componente de Face na tela.
     */
    static void init(lv_obj_t* parent) {
        parent_screen = parent;
        face_obj = lv_label_create(parent);
        lv_label_set_text(face_obj, "(O_O)"); // Padrão
        // Estilo: Fonte padrão grande (se disponível) ou padrão
        // Nota: Idealmente usar uma fonte customizada maior
        lv_obj_set_style_text_font(face_obj, &lv_font_montserrat_20, 0);
        lv_obj_align(face_obj, LV_ALIGN_CENTER, 0, -50);
        is_image = false;
    }

    /**
     * Define a face atual.
     * Tenta carregar imagem, se falhar usa ASCII.
     */
    static void setFace(FaceType type) {
        // Implementação ASCII por robustez
        if (is_image) {
            lv_obj_del(face_obj);
            face_obj = lv_label_create(parent_screen);
            lv_obj_align(face_obj, LV_ALIGN_CENTER, 0, -50);
            is_image = false;
        }

        const char* ascii = getAscii(type);
        lv_label_set_text(face_obj, ascii);
    }
};

// Inicialização estática
lv_obj_t* FaceHandler::face_obj = NULL;
bool FaceHandler::is_image = false;
lv_obj_t* FaceHandler::parent_screen = NULL;

#endif
