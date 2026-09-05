#include <vector>

#include "core/ConfigManager.h"
#include "Gamification.h"
#include "core/PwnPower.h"
#include "WiFiTools.h"
#include "core/PwnVoice.h"
#include "core/PwnPet.h"
#include "core/PwnUI.h"
#include "core/PwnAttack.h"
#include "AudioHandler.h"

// ConfigManager singleton
ConfigManager *ConfigManager::instance = nullptr;

// Gamification globals
GameStats Gamification::stats;
const char *Gamification::saveFile = "/game_stats.bin";
uint32_t Gamification::last_tick_ms = 0;

// PwnPower globals
XPowersPMU PwnPower::pmu;
bool       PwnPower::pmu_ok = false;
bool       PwnPower::is_critical_flag = false;
bool       PwnPower::last_vbus = false;
bool       PwnPower::last_charging = false;

// RAM RTC preservada entre deep sleeps (declarada extern em PwnPower.h)
RTC_DATA_ATTR RTC_SaveData rtc_save;

// WiFiTools sniffer list
std::vector<SniffedDevice> WiFiTools::nearby_devices;

// PwnVoice state
bool PwnVoice::is_listening = false;

// PwnPet state
PetStats PwnPet::stats;
const char *PwnPet::savePath = "/pwn_pet_save.json";

// PwnUI objects
lv_obj_t *PwnUI::scr_main       = nullptr;
lv_obj_t *PwnUI::bar_hunger     = nullptr;
lv_obj_t *PwnUI::bar_happy      = nullptr;
lv_obj_t *PwnUI::label_clock    = nullptr;
lv_obj_t *PwnUI::label_stats    = nullptr;
lv_obj_t *PwnUI::label_battery  = nullptr;
lv_obj_t *PwnUI::label_name     = nullptr;
lv_obj_t *PwnUI::label_mood     = nullptr;

// PwnAttack stats
AttackStats PwnAttack::stats;
bool        PwnAttack::scan_active = false;

// AudioHandler state
bool AudioHandler::audio_ok = false;