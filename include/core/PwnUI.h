#ifndef PWN_UI_H
#define PWN_UI_H

#include <lvgl.h>
#include <time.h>
#include "core/PwnPet.h"
#include "core/PwnPower.h"
#include "core/PwnBLE.h"
#include "core/PwnAttack.h"
#include "core/PwnVoice.h"
#include "core/PwnSleep.h"
#include "core/ConfigManager.h"
#include "Gamification.h"
#include "WiFiTools.h"
#include "FaceHandler.h"
#include "AudioHandler.h"
#include "EvilPortal.h"
#include "drivers/PwnRTC.h"
#include "pin_config.h"

/**
 * PwnUI v3 — Sistema de RELÓGIO + LAUNCHER para o AMOLED 368x448.
 *
 *  HOME (relógio)  ── botão "APLICATIVOS" ─▶  LAUNCHER (grade de apps)
 *        │                                          │
 *        └── rosto animado + data + bateria         ├─ Wi-Fi   (sniffer / handshakes)
 *                                                   ├─ Bluetooth (scan BLE)
 *  Sem tela de carregamento. Som = apenas BEEP.     ├─ Ataques (Evil Portal / Deauth)
 *  Alvos de toque grandes (>=48px) e alinhados.     ├─ Pet     (alimentar / brincar)
 *                                                   ├─ Energia (bateria / modos)
 *                                                   ├─ Sistema (rede / SD / RTC)
 *                                                   ├─ Ferramentas (voz / nível)
 *                                                   └─ Ajustes (toggles reais)
 */

// ------------------------------- Paleta ---------------------------------------
#define UI_BG        lv_color_hex(0x05080E)
#define UI_PANEL     lv_color_hex(0x111826)
#define UI_PANEL2    lv_color_hex(0x1B2434)
#define UI_GREEN     lv_color_hex(0x3DE8A0)
#define UI_CYAN      lv_color_hex(0x3BD7FF)
#define UI_PURPLE    lv_color_hex(0xB58BFF)
#define UI_ORANGE    lv_color_hex(0xFFB14E)
#define UI_RED       lv_color_hex(0xFF6B84)
#define UI_TEXT      lv_color_hex(0xEAF1FF)
#define UI_DIM       lv_color_hex(0x74849B)

class PwnUI {
private:
    // Telas / páginas (todas filhas da screen ativa, sobrepostas, hidden salvo a atual)
    static lv_obj_t *scr_main;
    static lv_obj_t *page_home,  *page_apps,  *page_wifi, *page_ble, *page_atk;
    static lv_obj_t *page_pet,   *page_power, *page_sys,  *page_tools, *page_set;

    // Rótulos dinâmicos
    static lv_obj_t *lbl_time, *lbl_date, *lbl_batt, *lbl_home_pet, *lbl_status;
    static lv_obj_t *lbl_wifi, *lbl_ble, *lbl_power, *lbl_sys, *lbl_tools, *lbl_atk;
    static lv_obj_t *lbl_pet, *bar_hunger, *bar_happy;

    // Pilha de navegação (0 = home)
    static lv_obj_t *nav_stack[10];
    static int       nav_top;

    // ------------------------------------------------------------------ helpers
    static void beep() { AudioHandler::beep(2400, 22, 45); }

    static void showObj(lv_obj_t *o) { if (o) lv_obj_remove_flag(o, LV_OBJ_FLAG_HIDDEN); }
    static void hideObj(lv_obj_t *o) { if (o) lv_obj_add_flag(o, LV_OBJ_FLAG_HIDDEN); }

    static void pushPage(lv_obj_t *p) {
        if (!p || nav_top >= 9) return;
        hideObj(nav_stack[nav_top]);
        nav_stack[++nav_top] = p;
        lv_obj_scroll_to_y(p, 0, LV_ANIM_OFF);
        showObj(p);
        PwnSleep::notifyActivity();
        refresh();
    }
    static void back() {
        if (nav_top <= 0) return;
        hideObj(nav_stack[nav_top--]);
        showObj(nav_stack[nav_top]);
        PwnSleep::notifyActivity();
        refresh();
    }
    static void goHome() {
        while (nav_top > 0) hideObj(nav_stack[nav_top--]);
        showObj(page_home);
        PwnSleep::notifyActivity();
    }

    // -------- callbacks genéricos --------
    static void cbOpen(lv_event_t *e) { beep(); pushPage((lv_obj_t *)lv_event_get_user_data(e)); }
    static void cbBack(lv_event_t *e) { (void)e; beep(); back(); }
    static void cbHome(lv_event_t *e) { (void)e; beep(); goHome(); }

    // -------- callbacks de ação (somente funções REAIS) --------
    static void aWifiScan(lv_event_t *e){ (void)e; beep(); WiFiTools::beginNewCapture(); WiFiTools::startSnifferScan(); }
    static void aWifiPass(lv_event_t *e){ (void)e; beep(); WiFiTools::startSnifferPassive(); }
    static void aWifiStop(lv_event_t *e){ (void)e; beep(); WiFiTools::stopSniffer(); }
    static void aWifiCh   (lv_event_t *e){ (void)e; beep(); static uint8_t ch=1; ch = (ch%13)+1; WiFiTools::setChannel(ch); }

    static void aBleScan(lv_event_t *e){ (void)e; beep(); int n=PwnBLE::scan(4); if(n>0) PwnPet::feed(1); }

    static void aEvilStart(lv_event_t *e){ (void)e; beep();
        ConfigManager *c=ConfigManager::getInstance();
        EvilPortal::start(c->getString("sys_ap_ssid").c_str(),
                          c->getString("atk_portal_template").c_str()); }
    static void aEvilStop (lv_event_t *e){ (void)e; beep(); EvilPortal::stop(); }
    static void aDeauth   (lv_event_t *e){ (void)e; AudioHandler::beepWarn();
        // Deauth simulado (seguro) — só roda se habilitado nas Config.
        if (!ConfigManager::getInstance()->get<bool>("atk_deauth_enabled")) return;
        if (!WiFiTools::nearby_devices.empty())
            PwnAttack::deauthSimulated(WiFiTools::nearby_devices.front().mac); }
    static void aEvilTwin (lv_event_t *e){ (void)e; beep(); PwnAttack::evilTwin("Free_WiFi"); }

    static void aFeed(lv_event_t *e){ (void)e; AudioHandler::beepOk(); PwnPet::feed(10); FaceHandler::setFace(FACE_HAPPY); }
    static void aPlay(lv_event_t *e){ (void)e; beep(); FaceHandler::setFace(FACE_EXCITED); PwnPet::addHandshake(true); }

    static void aPerf(lv_event_t *e){ beep();
        int lvl = (int)(intptr_t)lv_event_get_user_data(e);
        PwnPower::setPerformanceMode(lvl);
        ConfigManager::getInstance()->set<int>("pwr_cpu_freq_max", lvl>=2?240:(lvl==1?160:80)); }
    static void aDeep(lv_event_t *e){ (void)e; AudioHandler::beepWarn(); PwnSleep::enterDeep(); }

    static void aNtp(lv_event_t *e){ (void)e; beep();
        ConfigManager *c=ConfigManager::getInstance();
        PwnRTC::syncNTP(c->getString("sys_ntp_server").c_str(), c->get<int>("sys_timezone")); }
    static void aReboot(lv_event_t *e){ (void)e; AudioHandler::beepWarn(); delay(120); ESP.restart(); }

    static void aVoiceListen(lv_event_t *e){ (void)e; beep(); PwnVoice::listen(); }
    static void aVoiceSpeak (lv_event_t *e){ (void)e; beep(); PwnVoice::speak("mini lele online"); }

    // Slider de brilho: aplica ao vivo; grava na config ao soltar.
    static void aBright(lv_event_t *e){
        lv_obj_t *s=(lv_obj_t*)lv_event_get_target(e);
        int v=lv_slider_get_value(s);
        PwnSleep::setFullBrightness(v);
        if (lv_event_get_code(e)==LV_EVENT_RELEASED){
            ConfigManager::getInstance()->set<int>("disp_brightness", v);
        }
    }
    static void aTimeout(lv_event_t *e){
        lv_obj_t *s=(lv_obj_t*)lv_event_get_target(e);
        int v=lv_slider_get_value(s);
        PwnSleep::setScreenTimeout(v);
        if (lv_event_get_code(e)==LV_EVENT_RELEASED)
            ConfigManager::getInstance()->set<int>("disp_timeout_sec", v);
    }
    // Switch genérico -> grava chave bool (user_data = const char* key)
    static void aToggle(lv_event_t *e){ beep();
        lv_obj_t *sw=(lv_obj_t*)lv_event_get_target(e);
        const char *key=(const char*)lv_event_get_user_data(e);
        bool on=lv_obj_has_state(sw, LV_STATE_CHECKED);
        ConfigManager::getInstance()->set<bool>(key, on);
    }
    // Tocar no rosto = carinho/comida
    static void aFaceTap(lv_event_t *e){ (void)e; AudioHandler::beepOk(); PwnPet::feed(4); FaceHandler::setFace(FACE_HAPPY); }

    // ------------------------------------------------------------ construtores
    static void baseCard(lv_obj_t *o, lv_color_t border) {
        lv_obj_set_style_bg_color(o, UI_PANEL, 0);
        lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(o, 16, 0);
        lv_obj_set_style_border_color(o, border, 0);
        lv_obj_set_style_border_width(o, 1, 0);
        lv_obj_set_style_border_opa(o, LV_OPA_40, 0);
        lv_obj_set_style_pad_all(o, 12, 0);
        lv_obj_set_style_shadow_width(o, 0, 0);
        lv_obj_remove_flag(o, LV_OBJ_FLAG_SCROLLABLE);
    }

    // Página em branco de tela cheia (escondida por padrão)
    static lv_obj_t *newPage() {
        lv_obj_t *p = lv_obj_create(scr_main);
        lv_obj_set_size(p, LCD_WIDTH, LCD_HEIGHT);
        lv_obj_set_pos(p, 0, 0);
        lv_obj_set_style_bg_color(p, UI_BG, 0);
        lv_obj_set_style_bg_opa(p, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(p, 0, 0);
        lv_obj_set_style_radius(p, 0, 0);
        lv_obj_set_style_pad_all(p, 0, 0);
        lv_obj_remove_flag(p, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(p, LV_OBJ_FLAG_HIDDEN);
        return p;
    }

    // Subpágina com cabeçalho (voltar + título). Retorna a área de conteúdo (flex col, rolável).
    static lv_obj_t *mkSub(const char *title, lv_color_t accent, lv_obj_t **pageOut) {
        lv_obj_t *p = newPage();
        *pageOut = p;

        lv_obj_t *hdr = lv_obj_create(p);
        lv_obj_set_size(hdr, LCD_WIDTH, 52);
        lv_obj_set_pos(hdr, 0, 0);
        lv_obj_set_style_bg_color(hdr, UI_PANEL, 0);
        lv_obj_set_style_bg_opa(hdr, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(hdr, 0, 0);
        lv_obj_set_style_border_side(hdr, LV_BORDER_SIDE_BOTTOM, 0);
        lv_obj_set_style_border_color(hdr, accent, 0);
        lv_obj_set_style_border_width(hdr, 2, 0);
        lv_obj_set_style_border_opa(hdr, LV_OPA_60, 0);
        lv_obj_set_style_radius(hdr, 0, 0);
        lv_obj_set_style_pad_all(hdr, 0, 0);
        lv_obj_remove_flag(hdr, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *bk = lv_button_create(hdr);
        lv_obj_set_size(bk, 52, 44);
        lv_obj_align(bk, LV_ALIGN_LEFT_MID, 4, 0);
        lv_obj_set_style_bg_color(bk, UI_PANEL2, 0);
        lv_obj_set_style_radius(bk, 12, 0);
        lv_obj_set_style_shadow_width(bk, 0, 0);
        lv_obj_t *bl = lv_label_create(bk);
        lv_label_set_text(bl, LV_SYMBOL_LEFT);
        lv_obj_set_style_text_color(bl, accent, 0);
        lv_obj_center(bl);
        lv_obj_add_event_cb(bk, cbBack, LV_EVENT_CLICKED, NULL);

        lv_obj_t *tt = lv_label_create(hdr);
        lv_obj_set_style_text_font(tt, &lv_font_montserrat_20, 0);
        lv_obj_set_style_text_color(tt, UI_TEXT, 0);
        lv_label_set_text(tt, title);
        lv_obj_align(tt, LV_ALIGN_CENTER, 12, 0);

        lv_obj_t *c = lv_obj_create(p);
        lv_obj_set_size(c, LCD_WIDTH, LCD_HEIGHT - 52);
        lv_obj_set_pos(c, 0, 52);
        lv_obj_set_style_bg_opa(c, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(c, 0, 0);
        lv_obj_set_style_radius(c, 0, 0);
        lv_obj_set_style_pad_all(c, 12, 0);
        lv_obj_set_style_pad_row(c, 10, 0);
        lv_obj_set_flex_flow(c, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(c, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        return c;
    }

    // Botão de ação (52px, texto escuro sobre cor viva)
    static lv_obj_t *mkBtn(lv_obj_t *parent, const char *txt, lv_color_t col,
                           lv_event_cb_t cb, void *ud = NULL, lv_coord_t w = LV_PCT(100)) {
        lv_obj_t *b = lv_button_create(parent);
        lv_obj_set_size(b, w, 52);
        lv_obj_set_style_radius(b, 14, 0);
        lv_obj_set_style_bg_color(b, col, 0);
        lv_obj_set_style_bg_opa(b, LV_OPA_COVER, 0);
        lv_obj_set_style_shadow_width(b, 0, 0);
        lv_obj_t *l = lv_label_create(b);
        lv_label_set_text(l, txt);
        lv_obj_set_style_text_color(l, UI_BG, 0);
        lv_obj_set_style_text_font(l, &lv_font_montserrat_16, 0);
        lv_obj_center(l);
        if (cb) lv_obj_add_event_cb(b, cb, LV_EVENT_CLICKED, ud);
        return b;
    }

    // Linha com N botões lado a lado
    static lv_obj_t *mkRow(lv_obj_t *parent) {
        lv_obj_t *r = lv_obj_create(parent);
        lv_obj_set_size(r, LV_PCT(100), 56);
        lv_obj_set_style_bg_opa(r, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(r, 0, 0);
        lv_obj_set_style_pad_all(r, 0, 0);
        lv_obj_set_style_pad_column(r, 8, 0);
        lv_obj_set_flex_flow(r, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(r, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_remove_flag(r, LV_OBJ_FLAG_SCROLLABLE);
        return r;
    }

    // Card informativo com título + corpo dinâmico. Retorna o rótulo de corpo.
    static lv_obj_t *mkInfo(lv_obj_t *parent, lv_color_t accent, const char *title) {
        lv_obj_t *card = lv_obj_create(parent);
        lv_obj_set_width(card, LV_PCT(100));
        lv_obj_set_height(card, LV_SIZE_CONTENT);
        baseCard(card, accent);
        lv_obj_set_style_pad_row(card, 6, 0);
        lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);

        lv_obj_t *t = lv_label_create(card);
        lv_label_set_text(t, title);
        lv_obj_set_style_text_font(t, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(t, accent, 0);

        lv_obj_t *body = lv_label_create(card);
        lv_obj_set_style_text_font(body, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(body, UI_TEXT, 0);
        lv_label_set_long_mode(body, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(body, LV_PCT(100));
        lv_label_set_text(body, "...");
        return body;
    }

    // Bloco: legenda + barra de progresso
    static lv_obj_t *mkBar(lv_obj_t *parent, const char *cap, lv_color_t fill) {
        lv_obj_t *l = lv_label_create(parent);
        lv_label_set_text(l, cap);
        lv_obj_set_style_text_font(l, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(l, UI_DIM, 0);
        lv_obj_t *bar = lv_bar_create(parent);
        lv_obj_set_size(bar, LV_PCT(100), 16);
        lv_bar_set_range(bar, 0, 100);
        lv_obj_set_style_radius(bar, 8, LV_PART_MAIN);
        lv_obj_set_style_radius(bar, 8, LV_PART_INDICATOR);
        lv_obj_set_style_bg_color(bar, UI_PANEL2, LV_PART_MAIN);
        lv_obj_set_style_bg_color(bar, fill, LV_PART_INDICATOR);
        return bar;
    }

    // Linha de toggle (label + switch) ligada a uma chave de config bool
    static void mkToggle(lv_obj_t *parent, const char *txt, const char *key) {
        lv_obj_t *row = lv_obj_create(parent);
        lv_obj_set_size(row, LV_PCT(100), 48);
        baseCard(row, UI_PANEL2);
        lv_obj_set_style_pad_all(row, 10, 0);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_t *l = lv_label_create(row);
        lv_label_set_text(l, txt);
        lv_obj_set_style_text_color(l, UI_TEXT, 0);
        lv_obj_set_style_text_font(l, &lv_font_montserrat_14, 0);
        lv_obj_t *sw = lv_switch_create(row);
        lv_obj_set_style_bg_color(sw, UI_GREEN, LV_PART_INDICATOR | LV_STATE_CHECKED);
        if (ConfigManager::getInstance()->get<bool>(key)) lv_obj_add_state(sw, LV_STATE_CHECKED);
        lv_obj_add_event_cb(sw, aToggle, LV_EVENT_VALUE_CHANGED, (void*)key);
    }

    // Slider rotulado
    static lv_obj_t *mkSlider(lv_obj_t *parent, const char *cap, int mn, int mx, int val,
                              lv_color_t col, lv_event_cb_t cb) {
        lv_obj_t *l = lv_label_create(parent);
        lv_label_set_text(l, cap);
        lv_obj_set_style_text_font(l, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(l, UI_DIM, 0);
        lv_obj_t *s = lv_slider_create(parent);
        lv_obj_set_width(s, LV_PCT(100));
        lv_obj_set_height(s, 16);
        lv_slider_set_range(s, mn, mx);
        lv_slider_set_value(s, val, LV_ANIM_OFF);
        lv_obj_set_style_bg_color(s, UI_PANEL2, LV_PART_MAIN);
        lv_obj_set_style_bg_color(s, col, LV_PART_INDICATOR);
        lv_obj_set_style_bg_color(s, col, LV_PART_KNOB);
        lv_obj_set_style_pad_all(s, 6, LV_PART_KNOB);
        lv_obj_add_event_cb(s, cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_event_cb(s, cb, LV_EVENT_RELEASED, NULL);
        return s;
    }

    // Tile do launcher (ícone + nome), abre a página em user_data
    static void mkTile(lv_obj_t *grid, const char *sym, const char *name,
                       lv_color_t col, lv_obj_t *target) {
        lv_obj_t *t = lv_obj_create(grid);
        lv_obj_set_size(t, 165, 100);
        baseCard(t, col);
        lv_obj_add_flag(t, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_t *ic = lv_label_create(t);
        lv_label_set_text(ic, sym);
        lv_obj_set_style_text_font(ic, &lv_font_montserrat_28, 0);
        lv_obj_set_style_text_color(ic, col, 0);
        lv_obj_align(ic, LV_ALIGN_TOP_MID, 0, 6);
        lv_obj_t *nm = lv_label_create(t);
        lv_label_set_text(nm, name);
        lv_obj_set_style_text_font(nm, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(nm, UI_TEXT, 0);
        lv_obj_align(nm, LV_ALIGN_BOTTOM_MID, 0, -4);
        lv_obj_add_event_cb(t, cbOpen, LV_EVENT_CLICKED, target);
    }

public:
    // ===================================================================== init
    static void init() {
        scr_main = lv_screen_active();
        lv_obj_set_style_bg_color(scr_main, UI_BG, 0);
        lv_obj_set_style_bg_opa(scr_main, LV_OPA_COVER, 0);
        lv_obj_remove_flag(scr_main, LV_OBJ_FLAG_SCROLLABLE);

        buildHome();
        buildLauncher();
        buildWifi();
        buildBle();
        buildAttacks();
        buildPet();
        buildPower();
        buildSystem();
        buildTools();
        buildSettings();

        nav_stack[0] = page_home;
        nav_top = 0;
        showObj(page_home);       // relógio direto — sem tela de carregamento
    }

    // -------------------------------------------------------------- HOME/RELÓGIO
    static void buildHome() {
        page_home = newPage();
        lv_obj_remove_flag(page_home, LV_OBJ_FLAG_HIDDEN);

        // Status (canto sup. esq.) — sniffer/portal
        lbl_status = lv_label_create(page_home);
        lv_obj_set_style_text_font(lbl_status, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(lbl_status, UI_DIM, 0);
        lv_label_set_text(lbl_status, LV_SYMBOL_WIFI);
        lv_obj_align(lbl_status, LV_ALIGN_TOP_LEFT, 16, 14);

        // Bateria (canto sup. dir.)
        lbl_batt = lv_label_create(page_home);
        lv_obj_set_style_text_font(lbl_batt, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(lbl_batt, UI_GREEN, 0);
        lv_label_set_text(lbl_batt, LV_SYMBOL_BATTERY_FULL " --%");
        lv_obj_align(lbl_batt, LV_ALIGN_TOP_RIGHT, -16, 12);

        // Relógio grande HH:MM
        lbl_time = lv_label_create(page_home);
        lv_obj_set_style_text_font(lbl_time, &lv_font_montserrat_48, 0);
        lv_obj_set_style_text_color(lbl_time, UI_TEXT, 0);
        lv_label_set_text(lbl_time, "00:00");
        lv_obj_align(lbl_time, LV_ALIGN_TOP_MID, 0, 62);

        // Data
        lbl_date = lv_label_create(page_home);
        lv_obj_set_style_text_font(lbl_date, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(lbl_date, UI_PURPLE, 0);
        lv_label_set_text(lbl_date, "-- -- ----");
        lv_obj_align(lbl_date, LV_ALIGN_TOP_MID, 0, 122);

        // Painel do rosto animado (hero pwnagotchi)
        lv_obj_t *facePanel = lv_obj_create(page_home);
        lv_obj_set_size(facePanel, 220, 116);
        lv_obj_align(facePanel, LV_ALIGN_CENTER, 0, 14);
        baseCard(facePanel, UI_GREEN);
        lv_obj_add_flag(facePanel, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(facePanel, aFaceTap, LV_EVENT_CLICKED, NULL);
        FaceHandler::init(facePanel);

        // Nome + humor do pet
        lbl_home_pet = lv_label_create(page_home);
        lv_obj_set_style_text_font(lbl_home_pet, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(lbl_home_pet, UI_DIM, 0);
        lv_label_set_text(lbl_home_pet, "PwnBaby");
        lv_obj_align(lbl_home_pet, LV_ALIGN_CENTER, 0, 92);

        // Botão APLICATIVOS (alvo de toque grande no rodapé)
        lv_obj_t *b = lv_button_create(page_home);
        lv_obj_set_size(b, LCD_WIDTH - 40, 58);
        lv_obj_align(b, LV_ALIGN_BOTTOM_MID, 0, -18);
        lv_obj_set_style_radius(b, 18, 0);
        lv_obj_set_style_bg_color(b, UI_PANEL, 0);
        lv_obj_set_style_border_color(b, UI_CYAN, 0);
        lv_obj_set_style_border_width(b, 1, 0);
        lv_obj_set_style_border_opa(b, LV_OPA_60, 0);
        lv_obj_set_style_shadow_width(b, 0, 0);
        lv_obj_t *bl = lv_label_create(b);
        lv_label_set_text(bl, LV_SYMBOL_LIST "  APLICATIVOS");
        lv_obj_set_style_text_color(bl, UI_CYAN, 0);
        lv_obj_set_style_text_font(bl, &lv_font_montserrat_20, 0);
        lv_obj_center(bl);
        lv_obj_add_event_cb(b, cbAppsBtn, LV_EVENT_CLICKED, NULL);
    }
    static void cbAppsBtn(lv_event_t *e){ (void)e; beep(); pushPage(page_apps); }

    // ---------------------------------------------------------------- LAUNCHER
    static void buildLauncher() {
        page_apps = newPage();
        lv_obj_t *hdr = lv_obj_create(page_apps);
        lv_obj_set_size(hdr, LCD_WIDTH, 52);
        lv_obj_set_style_bg_color(hdr, UI_PANEL, 0);
        lv_obj_set_style_bg_opa(hdr, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(hdr, 2, 0);
        lv_obj_set_style_border_side(hdr, LV_BORDER_SIDE_BOTTOM, 0);
        lv_obj_set_style_border_color(hdr, UI_CYAN, 0);
        lv_obj_set_style_border_opa(hdr, LV_OPA_60, 0);
        lv_obj_set_style_radius(hdr, 0, 0);
        lv_obj_set_style_pad_all(hdr, 0, 0);
        lv_obj_remove_flag(hdr, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_t *bk = lv_button_create(hdr);
        lv_obj_set_size(bk, 52, 44);
        lv_obj_align(bk, LV_ALIGN_LEFT_MID, 4, 0);
        lv_obj_set_style_bg_color(bk, UI_PANEL2, 0);
        lv_obj_set_style_radius(bk, 12, 0);
        lv_obj_set_style_shadow_width(bk, 0, 0);
        lv_obj_t *bl = lv_label_create(bk); lv_label_set_text(bl, LV_SYMBOL_HOME);
        lv_obj_set_style_text_color(bl, UI_CYAN, 0); lv_obj_center(bl);
        lv_obj_add_event_cb(bk, cbHome, LV_EVENT_CLICKED, NULL);
        lv_obj_t *tt = lv_label_create(hdr);
        lv_label_set_text(tt, "APLICATIVOS");
        lv_obj_set_style_text_font(tt, &lv_font_montserrat_20, 0);
        lv_obj_set_style_text_color(tt, UI_TEXT, 0);
        lv_obj_align(tt, LV_ALIGN_CENTER, 12, 0);

        lv_obj_t *grid = lv_obj_create(page_apps);
        lv_obj_set_size(grid, LCD_WIDTH, LCD_HEIGHT - 52);
        lv_obj_set_pos(grid, 0, 52);
        lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(grid, 0, 0);
        lv_obj_set_style_pad_all(grid, 12, 0);
        lv_obj_set_style_pad_row(grid, 12, 0);
        lv_obj_set_style_pad_column(grid, 10, 0);
        lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
        lv_obj_set_flex_align(grid, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

        mkTile(grid, LV_SYMBOL_WIFI,      "Wi-Fi",     UI_GREEN,  page_wifi);
        mkTile(grid, LV_SYMBOL_BLUETOOTH, "Bluetooth", UI_CYAN,   page_ble);
        mkTile(grid, LV_SYMBOL_WARNING,   "Ataques",   UI_RED,    page_atk);
        mkTile(grid, LV_SYMBOL_EYE_OPEN,  "Pet",       UI_PURPLE, page_pet);
        mkTile(grid, LV_SYMBOL_CHARGE,    "Energia",   UI_ORANGE, page_power);
        mkTile(grid, LV_SYMBOL_DRIVE,     "Sistema",   UI_CYAN,   page_sys);
        mkTile(grid, LV_SYMBOL_AUDIO,     "Ferram.",   UI_GREEN,  page_tools);
        mkTile(grid, LV_SYMBOL_SETTINGS,  "Ajustes",   UI_DIM,    page_set);
    }

    // -------------------------------------------------------------------- Wi-Fi
    static void buildWifi() {
        lv_obj_t *c = mkSub("Wi-Fi", UI_GREEN, &page_wifi);
        lbl_wifi = mkInfo(c, UI_GREEN, "SNIFFER / CAPTURA");
        lv_obj_t *r1 = mkRow(c);
        mkBtn(r1, "SCAN",  UI_GREEN, aWifiScan, NULL, 108);
        mkBtn(r1, "PASSIVO", UI_CYAN, aWifiPass, NULL, 108);
        mkBtn(r1, "PARAR", UI_RED,   aWifiStop, NULL, 108);
        lv_obj_t *r2 = mkRow(c);
        mkBtn(r2, LV_SYMBOL_REFRESH " CANAL",   UI_ORANGE, aWifiCh, NULL, 165);
        mkBtn(r2, LV_SYMBOL_SAVE " NOVA CAP.", UI_PURPLE, aWifiScan, NULL, 165);
    }

    // ---------------------------------------------------------------- Bluetooth
    static void buildBle() {
        lv_obj_t *c = mkSub("Bluetooth", UI_CYAN, &page_ble);
        lbl_ble = mkInfo(c, UI_CYAN, "SCANNER BLE");
        mkBtn(c, LV_SYMBOL_BLUETOOTH "  ESCANEAR BLE", UI_CYAN, aBleScan);
    }

    // ------------------------------------------------------------------ Ataques
    static void buildAttacks() {
        lv_obj_t *c = mkSub("Ataques", UI_RED, &page_atk);
        lbl_atk = mkInfo(c, UI_RED, "STATUS");
        mkBtn(c, LV_SYMBOL_WARNING "  EVIL PORTAL: INICIAR", UI_RED, aEvilStart);
        mkBtn(c, LV_SYMBOL_STOP "  EVIL PORTAL: PARAR",   UI_ORANGE, aEvilStop);
        mkBtn(c, LV_SYMBOL_WIFI "  EVIL TWIN AP", UI_PURPLE, aEvilTwin);
        mkBtn(c, LV_SYMBOL_CLOSE "  DEAUTH (sim)", UI_RED, aDeauth);
    }

    // ---------------------------------------------------------------------- Pet
    static void buildPet() {
        lv_obj_t *c = mkSub("Pet", UI_PURPLE, &page_pet);
        lbl_pet = mkInfo(c, UI_PURPLE, "PWNAGOTCHI");
        bar_hunger = mkBar(c, "FOME", UI_ORANGE);
        bar_happy  = mkBar(c, "FELICIDADE", UI_GREEN);
        lv_obj_t *r = mkRow(c);
        mkBtn(r, LV_SYMBOL_OK " ALIMENTAR", UI_GREEN, aFeed, NULL, 165);
        mkBtn(r, LV_SYMBOL_PLAY " BRINCAR",  UI_PURPLE, aPlay, NULL, 165);
    }

    // ------------------------------------------------------------------ Energia
    static void buildPower() {
        lv_obj_t *c = mkSub("Energia", UI_ORANGE, &page_power);
        lbl_power = mkInfo(c, UI_ORANGE, "BATERIA / SISTEMA");
        lv_obj_t *r = mkRow(c);
        mkBtn(r, "ECO",    UI_GREEN,  aPerf, (void*)(intptr_t)0, 105);
        mkBtn(r, "NORMAL", UI_CYAN,   aPerf, (void*)(intptr_t)1, 105);
        mkBtn(r, "TURBO",  UI_ORANGE, aPerf, (void*)(intptr_t)2, 105);
        mkBtn(c, LV_SYMBOL_POWER "  DEEP SLEEP", UI_RED, aDeep);
    }

    // ------------------------------------------------------------------ Sistema
    static void buildSystem() {
        lv_obj_t *c = mkSub("Sistema", UI_CYAN, &page_sys);
        lbl_sys = mkInfo(c, UI_CYAN, "DISPOSITIVO / REDE");
        mkBtn(c, LV_SYMBOL_REFRESH "  SINCRONIZAR HORA (NTP)", UI_GREEN, aNtp);
        mkBtn(c, LV_SYMBOL_POWER "  REINICIAR", UI_RED, aReboot);
    }

    // -------------------------------------------------------------- Ferramentas
    static void buildTools() {
        lv_obj_t *c = mkSub("Ferramentas", UI_GREEN, &page_tools);
        lbl_tools = mkInfo(c, UI_GREEN, "SENSORES / VOZ");
        mkBtn(c, LV_SYMBOL_AUDIO "  OUVIR (VOZ)", UI_CYAN, aVoiceListen);
        mkBtn(c, LV_SYMBOL_VOLUME_MAX "  FALAR",  UI_PURPLE, aVoiceSpeak);
    }

    // -------------------------------------------------------------------- Ajustes
    static void buildSettings() {
        lv_obj_t *c = mkSub("Ajustes", UI_PURPLE, &page_set);
        ConfigManager *cfg = ConfigManager::getInstance();

        mkSlider(c, "BRILHO", 10, 255, cfg->get<int>("disp_brightness"), UI_ORANGE, aBright);
        mkSlider(c, "TEMPO DE TELA (s)", 10, 120, cfg->get<int>("disp_timeout_sec"), UI_CYAN, aTimeout);

        mkToggle(c, "Scan automatico",   "atk_auto_scan");
        mkToggle(c, "Scan BLE periodico","atk_ble_scan");
        mkToggle(c, "Deauth habilitado", "atk_deauth_enabled");
        mkToggle(c, "Capturar handshake","atk_capture_handshakes");
        mkToggle(c, "Deep sleep",        "pwr_deep_sleep_enabled");
        mkToggle(c, "Som no boot",       "sys_sound_on_boot");
        mkToggle(c, "Watchdog",          "sys_watchdog");
    }

    // ============================================================= navegação HW
    // Botão físico BOOT: curto = abre apps (na home) ou volta (nas subpáginas).
    static void nextTile() {
        if (nav_top == 0) { beep(); pushPage(page_apps); }
        else              { beep(); back(); }
    }

    // ================================================================= refresh
    static bool visible(lv_obj_t *p) { return p && !lv_obj_has_flag(p, LV_OBJ_FLAG_HIDDEN); }

    static void refresh() {
        // ---- HOME (sempre) ----
        String ts = PwnRTC::getTimestamp();     // "AAAA-MM-DD HH:MM:SS" quando RTC ok
        String hhmm = PwnRTC::getClock();
        if (lbl_time) lv_label_set_text(lbl_time, hhmm.c_str());
        if (lbl_date) lv_label_set_text(lbl_date, dateStr(ts).c_str());

        int batt = PwnPower::getBatteryPercent();
        bool chg = PwnPower::isCharging();
        bool usb = PwnPower::isVbusIn();
        if (lbl_batt) {
            lv_label_set_text_fmt(lbl_batt, "%s %d%%",
                chg ? LV_SYMBOL_CHARGE : (usb ? LV_SYMBOL_USB : LV_SYMBOL_BATTERY_FULL), batt);
            lv_obj_set_style_text_color(lbl_batt,
                (batt < 15 && !usb) ? UI_RED : (chg ? UI_GREEN : UI_TEXT), 0);
        }
        PetStats pet = PwnPet::getStats();
        if (lbl_home_pet)
            lv_label_set_text_fmt(lbl_home_pet, "%s  -  %s  -  Lv%d",
                pet.name.c_str(), PwnPet::getMoodStr().c_str(), (int)Gamification::getStats().level);
        if (lbl_status) {
            String st = "";
            if (WiFiTools::isSniffing()) st += LV_SYMBOL_WIFI " ";
            if (EvilPortal::isRunning()) st += LV_SYMBOL_WARNING " ";
            if (st == "") st = LV_SYMBOL_OK;
            lv_label_set_text(lbl_status, st.c_str());
        }

        // ---- Subpágina visível ----
        if (visible(page_wifi) && lbl_wifi) {
            String w = "Sniffer: " + String(WiFiTools::isSniffing() ? "ATIVO" : "parado") + "\n";
            w += "Dispositivos: " + String((int)WiFiTools::nearby_devices.size()) + "\n";
            w += "Handshakes: " + String((unsigned)WiFiTools::getHandshakeCount()) + "\n\n";
            int shown = 0;
            for (auto &d : WiFiTools::nearby_devices) {
                w += d.mac + "  " + String(d.rssi) + "dBm\n";
                if (++shown >= 7) break;
            }
            if (WiFiTools::nearby_devices.empty()) w += "(procurando...)";
            lv_label_set_text(lbl_wifi, w.c_str());
        }
        if (visible(page_ble) && lbl_ble) {
            String b = "Ultimo scan: " + String(PwnBLE::getLastCount()) + " disp.\n";
            b += "Total visto: " + String(PwnBLE::getTotalSeen()) + "\n";
            b += "Toque em ESCANEAR p/ nova varredura.";
            lv_label_set_text(lbl_ble, b.c_str());
        }
        if (visible(page_atk) && lbl_atk) {
            String a = "Evil Portal: " + String(EvilPortal::isRunning() ? "ATIVO" : "parado") + "\n";
            a += "Capturados: " + String(EvilPortal::getCapturedCount()) + "\n";
            a += "Deauth: " + String(ConfigManager::getInstance()->get<bool>("atk_deauth_enabled") ? "habilitado" : "bloqueado");
            lv_label_set_text(lbl_atk, a.c_str());
        }
        if (visible(page_pet)) {
            if (bar_hunger) lv_bar_set_value(bar_hunger, pet.hunger, LV_ANIM_ON);
            if (bar_happy)  lv_bar_set_value(bar_happy,  pet.happiness, LV_ANIM_ON);
            if (lbl_pet) {
                GameStats g = Gamification::getStats();
                String s = String(pet.name.c_str()) + "  -  " + PwnPet::getMoodStr() + "\n";
                s += "Nivel " + String((int)g.level) + "   XP " + Gamification::getXPStr() + "\n";
                s += "Idade: " + Gamification::getAgeStr();
                lv_label_set_text(lbl_pet, s.c_str());
            }
        }
        if (visible(page_power) && lbl_power) {
            String p = "Bateria: " + String(batt) + "%  (" + String(PwnPower::getBatteryVoltage(), 2) + " V)\n";
            p += "Status: " + PwnPower::getChargeStatusStr() + "\n";
            p += "Autonomia: ~" + String(PwnPower::getEstimatedHours(), 1) + " h\n";
            p += "RAM livre: " + String(ESP.getFreeHeap()/1024) + " KB";
            lv_label_set_text(lbl_power, p.c_str());
        }
        if (visible(page_sys) && lbl_sys) {
            ConfigManager *cfg = ConfigManager::getInstance();
            uint32_t up = millis()/1000;
            String s = "Nome: " + cfg->getString("sys_device_name") + "\n";
            s += "Modo WiFi: " + cfg->getString("sys_wifi_mode") + "\n";
            s += "AP: " + cfg->getString("sys_ap_ssid") + "\n";
            s += "RTC: " + String(PwnRTC::isOk() ? "OK" : "sem chip") + "\n";
            s += "Uptime: " + String(up/3600) + "h " + String((up/60)%60) + "m";
            lv_label_set_text(lbl_sys, s.c_str());
        }
        if (visible(page_tools) && lbl_tools) {
            String t = "Voz: toque OUVIR para captar comando.\n";
            t += "FALAR emite resposta TTS.\n";
            t += "Handshakes: " + String((unsigned)WiFiTools::getHandshakeCount());
            lv_label_set_text(lbl_tools, t.c_str());
        }
    }

    // update() chamado 1x/seg pelo main
    static void update() { refresh(); }

private:
    // Extrai "SEG - 08 SET" a partir de "AAAA-MM-DD HH:MM:SS"
    static String dateStr(const String &ts) {
        if (ts.length() < 10 || ts.charAt(4) != '-') return String("");
        int y = ts.substring(0,4).toInt();
        int mo = ts.substring(5,7).toInt();
        int d = ts.substring(8,10).toInt();
        if (y < 2020 || mo < 1 || mo > 12) return String("");
        static const char *wd[] = {"DOM","SEG","TER","QUA","QUI","SEX","SAB"};
        static const char *mn[] = {"JAN","FEV","MAR","ABR","MAI","JUN",
                                   "JUL","AGO","SET","OUT","NOV","DEZ"};
        struct tm tmv = {};
        tmv.tm_year = y-1900; tmv.tm_mon = mo-1; tmv.tm_mday = d; tmv.tm_hour = 12;
        mktime(&tmv);
        int w = tmv.tm_wday; if (w < 0 || w > 6) w = 0;
        char b[24];
        snprintf(b, sizeof(b), "%s  %02d %s", wd[w], d, mn[mo-1]);
        return String(b);
    }
};

#endif
