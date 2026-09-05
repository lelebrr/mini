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
#include "drivers/PwnRTC.h"
#include "core/PwnSleep.h"
#include "core/PwnBLE.h"

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
int        PwnPower::battery_capacity_mah = 250;  // padrão (PL502030); ajustável

// RAM RTC preservada entre deep sleeps (declarada extern em PwnPower.h)
RTC_DATA_ATTR RTC_SaveData rtc_save;

// WiFiTools sniffer list + captura
std::vector<SniffedDevice> WiFiTools::nearby_devices;
CapFrame        WiFiTools::cap_queue[WiFiTools::CAP_QUEUE];
volatile int    WiFiTools::cap_head = 0;
volatile int    WiFiTools::cap_tail = 0;
volatile uint32_t WiFiTools::eapol_count = 0;
volatile uint32_t WiFiTools::frames_captured = 0;
bool            WiFiTools::sniffing = false;
bool            WiFiTools::pcap_header_written = false;
String          WiFiTools::pcap_path = "";

// PwnVoice state
bool PwnVoice::is_listening = false;

// PwnPet state
PetStats PwnPet::stats;
const char *PwnPet::savePath = "/pwn_pet_save.json";

// PwnUI objects
lv_obj_t *PwnUI::scr_main       = nullptr;
lv_obj_t *PwnUI::bar_hunger     = nullptr;
lv_obj_t *PwnUI::bar_happy      = nullptr;
lv_obj_t *PwnUI::tv             = nullptr;
lv_obj_t *PwnUI::label_clock    = nullptr;
lv_obj_t *PwnUI::label_stats    = nullptr;
lv_obj_t *PwnUI::label_battery  = nullptr;
lv_obj_t *PwnUI::label_name     = nullptr;
lv_obj_t *PwnUI::label_mood     = nullptr;
lv_obj_t *PwnUI::label_wifi     = nullptr;
lv_obj_t *PwnUI::label_power    = nullptr;

// PwnAttack stats
AttackStats PwnAttack::stats;
bool        PwnAttack::scan_active = false;

// AudioHandler state
bool AudioHandler::audio_ok = false;

// RTC PCF85063
SensorPCF85063 PwnRTC::rtc;
bool           PwnRTC::ok = false;

// PwnSleep
PwnSleep::BrightnessFn PwnSleep::set_brightness = nullptr;
uint32_t PwnSleep::last_activity_ms = 0;
int      PwnSleep::full_brightness  = 200;
int      PwnSleep::dim_after_s      = 15;
int      PwnSleep::off_after_s      = 30;
int      PwnSleep::deep_after_s     = 300;
bool     PwnSleep::deep_enabled     = false;
bool     PwnSleep::dimmed           = false;
bool     PwnSleep::screen_off       = false;

// PwnBLE
int PwnBLE::last_count = 0;
int PwnBLE::total_seen = 0;