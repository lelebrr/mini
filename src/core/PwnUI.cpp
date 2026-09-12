#include "core/PwnUI.h"
#include "core/ConfigManager.h"
#include "core/PwnPower.h"
#include "core/PwnPet.h"
#include "core/PwnAttack.h"
#include "core/PwnSleep.h"
#include "drivers/PwnRTC.h"
#include "WiFiTools.h"
#include "BleBlue.h"
#include "BleIds.h"
#include "WpsBlue.h"
#include "EspNowScan.h"
#include "core/PwnBLE.h"
#include "EvilPortal.h"
#include "FaceHandler.h"
#include "AudioHandler.h"
#include <SD_MMC.h>
#include <lvgl.h>

extern bool UI_getAccel(float &x, float &y, float &z);

// Cores temáticas AMOLED Cyberpunk
#define COLOR_AMOLED_BG     lv_color_hex(0x040711)
#define COLOR_CARD_BG       lv_color_hex(0x0C1222)
#define COLOR_CARD_BORDER   lv_color_hex(0x182642)
#define COLOR_CYAN          lv_color_hex(0x00F0FF)
#define COLOR_GREEN         lv_color_hex(0x00FF88)
#define COLOR_AMBER         lv_color_hex(0xFFAA00)
#define COLOR_RED           lv_color_hex(0xFF3355)
#define COLOR_PURPLE        lv_color_hex(0xBD00FF)
#define COLOR_WHITE         lv_color_hex(0xFFFFFF)
#define COLOR_GREY          lv_color_hex(0x687890)
#define COLOR_DARK_GREY     lv_color_hex(0x141B2D)

static UITheme current_ui_theme = THEME_CYBERPUNK;
static int watchface_style = 0; // 0: Cyber, 1: Matrix, 2: Amber, 3: Stealth

static lv_obj_t *screen = nullptr;
static lv_obj_t *tileview = nullptr;
static lv_obj_t *status_bar = nullptr;

// Elementos da Barra de Status
static lv_obj_t *badge_mode = nullptr;
static lv_obj_t *lbl_mode = nullptr;
static lv_obj_t *lbl_wifi = nullptr;
static lv_obj_t *lbl_ble = nullptr;
static lv_obj_t *lbl_sd = nullptr;
static lv_obj_t *lbl_battery = nullptr;
static lv_obj_t *lbl_temp = nullptr;

// Tile 0: Pet Core
static lv_obj_t *lbl_pet_name = nullptr;
static lv_obj_t *lbl_pet_lvl = nullptr;
static lv_obj_t *lbl_pet_stage = nullptr;
static lv_obj_t *lbl_pet_speech = nullptr;
static lv_obj_t *bar_xp = nullptr;
static lv_obj_t *lbl_xp_val = nullptr;
static lv_obj_t *bar_hunger = nullptr;
static lv_obj_t *lbl_hunger_val = nullptr;
static lv_obj_t *bar_happiness = nullptr;
static lv_obj_t *lbl_happiness_val = nullptr;
static lv_obj_t *lbl_shakes_val = nullptr;

// Tile 1: Wi-Fi Radar & Espectro
static lv_obj_t *radar_box = nullptr;
static lv_obj_t *radar_sweep_line = nullptr;
static lv_obj_t *radar_blip1 = nullptr;
static lv_obj_t *radar_blip2 = nullptr;
static lv_obj_t *radar_blip3 = nullptr;
static lv_obj_t *lbl_ap_count_big = nullptr;
static lv_obj_t *lbl_wifi_channels = nullptr;
static lv_obj_t *lbl_wifi_open = nullptr;
static lv_obj_t *lbl_last_ap_ssid = nullptr;
static lv_obj_t *lbl_last_ap_rssi = nullptr;
static lv_obj_t *spec_bars[13] = {nullptr};

// Tile 2: BLE Sonar & Foxhunt
static lv_obj_t *sonar_ring1 = nullptr;
static lv_obj_t *sonar_ring2 = nullptr;
static lv_obj_t *arc_foxhunt = nullptr;
static lv_obj_t *lbl_foxhunt_rssi = nullptr;
static lv_obj_t *lbl_ble_total = nullptr;
static lv_obj_t *lbl_ble_apple = nullptr;
static lv_obj_t *lbl_ble_flipper = nullptr;
static lv_obj_t *lbl_ble_tags = nullptr;

// Tile 3: Arsenal RED
static lv_obj_t *card_arm_status = nullptr;
static lv_obj_t *lbl_arm_title = nullptr;
static lv_obj_t *lbl_arm_subtitle = nullptr;
static lv_obj_t *btn_portal_state = nullptr;
static lv_obj_t *lbl_portal_state = nullptr;
static bool arm_active = false;

// Tile 4: Capturas & PCAP
static lv_obj_t *lbl_handshakes_big = nullptr;
static lv_obj_t *bar_sd_space = nullptr;
static lv_obj_t *lbl_sd_space = nullptr;
static lv_obj_t *lbl_pcap_console = nullptr;

// Tile 5: Hardware HUD & IMU
static lv_obj_t *arc_battery = nullptr;
static lv_obj_t *lbl_arc_battery = nullptr;
static lv_obj_t *arc_temp = nullptr;
static lv_obj_t *lbl_arc_temp = nullptr;
static lv_obj_t *arc_ram = nullptr;
static lv_obj_t *lbl_arc_ram = nullptr;
static lv_obj_t *slider_brightness = nullptr;
static lv_obj_t *lbl_brightness_val = nullptr;
static lv_obj_t *sw_sound = nullptr;
static lv_obj_t *sw_autoscan = nullptr;
static lv_obj_t *horizon_box = nullptr;
static lv_obj_t *horizon_bubble = nullptr;
static lv_obj_t *lbl_imu_angles = nullptr;

// Tile 6: Relógio & Watchface Cyberpunk
static lv_obj_t *lbl_clock_time = nullptr;
static lv_obj_t *lbl_clock_date = nullptr;
static lv_obj_t *arc_clock_sec = nullptr;
static lv_obj_t *lbl_clock_theme_badge = nullptr;
static lv_obj_t *lbl_clock_pet_badge = nullptr;

// Boot Splash & Screensaver
static lv_obj_t *boot_splash_box = nullptr;
static lv_obj_t *screensaver_box = nullptr;
static bool screensaver_active = false;

static int current_tile = 0;
static const int num_tiles = 8;
static uint32_t last_update = 0;

// Indicador de paginas (pontos) na base da tela
static lv_obj_t *dots_row = nullptr;
static lv_obj_t *page_dots[num_tiles] = {nullptr};

// Extras do tile de Configuracoes
static lv_obj_t *lbl_timeout_val = nullptr;
static lv_obj_t *lbl_pmu_info = nullptr;

// Tile 1: Wi-Fi — listas e telemetria real
static lv_obj_t *lbl_wifi_scan_state = nullptr;   // badge SCAN ATIVO/PARADO
static lv_obj_t *lbl_wifi_frames = nullptr;
static lv_obj_t *lbl_wifi_eapol = nullptr;
static lv_obj_t *lbl_wifi_espnow = nullptr;
static lv_obj_t *lbl_ap_rows[4] = {nullptr};      // linhas da lista de APs
static lv_obj_t *lbl_sniff_state = nullptr;       // label do botao sniffer

// Tile 2: BLE — lista de dispositivos
static lv_obj_t *lbl_ble_hid = nullptr;
static lv_obj_t *lbl_ble_rows[3] = {nullptr};

// Tile 3: RED — estados reais dos cartoes
static lv_obj_t *lbl_deauth_state = nullptr;
static lv_obj_t *lbl_espnow_info = nullptr;
static lv_obj_t *lbl_wps_info = nullptr;
static bool deauth_on = false;

// ---- Sistema de tema -------------------------------------------------------
// Cor de destaque atual (accent) e registro dos objetos que a seguem.
// setTheme() repinta todos: antes ele mudava quase nada e o updateStatusBar()
// reaplicava ciano a cada 300 ms, fazendo o tema "voltar" sozinho.
static lv_color_t theme_accent_color = COLOR_CYAN;
static lv_obj_t *theme_objs[96] = {nullptr};
static int theme_n = 0;
static void theme_follow(lv_obj_t *o) {
    if (o && theme_n < 96) theme_objs[theme_n++] = o;
}
// Titulos dos tiles (repintados pelo tema)
static lv_obj_t *lbl_ti_wifi = nullptr;
static lv_obj_t *lbl_ti_ble = nullptr;
static lv_obj_t *lbl_ti_red = nullptr;
static lv_obj_t *lbl_ti_cap = nullptr;
static lv_obj_t *lbl_ti_hw = nullptr;
static lv_obj_t *lbl_ti_cfg = nullptr;

// Menu de energia (Desligar / Reiniciar / Apagar tela)
static lv_obj_t *power_menu = nullptr;

// Toast: apenas um por vez, com auto-delecao (antes eles se acumulavam na
// tela para sempre, cobrindo a UI).
static lv_obj_t *active_toast = nullptr;
static lv_timer_t *toast_timer = nullptr;
static void toast_auto_delete_cb(lv_timer_t *t) {
    (void)t;
    if (active_toast) { lv_obj_del(active_toast); active_toast = nullptr; }
    lv_timer_del(t);
    toast_timer = nullptr;
}

// Alerta modal: fecha ao tocar em OK ou apos 6 s (antes ficava na tela
// para sempre, bloqueando o toque em todo o resto). Feito com container
// simples para nao depender da API do lv_msgbox (muda entre versoes).
static lv_obj_t *active_alert = nullptr;
static lv_timer_t *alert_timer = nullptr;
static void alert_destroy() {
    if (alert_timer) { lv_timer_del(alert_timer); alert_timer = nullptr; }
    if (active_alert && lv_obj_is_valid(active_alert)) lv_obj_del(active_alert);
    active_alert = nullptr;
}
static void alert_auto_close_cb(lv_timer_t *t) {
    (void)t;
    alert_destroy();
}
static void alert_ok_cb(lv_event_t *e) {
    (void)e;
    alert_destroy();
}

// Animação de rotação do radar (Tile 1)
static void anim_radar_sweep_cb(void *var, int32_t v) {
    if (!var) return;
    lv_arc_set_angles((lv_obj_t *)var, v, (v + 35) % 360);
}

// Animação de pulso sonar (Tile 2)
static void anim_sonar_ring_cb(void *var, int32_t v) {
    if (!var) return;
    lv_obj_set_size((lv_obj_t *)var, v, v);
    lv_obj_set_style_opa((lv_obj_t *)var, (uint8_t)constrain(255 - (v * 255 / 140), 20, 255), 0);
}

// Eventos dos botões do Pet
static void btn_pet_feed_cb(lv_event_t *e) {
    (void)e;
    PwnPet::feed(15);
    FaceHandler::setFace(FACE_HAPPY);
    FaceHandler::triggerBounce();
    AudioHandler::beep(2200, 40);
    PwnUI::showToast("Nham nham! Pet alimentado (+15 Fome, +XP)!");
}

static void btn_pet_care_cb(lv_event_t *e) {
    (void)e;
    PwnPet::feed(3);
    FaceHandler::wink();
    FaceHandler::triggerBounce();
    AudioHandler::beep(2600, 35);
    PwnUI::showToast("Carinho dado! Lele esta feliz!");
}

static void btn_pet_face_cb(lv_event_t *e) {
    (void)e;
    FaceHandler::cycleExpression();
}

// Evento do Slider de Brilho
static void slider_brightness_cb(lv_event_t *e) {
    lv_obj_t *slider = (lv_obj_t *)lv_event_get_target(e);
    int val = (int)lv_slider_get_value(slider);
    if (PwnSleep::set_brightness) {
        PwnSleep::set_brightness(val);
        PwnSleep::full_brightness = val;
    }
    if (lbl_brightness_val) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d%%", (val * 100) / 255);
        lv_label_set_text(lbl_brightness_val, buf);
    }
    ConfigManager::getInstance()->set("disp_brightness", (int64_t)val);
}

// Evento do Slider de Timeout da tela (dim) — aplica e persiste na hora
static void slider_timeout_cb(lv_event_t *e) {
    lv_obj_t *slider = (lv_obj_t *)lv_event_get_target(e);
    int val = (int)lv_slider_get_value(slider);
    PwnSleep::setDimTimeout(val);        // salva em /config.json
    PwnSleep::setOffTimeout(val * 2);    // mantem a proporcao dim -> off
    PwnSleep::notifyActivity();          // reinicia o contador
    if (lbl_timeout_val) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d s", val);
        lv_label_set_text(lbl_timeout_val, buf);
    }
}

// Evento do Switch de Sons (liga/desliga o amplificador no ato)
static void sw_sound_cb(lv_event_t *e) {
    lv_obj_t *sw = (lv_obj_t *)lv_event_get_target(e);
    bool on = lv_obj_has_state(sw, LV_STATE_CHECKED);
    AudioHandler::setAmpPower(on);
    ConfigManager::getInstance()->set("sys_sound_amp", on);
    if (on) AudioHandler::beep(2200, 40);
    PwnUI::showToast(on ? "Som ligado" : "Som mudo");
}

// Evento do Switch do Sniffer (inicia/para a captura passiva no ato)
static void sw_sniffer_cb(lv_event_t *e) {
    lv_obj_t *sw = (lv_obj_t *)lv_event_get_target(e);
    bool on = lv_obj_has_state(sw, LV_STATE_CHECKED);
    ConfigManager::getInstance()->set("atk_auto_scan", on);
    if (on) { WiFiTools::beginNewCapture(); WiFiTools::startSnifferPassive(); }
    else { WiFiTools::stopSniffer(); }
    PwnUI::showToast(on ? "Sniffer iniciado" : "Sniffer parado");
}

// ---- Comandos da tela Wi-Fi -------------------------------------------------
static void btn_wifi_rescan_cb(lv_event_t *e) {
    (void)e;
    WiFiTools::beginNewCapture();
    WiFiTools::startSnifferPassive();
    AudioHandler::beep(2400, 40);
    PwnUI::showToast("Captura reiniciada (canal 1)");
}

static void btn_wifi_sniff_cb(lv_event_t *e) {
    (void)e;
    if (WiFiTools::isSniffing()) WiFiTools::stopSniffer();
    else WiFiTools::startSnifferPassive();
    AudioHandler::beep(1800, 40);
}

// ---- Comandos da tela BLE ---------------------------------------------------
static void btn_ble_scan_cb(lv_event_t *e) {
    (void)e;
    AudioHandler::beep(2200, 30);
    int n = PwnBLE::scan(2);   // scan ativo de 2 s (bloqueia brevemente)
    if (n > 0) PwnPet::feed(1);
    char b[40];
    snprintf(b, sizeof(b), "BLE: %d dispositivos vistos", n);
    PwnUI::showToast(b);
}

static void btn_ble_clear_cb(lv_event_t *e) {
    (void)e;
    BleBlue::clear();
    AudioHandler::beep(1200, 40);
    PwnUI::showToast("Inventario BLE limpo");
}

// ---- Comando do cartao Deauth (RED, laboratorio) ----------------------------
static void btn_deauth_toggle_cb(lv_event_t *e) {
    (void)e;
    deauth_on = !deauth_on;
    WiFiTools::enableDeauth(deauth_on);
    if (lbl_deauth_state) {
        lv_label_set_text(lbl_deauth_state, deauth_on ? "ATIVO" : "PARADO");
    }
    AudioHandler::beep(deauth_on ? 900 : 1600, 60);
}

// ---- Menu de energia (Desligar / Reiniciar / Apagar tela) ------------------
static void power_menu_cb(lv_event_t *e) {
    int opt = (int)(intptr_t)lv_event_get_user_data(e);
    PwnUI::hidePowerMenu();
    switch (opt) {
        case 0:   // Desligar (deep sleep)
            AudioHandler::beep(800, 120);
            PwnSleep::enterDeep();
            break;
        case 1:   // Reiniciar
            AudioHandler::beep(2000, 80);
            ESP.restart();
            break;
        case 2:   // Apagar a tela
            AudioHandler::beep(1500, 40);
            PwnSleep::turnScreenOff();
            break;
    }
}

void PwnUI::showPowerMenu() {
    if (power_menu) return;
    power_menu = lv_obj_create(lv_screen_active());
    lv_obj_set_size(power_menu, 300, 220);
    lv_obj_center(power_menu);
    lv_obj_set_style_bg_color(power_menu, lv_color_hex(0x0A0F1D), 0);
    lv_obj_set_style_border_color(power_menu, theme_accent_color, 0);
    lv_obj_set_style_border_width(power_menu, 2, 0);
    lv_obj_set_style_radius(power_menu, 16, 0);
    lv_obj_set_style_pad_all(power_menu, 16, 0);
    lv_obj_clear_flag(power_menu, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *hdr = lv_label_create(power_menu);
    lv_label_set_text(hdr, "ENERGIA");
    lv_obj_set_style_text_color(hdr, theme_accent_color, 0);
    lv_obj_set_style_text_font(hdr, &lv_font_montserrat_16, 0);
    lv_obj_align(hdr, LV_ALIGN_TOP_MID, 0, 0);

    struct { const char *txt; const char *sym; lv_color_t c; } opts[] = {
        {"Desligar",  LV_SYMBOL_POWER,  COLOR_RED},
        {"Reiniciar", LV_SYMBOL_REFRESH, COLOR_AMBER},
        {"Apagar tela", LV_SYMBOL_EYE_CLOSE, COLOR_CYAN},
    };
    for (int i = 0; i < 3; i++) {
        lv_obj_t *b = lv_button_create(power_menu);
        lv_obj_set_size(b, 268, 46);
        lv_obj_align(b, LV_ALIGN_TOP_MID, 0, 36 + i * 56);
        lv_obj_set_style_bg_color(b, COLOR_DARK_GREY, 0);
        lv_obj_set_style_border_color(b, opts[i].c, 0);
        lv_obj_set_style_border_width(b, 1, 0);
        lv_obj_set_style_radius(b, 10, 0);
        lv_obj_add_event_cb(b, power_menu_cb, LV_EVENT_CLICKED, (void*)(intptr_t)i);
        lv_obj_t *l = lv_label_create(b);
        char buf[32];
        snprintf(buf, sizeof(buf), "%s  %s", opts[i].sym, opts[i].txt);
        lv_label_set_text(l, buf);
        lv_obj_set_style_text_color(l, opts[i].c, 0);
        lv_obj_center(l);
    }

    // Fecha o menu se tocar fora (no fundo)
    lv_obj_add_flag(power_menu, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(power_menu, [](lv_event_t *e) {
        (void)e;
        PwnUI::hidePowerMenu();
    }, LV_EVENT_CLICKED, NULL);
}

void PwnUI::hidePowerMenu() {
    if (power_menu) {
        lv_obj_del(power_menu);
        power_menu = nullptr;
    }
}

bool PwnUI::isPowerMenuActive() {
    return power_menu != nullptr;
}

// Evento do Botão ARM
static void btn_arm_toggle_cb(lv_event_t *e) {
    (void)e;
    arm_active = !arm_active;
    if (arm_active) {
        AudioHandler::beep(1200, 80);
        AudioHandler::beep(800, 100);
        PwnUI::showAlert("MODO ARMADO ATIVADO! Cuidado ao emitir ataques.", 0xFF3355);
    } else {
        AudioHandler::beep(2000, 50);
        PwnUI::showToast("Sistema desarmado. Modo seguro BLUE.");
    }
}

// Evento do Evil Portal
static void btn_portal_toggle_cb(lv_event_t *e) {
    (void)e;
    if (!EvilPortal::isActive()) {
        EvilPortal::begin("WiFi_Livre_Conectar");
        if (lbl_portal_state) lv_label_set_text(lbl_portal_state, "ATIVO (PARAR)");
        if (btn_portal_state) lv_obj_set_style_bg_color(btn_portal_state, COLOR_RED, 0);
        AudioHandler::beep(1500, 60);
        PwnUI::showToast("Portal Cativo iniciado: WiFi_Livre_Conectar");
    } else {
        EvilPortal::stop();
        if (lbl_portal_state) lv_label_set_text(lbl_portal_state, "PARADO (LIGAR)");
        if (btn_portal_state) lv_obj_set_style_bg_color(btn_portal_state, COLOR_DARK_GREY, 0);
        AudioHandler::beep(1000, 60);
        PwnUI::showToast("Portal Cativo desligado.");
    }
}

// Criação do Tile 0 (Pet Dashboard)
static void create_tile_pet(lv_obj_t *tile) {
    // Linha superior: Nome + Nível + Estágio
    lv_obj_t *top_row = lv_obj_create(tile);
    lv_obj_set_size(top_row, 340, 42);
    lv_obj_align(top_row, LV_ALIGN_TOP_MID, 0, 8);
    lv_obj_set_style_bg_opa(top_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(top_row, 0, 0);
    lv_obj_set_style_pad_all(top_row, 0, 0);
    lv_obj_set_flex_flow(top_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(top_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lbl_pet_name = lv_label_create(top_row);
    lv_label_set_text(lbl_pet_name, "LELE");
    lv_obj_set_style_text_color(lbl_pet_name, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(lbl_pet_name, &lv_font_montserrat_20, 0);

    // Badge de Nível (container com fundo; bg em lv_label nao e renderizado)
    lv_obj_t *lvl_badge = lv_obj_create(top_row);
    lv_obj_set_size(lvl_badge, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(lvl_badge, lv_color_hex(0x281B00), 0);
    lv_obj_set_style_pad_hor(lvl_badge, 8, 0);
    lv_obj_set_style_pad_ver(lvl_badge, 3, 0);
    lv_obj_set_style_radius(lvl_badge, 10, 0);
    lv_obj_set_style_border_color(lvl_badge, COLOR_AMBER, 0);
    lv_obj_set_style_border_width(lvl_badge, 1, 0);
    lv_obj_clear_flag(lvl_badge, LV_OBJ_FLAG_SCROLLABLE);
    lbl_pet_lvl = lv_label_create(lvl_badge);
    lv_label_set_text(lbl_pet_lvl, "LVL 1");
    lv_obj_set_style_text_color(lbl_pet_lvl, COLOR_AMBER, 0);

    // Badge de Estágio (idem: container com fundo)
    lv_obj_t *stage_badge = lv_obj_create(top_row);
    lv_obj_set_size(stage_badge, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(stage_badge, lv_color_hex(0x1F0B2E), 0);
    lv_obj_set_style_pad_hor(stage_badge, 8, 0);
    lv_obj_set_style_pad_ver(stage_badge, 3, 0);
    lv_obj_set_style_radius(stage_badge, 10, 0);
    lv_obj_set_style_border_color(stage_badge, COLOR_PURPLE, 0);
    lv_obj_set_style_border_width(stage_badge, 1, 0);
    lv_obj_clear_flag(stage_badge, LV_OBJ_FLAG_SCROLLABLE);
    lbl_pet_stage = lv_label_create(stage_badge);
    lv_label_set_text(lbl_pet_stage, "FILHOTE");
    lv_obj_set_style_text_color(lbl_pet_stage, COLOR_PURPLE, 0);

    // Widget da Face Animada do Pet
    lv_obj_t *face_w = FaceHandler::createFaceWidget(tile, 340, 130);
    lv_obj_align(face_w, LV_ALIGN_TOP_MID, 0, 52);

    // Balão de Fala do Pet (container com fundo; bg em lv_label não aparece)
    lv_obj_t *balloon = lv_obj_create(tile);
    lv_obj_set_size(balloon, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(balloon, COLOR_CARD_BG, 0);
    lv_obj_set_style_pad_hor(balloon, 12, 0);
    lv_obj_set_style_pad_ver(balloon, 5, 0);
    lv_obj_set_style_radius(balloon, 8, 0);
    lv_obj_set_style_border_color(balloon, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(balloon, 1, 0);
    lv_obj_clear_flag(balloon, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(balloon, LV_ALIGN_TOP_MID, 0, 190);
    lbl_pet_speech = lv_label_create(balloon);
    lv_label_set_text(lbl_pet_speech, "\"Farejando pacotes no ar...\"");
    lv_obj_set_style_text_color(lbl_pet_speech, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_pet_speech, &lv_font_montserrat_12, 0);

    // Barra de XP
    lv_obj_t *xp_box = lv_obj_create(tile);
    lv_obj_set_size(xp_box, 340, 36);
    lv_obj_align(xp_box, LV_ALIGN_TOP_MID, 0, 226);
    lv_obj_set_style_bg_color(xp_box, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(xp_box, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(xp_box, 1, 0);
    lv_obj_set_style_radius(xp_box, 10, 0);
    lv_obj_set_style_pad_all(xp_box, 6, 0);

    lbl_xp_val = lv_label_create(xp_box);
    lv_label_set_text(lbl_xp_val, "XP: 0 / 100");
    lv_obj_set_style_text_color(lbl_xp_val, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(lbl_xp_val, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_xp_val, LV_ALIGN_TOP_LEFT, 4, 0);

    bar_xp = lv_bar_create(xp_box);
    lv_obj_set_size(bar_xp, 324, 8);
    lv_obj_align(bar_xp, LV_ALIGN_BOTTOM_MID, 0, -2);
    lv_bar_set_range(bar_xp, 0, 100);
    lv_bar_set_value(bar_xp, 25, LV_ANIM_ON);
    lv_obj_set_style_bg_color(bar_xp, COLOR_DARK_GREY, 0);
    lv_obj_set_style_bg_color(bar_xp, COLOR_PURPLE, LV_PART_INDICATOR);

    // Linha de Status (Fome, Felicidade, Handshakes)
    lv_obj_t *stats_row = lv_obj_create(tile);
    lv_obj_set_size(stats_row, 340, 68);
    lv_obj_align(stats_row, LV_ALIGN_TOP_MID, 0, 268);
    lv_obj_set_style_bg_color(stats_row, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(stats_row, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(stats_row, 1, 0);
    lv_obj_set_style_radius(stats_row, 12, 0);
    lv_obj_set_style_pad_all(stats_row, 6, 0);
    lv_obj_set_flex_flow(stats_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(stats_row, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Fome Card
    lv_obj_t *c_hunger = lv_obj_create(stats_row);
    lv_obj_set_size(c_hunger, 100, 54);
    lv_obj_set_style_bg_opa(c_hunger, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(c_hunger, 0, 0);
    lv_obj_set_style_pad_all(c_hunger, 2, 0);
    lbl_hunger_val = lv_label_create(c_hunger);
    lv_label_set_text(lbl_hunger_val, "Fome: 80%");
    lv_obj_set_style_text_color(lbl_hunger_val, COLOR_AMBER, 0);
    lv_obj_set_style_text_font(lbl_hunger_val, &lv_font_montserrat_12, 0);
    bar_hunger = lv_bar_create(c_hunger);
    lv_obj_set_size(bar_hunger, 94, 6);
    lv_obj_align(bar_hunger, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_bar_set_range(bar_hunger, 0, 100);
    lv_bar_set_value(bar_hunger, 80, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(bar_hunger, COLOR_DARK_GREY, 0);
    lv_obj_set_style_bg_color(bar_hunger, COLOR_AMBER, LV_PART_INDICATOR);

    // Felicidade Card
    lv_obj_t *c_happy = lv_obj_create(stats_row);
    lv_obj_set_size(c_happy, 100, 54);
    lv_obj_set_style_bg_opa(c_happy, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(c_happy, 0, 0);
    lv_obj_set_style_pad_all(c_happy, 2, 0);
    lbl_happiness_val = lv_label_create(c_happy);
    lv_label_set_text(lbl_happiness_val, "Humor: 60%");
    lv_obj_set_style_text_color(lbl_happiness_val, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(lbl_happiness_val, &lv_font_montserrat_12, 0);
    bar_happiness = lv_bar_create(c_happy);
    lv_obj_set_size(bar_happiness, 94, 6);
    lv_obj_align(bar_happiness, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_bar_set_range(bar_happiness, 0, 100);
    lv_bar_set_value(bar_happiness, 60, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(bar_happiness, COLOR_DARK_GREY, 0);
    lv_obj_set_style_bg_color(bar_happiness, COLOR_GREEN, LV_PART_INDICATOR);

    // Handshakes Card
    lv_obj_t *c_shakes = lv_obj_create(stats_row);
    lv_obj_set_size(c_shakes, 94, 54);
    lv_obj_set_style_bg_opa(c_shakes, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(c_shakes, 0, 0);
    lv_obj_set_style_pad_all(c_shakes, 2, 0);
    lv_obj_t *lbl_shk_t = lv_label_create(c_shakes);
    lv_label_set_text(lbl_shk_t, "Shakes");
    lv_obj_set_style_text_color(lbl_shk_t, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_shk_t, &lv_font_montserrat_12, 0);
    lbl_shakes_val = lv_label_create(c_shakes);
    lv_label_set_text(lbl_shakes_val, "0");
    lv_obj_set_style_text_color(lbl_shakes_val, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(lbl_shakes_val, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_shakes_val, LV_ALIGN_BOTTOM_MID, 0, -2);

    // Botões de Interação (Carinho, Alimentar, Expressão)
    lv_obj_t *btn_row = lv_obj_create(tile);
    lv_obj_set_size(btn_row, 340, 48);
    lv_obj_align(btn_row, LV_ALIGN_TOP_MID, 0, 342);
    lv_obj_set_style_bg_opa(btn_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btn_row, 0, 0);
    lv_obj_set_style_pad_all(btn_row, 0, 0);
    lv_obj_set_flex_flow(btn_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *btn1 = lv_button_create(btn_row);
    lv_obj_set_size(btn1, 105, 42);
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0x1B2C44), 0);
    lv_obj_set_style_border_color(btn1, COLOR_CYAN, 0);
    lv_obj_set_style_border_width(btn1, 1, 0);
    lv_obj_set_style_radius(btn1, 10, 0);
    lv_obj_add_event_cb(btn1, btn_pet_care_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *l1 = lv_label_create(btn1);
    lv_label_set_text(l1, "Carinho");
    lv_obj_center(l1);

    lv_obj_t *btn2 = lv_button_create(btn_row);
    lv_obj_set_size(btn2, 105, 42);
    lv_obj_set_style_bg_color(btn2, lv_color_hex(0x282010), 0);
    lv_obj_set_style_border_color(btn2, COLOR_AMBER, 0);
    lv_obj_set_style_border_width(btn2, 1, 0);
    lv_obj_set_style_radius(btn2, 10, 0);
    lv_obj_add_event_cb(btn2, btn_pet_feed_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *l2 = lv_label_create(btn2);
    lv_label_set_text(l2, "Alimentar");
    lv_obj_center(l2);

    lv_obj_t *btn3 = lv_button_create(btn_row);
    lv_obj_set_size(btn3, 105, 42);
    lv_obj_set_style_bg_color(btn3, lv_color_hex(0x241030), 0);
    lv_obj_set_style_border_color(btn3, COLOR_PURPLE, 0);
    lv_obj_set_style_border_width(btn3, 1, 0);
    lv_obj_set_style_radius(btn3, 10, 0);
    lv_obj_add_event_cb(btn3, btn_pet_face_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *l3 = lv_label_create(btn3);
    lv_label_set_text(l3, "Humor");
    lv_obj_center(l3);
}

// Criação do Tile 1 (Wi-Fi Radar & Spectrum)
static void create_tile_wifi(lv_obj_t *tile) {
    lv_obj_t *title = lv_label_create(tile);
    lv_label_set_text(title, "WIFI RADAR");
    lbl_ti_wifi = title;
    theme_follow(title);
    lv_obj_set_style_text_color(title, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 16, 10);

    lbl_wifi_scan_state = lv_label_create(tile);
    lv_label_set_text(lbl_wifi_scan_state, "SCAN ATIVO");
    lv_obj_set_style_text_color(lbl_wifi_scan_state, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(lbl_wifi_scan_state, &lv_font_montserrat_12, 0);
    lv_obj_set_style_bg_color(lbl_wifi_scan_state, lv_color_hex(0x0A2B18), 0);
    lv_obj_set_style_pad_hor(lbl_wifi_scan_state, 8, 0);
    lv_obj_set_style_pad_ver(lbl_wifi_scan_state, 2, 0);
    lv_obj_set_style_radius(lbl_wifi_scan_state, 8, 0);
    lv_obj_align(lbl_wifi_scan_state, LV_ALIGN_TOP_RIGHT, -16, 10);

    // Caixa do Radar Circular (menor, à esquerda; espaço à direita = telemetria)
    radar_box = lv_obj_create(tile);
    lv_obj_set_size(radar_box, 140, 140);
    lv_obj_align(radar_box, LV_ALIGN_TOP_LEFT, 14, 32);
    lv_obj_set_style_bg_color(radar_box, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(radar_box, COLOR_GREEN, 0);
    lv_obj_set_style_border_width(radar_box, 2, 0);
    lv_obj_set_style_radius(radar_box, LV_RADIUS_CIRCLE, 0);
    lv_obj_clear_flag(radar_box, LV_OBJ_FLAG_SCROLLABLE);

    // Círculos concêntricos de alcance
    lv_obj_t *r1 = lv_obj_create(radar_box);
    lv_obj_set_size(r1, 104, 104);
    lv_obj_center(r1);
    lv_obj_set_style_bg_opa(r1, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_color(r1, COLOR_GREEN, 0);
    lv_obj_set_style_border_width(r1, 1, 0);
    lv_obj_set_style_border_opa(r1, LV_OPA_40, 0);
    lv_obj_set_style_radius(r1, LV_RADIUS_CIRCLE, 0);

    lv_obj_t *r2 = lv_obj_create(radar_box);
    lv_obj_set_size(r2, 52, 52);
    lv_obj_center(r2);
    lv_obj_set_style_bg_opa(r2, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_color(r2, COLOR_GREEN, 0);
    lv_obj_set_style_border_width(r2, 1, 0);
    lv_obj_set_style_border_opa(r2, LV_OPA_50, 0);
    lv_obj_set_style_radius(r2, LV_RADIUS_CIRCLE, 0);

    // Linha de varredura (Sweep rotativo do radar)
    radar_sweep_line = lv_arc_create(radar_box);
    lv_obj_set_size(radar_sweep_line, 134, 134);
    lv_obj_center(radar_sweep_line);
    lv_arc_set_rotation(radar_sweep_line, 0);
    lv_arc_set_bg_angles(radar_sweep_line, 0, 360);
    lv_arc_set_angles(radar_sweep_line, 0, 40);
    lv_obj_set_style_arc_width(radar_sweep_line, 62, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(radar_sweep_line, COLOR_GREEN, LV_PART_INDICATOR);
    lv_obj_set_style_arc_opa(radar_sweep_line, LV_OPA_30, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(radar_sweep_line, 0, LV_PART_MAIN);
    lv_obj_remove_style(radar_sweep_line, NULL, LV_PART_KNOB);

    lv_anim_t a_sweep;
    lv_anim_init(&a_sweep);
    lv_anim_set_var(&a_sweep, radar_sweep_line);
    lv_anim_set_values(&a_sweep, 0, 360);
    lv_anim_set_duration(&a_sweep, 2500);
    lv_anim_set_repeat_count(&a_sweep, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a_sweep, anim_radar_sweep_cb);
    lv_anim_start(&a_sweep);

    // Blips (Pontos luminosos piscando no radar)
    radar_blip1 = lv_obj_create(radar_box);
    lv_obj_set_size(radar_blip1, 8, 8);
    lv_obj_set_style_bg_color(radar_blip1, COLOR_GREEN, 0);
    lv_obj_set_style_radius(radar_blip1, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(radar_blip1, LV_ALIGN_CENTER, 24, -18);

    radar_blip2 = lv_obj_create(radar_box);
    lv_obj_set_size(radar_blip2, 8, 8);
    lv_obj_set_style_bg_color(radar_blip2, COLOR_AMBER, 0);
    lv_obj_set_style_radius(radar_blip2, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(radar_blip2, LV_ALIGN_CENTER, -30, 28);

    radar_blip3 = lv_obj_create(radar_box);
    lv_obj_set_size(radar_blip3, 8, 8);
    lv_obj_set_style_bg_color(radar_blip3, COLOR_CYAN, 0);
    lv_obj_set_style_radius(radar_blip3, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(radar_blip3, LV_ALIGN_CENTER, -12, -42);

    // Card de telemetria real (à direita do radar)
    lv_obj_t *info_wifi = lv_obj_create(tile);
    theme_follow(info_wifi);
    lv_obj_set_size(info_wifi, 182, 140);
    lv_obj_align(info_wifi, LV_ALIGN_TOP_RIGHT, -14, 32);
    lv_obj_set_style_bg_color(info_wifi, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(info_wifi, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(info_wifi, 1, 0);
    lv_obj_set_style_radius(info_wifi, 12, 0);
    lv_obj_set_style_pad_all(info_wifi, 8, 0);
    lv_obj_clear_flag(info_wifi, LV_OBJ_FLAG_SCROLLABLE);

    lbl_ap_count_big = lv_label_create(info_wifi);
    lv_label_set_text(lbl_ap_count_big, "APs: 0");
    lv_obj_set_style_text_color(lbl_ap_count_big, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(lbl_ap_count_big, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_ap_count_big, LV_ALIGN_TOP_LEFT, 0, 0);

    lbl_wifi_open = lv_label_create(info_wifi);
    lv_label_set_text(lbl_wifi_open, "Abertas: 0");
    lv_obj_set_style_text_color(lbl_wifi_open, COLOR_AMBER, 0);
    lv_obj_set_style_text_font(lbl_wifi_open, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_wifi_open, LV_ALIGN_TOP_LEFT, 0, 24);

    lbl_wifi_frames = lv_label_create(info_wifi);
    lv_label_set_text(lbl_wifi_frames, "Frames: 0");
    lv_obj_set_style_text_color(lbl_wifi_frames, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(lbl_wifi_frames, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_wifi_frames, LV_ALIGN_TOP_LEFT, 0, 48);

    lbl_wifi_eapol = lv_label_create(info_wifi);
    lv_label_set_text(lbl_wifi_eapol, "EAPOL: 0");
    lv_obj_set_style_text_color(lbl_wifi_eapol, COLOR_PURPLE, 0);
    lv_obj_set_style_text_font(lbl_wifi_eapol, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_wifi_eapol, LV_ALIGN_TOP_LEFT, 0, 72);

    lbl_wifi_espnow = lv_label_create(info_wifi);
    lv_label_set_text(lbl_wifi_espnow, "ESP-NOW: 0");
    lv_obj_set_style_text_color(lbl_wifi_espnow, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_wifi_espnow, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_wifi_espnow, LV_ALIGN_TOP_LEFT, 0, 96);

    // Lista de Redes Detectadas (ao vivo)
    lv_obj_t *ap_list = lv_obj_create(tile);
    theme_follow(ap_list);
    lv_obj_set_size(ap_list, 340, 124);
    lv_obj_align(ap_list, LV_ALIGN_TOP_MID, 0, 182);
    lv_obj_set_style_bg_color(ap_list, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(ap_list, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(ap_list, 1, 0);
    lv_obj_set_style_radius(ap_list, 12, 0);
    lv_obj_set_style_pad_all(ap_list, 6, 0);
    lv_obj_clear_flag(ap_list, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_list_hdr = lv_label_create(ap_list);
    lv_label_set_text(lbl_list_hdr, "REDES DETECTADAS (SSID         RSSI  CH)");
    lv_obj_set_style_text_color(lbl_list_hdr, COLOR_GREY, 0);
    lv_obj_set_style_text_font(lbl_list_hdr, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_list_hdr, LV_ALIGN_TOP_LEFT, 0, 0);

    for (int i = 0; i < 4; i++) {
        lbl_ap_rows[i] = lv_label_create(ap_list);
        lv_label_set_text(lbl_ap_rows[i], "-");
        lv_obj_set_style_text_color(lbl_ap_rows[i], COLOR_WHITE, 0);
        lv_obj_set_style_text_font(lbl_ap_rows[i], &lv_font_montserrat_12, 0);
        lv_obj_align(lbl_ap_rows[i], LV_ALIGN_TOP_LEFT, 0, 22 + i * 24);
    }

    // Botões de comando
    lv_obj_t *wifi_btn_row = lv_obj_create(tile);
    lv_obj_set_size(wifi_btn_row, 340, 48);
    lv_obj_align(wifi_btn_row, LV_ALIGN_TOP_MID, 0, 316);
    lv_obj_set_style_bg_opa(wifi_btn_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(wifi_btn_row, 0, 0);
    lv_obj_set_style_pad_all(wifi_btn_row, 0, 0);
    lv_obj_set_flex_flow(wifi_btn_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(wifi_btn_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *btn_rescan = lv_button_create(wifi_btn_row);
    lv_obj_set_size(btn_rescan, 165, 44);
    lv_obj_set_style_bg_color(btn_rescan, lv_color_hex(0x0A2B18), 0);
    lv_obj_set_style_border_color(btn_rescan, COLOR_GREEN, 0);
    lv_obj_set_style_border_width(btn_rescan, 1, 0);
    lv_obj_set_style_radius(btn_rescan, 10, 0);
    lv_obj_add_event_cb(btn_rescan, btn_wifi_rescan_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *lr = lv_label_create(btn_rescan);
    lv_label_set_text(lr, LV_SYMBOL_REFRESH " Re-Scan");
    lv_obj_center(lr);

    lv_obj_t *btn_wifi_sniff = lv_button_create(wifi_btn_row);
    lv_obj_set_size(btn_wifi_sniff, 165, 44);
    lv_obj_set_style_bg_color(btn_wifi_sniff, lv_color_hex(0x102030), 0);
    lv_obj_set_style_border_color(btn_wifi_sniff, COLOR_CYAN, 0);
    lv_obj_set_style_border_width(btn_wifi_sniff, 1, 0);
    lv_obj_set_style_radius(btn_wifi_sniff, 10, 0);
    lv_obj_add_event_cb(btn_wifi_sniff, btn_wifi_sniff_cb, LV_EVENT_CLICKED, NULL);
    lbl_sniff_state = lv_label_create(btn_wifi_sniff);
    lv_label_set_text(lbl_sniff_state, "Sniffer: ON");
    lv_obj_center(lbl_sniff_state);
}

// Callback de troca de tema da UI
static void btn_theme_select_cb(lv_event_t *e) {
    int th = (int)(intptr_t)lv_event_get_user_data(e);
    PwnUI::setTheme((UITheme)th);
    ConfigManager::getInstance()->set("disp_theme", (int64_t)th);
    AudioHandler::beep(2000 + th * 250, 40);
    PwnUI::showToast("Tema aplicado e salvo!");
}

static void create_tile_ble(lv_obj_t *tile) {
    lv_obj_t *title = lv_label_create(tile);
    lv_label_set_text(title, "BLE SONAR & FOXHUNT");
    lbl_ti_ble = title;
    theme_follow(title);
    lv_obj_set_style_text_color(title, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 16, 10);

    // Sonar Visual Box (menor, à esquerda)
    lv_obj_t *sonar_box = lv_obj_create(tile);
    theme_follow(sonar_box);
    lv_obj_set_size(sonar_box, 150, 140);
    lv_obj_align(sonar_box, LV_ALIGN_TOP_LEFT, 14, 32);
    lv_obj_set_style_bg_color(sonar_box, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(sonar_box, COLOR_CYAN, 0);
    lv_obj_set_style_border_width(sonar_box, 1, 0);
    lv_obj_set_style_radius(sonar_box, 20, 0);
    lv_obj_clear_flag(sonar_box, LV_OBJ_FLAG_SCROLLABLE);

    // Anéis pulsantes de sonar
    sonar_ring1 = lv_obj_create(sonar_box);
    lv_obj_set_size(sonar_ring1, 34, 34);
    lv_obj_center(sonar_ring1);
    lv_obj_set_style_bg_opa(sonar_ring1, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_color(sonar_ring1, COLOR_CYAN, 0);
    lv_obj_set_style_border_width(sonar_ring1, 2, 0);
    lv_obj_set_style_radius(sonar_ring1, LV_RADIUS_CIRCLE, 0);

    sonar_ring2 = lv_obj_create(sonar_box);
    lv_obj_set_size(sonar_ring2, 68, 68);
    lv_obj_center(sonar_ring2);
    lv_obj_set_style_bg_opa(sonar_ring2, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_color(sonar_ring2, COLOR_PURPLE, 0);
    lv_obj_set_style_border_width(sonar_ring2, 2, 0);
    lv_obj_set_style_radius(sonar_ring2, LV_RADIUS_CIRCLE, 0);

    lv_anim_t s1, s2;
    lv_anim_init(&s1);
    lv_anim_set_var(&s1, sonar_ring1);
    lv_anim_set_values(&s1, 20, 140);
    lv_anim_set_duration(&s1, 2200);
    lv_anim_set_repeat_count(&s1, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&s1, anim_sonar_ring_cb);
    lv_anim_start(&s1);

    lv_anim_init(&s2);
    lv_anim_set_var(&s2, sonar_ring2);
    lv_anim_set_values(&s2, 60, 140);
    lv_anim_set_duration(&s2, 2200);
    lv_anim_set_repeat_count(&s2, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&s2, anim_sonar_ring_cb);
    lv_anim_start(&s2);

    // Ícone Bluetooth central
    lv_obj_t *bt_icon = lv_label_create(sonar_box);
    lv_label_set_text(bt_icon, LV_SYMBOL_BLUETOOTH);
    lv_obj_set_style_text_color(bt_icon, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(bt_icon, &lv_font_montserrat_24, 0);
    lv_obj_center(bt_icon);

    // Arc Medidor de Proximidade Foxhunt
    lv_obj_t *fox_box = lv_obj_create(tile);
    theme_follow(fox_box);
    lv_obj_set_size(fox_box, 340, 70);
    lv_obj_align(fox_box, LV_ALIGN_TOP_MID, 0, 178);
    lv_obj_set_style_bg_color(fox_box, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(fox_box, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(fox_box, 1, 0);
    lv_obj_set_style_radius(fox_box, 12, 0);
    lv_obj_set_style_pad_all(fox_box, 6, 0);
    lv_obj_clear_flag(fox_box, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_fox = lv_label_create(fox_box);
    lv_label_set_text(lbl_fox, "FOXHUNT (RSSI DO ALVO):");
    lv_obj_set_style_text_color(lbl_fox, COLOR_GREY, 0);
    lv_obj_set_style_text_font(lbl_fox, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_fox, LV_ALIGN_TOP_LEFT, 6, 0);

    arc_foxhunt = lv_arc_create(fox_box);
    lv_obj_set_size(arc_foxhunt, 52, 52);
    lv_obj_align(arc_foxhunt, LV_ALIGN_RIGHT_MID, -84, 2);
    lv_arc_set_range(arc_foxhunt, 0, 100);
    lv_arc_set_value(arc_foxhunt, 0);
    lv_obj_set_style_arc_color(arc_foxhunt, COLOR_CYAN, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_foxhunt, COLOR_DARK_GREY, LV_PART_MAIN);
    lv_obj_remove_style(arc_foxhunt, NULL, LV_PART_KNOB);

    lbl_foxhunt_rssi = lv_label_create(fox_box);
    lv_label_set_text(lbl_foxhunt_rssi, "inativo");
    lv_obj_set_style_text_color(lbl_foxhunt_rssi, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(lbl_foxhunt_rssi, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_foxhunt_rssi, LV_ALIGN_RIGHT_MID, -8, 0);

    // Card de classificação (à direita do sonar)
    lv_obj_t *info_ble = lv_obj_create(tile);
    theme_follow(info_ble);
    lv_obj_set_size(info_ble, 172, 140);
    lv_obj_align(info_ble, LV_ALIGN_TOP_RIGHT, -14, 32);
    lv_obj_set_style_bg_color(info_ble, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(info_ble, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(info_ble, 1, 0);
    lv_obj_set_style_radius(info_ble, 12, 0);
    lv_obj_set_style_pad_all(info_ble, 8, 0);
    lv_obj_clear_flag(info_ble, LV_OBJ_FLAG_SCROLLABLE);

    lbl_ble_total = lv_label_create(info_ble);
    lv_label_set_text(lbl_ble_total, "BLE: 0");
    lv_obj_set_style_text_color(lbl_ble_total, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_ble_total, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_ble_total, LV_ALIGN_TOP_LEFT, 0, 0);

    lbl_ble_apple = lv_label_create(info_ble);
    lv_label_set_text(lbl_ble_apple, "Apple: 0");
    lv_obj_set_style_text_color(lbl_ble_apple, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(lbl_ble_apple, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_ble_apple, LV_ALIGN_TOP_LEFT, 0, 24);

    lbl_ble_flipper = lv_label_create(info_ble);
    lv_label_set_text(lbl_ble_flipper, "Flipper: 0");
    lv_obj_set_style_text_color(lbl_ble_flipper, COLOR_AMBER, 0);
    lv_obj_set_style_text_font(lbl_ble_flipper, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_ble_flipper, LV_ALIGN_TOP_LEFT, 0, 48);

    lbl_ble_tags = lv_label_create(info_ble);
    lv_label_set_text(lbl_ble_tags, "Tags: 0");
    lv_obj_set_style_text_color(lbl_ble_tags, COLOR_PURPLE, 0);
    lv_obj_set_style_text_font(lbl_ble_tags, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_ble_tags, LV_ALIGN_TOP_LEFT, 0, 72);

    lbl_ble_hid = lv_label_create(info_ble);
    lv_label_set_text(lbl_ble_hid, "HID: 0");
    lv_obj_set_style_text_color(lbl_ble_hid, COLOR_RED, 0);
    lv_obj_set_style_text_font(lbl_ble_hid, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_ble_hid, LV_ALIGN_TOP_LEFT, 0, 96);

    // Lista de dispositivos BLE (ao vivo)
    lv_obj_t *ble_list = lv_obj_create(tile);
    theme_follow(ble_list);
    lv_obj_set_size(ble_list, 340, 94);
    lv_obj_align(ble_list, LV_ALIGN_TOP_MID, 0, 254);
    lv_obj_set_style_bg_color(ble_list, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(ble_list, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(ble_list, 1, 0);
    lv_obj_set_style_radius(ble_list, 12, 0);
    lv_obj_set_style_pad_all(ble_list, 6, 0);
    lv_obj_clear_flag(ble_list, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_ble_hdr = lv_label_create(ble_list);
    lv_label_set_text(lbl_ble_hdr, "DISPOSITIVOS NO AR (nome/tipo   RSSI)");
    lv_obj_set_style_text_color(lbl_ble_hdr, COLOR_GREY, 0);
    lv_obj_set_style_text_font(lbl_ble_hdr, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_ble_hdr, LV_ALIGN_TOP_LEFT, 0, 0);

    for (int i = 0; i < 3; i++) {
        lbl_ble_rows[i] = lv_label_create(ble_list);
        lv_label_set_text(lbl_ble_rows[i], "-");
        lv_obj_set_style_text_color(lbl_ble_rows[i], COLOR_CYAN, 0);
        lv_obj_set_style_text_font(lbl_ble_rows[i], &lv_font_montserrat_12, 0);
        lv_obj_align(lbl_ble_rows[i], LV_ALIGN_TOP_LEFT, 0, 22 + i * 22);
    }

    // Botões de comando BLE
    lv_obj_t *ble_btn_row = lv_obj_create(tile);
    lv_obj_set_size(ble_btn_row, 340, 44);
    lv_obj_align(ble_btn_row, LV_ALIGN_TOP_MID, 0, 356);
    lv_obj_set_style_bg_opa(ble_btn_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(ble_btn_row, 0, 0);
    lv_obj_set_style_pad_all(ble_btn_row, 0, 0);
    lv_obj_set_flex_flow(ble_btn_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ble_btn_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *btn_bscan = lv_button_create(ble_btn_row);
    lv_obj_set_size(btn_bscan, 165, 40);
    lv_obj_set_style_bg_color(btn_bscan, lv_color_hex(0x0A2430), 0);
    lv_obj_set_style_border_color(btn_bscan, COLOR_CYAN, 0);
    lv_obj_set_style_border_width(btn_bscan, 1, 0);
    lv_obj_set_style_radius(btn_bscan, 10, 0);
    lv_obj_add_event_cb(btn_bscan, btn_ble_scan_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *lb1 = lv_label_create(btn_bscan);
    lv_label_set_text(lb1, LV_SYMBOL_WIFI " Scan BLE (2s)");
    lv_obj_center(lb1);

    lv_obj_t *btn_bclear = lv_button_create(ble_btn_row);
    lv_obj_set_size(btn_bclear, 165, 40);
    lv_obj_set_style_bg_color(btn_bclear, lv_color_hex(0x241015), 0);
    lv_obj_set_style_border_color(btn_bclear, COLOR_RED, 0);
    lv_obj_set_style_border_width(btn_bclear, 1, 0);
    lv_obj_set_style_radius(btn_bclear, 10, 0);
    lv_obj_add_event_cb(btn_bclear, btn_ble_clear_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *lb2 = lv_label_create(btn_bclear);
    lv_label_set_text(lb2, LV_SYMBOL_TRASH " Limpar");
    lv_obj_center(lb2);
}

// Criação do Tile 3 (Arsenal RED / Laboratório)
static void create_tile_red(lv_obj_t *tile) {
    lv_obj_t *title = lv_label_create(tile);
    lv_label_set_text(title, "LABORATORIO RED");
    lbl_ti_red = title;
    theme_follow(title);
    lv_obj_set_style_text_color(title, COLOR_RED, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 16, 10);

    // Card de Status ARM
    card_arm_status = lv_obj_create(tile);
    lv_obj_set_size(card_arm_status, 340, 68);
    lv_obj_align(card_arm_status, LV_ALIGN_TOP_MID, 0, 36);
    lv_obj_set_style_bg_color(card_arm_status, lv_color_hex(0x200C12), 0);
    lv_obj_set_style_border_color(card_arm_status, COLOR_RED, 0);
    lv_obj_set_style_border_width(card_arm_status, 2, 0);
    lv_obj_set_style_radius(card_arm_status, 12, 0);
    lv_obj_set_style_pad_all(card_arm_status, 8, 0);
    lv_obj_add_flag(card_arm_status, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(card_arm_status, btn_arm_toggle_cb, LV_EVENT_CLICKED, NULL);

    lbl_arm_title = lv_label_create(card_arm_status);
    lv_label_set_text(lbl_arm_title, "STATUS: DESARMADO (Seguro)");
    lv_obj_set_style_text_color(lbl_arm_title, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_arm_title, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_arm_title, LV_ALIGN_TOP_LEFT, 0, 0);

    lbl_arm_subtitle = lv_label_create(card_arm_status);
    lv_label_set_text(lbl_arm_subtitle, "Toque para alternar chave fisica ARM");
    lv_obj_set_style_text_color(lbl_arm_subtitle, COLOR_GREY, 0);
    lv_obj_set_style_text_font(lbl_arm_subtitle, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_arm_subtitle, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    // Grid 2x2 de Ataques/Laboratório
    lv_obj_t *grid_box = lv_obj_create(tile);
    lv_obj_set_size(grid_box, 340, 210);
    lv_obj_align(grid_box, LV_ALIGN_TOP_MID, 0, 114);
    lv_obj_set_style_bg_opa(grid_box, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid_box, 0, 0);
    lv_obj_set_style_pad_all(grid_box, 0, 0);
    lv_obj_set_flex_flow(grid_box, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(grid_box, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START);

    // Card 1: Evil Portal
    btn_portal_state = lv_button_create(grid_box);
    lv_obj_set_size(btn_portal_state, 164, 96);
    lv_obj_set_style_bg_color(btn_portal_state, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(btn_portal_state, COLOR_RED, 0);
    lv_obj_set_style_border_width(btn_portal_state, 1, 0);
    lv_obj_set_style_radius(btn_portal_state, 10, 0);
    lv_obj_add_event_cb(btn_portal_state, btn_portal_toggle_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lp1 = lv_label_create(btn_portal_state);
    lv_label_set_text(lp1, "Evil Portal\nCativo");
    lv_obj_set_style_text_color(lp1, COLOR_WHITE, 0);
    lv_obj_align(lp1, LV_ALIGN_TOP_LEFT, 0, 0);

    lbl_portal_state = lv_label_create(btn_portal_state);
    lv_label_set_text(lbl_portal_state, "INICIAR");
    lv_obj_set_style_text_color(lbl_portal_state, COLOR_CYAN, 0);
    lv_obj_align(lbl_portal_state, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    // Card 2: ESP-NOW Inventory (passivo, BLUE)
    lv_obj_t *b_spam = lv_button_create(grid_box);
    lv_obj_set_size(b_spam, 164, 96);
    lv_obj_set_style_bg_color(b_spam, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(b_spam, COLOR_AMBER, 0);
    lv_obj_set_style_border_width(b_spam, 1, 0);
    lv_obj_set_style_radius(b_spam, 10, 0);
    lv_obj_t *lp2 = lv_label_create(b_spam);
    lv_label_set_text(lp2, "ESP-NOW Scan\nInventario");
    lv_obj_set_style_text_color(lp2, COLOR_WHITE, 0);
    lv_obj_align(lp2, LV_ALIGN_TOP_LEFT, 0, 0);
    lbl_espnow_info = lv_label_create(b_spam);
    lv_label_set_text(lbl_espnow_info, "0 peers");
    lv_obj_set_style_text_color(lbl_espnow_info, COLOR_AMBER, 0);
    lv_obj_set_style_text_font(lbl_espnow_info, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_espnow_info, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    // Card 3: Deauth Lab (toggle de comando)
    lv_obj_t *b_deauth = lv_button_create(grid_box);
    lv_obj_set_size(b_deauth, 164, 96);
    lv_obj_set_style_bg_color(b_deauth, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(b_deauth, COLOR_RED, 0);
    lv_obj_set_style_border_width(b_deauth, 1, 0);
    lv_obj_set_style_radius(b_deauth, 10, 0);
    lv_obj_add_event_cb(b_deauth, btn_deauth_toggle_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *lp3 = lv_label_create(b_deauth);
    lv_label_set_text(lp3, "Deauth Lab\nToggle");
    lv_obj_set_style_text_color(lp3, COLOR_WHITE, 0);
    lv_obj_align(lp3, LV_ALIGN_TOP_LEFT, 0, 0);
    lbl_deauth_state = lv_label_create(b_deauth);
    lv_label_set_text(lbl_deauth_state, "PARADO");
    lv_obj_set_style_text_color(lbl_deauth_state, COLOR_RED, 0);
    lv_obj_set_style_text_font(lbl_deauth_state, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_deauth_state, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    // Card 4: WPS Audit
    lv_obj_t *b_wps = lv_button_create(grid_box);
    lv_obj_set_size(b_wps, 164, 96);
    lv_obj_set_style_bg_color(b_wps, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(b_wps, COLOR_PURPLE, 0);
    lv_obj_set_style_border_width(b_wps, 1, 0);
    lv_obj_set_style_radius(b_wps, 10, 0);
    lv_obj_t *lp4 = lv_label_create(b_wps);
    lv_label_set_text(lp4, "WPS Audit\nPixie / PIN");
    lv_obj_set_style_text_color(lp4, COLOR_WHITE, 0);
    lv_obj_align(lp4, LV_ALIGN_TOP_LEFT, 0, 0);
    lbl_wps_info = lv_label_create(b_wps);
    lv_label_set_text(lbl_wps_info, "0 ON / 0 LOCK");
    lv_obj_set_style_text_color(lbl_wps_info, COLOR_PURPLE, 0);
    lv_obj_set_style_text_font(lbl_wps_info, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_wps_info, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    // Aviso de Contrato de Segurança
    lv_obj_t *lbl_warn = lv_label_create(tile);
    lv_label_set_text(lbl_warn, "Contrato: Operacao permitida apenas para MACs em /sd/allowlist.txt");
    lv_obj_set_style_text_color(lbl_warn, COLOR_GREY, 0);
    lv_obj_set_style_text_font(lbl_warn, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_warn, LV_ALIGN_BOTTOM_MID, 0, -12);
}

// Criação do Tile 4 (Capturas & PCAP Monitor)
static void create_tile_capture(lv_obj_t *tile) {
    lv_obj_t *title = lv_label_create(tile);
    lv_label_set_text(title, "MONITOR DE CAPTURA PCAP");
    lbl_ti_cap = title;
    theme_follow(title);
    lv_obj_set_style_text_color(title, COLOR_AMBER, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 16, 10);

    // Troféu de Handshakes
    lv_obj_t *trophy_box = lv_obj_create(tile);
    lv_obj_set_size(trophy_box, 340, 80);
    lv_obj_align(trophy_box, LV_ALIGN_TOP_MID, 0, 36);
    lv_obj_set_style_bg_color(trophy_box, lv_color_hex(0x2A1C06), 0);
    lv_obj_set_style_border_color(trophy_box, COLOR_AMBER, 0);
    lv_obj_set_style_border_width(trophy_box, 2, 0);
    lv_obj_set_style_radius(trophy_box, 12, 0);
    lv_obj_set_style_pad_all(trophy_box, 8, 0);

    lbl_handshakes_big = lv_label_create(trophy_box);
    lv_label_set_text(lbl_handshakes_big, "0 HANDSHAKES");
    lv_obj_set_style_text_color(lbl_handshakes_big, COLOR_AMBER, 0);
    lv_obj_set_style_text_font(lbl_handshakes_big, &lv_font_montserrat_20, 0);
    lv_obj_align(lbl_handshakes_big, LV_ALIGN_TOP_LEFT, 0, 2);

    lv_obj_t *lbl_shk_sub = lv_label_create(trophy_box);
    lv_label_set_text(lbl_shk_sub, "EAPOL 4-Way Frames gravados no SD");
    lv_obj_set_style_text_color(lbl_shk_sub, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_shk_sub, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_shk_sub, LV_ALIGN_BOTTOM_LEFT, 0, -2);

    // Barra de Espaço no Cartão SD
    lv_obj_t *sd_box = lv_obj_create(tile);
    lv_obj_set_size(sd_box, 340, 68);
    lv_obj_align(sd_box, LV_ALIGN_TOP_MID, 0, 124);
    lv_obj_set_style_bg_color(sd_box, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(sd_box, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(sd_box, 1, 0);
    lv_obj_set_style_radius(sd_box, 12, 0);
    lv_obj_set_style_pad_all(sd_box, 8, 0);

    lbl_sd_space = lv_label_create(sd_box);
    lv_label_set_text(lbl_sd_space, "Cartao SD: Montado");
    lv_obj_set_style_text_color(lbl_sd_space, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_sd_space, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_sd_space, LV_ALIGN_TOP_LEFT, 0, 0);

    bar_sd_space = lv_bar_create(sd_box);
    lv_obj_set_size(bar_sd_space, 320, 8);
    lv_obj_align(bar_sd_space, LV_ALIGN_BOTTOM_MID, 0, -4);
    lv_bar_set_range(bar_sd_space, 0, 100);
    lv_bar_set_value(bar_sd_space, 30, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(bar_sd_space, COLOR_DARK_GREY, 0);
    lv_obj_set_style_bg_color(bar_sd_space, COLOR_GREEN, LV_PART_INDICATOR);

    // Terminal / Console de Captura
    lv_obj_t *term_box = lv_obj_create(tile);
    lv_obj_set_size(term_box, 340, 180);
    lv_obj_align(term_box, LV_ALIGN_TOP_MID, 0, 200);
    lv_obj_set_style_bg_color(term_box, lv_color_hex(0x060810), 0);
    lv_obj_set_style_border_color(term_box, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(term_box, 1, 0);
    lv_obj_set_style_radius(term_box, 10, 0);
    lv_obj_set_style_pad_all(term_box, 8, 0);

    lbl_pcap_console = lv_label_create(term_box);
    lv_label_set_text(lbl_pcap_console, 
        "[+] Sniffer 802.11 ativo no canal 6\n"
        "[!] Gravando trafego em /capturas/\n"
        "[*] Aguardando troca de chaves EAPOL...\n"
        "[i] Beacons recebidos: 120\n"
        "[+] Buffer circular pronto.");
    lv_obj_set_style_text_color(lbl_pcap_console, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(lbl_pcap_console, &lv_font_montserrat_12, 0);
}


// Criação do Tile 5 (Hardware HUD & Nível IMU)
static void create_tile_hardware(lv_obj_t *tile) {
    lv_obj_t *title = lv_label_create(tile);
    lv_label_set_text(title, "HARDWARE & IMU");
    lbl_ti_hw = title;
    theme_follow(title);
    lv_obj_set_style_text_color(title, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 16, 6);

    // 3 Medidores Circulares (Arcs) em Linha
    lv_obj_t *arcs_row = lv_obj_create(tile);
    lv_obj_set_size(arcs_row, 340, 92);
    lv_obj_align(arcs_row, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_set_style_bg_color(arcs_row, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(arcs_row, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(arcs_row, 1, 0);
    lv_obj_set_style_radius(arcs_row, 12, 0);
    lv_obj_set_style_pad_all(arcs_row, 2, 0);
    lv_obj_set_flex_flow(arcs_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(arcs_row, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Arc 1: Bateria
    lv_obj_t *b1 = lv_obj_create(arcs_row);
    lv_obj_set_size(b1, 86, 86);
    lv_obj_set_style_bg_opa(b1, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(b1, 0, 0);
    arc_battery = lv_arc_create(b1);
    lv_obj_set_size(arc_battery, 78, 78);
    lv_obj_center(arc_battery);
    lv_arc_set_range(arc_battery, 0, 100);
    lv_arc_set_value(arc_battery, 85);
    lv_obj_set_style_arc_color(arc_battery, COLOR_GREEN, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_battery, COLOR_DARK_GREY, LV_PART_MAIN);
    lv_obj_remove_style(arc_battery, NULL, LV_PART_KNOB);
    lbl_arc_battery = lv_label_create(b1);
    lv_label_set_text(lbl_arc_battery, "85%");
    lv_obj_set_style_text_color(lbl_arc_battery, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_arc_battery, &lv_font_montserrat_12, 0);
    lv_obj_center(lbl_arc_battery);

    // Arc 2: Temperatura
    lv_obj_t *b2 = lv_obj_create(arcs_row);
    lv_obj_set_size(b2, 86, 86);
    lv_obj_set_style_bg_opa(b2, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(b2, 0, 0);
    arc_temp = lv_arc_create(b2);
    lv_obj_set_size(arc_temp, 78, 78);
    lv_obj_center(arc_temp);
    lv_arc_set_range(arc_temp, 20, 85);
    lv_arc_set_value(arc_temp, 42);
    lv_obj_set_style_arc_color(arc_temp, COLOR_CYAN, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_temp, COLOR_DARK_GREY, LV_PART_MAIN);
    lv_obj_remove_style(arc_temp, NULL, LV_PART_KNOB);
    lbl_arc_temp = lv_label_create(b2);
    lv_label_set_text(lbl_arc_temp, "42C");
    lv_obj_set_style_text_color(lbl_arc_temp, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_arc_temp, &lv_font_montserrat_12, 0);
    lv_obj_center(lbl_arc_temp);

    // Arc 3: RAM
    lv_obj_t *b3 = lv_obj_create(arcs_row);
    lv_obj_set_size(b3, 86, 86);
    lv_obj_set_style_bg_opa(b3, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(b3, 0, 0);
    arc_ram = lv_arc_create(b3);
    lv_obj_set_size(arc_ram, 78, 78);
    lv_obj_center(arc_ram);
    lv_arc_set_range(arc_ram, 0, 100);
    lv_arc_set_value(arc_ram, 48);
    lv_obj_set_style_arc_color(arc_ram, COLOR_PURPLE, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_ram, COLOR_DARK_GREY, LV_PART_MAIN);
    lv_obj_remove_style(arc_ram, NULL, LV_PART_KNOB);
    lbl_arc_ram = lv_label_create(b3);
    lv_label_set_text(lbl_arc_ram, "RAM");
    lv_obj_set_style_text_color(lbl_arc_ram, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_arc_ram, &lv_font_montserrat_12, 0);
    lv_obj_center(lbl_arc_ram);

    // Horizonte Artificial IMU QMI8658 (largura total)
    horizon_box = lv_obj_create(tile);
    theme_follow(horizon_box);
    lv_obj_set_size(horizon_box, 340, 148);
    lv_obj_align(horizon_box, LV_ALIGN_TOP_MID, 0, 128);
    lv_obj_set_style_bg_color(horizon_box, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(horizon_box, COLOR_CYAN, 0);
    lv_obj_set_style_border_width(horizon_box, 1, 0);
    lv_obj_set_style_radius(horizon_box, 10, 0);
    lv_obj_set_style_pad_all(horizon_box, 4, 0);
    lv_obj_clear_flag(horizon_box, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_hz = lv_label_create(horizon_box);
    lv_label_set_text(lbl_hz, "GIRO / IMU");
    lv_obj_set_style_text_color(lbl_hz, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(lbl_hz, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_hz, LV_ALIGN_TOP_MID, 0, 0);

    // Círculo central do horizonte
    lv_obj_t *hz_ring = lv_obj_create(horizon_box);
    lv_obj_set_size(hz_ring, 56, 56);
    lv_obj_align(hz_ring, LV_ALIGN_CENTER, 0, -6);
    lv_obj_set_style_bg_opa(hz_ring, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_color(hz_ring, COLOR_CYAN, 0);
    lv_obj_set_style_border_width(hz_ring, 1, 0);
    lv_obj_set_style_radius(hz_ring, LV_RADIUS_CIRCLE, 0);

    // Esfera fluorescente do giroscópio que se move com o movimento da placa
    horizon_bubble = lv_obj_create(horizon_box);
    lv_obj_set_size(horizon_bubble, 16, 16);
    lv_obj_set_style_bg_color(horizon_bubble, COLOR_AMBER, 0);
    lv_obj_set_style_border_width(horizon_bubble, 0, 0);
    lv_obj_set_style_radius(horizon_bubble, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(horizon_bubble, LV_ALIGN_CENTER, 0, -6);

    lbl_imu_angles = lv_label_create(horizon_box);
    lv_label_set_text(lbl_imu_angles, "0°  0°");
    lv_obj_set_style_text_color(lbl_imu_angles, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_imu_angles, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_imu_angles, LV_ALIGN_BOTTOM_MID, 0, 0);

    // Card do PMU (tensao / carga / temperatura)
    lv_obj_t *pmu_box = lv_obj_create(tile);
    theme_follow(pmu_box);
    lv_obj_set_size(pmu_box, 340, 62);
    lv_obj_align(pmu_box, LV_ALIGN_TOP_MID, 0, 284);
    lv_obj_set_style_bg_color(pmu_box, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(pmu_box, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(pmu_box, 1, 0);
    lv_obj_set_style_radius(pmu_box, 10, 0);
    lv_obj_set_style_pad_all(pmu_box, 6, 0);
    lv_obj_clear_flag(pmu_box, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_pmu_hdr = lv_label_create(pmu_box);
    lv_label_set_text(lbl_pmu_hdr, "PMU AXP2101");
    lv_obj_set_style_text_color(lbl_pmu_hdr, COLOR_GREY, 0);
    lv_obj_set_style_text_font(lbl_pmu_hdr, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_pmu_hdr, LV_ALIGN_TOP_LEFT, 2, 0);

    lbl_pmu_info = lv_label_create(pmu_box);
    lv_label_set_text(lbl_pmu_info, "-- V");
    lv_obj_set_style_text_color(lbl_pmu_info, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_pmu_info, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_pmu_info, LV_ALIGN_BOTTOM_LEFT, 2, -2);
}


// Callback de troca de tema do Relógio ao tocar no Watchface
static void btn_watchface_tap_cb(lv_event_t *e) {
    (void)e;
    watchface_style = (watchface_style + 1) % 4;
    // Sincroniza o relogio com o tema global (agora e a mesma coisa)
    PwnUI::setTheme((UITheme)watchface_style);
    ConfigManager::getInstance()->set("disp_theme", (int64_t)watchface_style);
    AudioHandler::beep(2200, 30);
    PwnUI::showToast("Watchface sincronizado ao tema!");
}

// Criação do Tile 6: Relógio & Watchface Cyberpunk
static void create_tile_clock(lv_obj_t *tile) {
    lv_obj_add_flag(tile, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(tile, btn_watchface_tap_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *top_box = lv_obj_create(tile);
    lv_obj_set_size(top_box, 340, 36);
    lv_obj_align(top_box, LV_ALIGN_TOP_MID, 0, 8);
    lv_obj_set_style_bg_opa(top_box, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(top_box, 0, 0);

    lbl_clock_theme_badge = lv_label_create(top_box);
    lv_label_set_text(lbl_clock_theme_badge, "WATCHFACE CYBER");
    lv_obj_set_style_text_color(lbl_clock_theme_badge, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(lbl_clock_theme_badge, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_clock_theme_badge, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t *lbl_tap_hint = lv_label_create(top_box);
    lv_label_set_text(lbl_tap_hint, "Toque p/ Tema");
    lv_obj_set_style_text_color(lbl_tap_hint, COLOR_GREY, 0);
    lv_obj_set_style_text_font(lbl_tap_hint, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_tap_hint, LV_ALIGN_RIGHT_MID, 0, 0);

    // Arc circular de segundos
    arc_clock_sec = lv_arc_create(tile);
    lv_obj_set_size(arc_clock_sec, 250, 250);
    lv_obj_align(arc_clock_sec, LV_ALIGN_CENTER, 0, -25);
    lv_arc_set_rotation(arc_clock_sec, 270);
    lv_arc_set_bg_angles(arc_clock_sec, 0, 360);
    lv_arc_set_angles(arc_clock_sec, 0, 180);
    lv_obj_set_style_arc_width(arc_clock_sec, 8, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_clock_sec, COLOR_CYAN, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc_clock_sec, 2, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_clock_sec, COLOR_DARK_GREY, LV_PART_MAIN);
    lv_obj_remove_style(arc_clock_sec, NULL, LV_PART_KNOB);

    // Horário Grande
    lbl_clock_time = lv_label_create(tile);
    lv_label_set_text(lbl_clock_time, "12:00:00");
    lv_obj_set_style_text_color(lbl_clock_time, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_clock_time, &lv_font_montserrat_28, 0);
    lv_obj_align(lbl_clock_time, LV_ALIGN_CENTER, 0, -35);

    // Data
    lbl_clock_date = lv_label_create(tile);
    lv_label_set_text(lbl_clock_date, "MINI LELE RTC");
    lv_obj_set_style_text_color(lbl_clock_date, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(lbl_clock_date, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_clock_date, LV_ALIGN_CENTER, 0, 5);

    // Mini-Status do Pet no Relógio
    lv_obj_t *stat_card = lv_obj_create(tile);
    lv_obj_set_size(stat_card, 340, 72);
    lv_obj_align(stat_card, LV_ALIGN_BOTTOM_MID, 0, -14);
    lv_obj_set_style_bg_color(stat_card, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(stat_card, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(stat_card, 1, 0);
    lv_obj_set_style_radius(stat_card, 12, 0);

    lbl_clock_pet_badge = lv_label_create(stat_card);
    lv_label_set_text(lbl_clock_pet_badge, "LELE: ( ^ _ ^ ) • Fome: 80% • Humor: 60%");
    lv_obj_set_style_text_color(lbl_clock_pet_badge, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(lbl_clock_pet_badge, &lv_font_montserrat_12, 0);
    lv_obj_center(lbl_clock_pet_badge);
}

// Criação do Tile 7: Configurações (brilho, timeout de tela, som, sniffer, temas)
static void create_tile_settings(lv_obj_t *tile) {
    lv_obj_t *title = lv_label_create(tile);
    lv_label_set_text(title, "CONFIGURACOES");
    lbl_ti_cfg = title;
    theme_follow(title);
    lv_obj_set_style_text_color(title, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 16, 6);

    // Brilho do AMOLED
    lv_obj_t *bright_box = lv_obj_create(tile);
    theme_follow(bright_box);
    lv_obj_set_size(bright_box, 340, 62);
    lv_obj_align(bright_box, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_set_style_bg_color(bright_box, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(bright_box, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(bright_box, 1, 0);
    lv_obj_set_style_radius(bright_box, 10, 0);
    lv_obj_set_style_pad_all(bright_box, 6, 0);
    lv_obj_clear_flag(bright_box, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_b_title = lv_label_create(bright_box);
    lv_label_set_text(lbl_b_title, "Brilho AMOLED");
    lv_obj_set_style_text_color(lbl_b_title, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_b_title, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_b_title, LV_ALIGN_TOP_LEFT, 2, 0);

    lbl_brightness_val = lv_label_create(bright_box);
    lv_label_set_text(lbl_brightness_val, "80%");
    lv_obj_set_style_text_color(lbl_brightness_val, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(lbl_brightness_val, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_brightness_val, LV_ALIGN_TOP_RIGHT, -2, 0);

    slider_brightness = lv_slider_create(bright_box);
    lv_obj_set_size(slider_brightness, 320, 14);
    lv_obj_align(slider_brightness, LV_ALIGN_BOTTOM_MID, 0, -2);
    lv_slider_set_range(slider_brightness, 15, 255);
    lv_slider_set_value(slider_brightness,
        ConfigManager::getInstance()->getInt("disp_brightness", 200), LV_ANIM_OFF);
    lv_obj_set_style_bg_color(slider_brightness, COLOR_DARK_GREY, 0);
    lv_obj_set_style_bg_color(slider_brightness, COLOR_CYAN, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider_brightness, COLOR_WHITE, LV_PART_KNOB);
    lv_obj_add_event_cb(slider_brightness, slider_brightness_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // Timeout da tela (dim -> off)
    lv_obj_t *timeout_box = lv_obj_create(tile);
    theme_follow(timeout_box);
    lv_obj_set_size(timeout_box, 340, 62);
    lv_obj_align(timeout_box, LV_ALIGN_TOP_MID, 0, 98);
    lv_obj_set_style_bg_color(timeout_box, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(timeout_box, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(timeout_box, 1, 0);
    lv_obj_set_style_radius(timeout_box, 10, 0);
    lv_obj_set_style_pad_all(timeout_box, 6, 0);
    lv_obj_clear_flag(timeout_box, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_t_title = lv_label_create(timeout_box);
    lv_label_set_text(lbl_t_title, "Timeout da tela");
    lv_obj_set_style_text_color(lbl_t_title, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_t_title, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_t_title, LV_ALIGN_TOP_LEFT, 2, 0);

    int tmo = ConfigManager::getInstance()->getInt("disp_timeout_sec", 30);
    if (tmo < 5) tmo = 5;
    if (tmo > 120) tmo = 120;

    lbl_timeout_val = lv_label_create(timeout_box);
    char tbuf[16];
    snprintf(tbuf, sizeof(tbuf), "%d s", tmo);
    lv_label_set_text(lbl_timeout_val, tbuf);
    lv_obj_set_style_text_color(lbl_timeout_val, COLOR_AMBER, 0);
    lv_obj_set_style_text_font(lbl_timeout_val, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_timeout_val, LV_ALIGN_TOP_RIGHT, -2, 0);

    lv_obj_t *slider_timeout = lv_slider_create(timeout_box);
    lv_obj_set_size(slider_timeout, 320, 14);
    lv_obj_align(slider_timeout, LV_ALIGN_BOTTOM_MID, 0, -2);
    lv_slider_set_range(slider_timeout, 5, 120);
    lv_slider_set_value(slider_timeout, tmo, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(slider_timeout, COLOR_DARK_GREY, 0);
    lv_obj_set_style_bg_color(slider_timeout, COLOR_AMBER, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider_timeout, COLOR_WHITE, LV_PART_KNOB);
    lv_obj_add_event_cb(slider_timeout, slider_timeout_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // Switches: Sons (amplificador) e Sniffer (captura passiva)
    lv_obj_t *sw_box = lv_obj_create(tile);
    theme_follow(sw_box);
    lv_obj_set_size(sw_box, 340, 56);
    lv_obj_align(sw_box, LV_ALIGN_TOP_MID, 0, 166);
    lv_obj_set_style_bg_color(sw_box, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(sw_box, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(sw_box, 1, 0);
    lv_obj_set_style_radius(sw_box, 10, 0);
    lv_obj_set_style_pad_all(sw_box, 6, 0);
    lv_obj_set_flex_flow(sw_box, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(sw_box, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *lbl_sw1 = lv_label_create(sw_box);
    lv_label_set_text(lbl_sw1, "Sons");
    lv_obj_set_style_text_color(lbl_sw1, COLOR_WHITE, 0);
    bool sound_on = ConfigManager::getInstance()->getBool("sys_sound_amp", true);
    sw_sound = lv_switch_create(sw_box);
    if (sound_on) lv_obj_add_state(sw_sound, LV_STATE_CHECKED);
    AudioHandler::setAmpPower(sound_on);
    lv_obj_add_event_cb(sw_sound, sw_sound_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *lbl_sw2 = lv_label_create(sw_box);
    lv_label_set_text(lbl_sw2, "Sniffer");
    lv_obj_set_style_text_color(lbl_sw2, COLOR_WHITE, 0);
    sw_autoscan = lv_switch_create(sw_box);
    if (ConfigManager::getInstance()->getBool("atk_auto_scan", true))
        lv_obj_add_state(sw_autoscan, LV_STATE_CHECKED);
    lv_obj_add_event_cb(sw_autoscan, sw_sniffer_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // Seletor de Temas da UI (4 Botões)
    lv_obj_t *theme_box = lv_obj_create(tile);
    theme_follow(theme_box);
    lv_obj_set_size(theme_box, 340, 68);
    lv_obj_align(theme_box, LV_ALIGN_TOP_MID, 0, 228);
    lv_obj_set_style_bg_color(theme_box, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(theme_box, COLOR_CARD_BORDER, 0);
    lv_obj_set_style_border_width(theme_box, 1, 0);
    lv_obj_set_style_radius(theme_box, 10, 0);
    lv_obj_set_style_pad_all(theme_box, 4, 0);
    lv_obj_clear_flag(theme_box, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_th = lv_label_create(theme_box);
    lv_label_set_text(lbl_th, "TEMA DA INTERFACE:");
    lv_obj_set_style_text_color(lbl_th, COLOR_GREY, 0);
    lv_obj_set_style_text_font(lbl_th, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_th, LV_ALIGN_TOP_LEFT, 4, 0);

    lv_obj_t *th_row = lv_obj_create(theme_box);
    lv_obj_set_size(th_row, 330, 38);
    lv_obj_align(th_row, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(th_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(th_row, 0, 0);
    lv_obj_set_style_pad_all(th_row, 0, 0);
    lv_obj_set_flex_flow(th_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(th_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    const char *th_names[] = {"Cyber", "Matrix", "Amber", "Stealth"};
    lv_color_t th_colors[] = {COLOR_CYAN, COLOR_GREEN, COLOR_AMBER, COLOR_WHITE};
    for (int t = 0; t < 4; t++) {
        lv_obj_t *b = lv_button_create(th_row);
        lv_obj_set_size(b, 78, 32);
        lv_obj_set_style_bg_color(b, COLOR_DARK_GREY, 0);
        lv_obj_set_style_border_color(b, th_colors[t], 0);
        lv_obj_set_style_border_width(b, 1, 0);
        lv_obj_set_style_radius(b, 8, 0);
        lv_obj_add_event_cb(b, btn_theme_select_cb, LV_EVENT_CLICKED, (void*)(intptr_t)t);
        lv_obj_t *lbl = lv_label_create(b);
        lv_label_set_text(lbl, th_names[t]);
        lv_obj_set_style_text_color(lbl, th_colors[t], 0);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12, 0);
        lv_obj_center(lbl);
    }

    // Sobre / dica de navegação
    lv_obj_t *lbl_about = lv_label_create(tile);
    lv_label_set_text(lbl_about, "Mini Lele v2.1 • 8 telas\nDeslize ou use o botao BOOT para navegar");
    lv_label_set_long_mode(lbl_about, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(lbl_about, 320);
    lv_obj_set_style_text_color(lbl_about, COLOR_GREY, 0);
    lv_obj_set_style_text_font(lbl_about, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_align(lbl_about, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(lbl_about, LV_ALIGN_BOTTOM_MID, 0, -8);
}

void PwnUI::showBootSplash() {
    boot_splash_box = lv_obj_create(screen);
    lv_obj_set_size(boot_splash_box, 368, 448);
    lv_obj_set_style_bg_color(boot_splash_box, COLOR_AMOLED_BG, 0);
    lv_obj_set_style_border_width(boot_splash_box, 0, 0);
    lv_obj_align(boot_splash_box, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *logo = lv_label_create(boot_splash_box);
    lv_label_set_text(logo, "SENTINEL - S3");
    lv_obj_set_style_text_color(logo, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(logo, &lv_font_montserrat_24, 0);
    lv_obj_align(logo, LV_ALIGN_TOP_MID, 0, 60);

    lv_obj_t *sub = lv_label_create(boot_splash_box);
    lv_label_set_text(sub, "MINI LELE CYBER EDITION");
    lv_obj_set_style_text_color(sub, COLOR_PURPLE, 0);
    lv_obj_set_style_text_font(sub, &lv_font_montserrat_12, 0);
    lv_obj_align(sub, LV_ALIGN_TOP_MID, 0, 95);

    lv_obj_t *diag = lv_label_create(boot_splash_box);
    lv_label_set_text(diag, 
        "[OK] AMOLED 368x448 QSPI ONLINE\n"
        "[OK] 8MB OCTAL PSRAM MOUNTED\n"
        "[OK] QMI8658 6-AXIS IMU READY\n"
        "[OK] PWNPET NEURAL CORE LOADED\n"
        "[OK] RED/BLUE DEFENSIVE MATRIX");
    lv_obj_set_style_text_color(diag, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(diag, &lv_font_montserrat_12, 0);
    lv_obj_align(diag, LV_ALIGN_CENTER, 0, 20);

    lv_obj_t *pbar = lv_bar_create(boot_splash_box);
    lv_obj_set_size(pbar, 280, 10);
    lv_obj_align(pbar, LV_ALIGN_BOTTOM_MID, 0, -60);
    lv_bar_set_range(pbar, 0, 100);
    lv_bar_set_value(pbar, 100, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(pbar, COLOR_CYAN, LV_PART_INDICATOR);

    AudioHandler::beep(1760, 50);
    AudioHandler::beep(2200, 70);
    AudioHandler::beep(2793, 100);

    // Deleta o splash suavemente após carregar
    lv_timer_create([](lv_timer_t *t){
        lv_obj_t *obj = (lv_obj_t *)lv_timer_get_user_data(t);
        if (obj) lv_obj_del(obj);
        lv_timer_del(t);
    }, 1800, boot_splash_box);
}

void PwnUI::showScreensaver(bool en) {
    screensaver_active = en;
    if (en) {
        if (!screensaver_box) {
            screensaver_box = lv_obj_create(screen);
            lv_obj_set_size(screensaver_box, 368, 448);
            lv_obj_set_style_bg_color(screensaver_box, lv_color_hex(0x000000), 0);
            lv_obj_set_style_border_width(screensaver_box, 0, 0);
            lv_obj_center(screensaver_box);
            lv_obj_add_flag(screensaver_box, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(screensaver_box, [](lv_event_t* e){
                (void)e;
                PwnUI::showScreensaver(false);
                PwnSleep::notifyActivity();
            }, LV_EVENT_CLICKED, NULL);

            lv_obj_t *s_pet = lv_label_create(screensaver_box);
            lv_label_set_text(s_pet, "( - _ - ) zZ");
            lv_obj_set_style_text_color(s_pet, COLOR_CYAN, 0);
            lv_obj_set_style_text_font(s_pet, &lv_font_montserrat_20, 0);
            lv_obj_center(s_pet);
        }
        lv_obj_clear_flag(screensaver_box, LV_OBJ_FLAG_HIDDEN);
    } else {
        if (screensaver_box) lv_obj_add_flag(screensaver_box, LV_OBJ_FLAG_HIDDEN);
    }
}

bool PwnUI::isScreensaverActive() {
    return screensaver_active;
}

void PwnUI::setTheme(UITheme theme) {
    current_ui_theme = theme;
    if (theme == THEME_MATRIX) theme_accent_color = COLOR_GREEN;
    else if (theme == THEME_AMBER) theme_accent_color = COLOR_AMBER;
    else if (theme == THEME_STEALTH) theme_accent_color = COLOR_WHITE;
    else { theme_accent_color = COLOR_CYAN; theme = THEME_CYBERPUNK; }
    lv_color_t p = theme_accent_color;
    const char *th_name = (theme == THEME_MATRIX) ? "MATRIX"
                        : (theme == THEME_AMBER)  ? "AMBER"
                        : (theme == THEME_STEALTH)? "STEALTH" : "CYBER";

    // Rosto do pet acompanha o tema
    FaceHandler::setColor(p);

    // Repinta TODOS os objetos registrados (texto + borda). Antes esta funcao
    // mudava apenas 4 widgets e o updateStatusBar() reaplicava ciano em 300 ms.
    for (int i = 0; i < theme_n; i++) {
        lv_obj_t *o = theme_objs[i];
        if (!o || !lv_obj_is_valid(o)) continue;
        lv_obj_set_style_text_color(o, p, 0);
        lv_obj_set_style_border_color(o, p, 0);
    }

    // Elementos com partes especiais
    if (arc_clock_sec) lv_obj_set_style_arc_color(arc_clock_sec, p, LV_PART_INDICATOR);
    if (slider_brightness) lv_obj_set_style_bg_color(slider_brightness, p, LV_PART_INDICATOR);
    if (lbl_clock_theme_badge) {
        char buf[32];
        snprintf(buf, sizeof(buf), "WATCHFACE %s", th_name);
        lv_label_set_text(lbl_clock_theme_badge, buf);
        lv_obj_set_style_text_color(lbl_clock_theme_badge, p, 0);
    }
    if (lbl_clock_date) lv_obj_set_style_text_color(lbl_clock_date, p, 0);
}

UITheme PwnUI::getTheme() {
    return current_ui_theme;
}

void PwnUI::init() {
    screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, COLOR_AMOLED_BG, 0);

    // Barra de Status Superior (Altura 34px)
    status_bar = lv_obj_create(screen);
    theme_follow(status_bar);
    lv_obj_set_size(status_bar, 368, 34);
    lv_obj_align(status_bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(status_bar, lv_color_hex(0x0A0F1D), 0);
    lv_obj_set_style_border_color(status_bar, lv_color_hex(0x18243C), 0);
    lv_obj_set_style_border_width(status_bar, 1, 0);
    lv_obj_set_style_border_side(status_bar, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_pad_hor(status_bar, 8, 0);
    lv_obj_set_style_pad_ver(status_bar, 2, 0);
    lv_obj_set_flex_flow(status_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(status_bar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Badge de Modo (BLUE ou RED)
    badge_mode = lv_obj_create(status_bar);
    theme_follow(badge_mode);
    lv_obj_set_size(badge_mode, 54, 22);
    lv_obj_set_style_bg_color(badge_mode, lv_color_hex(0x051E2E), 0);
    lv_obj_set_style_border_color(badge_mode, COLOR_CYAN, 0);
    lv_obj_set_style_border_width(badge_mode, 1, 0);
    lv_obj_set_style_radius(badge_mode, 11, 0);
    lv_obj_set_style_pad_all(badge_mode, 0, 0);
    lbl_mode = lv_label_create(badge_mode);
    lv_label_set_text(lbl_mode, "BLUE");
    lv_obj_set_style_text_color(lbl_mode, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(lbl_mode, &lv_font_montserrat_12, 0);
    theme_follow(lbl_mode);
    lv_obj_center(lbl_mode);

    // Ícone Wi-Fi
    lbl_wifi = lv_label_create(status_bar);
    lv_label_set_text(lbl_wifi, LV_SYMBOL_WIFI " 0");
    lv_obj_set_style_text_color(lbl_wifi, COLOR_GREY, 0);
    lv_obj_set_style_text_font(lbl_wifi, &lv_font_montserrat_12, 0);

    // Ícone BLE
    lbl_ble = lv_label_create(status_bar);
    lv_label_set_text(lbl_ble, LV_SYMBOL_BLUETOOTH " 0");
    lv_obj_set_style_text_color(lbl_ble, COLOR_GREY, 0);
    lv_obj_set_style_text_font(lbl_ble, &lv_font_montserrat_12, 0);

    // Ícone SD
    lbl_sd = lv_label_create(status_bar);
    lv_label_set_text(lbl_sd, LV_SYMBOL_SD_CARD);
    lv_obj_set_style_text_color(lbl_sd, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(lbl_sd, &lv_font_montserrat_12, 0);

    // Bateria
    lbl_battery = lv_label_create(status_bar);
    lv_label_set_text(lbl_battery, LV_SYMBOL_BATTERY_3 " ---%");
    lv_obj_set_style_text_color(lbl_battery, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_battery, &lv_font_montserrat_12, 0);

    // Temperatura
    lbl_temp = lv_label_create(status_bar);
    lv_label_set_text(lbl_temp, "28C");
    lv_obj_set_style_text_color(lbl_temp, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(lbl_temp, &lv_font_montserrat_12, 0);

    // Tileview com 8 Tiles
    tileview = lv_tileview_create(screen);
    lv_obj_set_size(tileview, 368, 414);
    lv_obj_align(tileview, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(tileview, COLOR_AMOLED_BG, 0);

    // Tile 0: Pet
    lv_obj_t *t0 = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_HOR);
    lv_obj_set_style_bg_color(t0, COLOR_AMOLED_BG, 0);
    create_tile_pet(t0);

    // Tile 1: Wi-Fi Radar & Espectro
    lv_obj_t *t1 = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_HOR);
    lv_obj_set_style_bg_color(t1, COLOR_AMOLED_BG, 0);
    create_tile_wifi(t1);

    // Tile 2: BLE Sonar
    lv_obj_t *t2 = lv_tileview_add_tile(tileview, 2, 0, LV_DIR_HOR);
    lv_obj_set_style_bg_color(t2, COLOR_AMOLED_BG, 0);
    create_tile_ble(t2);

    // Tile 3: Arsenal RED
    lv_obj_t *t3 = lv_tileview_add_tile(tileview, 3, 0, LV_DIR_HOR);
    lv_obj_set_style_bg_color(t3, COLOR_AMOLED_BG, 0);
    create_tile_red(t3);

    // Tile 4: Capturas PCAP
    lv_obj_t *t4 = lv_tileview_add_tile(tileview, 4, 0, LV_DIR_HOR);
    lv_obj_set_style_bg_color(t4, COLOR_AMOLED_BG, 0);
    create_tile_capture(t4);

    // Tile 5: Hardware HUD, IMU & Temas
    lv_obj_t *t5 = lv_tileview_add_tile(tileview, 5, 0, LV_DIR_HOR);
    lv_obj_set_style_bg_color(t5, COLOR_AMOLED_BG, 0);
    create_tile_hardware(t5);

    // Tile 6: Relógio & Watchface Cyberpunk
    lv_obj_t *t6 = lv_tileview_add_tile(tileview, 6, 0, LV_DIR_HOR);
    lv_obj_set_style_bg_color(t6, COLOR_AMOLED_BG, 0);
    create_tile_clock(t6);

    // Tile 7: Configurações
    lv_obj_t *t7 = lv_tileview_add_tile(tileview, 7, 0, LV_DIR_HOR);
    lv_obj_set_style_bg_color(t7, COLOR_AMOLED_BG, 0);
    create_tile_settings(t7);

    // Indicador de páginas (8 pontos) na base da tela — não intercepta toques
    dots_row = lv_obj_create(screen);
    lv_obj_remove_style_all(dots_row);
    lv_obj_set_size(dots_row, 368, 14);
    lv_obj_align(dots_row, LV_ALIGN_BOTTOM_MID, 0, -2);
    lv_obj_set_flex_flow(dots_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(dots_row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(dots_row, 6, 0);
    lv_obj_clear_flag(dots_row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(dots_row, LV_OBJ_FLAG_CLICKABLE);
    for (int i = 0; i < num_tiles; i++) {
        page_dots[i] = lv_obj_create(dots_row);
        lv_obj_set_size(page_dots[i], 6, 6);
        lv_obj_set_style_radius(page_dots[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(page_dots[i], COLOR_GREY, 0);
        lv_obj_set_style_bg_opa(page_dots[i], LV_OPA_40, 0);
        lv_obj_set_style_border_width(page_dots[i], 0, 0);
        lv_obj_clear_flag(page_dots[i], LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_clear_flag(page_dots[i], LV_OBJ_FLAG_CLICKABLE);
    }

    // Restaura o tema salvo em /config.json (o tema agora persiste entre boots)
    int saved_th = ConfigManager::getInstance()->getInt("disp_theme", (int)current_ui_theme);
    if (saved_th < 0 || saved_th > 3) saved_th = 0;
    PwnUI::setTheme((UITheme)saved_th);

    // Mostra o Splash screen cinematográfico
    showBootSplash();

    Serial.println("[PwnUI] Sistema visual completo com 8 telas (pet, wifi, ble, red, capturas, hardware, relogio, config)!");
}

// Sincroniza o tile ativo (quando o usuario desliza com o dedo, current_tile
// ficava dessincronizado) e atualiza o indicador de páginas
static void sync_active_tile_dots() {
    if (!tileview) return;
    lv_obj_t *act = lv_tileview_get_tile_active(tileview);
    if (act) {
        int32_t idx = lv_obj_get_index(act);
        if (idx >= 0 && idx < num_tiles) current_tile = idx;
    }
    for (int i = 0; i < num_tiles; i++) {
        if (!page_dots[i]) continue;
        bool on = (i == current_tile);
        lv_obj_set_style_bg_color(page_dots[i], on ? theme_accent_color : COLOR_GREY, 0);
        lv_obj_set_style_bg_opa(page_dots[i], on ? LV_OPA_COVER : LV_OPA_40, 0);
    }
}

void PwnUI::update() {
    // Tick de animação do Pet (piscar, saccade, expressao)
    FaceHandler::tick();

    if (millis() - last_update < 300) return;
    last_update = millis();
    updateStatusBar();
    updatePetStats();
    sync_active_tile_dots();

    // Info do PMU (Tile 5: Hardware & IMU)
    if (lbl_pmu_info) {
        char pbuf[96];
        snprintf(pbuf, sizeof(pbuf), "%.2f V • %s • %.0f C • %s",
                 PwnPower::pmu_ok ? PwnPower::getBatteryVoltage() : 0.0f,
                 PwnPower::isCharging() ? "carregando" : "bateria",
                 (double)PwnPower::getTemperature(),
                 PwnPower::last_vbus ? "USB-C" : "sem cabo");
        lv_label_set_text(lbl_pmu_info, pbuf);
    }

    // ---- Wi-Fi: telemetria e lista de redes ao vivo (Tile 1) ----
    int ap_n = WiFiTools::getAPCount();
    if (lbl_ap_count_big) {
        char buf[24];
        snprintf(buf, sizeof(buf), "APs: %d", ap_n);
        lv_label_set_text(lbl_ap_count_big, buf);
    }
    if (lbl_wifi_open) {
        int open_n = 0;
        for (int i = 0; i < ap_n; i++)
            if (WiFiTools::getAP(i).encryption == 0) open_n++;
        char buf[24];
        snprintf(buf, sizeof(buf), "Abertas: %d", open_n);
        lv_label_set_text(lbl_wifi_open, buf);
    }
    if (lbl_wifi_frames) {
        char buf[24];
        snprintf(buf, sizeof(buf), "Frames: %u", (unsigned)WiFiTools::frames_captured);
        lv_label_set_text(lbl_wifi_frames, buf);
    }
    if (lbl_wifi_eapol) {
        char buf[24];
        snprintf(buf, sizeof(buf), "EAPOL: %u", (unsigned)WiFiTools::eapol_count);
        lv_label_set_text(lbl_wifi_eapol, buf);
    }
    if (lbl_wifi_espnow) {
        char buf[24];
        snprintf(buf, sizeof(buf), "ESP-NOW: %d", EspNowScan::count());
        lv_label_set_text(lbl_wifi_espnow, buf);
    }
    for (int i = 0; i < 4; i++) {
        if (!lbl_ap_rows[i]) continue;
        if (i < ap_n) {
            SniffedDevice dev = WiFiTools::getAP(i);
            char nm[20];
            strncpy(nm, dev.ssid.length() ? dev.ssid.c_str() : "(rede oculta)", sizeof(nm) - 1);
            nm[sizeof(nm) - 1] = 0;
            bool open = (dev.encryption == 0);
            char row[52];
            snprintf(row, sizeof(row), "%-19s %3ddBm CH%-2d %s",
                     nm, dev.rssi, dev.channel, open ? "ABERTA" : "");
            lv_label_set_text(lbl_ap_rows[i], row);
            lv_obj_set_style_text_color(lbl_ap_rows[i], open ? COLOR_GREEN : COLOR_WHITE, 0);
        } else {
            lv_label_set_text(lbl_ap_rows[i], "");
        }
    }
    if (lbl_wifi_scan_state) {
        bool on = WiFiTools::isSniffing();
        lv_label_set_text(lbl_wifi_scan_state, on ? "SCAN ATIVO" : "SCAN PARADO");
        lv_obj_set_style_text_color(lbl_wifi_scan_state, on ? COLOR_GREEN : COLOR_RED, 0);
    }
    if (lbl_sniff_state) {
        lv_label_set_text(lbl_sniff_state, WiFiTools::isSniffing() ? "Sniffer: ON" : "Sniffer: OFF");
    }

    // Atualiza Nível Bolha / Horizonte Artificial com QMI8658 (Tile 5)
    float ax = 0, ay = 0, az = 0;
    if (UI_getAccel(ax, ay, az)) {
        int bx = constrain((int)(ax * 30.0f), -20, 20);
        int by = constrain((int)(ay * 22.0f), -16, 16);
        if (horizon_bubble) lv_obj_align(horizon_bubble, LV_ALIGN_CENTER, bx, by);
        if (lbl_imu_angles) {
            char b[32];
            snprintf(b, sizeof(b), "X:%+.0f° Y:%+.0f°", ax * 90.0f, ay * 90.0f);
            lv_label_set_text(lbl_imu_angles, b);
        }
    }

    // Atualiza Relógio e Watchface (Tile 6)
    String t_str = PwnRTC::getTimeString();
    if (lbl_clock_time && t_str.length() > 0) {
        lv_label_set_text(lbl_clock_time, t_str.c_str());
    }
    time_t rawtime = time(nullptr);
    struct tm *ti = localtime(&rawtime);
    if (ti && arc_clock_sec) {
        int sec_angle = (ti->tm_sec * 360) / 60;
        lv_arc_set_angles(arc_clock_sec, 0, sec_angle);
    }
    if (lbl_clock_pet_badge) {
        char p_buf[64];
        snprintf(p_buf, sizeof(p_buf), "LELE: %s • Fome: %d%% • Humor: %d%%",
                 FaceHandler::getFaceAscii(), PwnPet::stats.hunger, PwnPet::stats.happiness);
        lv_label_set_text(lbl_clock_pet_badge, p_buf);
    }

    // ---- BLE: classificação, lista e foxhunt (Tile 2) ----
    if (lbl_ble_total) {
        char buf[24];
        snprintf(buf, sizeof(buf), "BLE: %d", BleBlue::count());
        lv_label_set_text(lbl_ble_total, buf);
    }
    if (lbl_ble_apple) {
        int a = BleBlue::countVendor(VND_APPLE_CONTINUITY) + BleBlue::countVendor(VND_APPLE_FINDMY)
              + BleBlue::countVendor(VND_AIRPODS) + BleBlue::countVendor(VND_IBEACON);
        char buf[24];
        snprintf(buf, sizeof(buf), "Apple: %d", a);
        lv_label_set_text(lbl_ble_apple, buf);
    }
    if (lbl_ble_flipper) {
        char buf[24];
        snprintf(buf, sizeof(buf), "Flipper: %d", BleBlue::countVendor(VND_FLIPPER));
        lv_label_set_text(lbl_ble_flipper, buf);
    }
    if (lbl_ble_tags) {
        char buf[24];
        snprintf(buf, sizeof(buf), "Tags: %d", BleBlue::countVendor(VND_TILE));
        lv_label_set_text(lbl_ble_tags, buf);
    }
    if (lbl_ble_hid) {
        char buf[24];
        snprintf(buf, sizeof(buf), "HID: %d", BleBlue::countHid());
        lv_label_set_text(lbl_ble_hid, buf);
    }
    for (int i = 0; i < 3; i++) {
        if (!lbl_ble_rows[i]) continue;
        const BleDevice *dv = BleBlue::get(i);
        if (dv && dv->used) {
            const char *src = (dv->ad.has_name && dv->ad.name[0]) ? dv->ad.name
                                                                  : bleVendorName(dv->ad.vendor);
            char nm[18];
            strncpy(nm, src, sizeof(nm) - 1);
            nm[sizeof(nm) - 1] = 0;
            char row[44];
            snprintf(row, sizeof(row), "%-17s %4ddBm", nm, dv->rssi);
            lv_label_set_text(lbl_ble_rows[i], row);
        } else {
            lv_label_set_text(lbl_ble_rows[i], "");
        }
    }
    if (arc_foxhunt && lbl_foxhunt_rssi) {
        if (BleIds::foxhuntActive()) {
            int r = BleIds::foxhuntRssi();
            int val = constrain(((r + 100) * 100) / 70, 0, 100);  // -100..-30 dBm -> 0..100
            lv_arc_set_value(arc_foxhunt, val);
            char fb[16];
            snprintf(fb, sizeof(fb), "%d dBm", r);
            lv_label_set_text(lbl_foxhunt_rssi, fb);
        } else {
            lv_arc_set_value(arc_foxhunt, 0);
            lv_label_set_text(lbl_foxhunt_rssi, "inativo");
        }
    }

    // ---- RED: contadores reais (Tile 3) ----
    if (lbl_espnow_info) {
        char buf[24];
        snprintf(buf, sizeof(buf), "%d peers", EspNowScan::count());
        lv_label_set_text(lbl_espnow_info, buf);
    }
    if (lbl_wps_info) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d ON / %d LOCK",
                 WpsBlue::countWpsOn(), WpsBlue::countWpsLocked());
        lv_label_set_text(lbl_wps_info, buf);
    }

    // ---- Capturas: SD e console ao vivo (Tile 4) ----
    if (lbl_sd_space && bar_sd_space) {
        if (SD_MMC.cardSize() > 0 && SD_MMC.totalBytes() > 0) {
            uint64_t tot = SD_MMC.totalBytes(), used = SD_MMC.usedBytes();
            int pct = (int)(used * 100 / tot);
            char b[48];
            snprintf(b, sizeof(b), "Cartao SD: %u/%u MB usados",
                     (unsigned)(used >> 20), (unsigned)(tot >> 20));
            lv_label_set_text(lbl_sd_space, b);
            lv_bar_set_value(bar_sd_space, pct, LV_ANIM_OFF);
        } else {
            lv_label_set_text(lbl_sd_space, "Cartao SD: Nao montado");
        }
    }
    if (lbl_pcap_console) {
        char cb[192];
        snprintf(cb, sizeof(cb),
                 "[+] Frames: %u   EAPOL: %u\n"
                 "[i] Sniffer: %s   Portal: %s\n"
                 "[*] PCAP: %s",
                 (unsigned)WiFiTools::frames_captured, (unsigned)WiFiTools::eapol_count,
                 WiFiTools::isSniffing() ? "ATIVO" : "PARADO",
                 EvilPortal::isActive() ? "ON" : "OFF",
                 WiFiTools::pcap_path.length() ? WiFiTools::pcap_path.c_str() : "aguardando inicio");
        lv_label_set_text(lbl_pcap_console, cb);
    }

    // Atualiza Handshakes no PCAP
    if (lbl_handshakes_big) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d HANDSHAKES", (int)WiFiTools::eapol_count);
        lv_label_set_text(lbl_handshakes_big, buf);
    }

    // Atualiza Hardware Arcs
    if (arc_battery) {
        int pct = PwnPower::getBatteryPercent();
        lv_arc_set_value(arc_battery, pct);
        if (lbl_arc_battery) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%d%%", pct);
            lv_label_set_text(lbl_arc_battery, buf);
        }
    }
    if (arc_temp) {
        float temp = PwnPower::getTemperature();
        lv_arc_set_value(arc_temp, (int)temp);
        if (lbl_arc_temp) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%.0fC", temp);
            lv_label_set_text(lbl_arc_temp, buf);
        }
    }
}

void PwnUI::nextTile() {
    if (current_tile < num_tiles - 1) {
        current_tile++;
        lv_tileview_set_tile_by_index(tileview, current_tile, 0, LV_ANIM_ON);
    }
}

void PwnUI::prevTile() {
    if (current_tile > 0) {
        current_tile--;
        lv_tileview_set_tile_by_index(tileview, current_tile, 0, LV_ANIM_ON);
    }
}

int PwnUI::getCurrentTile() { return current_tile; }

void PwnUI::setTile(int tile) {
    if (tile >= 0 && tile < num_tiles) {
        current_tile = tile;
        lv_tileview_set_tile_by_index(tileview, current_tile, 0, LV_ANIM_ON);
    }
}

void PwnUI::showNotification(const char* msg, uint32_t color) {
    // Alerta com botao OK + auto-close: antes o alerta ficava para sempre na
    // tela, bloqueando qualquer interacao (toque) ate reiniciar o aparelho.
    alert_destroy();
    active_alert = lv_obj_create(lv_screen_active());
    lv_obj_set_size(active_alert, 300, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(active_alert, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(active_alert, lv_color_hex(color), 0);
    lv_obj_set_style_border_width(active_alert, 2, 0);
    lv_obj_set_style_radius(active_alert, 14, 0);
    lv_obj_set_style_pad_all(active_alert, 14, 0);
    lv_obj_clear_flag(active_alert, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(active_alert);

    lv_obj_t *lbl = lv_label_create(active_alert);
    lv_label_set_text(lbl, msg);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(lbl, 270);
    lv_obj_set_style_text_color(lbl, lv_color_hex(color), 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *ok = lv_button_create(active_alert);
    lv_obj_set_size(ok, 90, 34);
    lv_obj_set_style_bg_color(ok, COLOR_DARK_GREY, 0);
    lv_obj_set_style_border_color(ok, lv_color_hex(color), 0);
    lv_obj_set_style_border_width(ok, 1, 0);
    lv_obj_set_style_radius(ok, 10, 0);
    lv_obj_align(ok, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(ok, alert_ok_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *okl = lv_label_create(ok);
    lv_label_set_text(okl, "OK");
    lv_obj_set_style_text_color(okl, COLOR_WHITE, 0);
    lv_obj_center(okl);

    // Fecha sozinho apos 6 s
    alert_timer = lv_timer_create(alert_auto_close_cb, 6000, NULL);
}

void PwnUI::showAlert(const char* msg, uint32_t color) { 
    showNotification(msg, color); 
}

void PwnUI::showToast(const char* msg) {
    // Remove o toast anterior (eles se acumulavam e nunca sumiam)
    if (active_toast) { lv_obj_del(active_toast); active_toast = nullptr; }
    if (toast_timer)  { lv_timer_del(toast_timer); toast_timer = nullptr; }

    active_toast = lv_obj_create(lv_screen_active());
    lv_obj_set_size(active_toast, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(active_toast, COLOR_CARD_BG, 0);
    lv_obj_set_style_border_color(active_toast, theme_accent_color, 0);
    lv_obj_set_style_border_width(active_toast, 1, 0);
    lv_obj_set_style_radius(active_toast, 8, 0);
    lv_obj_set_style_pad_all(active_toast, 8, 0);
    lv_obj_clear_flag(active_toast, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(active_toast, LV_ALIGN_BOTTOM_MID, 0, -50);
    lv_obj_t *tlbl = lv_label_create(active_toast);
    lv_label_set_text(tlbl, msg);
    lv_obj_set_style_text_color(tlbl, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(tlbl, &lv_font_montserrat_12, 0);
    toast_timer = lv_timer_create(toast_auto_delete_cb, 2600, NULL);
}

void PwnUI::updateStatusBar() {
    if (!status_bar) return;

    // Atualiza Bateria
    int pct = PwnPower::getBatteryPercent();
    bool charging = PwnPower::isCharging();
    char buf[32];
    const char *bat_sym = LV_SYMBOL_BATTERY_FULL;
    if (pct < 20) bat_sym = LV_SYMBOL_BATTERY_EMPTY;
    else if (pct < 45) bat_sym = LV_SYMBOL_BATTERY_1;
    else if (pct < 70) bat_sym = LV_SYMBOL_BATTERY_2;
    else if (pct < 90) bat_sym = LV_SYMBOL_BATTERY_3;

    snprintf(buf, sizeof(buf), "%s%s %d%%", charging ? LV_SYMBOL_CHARGE : "", bat_sym, pct);
    lv_label_set_text(lbl_battery, buf);
    lv_obj_set_style_text_color(lbl_battery, pct > 20 ? COLOR_WHITE : COLOR_RED, 0);

    // Atualiza Wi-Fi & BLE
    int ap_count = WiFiTools::getAPCount();
    snprintf(buf, sizeof(buf), LV_SYMBOL_WIFI " %d", ap_count);
    lv_label_set_text(lbl_wifi, buf);
    lv_obj_set_style_text_color(lbl_wifi, ap_count > 0 ? COLOR_GREEN : COLOR_GREY, 0);

    int ble_count = BleBlue::count();
    snprintf(buf, sizeof(buf), LV_SYMBOL_BLUETOOTH " %d", ble_count);
    lv_label_set_text(lbl_ble, buf);
    lv_obj_set_style_text_color(lbl_ble, ble_count > 0 ? COLOR_CYAN : COLOR_GREY, 0);

    // Atualiza Temperatura
    snprintf(buf, sizeof(buf), "%.0fC", PwnPower::getTemperature());
    lv_label_set_text(lbl_temp, buf);

    // Atualiza Modo (ARM)
    if (arm_active) {
        lv_label_set_text(lbl_mode, "RED");
        lv_obj_set_style_text_color(lbl_mode, COLOR_RED, 0);
        lv_obj_set_style_border_color(badge_mode, COLOR_RED, 0);
        if (lbl_arm_title) lv_label_set_text(lbl_arm_title, "STATUS: ARMADO (Laboratorio)");
    } else {
        lv_label_set_text(lbl_mode, "BLUE");
        lv_obj_set_style_text_color(lbl_mode, theme_accent_color, 0);
        lv_obj_set_style_border_color(badge_mode, theme_accent_color, 0);
        if (lbl_arm_title) lv_label_set_text(lbl_arm_title, "STATUS: DESARMADO (Seguro)");
    }
}

void PwnUI::updateBattery(int pct, bool charging) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%s %d%%", charging ? LV_SYMBOL_CHARGE : LV_SYMBOL_BATTERY_FULL, pct);
    if (lbl_battery) lv_label_set_text(lbl_battery, buf);
}

void PwnUI::updateWiFi(bool connected, int ap_count) {
    char buf[24];
    snprintf(buf, sizeof(buf), LV_SYMBOL_WIFI " %d", ap_count);
    if (lbl_wifi) {
        lv_label_set_text(lbl_wifi, buf);
        lv_obj_set_style_text_color(lbl_wifi, connected ? COLOR_GREEN : COLOR_GREY, 0);
    }
}

void PwnUI::updateBLE(int count) {
    char buf[24];
    snprintf(buf, sizeof(buf), LV_SYMBOL_BLUETOOTH " %d", count);
    if (lbl_ble) lv_label_set_text(lbl_ble, buf);
}

void PwnUI::updateClock(const char* time_str) {
    (void)time_str;
}

void PwnUI::updatePetStats() {
    if (!lbl_pet_name) return;

    // Nível e Estágio
    char buf[32];
    snprintf(buf, sizeof(buf), "LVL %d", PwnPet::stats.level);
    lv_label_set_text(lbl_pet_lvl, buf);
    lv_label_set_text(lbl_pet_stage, PwnPet::getStageName());

    // XP
    int xp_curr = PwnPet::stats.xp % 100;
    snprintf(buf, sizeof(buf), "XP: %d / 100", xp_curr);
    lv_label_set_text(lbl_xp_val, buf);
    if (bar_xp) lv_bar_set_value(bar_xp, xp_curr, LV_ANIM_ON);

    // Fome e Felicidade
    snprintf(buf, sizeof(buf), "Fome: %d%%", PwnPet::stats.hunger);
    lv_label_set_text(lbl_hunger_val, buf);
    if (bar_hunger) lv_bar_set_value(bar_hunger, PwnPet::stats.hunger, LV_ANIM_ON);

    snprintf(buf, sizeof(buf), "Humor: %d%%", PwnPet::stats.happiness);
    lv_label_set_text(lbl_happiness_val, buf);
    if (bar_happiness) lv_bar_set_value(bar_happiness, PwnPet::stats.happiness, LV_ANIM_ON);

    // Handshakes
    snprintf(buf, sizeof(buf), "%d", PwnPet::stats.total_handshakes);
    lv_label_set_text(lbl_shakes_val, buf);
}

