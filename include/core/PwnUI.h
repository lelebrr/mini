#ifndef PWN_UI_H
#define PWN_UI_H

#include <lvgl.h>
#include <time.h>
#include <math.h>
#include <new>
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

// Leitura do acelerômetro (QMI8658) exposta pelo main.cpp (instância global).
extern bool UI_getAccel(float &x, float &y, float &z);

/**
 * PwnUI v4 — "o sistema mais completo": RELÓGIO + LAUNCHER + ÁRVORE DE MENUS.
 *
 *  HOME (relógio, rosto animado, bateria)
 *    └─ APLICATIVOS (grade)
 *         Wi-Fi · Bluetooth · Ataques · Pet · Energia · Sensores ·
 *         Hora · Cronômetro · Timer · Lanterna · Sistema · Ferramentas · Ajustes
 *
 *  AJUSTES → 8 categorias → TODAS as ~90 chaves de configuração editáveis
 *  no próprio relógio (switch / stepper / seletor / teclado na tela).
 *
 *  Sem tela de carregamento. Som = apenas BEEP. Alvos de toque grandes.
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
    // ---- tipos de item de configuração ----
    enum SType { T_BOOL, T_INT, T_FLT, T_ENUM, T_STR };
    struct SItem {
        const char *key; const char *label; uint8_t type;
        int mn; int mx; int step; const char *opts; const char *unit;
    };
    struct Ctx  { const SItem *it; lv_obj_t *val; };          // linha de config
    struct DCtx { int *v; int mn; int mx; int step; lv_obj_t *lbl; const char *unit; }; // int direto

    // ============================ TELAS / ESTADO =============================
    inline static lv_obj_t *scr_main   = nullptr;
    inline static lv_obj_t *page_home  = nullptr;
    inline static lv_obj_t *page_apps  = nullptr;
    inline static lv_obj_t *page_wifi  = nullptr;
    inline static lv_obj_t *page_ble   = nullptr;
    inline static lv_obj_t *page_atk   = nullptr;
    inline static lv_obj_t *page_pet   = nullptr;
    inline static lv_obj_t *page_power = nullptr;
    inline static lv_obj_t *page_sys   = nullptr;
    inline static lv_obj_t *page_tools = nullptr;
    inline static lv_obj_t *page_sensors = nullptr;
    inline static lv_obj_t *page_clock = nullptr;
    inline static lv_obj_t *page_sw    = nullptr;
    inline static lv_obj_t *page_timer = nullptr;
    inline static lv_obj_t *page_flash = nullptr;
    inline static lv_obj_t *page_settings = nullptr;
    inline static lv_obj_t *cat_pages[8]   = { nullptr };
    // categorias de ajustes construidas sob demanda (economia de RAM no boot)
    inline static lv_obj_t    *cat_content[8] = { nullptr };
    inline static const SItem *cat_items[8]   = { nullptr };
    inline static int          cat_count[8]   = { 0 };
    inline static bool         cat_built[8]   = { false };

    // rótulos dinâmicos
    inline static lv_obj_t *lbl_time = nullptr, *lbl_date = nullptr, *lbl_batt = nullptr;
    inline static lv_obj_t *lbl_home_pet = nullptr, *lbl_status = nullptr;
    inline static lv_obj_t *lbl_wifi = nullptr, *lbl_ble = nullptr, *lbl_atk = nullptr;
    inline static lv_obj_t *lbl_power = nullptr, *lbl_sys = nullptr, *lbl_tools = nullptr;
    inline static lv_obj_t *lbl_pet = nullptr, *bar_hunger = nullptr, *bar_happy = nullptr;
    inline static lv_obj_t *lbl_sensors = nullptr, *bar_sx = nullptr, *bar_sy = nullptr;
    inline static lv_obj_t *lbl_sw = nullptr, *lbl_timer = nullptr;

    // teclado na tela (overlay reutilizável)
    inline static lv_obj_t *kb_overlay = nullptr, *kb_ta = nullptr, *kb_title = nullptr;
    inline static lv_obj_t *kb_widget = nullptr;
    inline static const SItem *g_edit_it = nullptr;
    inline static lv_obj_t *g_edit_val = nullptr;

    // cronômetro / timer / relógio
    inline static uint32_t g_sw_start = 0, g_sw_acc = 0; inline static bool g_sw_run = false;
    inline static uint32_t g_tmr_end = 0; inline static bool g_tmr_run = false;
    inline static int g_tmr_set[2] = { 5, 0 };     // min, seg
    inline static int g_clk[6] = { 2026, 1, 1, 0, 0, 0 };

    // pilha de navegação
    inline static lv_obj_t *nav_stack[12] = { nullptr };
    inline static int nav_top = 0;

    static int  N_(int bytes, int one) { return bytes / one; }
    static void beep() { AudioHandler::beep(2400, 20, 45); }
    static void showObj(lv_obj_t *o) { if (o) lv_obj_remove_flag(o, LV_OBJ_FLAG_HIDDEN); }
    static void hideObj(lv_obj_t *o) { if (o) lv_obj_add_flag(o, LV_OBJ_FLAG_HIDDEN); }
    static bool visible(lv_obj_t *p) { return p && !lv_obj_has_flag(p, LV_OBJ_FLAG_HIDDEN); }

    // ============================ NAVEGAÇÃO ================================
    static void pushPage(lv_obj_t *p) {
        if (!p || nav_top >= 11) return;
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
    static void cbOpen(lv_event_t *e) { beep(); pushPage((lv_obj_t *)lv_event_get_user_data(e)); }
    static void cbBack(lv_event_t *e) { (void)e; beep(); back(); }
    static void cbHome(lv_event_t *e) { (void)e; beep(); goHome(); }
    static void cbAppsBtn(lv_event_t *e) { (void)e; beep(); pushPage(page_apps); }
    static void cbOpenFlash(lv_event_t *e) { (void)e; beep(); pushPage(page_flash);
        PwnSleep::setFullBrightness(255); }
    static void cbFlashBack(lv_event_t *e) { (void)e; beep();
        PwnSleep::setFullBrightness(ConfigManager::getInstance()->get<int>("disp_brightness")); back(); }
    static void cbOpenCat(lv_event_t *e) { beep();
        int i=(int)(intptr_t)lv_event_get_user_data(e);
        if (i<0 || i>7) return;
        if (!cat_built[i]) { for (int k=0;k<cat_count[i];k++) settingRow(cat_content[i], &cat_items[i][k]); cat_built[i]=true; }
        pushPage(cat_pages[i]); }

    // ============================ AÇÕES REAIS =============================
    static void aWifiScan(lv_event_t *e){ (void)e; beep(); WiFiTools::beginNewCapture(); WiFiTools::startSnifferScan(); }
    static void aWifiPass(lv_event_t *e){ (void)e; beep(); WiFiTools::startSnifferPassive(); }
    static void aWifiStop(lv_event_t *e){ (void)e; beep(); WiFiTools::stopSniffer(); }
    static void aWifiCh  (lv_event_t *e){ (void)e; beep(); static uint8_t ch=1; ch=(ch%13)+1; WiFiTools::setChannel(ch); }
    static void aBleScan (lv_event_t *e){ (void)e; beep(); int n=PwnBLE::scan(4); if(n>0) PwnPet::feed(1); }
    static void aEvilStart(lv_event_t *e){ (void)e; beep(); ConfigManager *c=ConfigManager::getInstance();
        EvilPortal::start(c->getString("sys_ap_ssid").c_str(), c->getString("atk_portal_template").c_str()); }
    static void aEvilStop(lv_event_t *e){ (void)e; beep(); EvilPortal::stop(); }
    static void aEvilTwin(lv_event_t *e){ (void)e; beep(); PwnAttack::evilTwin("Free_WiFi"); }
    static void aDeauth(lv_event_t *e){ (void)e; AudioHandler::beepWarn();
        if(!ConfigManager::getInstance()->get<bool>("atk_deauth_enabled")) return;
        if(!WiFiTools::nearby_devices.empty()) PwnAttack::deauthSimulated(WiFiTools::nearby_devices.front().mac); }
    static void aFeed(lv_event_t *e){ (void)e; AudioHandler::beepOk(); PwnPet::feed(10); FaceHandler::setFace(FACE_HAPPY); }
    static void aPlay(lv_event_t *e){ (void)e; beep(); FaceHandler::setFace(FACE_EXCITED); PwnPet::addHandshake(true); }
    static void aFaceTap(lv_event_t *e){ (void)e; AudioHandler::beepOk(); PwnPet::feed(4); FaceHandler::setFace(FACE_HAPPY); }
    static void aPerf(lv_event_t *e){ beep(); int lvl=(int)(intptr_t)lv_event_get_user_data(e);
        PwnPower::setPerformanceMode(lvl); ConfigManager::getInstance()->set<int>("pwr_cpu_freq_max", lvl>=2?240:(lvl==1?160:80)); }
    static void aDeep(lv_event_t *e){ (void)e; AudioHandler::beepWarn(); PwnSleep::enterDeep(); }
    static void aNtp(lv_event_t *e){ (void)e; beep(); ConfigManager *c=ConfigManager::getInstance();
        PwnRTC::syncNTP(c->getString("sys_ntp_server").c_str(), c->get<int>("sys_timezone")); }
    static void aReboot(lv_event_t *e){ (void)e; AudioHandler::beepWarn(); delay(120); ESP.restart(); }
    static void aVoiceListen(lv_event_t *e){ (void)e; beep(); PwnVoice::listen(); }
    static void aVoiceSpeak(lv_event_t *e){ (void)e; beep(); PwnVoice::speak("mini lele online"); }
    static void aClockSave(lv_event_t *e){ (void)e; AudioHandler::beepOk();
        PwnRTC::setManual(g_clk[0],g_clk[1],g_clk[2],g_clk[3],g_clk[4],g_clk[5]); }
    static void aSwToggle(lv_event_t *e){ (void)e; beep();
        if(g_sw_run){ g_sw_acc += millis()-g_sw_start; g_sw_run=false; }
        else { g_sw_start=millis(); g_sw_run=true; } }
    static void aSwReset(lv_event_t *e){ (void)e; beep(); g_sw_run=false; g_sw_acc=0; if(lbl_sw) lv_label_set_text(lbl_sw,"00:00"); }
    static void aTmrStart(lv_event_t *e){ (void)e; AudioHandler::beepOk();
        if(!g_tmr_run){ g_tmr_end=millis()+(uint32_t)(g_tmr_set[0]*60+g_tmr_set[1])*1000UL; g_tmr_run=(g_tmr_set[0]||g_tmr_set[1]); } }
    static void aTmrStop(lv_event_t *e){ (void)e; beep(); g_tmr_run=false; }

    // ===================== EDITORES DE CONFIGURAÇÃO =======================
    static void applyKey(const char *key) {
        ConfigManager *cfg = ConfigManager::getInstance();
        if      (!strcmp(key,"disp_brightness"))          PwnSleep::setFullBrightness(cfg->get<int>(key));
        else if (!strcmp(key,"disp_timeout_sec"))         PwnSleep::setScreenTimeout(cfg->get<int>(key));
        else if (!strcmp(key,"pwr_cpu_freq_max")){ int v=cfg->get<int>(key); PwnPower::setPerformanceMode(v>=200?2:(v>=120?1:0)); }
        else if (!strcmp(key,"pwr_battery_capacity_mah")) PwnPower::setBatteryCapacity(cfg->get<int>(key));
        else if (!strcmp(key,"pwr_charge_current_ma"))    PwnPower::setChargeCurrentMa(cfg->get<int>(key));
    }
    static String valStr(const SItem *it) {
        ConfigManager *cfg = ConfigManager::getInstance();
        char b[28];
        if (it->type==T_INT){ snprintf(b,sizeof(b),"%d%s",cfg->get<int>(it->key), it->unit?it->unit:""); return String(b); }
        if (it->type==T_FLT){ snprintf(b,sizeof(b),"%.1f",cfg->get<float>(it->key)); return String(b); }
        if (it->type==T_ENUM) return cfg->getString(it->key);
        if (it->type==T_STR){
            if (strstr(it->key,"pass")) return String("****");
            String s=cfg->getString(it->key);
            if (s.length()==0) s="(vazio)";
            if (s.length()>13) s=s.substring(0,12)+"..";
            return s;
        }
        return String("");
    }
    static int enumSplit(const char *opts, char out[][20], int maxN) {
        int n=0; const char *p=opts;
        while (*p && n<maxN){ int i=0; while(*p && *p!=',' && i<19) out[n][i++]=*p++; out[n][i]='\0'; n++; if(*p==',') p++; }
        return n;
    }
    static void cbBool(lv_event_t *e){ beep();
        lv_obj_t *sw=(lv_obj_t*)lv_event_get_target(e);
        const SItem *it=(const SItem*)lv_event_get_user_data(e);
        ConfigManager::getInstance()->set<bool>(it->key, lv_obj_has_state(sw,LV_STATE_CHECKED));
        applyKey(it->key);
    }
    static void cbEnum(lv_event_t *e){ beep();
        Ctx *c=(Ctx*)lv_event_get_user_data(e);
        char o[8][20]; int n=enumSplit(c->it->opts,o,8); if(n<1) return;
        String cur=ConfigManager::getInstance()->getString(c->it->key);
        int idx=0; for(int i=0;i<n;i++) if(cur==o[i]){ idx=i; break; }
        idx=(idx+1)%n;
        ConfigManager::getInstance()->set<String>(c->it->key, String(o[idx]));
        applyKey(c->it->key);
        lv_label_set_text(c->val, o[idx]);
    }
    static void cbStr(lv_event_t *e){ beep(); openKeyboard((Ctx*)lv_event_get_user_data(e)); }
    static void stepDelta(Ctx *c, int dir){ beep();
        const SItem *it=c->it; ConfigManager *cfg=ConfigManager::getInstance();
        if (it->type==T_FLT){ int v=(int)lroundf(cfg->get<float>(it->key)*10.0f); v+=dir*it->step;
            if(v<it->mn)v=it->mn; if(v>it->mx)v=it->mx; cfg->set<float>(it->key, v/10.0f); }
        else { int v=cfg->get<int>(it->key); v+=dir*it->step;
            if(v<it->mn)v=it->mn; if(v>it->mx)v=it->mx; cfg->set<int>(it->key, v); }
        applyKey(it->key);
        lv_label_set_text(c->val, valStr(it).c_str());
    }
    static void cbMinus(lv_event_t *e){ stepDelta((Ctx*)lv_event_get_user_data(e), -1); }
    static void cbPlus (lv_event_t *e){ stepDelta((Ctx*)lv_event_get_user_data(e), +1); }

    static void dstep(DCtx *c, int dir){ beep(); *c->v += dir*c->step;
        if(*c->v<c->mn)*c->v=c->mn; if(*c->v>c->mx)*c->v=c->mx;
        char b[16]; snprintf(b,sizeof(b),"%d%s",*c->v, c->unit?c->unit:""); lv_label_set_text(c->lbl,b); }
    static void cbDMinus(lv_event_t *e){ dstep((DCtx*)lv_event_get_user_data(e), -1); }
    static void cbDPlus (lv_event_t *e){ dstep((DCtx*)lv_event_get_user_data(e), +1); }

    // teclado
    static void openKeyboard(Ctx *c){
        g_edit_it=c->it; g_edit_val=c->val;
        bool pass = strstr(c->it->key,"pass")!=nullptr;
        lv_textarea_set_one_line(kb_ta, true);
        lv_textarea_set_password_mode(kb_ta, pass);
        String cur=ConfigManager::getInstance()->getString(c->it->key);
        lv_textarea_set_text(kb_ta, cur.c_str());
        lv_label_set_text(kb_title, c->it->label);
        lv_keyboard_set_textarea(kb_widget, kb_ta);
        showObj(kb_overlay);
        lv_obj_move_foreground(kb_overlay);
    }
    static void cbKb(lv_event_t *e){
        lv_event_code_t code=lv_event_get_code(e);
        if (code==LV_EVENT_READY){
            if (g_edit_it){
                ConfigManager::getInstance()->set<String>(g_edit_it->key, String(lv_textarea_get_text(kb_ta)));
                applyKey(g_edit_it->key);
                if (g_edit_val) lv_label_set_text(g_edit_val, valStr(g_edit_it).c_str());
            }
            AudioHandler::beepOk();
        }
        hideObj(kb_overlay);
    }

    // ============================ CONSTRUTORES ============================
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
    static void stepStyle(lv_obj_t *b, lv_color_t col) {
        lv_obj_set_style_bg_color(b, col, 0);
        lv_obj_set_style_radius(b, 10, 0);
        lv_obj_set_style_shadow_width(b, 0, 0);
        lv_obj_set_style_pad_all(b, 0, 0);
    }
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
    static lv_obj_t *mkHeader(lv_obj_t *p, const char *title, lv_color_t accent, lv_event_cb_t backCb, const char *sym) {
        lv_obj_t *hdr = lv_obj_create(p);
        lv_obj_set_size(hdr, LCD_WIDTH, 52);
        lv_obj_set_pos(hdr, 0, 0);
        lv_obj_set_style_bg_color(hdr, UI_PANEL, 0);
        lv_obj_set_style_bg_opa(hdr, LV_OPA_COVER, 0);
        lv_obj_set_style_border_color(hdr, accent, 0);
        lv_obj_set_style_border_side(hdr, LV_BORDER_SIDE_BOTTOM, 0);
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
        lv_obj_t *bl = lv_label_create(bk); lv_label_set_text(bl, sym);
        lv_obj_set_style_text_color(bl, accent, 0); lv_obj_center(bl);
        lv_obj_add_event_cb(bk, backCb, LV_EVENT_CLICKED, NULL);
        lv_obj_t *tt = lv_label_create(hdr);
        lv_label_set_text(tt, title);
        lv_obj_set_style_text_font(tt, &lv_font_montserrat_20, 0);
        lv_obj_set_style_text_color(tt, UI_TEXT, 0);
        lv_obj_align(tt, LV_ALIGN_CENTER, 12, 0);
        return hdr;
    }
    static lv_obj_t *mkContent(lv_obj_t *p) {
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
    // subpágina padrão (cabeçalho volta + conteúdo rolável)
    static lv_obj_t *mkSub(const char *title, lv_color_t accent, lv_obj_t **pageOut) {
        lv_obj_t *p = newPage();
        *pageOut = p;
        mkHeader(p, title, accent, cbBack, LV_SYMBOL_LEFT);
        return mkContent(p);
    }
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
    // stepper de inteiro direto (relógio / timer)
    static void dRow(lv_obj_t *parent, const char *name, int *store, int mn, int mx, int step, const char *unit) {
        lv_obj_t *row = lv_obj_create(parent);
        lv_obj_set_width(row, LV_PCT(100)); lv_obj_set_height(row, 54);
        baseCard(row, UI_PANEL2); lv_obj_set_style_pad_all(row, 10, 0);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_t *nm = lv_label_create(row); lv_label_set_text(nm, name);
        lv_obj_set_style_text_color(nm, UI_TEXT, 0); lv_obj_set_style_text_font(nm, &lv_font_montserrat_16, 0);
        lv_obj_t *box = lv_obj_create(row); lv_obj_set_size(box, 150, 42);
        lv_obj_set_style_bg_opa(box, LV_OPA_TRANSP, 0); lv_obj_set_style_border_width(box, 0, 0);
        lv_obj_set_style_pad_all(box, 0, 0); lv_obj_set_style_pad_column(box, 4, 0);
        lv_obj_set_flex_flow(box, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(box, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_remove_flag(box, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_t *bm = lv_button_create(box); lv_obj_set_size(bm, 40, 40); stepStyle(bm, UI_ORANGE);
        lv_obj_t *lm = lv_label_create(bm); lv_label_set_text(lm, LV_SYMBOL_MINUS);
        lv_obj_set_style_text_color(lm, UI_BG, 0); lv_obj_center(lm);
        lv_obj_t *v = lv_label_create(box); char b[16]; snprintf(b,sizeof(b),"%d%s",*store, unit?unit:""); lv_label_set_text(v, b);
        lv_obj_set_style_text_color(v, UI_TEXT, 0); lv_obj_set_style_text_font(v, &lv_font_montserrat_16, 0);
        lv_obj_set_width(v, 56); lv_obj_set_style_text_align(v, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_t *bp = lv_button_create(box); lv_obj_set_size(bp, 40, 40); stepStyle(bp, UI_GREEN);
        lv_obj_t *lp = lv_label_create(bp); lv_label_set_text(lp, LV_SYMBOL_PLUS);
        lv_obj_set_style_text_color(lp, UI_BG, 0); lv_obj_center(lp);
        DCtx *c = new DCtx{store, mn, mx, step, v, unit};
        lv_obj_add_event_cb(bm, cbDMinus, LV_EVENT_CLICKED, c);
        lv_obj_add_event_cb(bp, cbDPlus,  LV_EVENT_CLICKED, c);
    }
    // linha de configuração genérica
    static void settingRow(lv_obj_t *parent, const SItem *it) {
        lv_obj_t *row = lv_obj_create(parent);
        lv_obj_set_width(row, LV_PCT(100)); lv_obj_set_height(row, 54);
        baseCard(row, UI_PANEL2); lv_obj_set_style_pad_all(row, 10, 0);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_t *name = lv_label_create(row);
        lv_label_set_text(name, it->label);
        lv_obj_set_style_text_color(name, UI_TEXT, 0);
        lv_obj_set_style_text_font(name, &lv_font_montserrat_14, 0);
        lv_obj_set_width(name, 150);
        lv_label_set_long_mode(name, LV_LABEL_LONG_DOT);

        if (it->type==T_BOOL) {
            lv_obj_t *sw = lv_switch_create(row);
            lv_obj_set_style_bg_color(sw, UI_GREEN, LV_PART_INDICATOR | LV_STATE_CHECKED);
            if (ConfigManager::getInstance()->get<bool>(it->key)) lv_obj_add_state(sw, LV_STATE_CHECKED);
            lv_obj_add_event_cb(sw, cbBool, LV_EVENT_VALUE_CHANGED, (void*)it);
        } else if (it->type==T_ENUM || it->type==T_STR) {
            lv_obj_t *btn = lv_button_create(row);
            lv_obj_set_size(btn, 150, 40);
            lv_obj_set_style_bg_color(btn, UI_PANEL, 0);
            lv_obj_set_style_border_color(btn, it->type==T_STR ? UI_CYAN : UI_PURPLE, 0);
            lv_obj_set_style_border_width(btn, 1, 0);
            lv_obj_set_style_border_opa(btn, LV_OPA_60, 0);
            lv_obj_set_style_radius(btn, 10, 0);
            lv_obj_set_style_shadow_width(btn, 0, 0);
            lv_obj_t *v = lv_label_create(btn);
            lv_label_set_text(v, valStr(it).c_str());
            lv_obj_set_style_text_color(v, UI_TEXT, 0);
            lv_obj_set_style_text_font(v, &lv_font_montserrat_14, 0);
            lv_obj_center(v);
            Ctx *c = new Ctx{it, v};
            lv_obj_add_event_cb(btn, it->type==T_STR ? cbStr : cbEnum, LV_EVENT_CLICKED, c);
        } else { // T_INT / T_FLT
            lv_obj_t *box = lv_obj_create(row); lv_obj_set_size(box, 150, 42);
            lv_obj_set_style_bg_opa(box, LV_OPA_TRANSP, 0); lv_obj_set_style_border_width(box, 0, 0);
            lv_obj_set_style_pad_all(box, 0, 0); lv_obj_set_style_pad_column(box, 4, 0);
            lv_obj_set_flex_flow(box, LV_FLEX_FLOW_ROW);
            lv_obj_set_flex_align(box, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
            lv_obj_remove_flag(box, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_t *bm = lv_button_create(box); lv_obj_set_size(bm, 40, 40); stepStyle(bm, UI_ORANGE);
            lv_obj_t *lm = lv_label_create(bm); lv_label_set_text(lm, LV_SYMBOL_MINUS);
            lv_obj_set_style_text_color(lm, UI_BG, 0); lv_obj_center(lm);
            lv_obj_t *v = lv_label_create(box); lv_label_set_text(v, valStr(it).c_str());
            lv_obj_set_style_text_color(v, UI_TEXT, 0); lv_obj_set_style_text_font(v, &lv_font_montserrat_16, 0);
            lv_obj_set_width(v, 56); lv_obj_set_style_text_align(v, LV_TEXT_ALIGN_CENTER, 0);
            lv_obj_t *bp = lv_button_create(box); lv_obj_set_size(bp, 40, 40); stepStyle(bp, UI_GREEN);
            lv_obj_t *lp = lv_label_create(bp); lv_label_set_text(lp, LV_SYMBOL_PLUS);
            lv_obj_set_style_text_color(lp, UI_BG, 0); lv_obj_center(lp);
            Ctx *c = new Ctx{it, v};
            lv_obj_add_event_cb(bm, cbMinus, LV_EVENT_CLICKED, c);
            lv_obj_add_event_cb(bp, cbPlus,  LV_EVENT_CLICKED, c);
        }
    }
    static void mkTile(lv_obj_t *grid, const char *sym, const char *name, lv_color_t col,
                       lv_obj_t **targetSlot, lv_event_cb_t cb) {
        lv_obj_t *t = lv_obj_create(grid);
        lv_obj_set_size(t, 165, 96);
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
        lv_obj_add_event_cb(t, cb, LV_EVENT_CLICKED, targetSlot ? *targetSlot : NULL);
    }

public:
    // ================================================================= init
    static void init() {
        scr_main = lv_screen_active();
        lv_obj_set_style_bg_color(scr_main, UI_BG, 0);
        lv_obj_set_style_bg_opa(scr_main, LV_OPA_COVER, 0);
        lv_obj_remove_flag(scr_main, LV_OBJ_FLAG_SCROLLABLE);

        buildHome();
        buildWifi();  buildBle();   buildAttacks(); buildPet();  buildPower();
        buildSystem(); buildTools(); buildSensors(); buildClockSet();
        buildStopwatch(); buildTimer(); buildFlash();
        buildAllSettings();
        buildKeyboard();
        buildLauncher();   // por último: referencia todas as páginas

        nav_stack[0] = page_home;
        nav_top = 0;
        showObj(page_home);
    }

    // --------------------------------------------------------- HOME (relógio)
    static void buildHome() {
        page_home = newPage();
        lv_obj_remove_flag(page_home, LV_OBJ_FLAG_HIDDEN);

        lbl_status = lv_label_create(page_home);
        lv_obj_set_style_text_font(lbl_status, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(lbl_status, UI_DIM, 0);
        lv_label_set_text(lbl_status, LV_SYMBOL_OK);
        lv_obj_align(lbl_status, LV_ALIGN_TOP_LEFT, 16, 14);

        lbl_batt = lv_label_create(page_home);
        lv_obj_set_style_text_font(lbl_batt, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(lbl_batt, UI_GREEN, 0);
        lv_label_set_text(lbl_batt, LV_SYMBOL_BATTERY_FULL " --%");
        lv_obj_align(lbl_batt, LV_ALIGN_TOP_RIGHT, -16, 12);

        lbl_time = lv_label_create(page_home);
        lv_obj_set_style_text_font(lbl_time, &lv_font_montserrat_48, 0);
        lv_obj_set_style_text_color(lbl_time, UI_TEXT, 0);
        lv_label_set_text(lbl_time, "00:00");
        lv_obj_align(lbl_time, LV_ALIGN_TOP_MID, 0, 60);

        lbl_date = lv_label_create(page_home);
        lv_obj_set_style_text_font(lbl_date, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(lbl_date, UI_PURPLE, 0);
        lv_label_set_text(lbl_date, "-- -- ----");
        lv_obj_align(lbl_date, LV_ALIGN_TOP_MID, 0, 120);

        lv_obj_t *facePanel = lv_obj_create(page_home);
        lv_obj_set_size(facePanel, 220, 116);
        lv_obj_align(facePanel, LV_ALIGN_CENTER, 0, 14);
        baseCard(facePanel, UI_GREEN);
        lv_obj_add_flag(facePanel, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(facePanel, aFaceTap, LV_EVENT_CLICKED, NULL);
        FaceHandler::init(facePanel);

        lbl_home_pet = lv_label_create(page_home);
        lv_obj_set_style_text_font(lbl_home_pet, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(lbl_home_pet, UI_DIM, 0);
        lv_label_set_text(lbl_home_pet, "PwnBaby");
        lv_obj_align(lbl_home_pet, LV_ALIGN_CENTER, 0, 92);

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

    // ---------------------------------------------------------- LAUNCHER
    static void buildLauncher() {
        page_apps = newPage();
        mkHeader(page_apps, "APLICATIVOS", UI_CYAN, cbHome, LV_SYMBOL_HOME);
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

        mkTile(grid, LV_SYMBOL_WIFI,      "Wi-Fi",      UI_GREEN,  &page_wifi,  cbOpen);
        mkTile(grid, LV_SYMBOL_BLUETOOTH, "Bluetooth",  UI_CYAN,   &page_ble,   cbOpen);
        mkTile(grid, LV_SYMBOL_WARNING,   "Ataques",    UI_RED,    &page_atk,   cbOpen);
        mkTile(grid, LV_SYMBOL_EYE_OPEN,  "Pet",        UI_PURPLE, &page_pet,   cbOpen);
        mkTile(grid, LV_SYMBOL_CHARGE,    "Energia",    UI_ORANGE, &page_power, cbOpen);
        mkTile(grid, LV_SYMBOL_GPS,       "Sensores",   UI_CYAN,   &page_sensors, cbOpen);
        mkTile(grid, LV_SYMBOL_BELL,      "Hora",       UI_PURPLE, &page_clock, cbOpen);
        mkTile(grid, LV_SYMBOL_LOOP,      "Cronometro", UI_GREEN,  &page_sw,    cbOpen);
        mkTile(grid, LV_SYMBOL_REFRESH,   "Timer",      UI_ORANGE, &page_timer, cbOpen);
        mkTile(grid, LV_SYMBOL_IMAGE,     "Lanterna",   UI_TEXT,   &page_flash, cbOpenFlash);
        mkTile(grid, LV_SYMBOL_DRIVE,     "Sistema",    UI_CYAN,   &page_sys,   cbOpen);
        mkTile(grid, LV_SYMBOL_AUDIO,     "Ferramentas",UI_GREEN,  &page_tools, cbOpen);
        mkTile(grid, LV_SYMBOL_SETTINGS,  "Ajustes",    UI_DIM,    &page_settings, cbOpen);
    }

    // ---------------------------------------------------------- app pages
    static void buildWifi() {
        lv_obj_t *c = mkSub("Wi-Fi", UI_GREEN, &page_wifi);
        lbl_wifi = mkInfo(c, UI_GREEN, "SNIFFER / CAPTURA");
        lv_obj_t *r1 = mkRow(c);
        mkBtn(r1, "SCAN", UI_GREEN, aWifiScan, NULL, 108);
        mkBtn(r1, "PASSIVO", UI_CYAN, aWifiPass, NULL, 108);
        mkBtn(r1, "PARAR", UI_RED, aWifiStop, NULL, 108);
        lv_obj_t *r2 = mkRow(c);
        mkBtn(r2, LV_SYMBOL_REFRESH " CANAL", UI_ORANGE, aWifiCh, NULL, 165);
        mkBtn(r2, LV_SYMBOL_SAVE " NOVA CAP.", UI_PURPLE, aWifiScan, NULL, 165);
    }
    static void buildBle() {
        lv_obj_t *c = mkSub("Bluetooth", UI_CYAN, &page_ble);
        lbl_ble = mkInfo(c, UI_CYAN, "SCANNER BLE");
        mkBtn(c, LV_SYMBOL_BLUETOOTH "  ESCANEAR BLE", UI_CYAN, aBleScan);
    }
    static void buildAttacks() {
        lv_obj_t *c = mkSub("Ataques", UI_RED, &page_atk);
        lbl_atk = mkInfo(c, UI_RED, "STATUS");
        mkBtn(c, LV_SYMBOL_WARNING "  EVIL PORTAL: ON", UI_RED, aEvilStart);
        mkBtn(c, LV_SYMBOL_STOP "  EVIL PORTAL: OFF", UI_ORANGE, aEvilStop);
        mkBtn(c, LV_SYMBOL_WIFI "  EVIL TWIN AP", UI_PURPLE, aEvilTwin);
        mkBtn(c, LV_SYMBOL_CLOSE "  DEAUTH (sim)", UI_RED, aDeauth);
    }
    static void buildPet() {
        lv_obj_t *c = mkSub("Pet", UI_PURPLE, &page_pet);
        lbl_pet = mkInfo(c, UI_PURPLE, "PWNAGOTCHI");
        bar_hunger = mkBar(c, "FOME", UI_ORANGE);
        bar_happy  = mkBar(c, "FELICIDADE", UI_GREEN);
        lv_obj_t *r = mkRow(c);
        mkBtn(r, LV_SYMBOL_OK " ALIMENTAR", UI_GREEN, aFeed, NULL, 165);
        mkBtn(r, LV_SYMBOL_PLAY " BRINCAR", UI_PURPLE, aPlay, NULL, 165);
    }
    static void buildPower() {
        lv_obj_t *c = mkSub("Energia", UI_ORANGE, &page_power);
        lbl_power = mkInfo(c, UI_ORANGE, "BATERIA / SISTEMA");
        lv_obj_t *r = mkRow(c);
        mkBtn(r, "ECO", UI_GREEN, aPerf, (void*)(intptr_t)0, 105);
        mkBtn(r, "NORMAL", UI_CYAN, aPerf, (void*)(intptr_t)1, 105);
        mkBtn(r, "TURBO", UI_ORANGE, aPerf, (void*)(intptr_t)2, 105);
        mkBtn(c, LV_SYMBOL_POWER "  DEEP SLEEP", UI_RED, aDeep);
    }
    static void buildSystem() {
        lv_obj_t *c = mkSub("Sistema", UI_CYAN, &page_sys);
        lbl_sys = mkInfo(c, UI_CYAN, "DISPOSITIVO / REDE");
        mkBtn(c, LV_SYMBOL_REFRESH "  SINCRONIZAR HORA (NTP)", UI_GREEN, aNtp);
        mkBtn(c, LV_SYMBOL_POWER "  REINICIAR", UI_RED, aReboot);
    }
    static void buildTools() {
        lv_obj_t *c = mkSub("Ferramentas", UI_GREEN, &page_tools);
        lbl_tools = mkInfo(c, UI_GREEN, "VOZ / MICROFONE");
        mkBtn(c, LV_SYMBOL_AUDIO "  OUVIR (VOZ)", UI_CYAN, aVoiceListen);
        mkBtn(c, LV_SYMBOL_VOLUME_MAX "  FALAR", UI_PURPLE, aVoiceSpeak);
    }
    static void buildSensors() {
        lv_obj_t *c = mkSub("Sensores", UI_CYAN, &page_sensors);
        lbl_sensors = mkInfo(c, UI_CYAN, "ACELEROMETRO (QMI8658)");
        bar_sx = mkBar(c, "INCLINACAO X", UI_ORANGE);
        bar_sy = mkBar(c, "INCLINACAO Y", UI_GREEN);
    }
    static void buildClockSet() {
        // carrega hora atual do RTC como ponto de partida
        String ts = PwnRTC::getTimestamp();
        if (ts.length()>=19 && ts.charAt(4)=='-') {
            g_clk[0]=ts.substring(0,4).toInt();  g_clk[1]=ts.substring(5,7).toInt();
            g_clk[2]=ts.substring(8,10).toInt(); g_clk[3]=ts.substring(11,13).toInt();
            g_clk[4]=ts.substring(14,16).toInt();g_clk[5]=ts.substring(17,19).toInt();
        }
        lv_obj_t *c = mkSub("Ajustar Hora", UI_PURPLE, &page_clock);
        dRow(c, "Ano",  &g_clk[0], 2020, 2099, 1, "");
        dRow(c, "Mes",  &g_clk[1], 1, 12, 1, "");
        dRow(c, "Dia",  &g_clk[2], 1, 31, 1, "");
        dRow(c, "Hora", &g_clk[3], 0, 23, 1, "");
        dRow(c, "Min",  &g_clk[4], 0, 59, 1, "");
        dRow(c, "Seg",  &g_clk[5], 0, 59, 1, "");
        mkBtn(c, LV_SYMBOL_SAVE "  SALVAR HORA", UI_GREEN, aClockSave);
    }
    static void buildStopwatch() {
        lv_obj_t *c = mkSub("Cronometro", UI_GREEN, &page_sw);
        lbl_sw = lv_label_create(c);
        lv_obj_set_style_text_font(lbl_sw, &lv_font_montserrat_48, 0);
        lv_obj_set_style_text_color(lbl_sw, UI_TEXT, 0);
        lv_label_set_text(lbl_sw, "00:00");
        lv_obj_set_style_pad_top(lbl_sw, 20, 0);
        lv_obj_t *r = mkRow(c);
        mkBtn(r, LV_SYMBOL_PLAY "/" LV_SYMBOL_PAUSE, UI_GREEN, aSwToggle, NULL, 165);
        mkBtn(r, LV_SYMBOL_REFRESH " ZERAR", UI_ORANGE, aSwReset, NULL, 165);
    }
    static void buildTimer() {
        lv_obj_t *c = mkSub("Timer", UI_ORANGE, &page_timer);
        lbl_timer = lv_label_create(c);
        lv_obj_set_style_text_font(lbl_timer, &lv_font_montserrat_48, 0);
        lv_obj_set_style_text_color(lbl_timer, UI_TEXT, 0);
        lv_label_set_text(lbl_timer, "05:00");
        dRow(c, "Minutos",  &g_tmr_set[0], 0, 99, 1, "");
        dRow(c, "Segundos", &g_tmr_set[1], 0, 59, 5, "");
        lv_obj_t *r = mkRow(c);
        mkBtn(r, LV_SYMBOL_PLAY " INICIAR", UI_GREEN, aTmrStart, NULL, 165);
        mkBtn(r, LV_SYMBOL_STOP " PARAR", UI_RED, aTmrStop, NULL, 165);
    }
    static void buildFlash() {
        page_flash = newPage();
        lv_obj_set_style_bg_color(page_flash, lv_color_hex(0xFFFFFF), 0);
        mkHeader(page_flash, "Lanterna", UI_ORANGE, cbFlashBack, LV_SYMBOL_LEFT);
        lv_obj_t *area = lv_obj_create(page_flash);
        lv_obj_set_size(area, LCD_WIDTH, LCD_HEIGHT - 52);
        lv_obj_set_pos(area, 0, 52);
        lv_obj_set_style_bg_color(area, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_opa(area, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(area, 0, 0);
        lv_obj_set_style_radius(area, 0, 0);
        lv_obj_remove_flag(area, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_t *l = lv_label_create(area);
        lv_label_set_text(l, LV_SYMBOL_IMAGE "  LANTERNA");
        lv_obj_set_style_text_color(l, lv_color_hex(0x333333), 0);
        lv_obj_set_style_text_font(l, &lv_font_montserrat_20, 0);
        lv_obj_center(l);
    }

    // ---------------------------------------------------- ÁRVORE DE AJUSTES
    static lv_obj_t *buildCatShell(const char *title, lv_color_t col, const SItem *items, int n, int idx) {
        lv_obj_t *page; lv_obj_t *c = mkSub(title, col, &page);
        cat_content[idx]=c; cat_items[idx]=items; cat_count[idx]=n; cat_built[idx]=false;
        return page;
    }
    static void buildAllSettings() {
        // -------- tabelas de TODAS as chaves de ConfigManager --------
        static const SItem CAT_PET[] = {
            {"pet_name","Nome",T_STR,0,0,0,0,0},
            {"pet_voice_enabled","Voz ativa",T_BOOL,0,0,0,0,0},
            {"pet_hunger_rate","Taxa de fome",T_INT,1,5,1,0,""},
            {"pet_happiness_decay","Queda felicidade",T_INT,1,5,1,0,""},
            {"pet_evolution_enabled","Evolucao",T_BOOL,0,0,0,0,0},
            {"pet_sfx_volume","Volume SFX",T_INT,0,100,5,0,"%"},
            {"pet_tts_speed","Velocidade TTS",T_FLT,5,20,1,0,0},
            {"pet_wake_word_enabled","Wake word",T_BOOL,0,0,0,0,0},
            {"pet_wake_word","Palavra ativacao",T_STR,0,0,0,0,0},
            {"pet_mood_dynamic","Humor dinamico",T_BOOL,0,0,0,0,0},
            {"pet_voice_remote","Voz remota",T_BOOL,0,0,0,0,0},
            {"pet_allow_badusb","Permitir BadUSB",T_BOOL,0,0,0,0,0},
            {"pet_allow_swear_words","Palavroes",T_BOOL,0,0,0,0,0},
        };
        static const SItem CAT_DISP[] = {
            {"disp_brightness","Brilho",T_INT,10,255,5,0,""},
            {"disp_timeout_sec","Timeout tela",T_INT,10,120,5,0,"s"},
            {"disp_screensaver","Protetor tela",T_ENUM,0,0,0,"matrix,sleep,blank",0},
            {"disp_theme","Tema",T_ENUM,0,0,0,"cyber_favela,dark,neon",0},
            {"disp_rotation","Rotacao",T_INT,0,3,1,0,""},
            {"disp_show_fps","Mostrar FPS",T_BOOL,0,0,0,0,0},
            {"disp_matrix_color","Cor matrix",T_STR,0,0,0,0,0},
            {"disp_gamma_correction","Correcao gama",T_BOOL,0,0,0,0,0},
            {"disp_font_size","Tamanho fonte",T_ENUM,0,0,0,"small,medium,large",0},
            {"disp_animations","Animacoes",T_BOOL,0,0,0,0,0},
        };
        static const SItem CAT_PWR[] = {
            {"pwr_deep_sleep_enabled","Deep sleep",T_BOOL,0,0,0,0,0},
            {"pwr_cpu_freq_max","CPU max",T_INT,80,240,20,0,"MHz"},
            {"pwr_cpu_freq_min","CPU min",T_INT,40,160,20,0,"MHz"},
            {"pwr_auto_dim","Auto dim",T_BOOL,0,0,0,0,0},
            {"pwr_battery_critical","Bateria critica",T_INT,3,25,1,0,"%"},
            {"pwr_zombie_mode","Modo zumbi",T_BOOL,0,0,0,0,0},
            {"pwr_disable_leds","Desligar LEDs",T_BOOL,0,0,0,0,0},
            {"pwr_wifi_powersave","WiFi economia",T_BOOL,0,0,0,0,0},
            {"pwr_peripheral_shutdown","Deslig. perifericos",T_BOOL,0,0,0,0,0},
            {"pwr_siesta_mode","Modo siesta",T_BOOL,0,0,0,0,0},
            {"pwr_deep_sleep_after_sec","Deep sleep apos",T_INT,30,1800,30,0,"s"},
        };
        static const SItem CAT_BAT[] = {
            {"pwr_battery_capacity_mah","Capacidade",T_INT,100,2000,50,0,"mAh"},
            {"pwr_charge_current_ma","Corrente carga",T_INT,50,300,25,0,"mA"},
            {"pwr_charge_target_mv","Tensao corte",T_INT,4000,4300,25,0,"mV"},
            {"pwr_low_warn_pct","Aviso baixa",T_INT,3,30,1,0,"%"},
            {"pwr_low_shutdown_pct","Desligar em",T_INT,1,15,1,0,"%"},
        };
        static const SItem CAT_WIFI[] = {
            {"atk_auto_scan","Scan automatico",T_BOOL,0,0,0,0,0},
            {"atk_scan_interval","Intervalo scan",T_INT,5,120,5,0,"s"},
            {"atk_channel_hop","Channel hop",T_BOOL,0,0,0,0,0},
            {"atk_channels","Canais",T_STR,0,0,0,0,0},
            {"atk_deauth_enabled","Deauth",T_BOOL,0,0,0,0,0},
            {"atk_deauth_reason","Deauth reason",T_INT,1,14,1,0,""},
            {"atk_beacon_spam","Beacon spam",T_BOOL,0,0,0,0,0},
            {"atk_beacon_list","Lista beacon",T_STR,0,0,0,0,0},
            {"atk_evil_portal","Evil portal",T_BOOL,0,0,0,0,0},
            {"atk_portal_template","Template portal",T_STR,0,0,0,0,0},
            {"atk_capture_handshakes","Capturar handshake",T_BOOL,0,0,0,0,0},
            {"atk_capture_pmkid","Capturar PMKID",T_BOOL,0,0,0,0,0},
            {"atk_whitelist","Whitelist",T_STR,0,0,0,0,0},
            {"atk_blacklist","Blacklist",T_STR,0,0,0,0,0},
            {"atk_random_mac","MAC aleatorio",T_BOOL,0,0,0,0,0},
            {"atk_led_feedback","LED feedback",T_BOOL,0,0,0,0,0},
            {"atk_save_pcaps","Salvar PCAPs",T_BOOL,0,0,0,0,0},
            {"atk_sniffer_filter","Filtro sniffer",T_STR,0,0,0,0,0},
            {"atk_aggressive_level","Agressividade",T_INT,1,3,1,0,""},
            {"atk_karma_enabled","Karma",T_BOOL,0,0,0,0,0},
            {"atk_probe_logging","Log de probes",T_BOOL,0,0,0,0,0},
            {"atk_beacon_spam_timer","Beacon timer",T_INT,1,60,1,0,"min"},
            {"atk_target_manufacturer","Fabricante alvo",T_STR,0,0,0,0,0},
        };
        static const SItem CAT_BLE[] = {
            {"atk_ble_scan","Scan BLE periodico",T_BOOL,0,0,0,0,0},
            {"atk_ble_interval_sec","Intervalo BLE",T_INT,10,300,10,0,"s"},
        };
        static const SItem CAT_SYS[] = {
            {"sys_device_name","Nome dispositivo",T_STR,0,0,0,0,0},
            {"sys_hostname","Hostname",T_STR,0,0,0,0,0},
            {"sys_wifi_mode","Modo WiFi",T_ENUM,0,0,0,"AP,STA,AP_STA",0},
            {"sys_ap_ssid","AP SSID",T_STR,0,0,0,0,0},
            {"sys_ap_pass","AP senha",T_STR,0,0,0,0,0},
            {"sys_sta_ssid","STA SSID",T_STR,0,0,0,0,0},
            {"sys_sta_pass","STA senha",T_STR,0,0,0,0,0},
            {"sys_web_user","Web usuario",T_STR,0,0,0,0,0},
            {"sys_web_pass","Web senha",T_STR,0,0,0,0,0},
            {"sys_ntp_server","Servidor NTP",T_STR,0,0,0,0,0},
            {"sys_timezone","Fuso horario",T_INT,-12,14,1,0,"h"},
            {"sys_ota_enabled","OTA",T_BOOL,0,0,0,0,0},
            {"sys_auto_backup","Auto backup",T_BOOL,0,0,0,0,0},
            {"sys_usb_mode","Modo USB",T_ENUM,0,0,0,"serial,mass_storage",0},
            {"sys_language","Idioma",T_ENUM,0,0,0,"pt_br,en_us",0},
            {"sys_sound_on_boot","Som no boot",T_BOOL,0,0,0,0,0},
            {"sys_led_notification","LED notificacao",T_BOOL,0,0,0,0,0},
            {"sys_haptic_feedback","Haptico",T_BOOL,0,0,0,0,0},
            {"sys_watchdog","Watchdog",T_BOOL,0,0,0,0,0},
            {"sys_factory_reset","Reset de fabrica",T_BOOL,0,0,0,0,0},
        };
        static const SItem CAT_WEB[] = {
            {"web_theme","Tema web",T_ENUM,0,0,0,"cyber_favela,dark,neon",0},
            {"web_kiosk_mode","Modo kiosk",T_BOOL,0,0,0,0,0},
            {"web_live_logs","Logs ao vivo",T_BOOL,0,0,0,0,0},
            {"web_stealth_mode","Modo stealth",T_BOOL,0,0,0,0,0},
            {"web_map_heatmap","Mapa heatmap",T_BOOL,0,0,0,0,0},
            {"web_log_level","Nivel de log",T_INT,0,5,1,0,""},
        };

        cat_pages[0]=buildCatShell("Pet",       UI_PURPLE, CAT_PET,  (int)(sizeof(CAT_PET)/sizeof(SItem)), 0);
        cat_pages[1]=buildCatShell("Tela",      UI_CYAN,   CAT_DISP, (int)(sizeof(CAT_DISP)/sizeof(SItem)), 1);
        cat_pages[2]=buildCatShell("Energia",   UI_ORANGE, CAT_PWR,  (int)(sizeof(CAT_PWR)/sizeof(SItem)), 2);
        cat_pages[3]=buildCatShell("Bateria",   UI_GREEN,  CAT_BAT,  (int)(sizeof(CAT_BAT)/sizeof(SItem)), 3);
        cat_pages[4]=buildCatShell("WiFi / Ataques", UI_RED, CAT_WIFI,(int)(sizeof(CAT_WIFI)/sizeof(SItem)), 4);
        cat_pages[5]=buildCatShell("Bluetooth", UI_CYAN,   CAT_BLE,  (int)(sizeof(CAT_BLE)/sizeof(SItem)), 5);
        cat_pages[6]=buildCatShell("Sistema",   UI_GREEN,  CAT_SYS,  (int)(sizeof(CAT_SYS)/sizeof(SItem)), 6);
        cat_pages[7]=buildCatShell("Web",       UI_PURPLE, CAT_WEB,  (int)(sizeof(CAT_WEB)/sizeof(SItem)), 7);

        lv_obj_t *c = mkSub("Ajustes", UI_PURPLE, &page_settings);
        const char *names[] = {"Pet","Tela","Energia","Bateria","WiFi / Ataques","Bluetooth","Sistema","Web"};
        lv_color_t cols[]   = {UI_PURPLE,UI_CYAN,UI_ORANGE,UI_GREEN,UI_RED,UI_CYAN,UI_GREEN,UI_PURPLE};
        for (int i=0;i<8;i++) mkBtn(c, names[i], cols[i], cbOpenCat, (void*)(intptr_t)i);
    }

    // ---------------------------------------------------- teclado overlay
    static void buildKeyboard() {
        kb_overlay = lv_obj_create(scr_main);
        lv_obj_set_size(kb_overlay, LCD_WIDTH, LCD_HEIGHT);
        lv_obj_set_pos(kb_overlay, 0, 0);
        lv_obj_set_style_bg_color(kb_overlay, UI_BG, 0);
        lv_obj_set_style_bg_opa(kb_overlay, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(kb_overlay, 0, 0);
        lv_obj_set_style_radius(kb_overlay, 0, 0);
        lv_obj_set_style_pad_all(kb_overlay, 8, 0);
        lv_obj_remove_flag(kb_overlay, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(kb_overlay, LV_OBJ_FLAG_HIDDEN);

        kb_title = lv_label_create(kb_overlay);
        lv_obj_set_style_text_font(kb_title, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(kb_title, UI_CYAN, 0);
        lv_label_set_text(kb_title, "Editar");
        lv_obj_align(kb_title, LV_ALIGN_TOP_LEFT, 6, 4);

        kb_ta = lv_textarea_create(kb_overlay);
        lv_obj_set_size(kb_ta, LCD_WIDTH - 24, 46);
        lv_obj_align(kb_ta, LV_ALIGN_TOP_MID, 0, 28);
        lv_textarea_set_one_line(kb_ta, true);
        lv_obj_set_style_text_font(kb_ta, &lv_font_montserrat_16, 0);

        kb_widget = lv_keyboard_create(kb_overlay);
        lv_obj_set_size(kb_widget, LCD_WIDTH, 250);
        lv_obj_align(kb_widget, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_keyboard_set_textarea(kb_widget, kb_ta);
        lv_obj_add_event_cb(kb_widget, cbKb, LV_EVENT_READY, NULL);
        lv_obj_add_event_cb(kb_widget, cbKb, LV_EVENT_CANCEL, NULL);
    }

    // ============================================================= nav HW
    static void nextTile() { if (nav_top==0) { beep(); pushPage(page_apps); } else { beep(); back(); } }

    // ============================================================= refresh
    static void refresh() {
        String ts = PwnRTC::getTimestamp();
        if (lbl_time) lv_label_set_text(lbl_time, PwnRTC::getClock().c_str());
        if (lbl_date) lv_label_set_text(lbl_date, dateStr(ts).c_str());

        int batt = PwnPower::getBatteryPercent();
        bool chg = PwnPower::isCharging();
        bool usb = PwnPower::isVbusIn();
        if (lbl_batt) {
            lv_label_set_text_fmt(lbl_batt, "%s %d%%",
                chg ? LV_SYMBOL_CHARGE : (usb ? LV_SYMBOL_USB : LV_SYMBOL_BATTERY_FULL), batt);
            lv_obj_set_style_text_color(lbl_batt, (batt<15 && !usb) ? UI_RED : (chg ? UI_GREEN : UI_TEXT), 0);
        }
        PetStats pet = PwnPet::getStats();
        if (lbl_home_pet)
            lv_label_set_text_fmt(lbl_home_pet, "%s  -  %s  -  Lv%d",
                pet.name.c_str(), PwnPet::getMoodStr().c_str(), (int)Gamification::getStats().level);
        if (lbl_status) {
            String st="";
            if (WiFiTools::isSniffing()) st += LV_SYMBOL_WIFI " ";
            if (EvilPortal::isRunning()) st += LV_SYMBOL_WARNING " ";
            if (st=="") st = LV_SYMBOL_OK;
            lv_label_set_text(lbl_status, st.c_str());
        }

        if (visible(page_wifi) && lbl_wifi) {
            String w = String("Sniffer: ") + (WiFiTools::isSniffing()?"ATIVO":"parado") + "\n";
            w += "Dispositivos: " + String((int)WiFiTools::nearby_devices.size()) + "\n";
            w += "Handshakes: " + String((unsigned)WiFiTools::getHandshakeCount()) + "\n\n";
            int shown=0;
            for (auto &d : WiFiTools::nearby_devices) { w += d.mac + "  " + String(d.rssi) + "dBm\n"; if(++shown>=7) break; }
            if (WiFiTools::nearby_devices.empty()) w += "(procurando...)";
            lv_label_set_text(lbl_wifi, w.c_str());
        }
        if (visible(page_ble) && lbl_ble) {
            String b = "Ultimo scan: " + String(PwnBLE::getLastCount()) + " disp.\n";
            b += "Total visto: " + String(PwnBLE::getTotalSeen()) + "\n";
            b += "Toque em ESCANEAR p/ varredura.";
            lv_label_set_text(lbl_ble, b.c_str());
        }
        if (visible(page_atk) && lbl_atk) {
            String a = String("Evil Portal: ") + (EvilPortal::isRunning()?"ATIVO":"parado") + "\n";
            a += "Capturados: " + String(EvilPortal::getCapturedCount()) + "\n";
            a += String("Deauth: ") + (ConfigManager::getInstance()->get<bool>("atk_deauth_enabled")?"habilitado":"bloqueado");
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
            String p = "Bateria: " + String(batt) + "%  (" + String(PwnPower::getBatteryVoltage(),2) + " V)\n";
            p += "Status: " + PwnPower::getChargeStatusStr() + "\n";
            p += "Autonomia: ~" + String(PwnPower::getEstimatedHours(),1) + " h\n";
            p += "RAM livre: " + String(ESP.getFreeHeap()/1024) + " KB";
            lv_label_set_text(lbl_power, p.c_str());
        }
        if (visible(page_sys) && lbl_sys) {
            ConfigManager *cfg = ConfigManager::getInstance();
            uint32_t up = millis()/1000;
            String s = "Nome: " + cfg->getString("sys_device_name") + "\n";
            s += "Modo WiFi: " + cfg->getString("sys_wifi_mode") + "\n";
            s += "AP: " + cfg->getString("sys_ap_ssid") + "\n";
            s += String("RTC: ") + (PwnRTC::isOk()?"OK":"sem chip") + "\n";
            s += "Uptime: " + String(up/3600) + "h " + String((up/60)%60) + "m";
            lv_label_set_text(lbl_sys, s.c_str());
        }
        if (visible(page_tools) && lbl_tools) {
            String t = "Voz: toque OUVIR p/ captar comando.\nFALAR emite resposta TTS.\n";
            t += "Handshakes: " + String((unsigned)WiFiTools::getHandshakeCount());
            lv_label_set_text(lbl_tools, t.c_str());
        }
        if (visible(page_sensors) && lbl_sensors) {
            float x,y,z;
            if (UI_getAccel(x,y,z)) {
                float mag = sqrtf(x*x+y*y+z*z);
                lv_label_set_text_fmt(lbl_sensors, "X: %+.2f g\nY: %+.2f g\nZ: %+.2f g\n|a|: %.2f g", x,y,z,mag);
                int px=(int)((x*0.5f+0.5f)*100); if(px<0)px=0; if(px>100)px=100;
                int py=(int)((y*0.5f+0.5f)*100); if(py<0)py=0; if(py>100)py=100;
                if (bar_sx) lv_bar_set_value(bar_sx, px, LV_ANIM_OFF);
                if (bar_sy) lv_bar_set_value(bar_sy, py, LV_ANIM_OFF);
            } else {
                lv_label_set_text(lbl_sensors, "IMU indisponivel");
            }
        }
        if (visible(page_sw) && lbl_sw) {
            uint32_t el = g_sw_acc + (g_sw_run ? millis()-g_sw_start : 0);
            uint32_t s = el/1000;
            lv_label_set_text_fmt(lbl_sw, "%02u:%02u", (unsigned)(s/60), (unsigned)(s%60));
        }
        // timer dispara mesmo com a página fechada
        if (g_tmr_run) {
            long rem = (long)g_tmr_end - (long)millis();
            if (rem <= 0) { g_tmr_run=false; rem=0; AudioHandler::beepWarn(); }
            if (visible(page_timer) && lbl_timer) { uint32_t s=(uint32_t)rem/1000; lv_label_set_text_fmt(lbl_timer,"%02u:%02u",(unsigned)(s/60),(unsigned)(s%60)); }
        } else if (visible(page_timer) && lbl_timer) {
            lv_label_set_text_fmt(lbl_timer, "%02u:%02u", (unsigned)g_tmr_set[0], (unsigned)g_tmr_set[1]);
        }
    }
    static void update() { refresh(); }

private:
    static String dateStr(const String &ts) {
        if (ts.length()<10 || ts.charAt(4)!='-') return String("");
        int y=ts.substring(0,4).toInt(), mo=ts.substring(5,7).toInt(), d=ts.substring(8,10).toInt();
        if (y<2020 || mo<1 || mo>12) return String("");
        static const char *wd[]={"DOM","SEG","TER","QUA","QUI","SEX","SAB"};
        static const char *mn[]={"JAN","FEV","MAR","ABR","MAI","JUN","JUL","AGO","SET","OUT","NOV","DEZ"};
        struct tm tmv={}; tmv.tm_year=y-1900; tmv.tm_mon=mo-1; tmv.tm_mday=d; tmv.tm_hour=12;
        mktime(&tmv); int w=tmv.tm_wday; if(w<0||w>6) w=0;
        char b[24]; snprintf(b,sizeof(b),"%s  %02d %s", wd[w], d, mn[mo-1]);
        return String(b);
    }
};

#endif
