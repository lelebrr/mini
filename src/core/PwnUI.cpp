#include "core/PwnUI.h"
#include "core/ConfigManager.h"
#include "core/PwnPower.h"
#include "WiFiTools.h"
#include <lvgl.h>

static lv_obj_t *screen = nullptr;
static lv_obj_t *tileview = nullptr;
static lv_obj_t *status_bar = nullptr;
static lv_obj_t *lbl_mode = nullptr;
static lv_obj_t *lbl_battery = nullptr;
static lv_obj_t *lbl_wifi = nullptr;
static lv_obj_t *lbl_ble = nullptr;
static lv_obj_t *lbl_sd = nullptr;
static lv_obj_t *lbl_temp = nullptr;

static int current_tile = 0;
static int num_tiles = 6;
static uint32_t last_update = 0;

#define COLOR_BG     lv_color_hex(0x000000)
#define COLOR_BLUE   lv_color_hex(0x00FFFF)
#define COLOR_RED    lv_color_hex(0xFF3333)
#define COLOR_GREEN  lv_color_hex(0x00FF00)
#define COLOR_AMBER  lv_color_hex(0xFFAA00)
#define COLOR_WHITE  lv_color_hex(0xFFFFFF)
#define COLOR_GREY   lv_color_hex(0x666666)

void PwnUI::init() {
    screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, COLOR_BG, 0);

    status_bar = lv_obj_create(screen);
    lv_obj_set_size(status_bar, 368, 30);
    lv_obj_align(status_bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(status_bar, lv_color_hex(0x111111), 0);
    lv_obj_set_style_border_width(status_bar, 0, 0);
    lv_obj_set_flex_flow(status_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(status_bar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lbl_mode = lv_label_create(status_bar);
    lv_label_set_text(lbl_mode, "BLUE");
    lv_obj_set_style_text_color(lbl_mode, COLOR_BLUE, 0);

    lbl_battery = lv_label_create(status_bar);
    lv_label_set_text(lbl_battery, "---%");
    lv_obj_set_style_text_color(lbl_battery, COLOR_WHITE, 0);

    lbl_wifi = lv_label_create(status_bar);
    lv_label_set_text(lbl_wifi, "WiFi:0");
    lv_obj_set_style_text_color(lbl_wifi, COLOR_GREY, 0);

    lbl_ble = lv_label_create(status_bar);
    lv_label_set_text(lbl_ble, "BLE:0");
    lv_obj_set_style_text_color(lbl_ble, COLOR_GREY, 0);

    lbl_sd = lv_label_create(status_bar);
    lv_label_set_text(lbl_sd, "SD");
    lv_obj_set_style_text_color(lbl_sd, COLOR_GREY, 0);

    lbl_temp = lv_label_create(status_bar);
    lv_label_set_text(lbl_temp, "--C");
    lv_obj_set_style_text_color(lbl_temp, COLOR_WHITE, 0);

    tileview = lv_tileview_create(screen);
    lv_obj_set_size(tileview, 368, 418);
    lv_obj_align(tileview, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(tileview, COLOR_BG, 0);

    for (int i = 0; i < num_tiles; i++) {
        lv_obj_t *tile = lv_tileview_add_tile(tileview, i, 0, LV_DIR_HOR);
        lv_obj_set_style_bg_color(tile, COLOR_BG, 0);
        lv_obj_t *lbl = lv_label_create(tile);
        lv_obj_set_style_text_color(lbl, COLOR_WHITE, 0);
        switch (i) {
            case 0: lv_label_set_text(lbl, "HOME\n\nMini Lele\nPronto!"); break;
            case 1: lv_label_set_text(lbl, "WiFi\n\nAguardando..."); break;
            case 2: lv_label_set_text(lbl, "BLE\n\nAguardando..."); break;
            case 3: lv_label_set_text(lbl, "Ataques\n\nAguardando..."); break;
            case 4: lv_label_set_text(lbl, "Captura\n\nAguardando..."); break;
            case 5: lv_label_set_text(lbl, "Configuracoes\n\nAjustes do sistema"); break;
        }
    }
}

void PwnUI::update() {
    if (millis() - last_update < 500) return;
    last_update = millis();
    updateStatusBar();
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
    lv_obj_t *notif = lv_msgbox_create(NULL);
    lv_obj_set_style_bg_color(notif, lv_color_hex(0x222222), 0);
    lv_obj_t *lbl = lv_msgbox_get_content(notif);
    lv_label_set_text(lbl, msg);
    lv_obj_set_style_text_color(lbl, lv_color_hex(color), 0);
    lv_obj_center(notif);
}

void PwnUI::showAlert(const char* msg, uint32_t color) { showNotification(msg, color); }

void PwnUI::showToast(const char* msg) {
    lv_obj_t *toast = lv_label_create(lv_screen_active());
    lv_label_set_text(toast, msg);
    lv_obj_set_style_text_color(toast, COLOR_WHITE, 0);
    lv_obj_set_style_bg_color(toast, lv_color_hex(0x333333), 0);
    lv_obj_set_style_pad_all(toast, 8, 0);
    lv_obj_align(toast, LV_ALIGN_BOTTOM_MID, 0, -40);
}

void PwnUI::updateStatusBar() {
    if (!status_bar) return;
    int pct = PwnPower::getBatteryPercent();
    char buf[16];
    snprintf(buf, sizeof(buf), "%d%%", pct);
    lv_label_set_text(lbl_battery, buf);
    lv_obj_set_style_text_color(lbl_battery, pct > 20 ? COLOR_GREEN : COLOR_RED, 0);
    snprintf(buf, sizeof(buf), "WiFi:%d", WiFiTools::getAPCount());
    lv_label_set_text(lbl_wifi, buf);
    snprintf(buf, sizeof(buf), "BLE:%d", WiFiTools::getDeviceCount());
    lv_label_set_text(lbl_ble, buf);
    snprintf(buf, sizeof(buf), "%.0fC", PwnPower::getTemperature());
    lv_label_set_text(lbl_temp, buf);
}

void PwnUI::updateBattery(int pct, bool charging) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%d%%%c", pct, charging ? '*' : ' ');
    lv_label_set_text(lbl_battery, buf);
}

void PwnUI::updateWiFi(bool connected, int ap_count) {
    char buf[16];
    snprintf(buf, sizeof(buf), "WiFi:%d", ap_count);
    lv_label_set_text(lbl_wifi, buf);
    lv_obj_set_style_text_color(lbl_wifi, connected ? COLOR_GREEN : COLOR_GREY, 0);
}

void PwnUI::updateBLE(int count) {
    char buf[16];
    snprintf(buf, sizeof(buf), "BLE:%d", count);
    lv_label_set_text(lbl_ble, buf);
}

void PwnUI::updateClock(const char* time_str) {}
void PwnUI::updatePetStats() {}
