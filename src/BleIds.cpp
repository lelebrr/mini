#include "BleIds.h"
#include <SD_MMC.h>

// ---- alertas ----
static BleIds::AlertCb _alert = nullptr;
static int  _alert_count = 0;

// ---- baseline (addrs conhecidos) ----
#define BLE_BASE_MAX 64
static uint8_t _base[BLE_BASE_MAX][6];
static int     _base_n = 0;

// ---- spam por categoria (contagem decaída) ----
enum { CAT_SWIFTPAIR=0, CAT_FASTPAIR, CAT_APPLE_NEARBY, CAT_SAMSUNG, CAT_APPLE_OTHER, CAT_N };
static const char* CAT_NAME[CAT_N] = { "Swift Pair", "Fast Pair", "Apple Nearby", "Samsung", "Apple Continuity" };
static float _spam[CAT_N] = {0};
static bool  _spam_flag[CAT_N] = {false};
static const float SPAM_THRESH = 25.0f;   // pico da janela decaída
static const float SPAM_CLEAR  = 10.0f;   // histerese

// ---- HID / Flipper dedup (por addr recente) ----
#define SEEN_MAX 48
static uint8_t _hid_seen[SEEN_MAX][6]; static int _hid_n = 0;
static uint8_t _flp_seen[SEEN_MAX][6]; static int _flp_n = 0;

// ---- stalker ----
struct StalkEntry { uint8_t addr[6]; uint16_t persistence; uint32_t last_episode; int8_t rssi; bool used, alerted; };
#define STALK_MAX 32
static StalkEntry _stalk[STALK_MAX];
static uint32_t _episode = 0;     // episódio de movimento corrente
static int      _motion_ttl = 0;  // >0 => em movimento
static const int MOTION_TTL = 3;  // ticks que o movimento "dura"
static const int STALK_THRESH = 3;// episódios distintos p/ disparar

// ---- foxhunt ----
static bool    _fox_active = false;
static uint8_t _fox_addr[6];
static int     _fox_rssi = 0;
static bool    _fox_seen = false;

// ===========================================================================
static bool macEq(const uint8_t*a,const uint8_t*b){ return memcmp(a,b,6)==0; }
static void macToStr(const uint8_t b[6],char*o){ snprintf(o,18,"%02X:%02X:%02X:%02X:%02X:%02X",b[0],b[1],b[2],b[3],b[4],b[5]); }
static bool seenHas(uint8_t arr[][6], int n, const uint8_t*a){ for(int i=0;i<n;i++) if(macEq(arr[i],a)) return true; return false; }
static void seenAdd(uint8_t arr[][6], int&n, const uint8_t*a){ if(n<SEEN_MAX) memcpy(arr[n++],a,6); }

void BleIds::setAlertCallback(AlertCb cb){ _alert=cb; }
void BleIds::_emit(const char* msg, uint32_t color){
    _alert_count++;
    Serial.printf("[BleIds] %s\n", msg);
    if (_alert) _alert(msg, color);
    if (!SD_MMC.exists("/logs")) SD_MMC.mkdir("/logs");
    File f = SD_MMC.open("/logs/ble_ids.jsonl", FILE_APPEND);
    if (f) { f.printf("{\"t\":%lu,\"alert\":\"%s\"}\n",(unsigned long)millis(),msg); f.close(); }
}
int BleIds::alertCount(){ return _alert_count; }

void BleIds::init(){
    _alert_count=0; for(int i=0;i<CAT_N;i++){_spam[i]=0;_spam_flag[i]=false;}
    _hid_n=_flp_n=0; memset(_stalk,0,sizeof(_stalk));
    _episode=0; _motion_ttl=0; _fox_active=false; _fox_seen=false; _fox_rssi=0;
    Serial.println("[BleIds] IDS BLE inicializado (BLUE)");
    loadBaseline();
}

// ---- baseline ----
void BleIds::loadBaseline(const char* path){
    _base_n=0;
    File f=SD_MMC.open(path, FILE_READ);
    if(!f){ Serial.println("[BleIds] baseline BLE ausente (ok)"); return; }
    while(f.available() && _base_n<BLE_BASE_MAX){
        String line=f.readStringUntil('\n'); line.trim();
        if(line.length()<17 || line[0]=='#') continue;
        int h=line.indexOf('#'); String mp=(h>=0)?line.substring(0,h):line; mp.trim();
        uint8_t b[6];
        if(sscanf(mp.c_str(),"%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",&b[0],&b[1],&b[2],&b[3],&b[4],&b[5])==6)
            memcpy(_base[_base_n++],b,6);
    }
    f.close();
    Serial.printf("[BleIds] baseline BLE: %d addr(s)\n",_base_n);
}
bool BleIds::isKnown(const uint8_t addr[6]){ for(int i=0;i<_base_n;i++) if(macEq(_base[i],addr)) return true; return false; }

// ---- movimento ----
void BleIds::notifyMotion(){
    if(_motion_ttl==0) _episode++;   // borda de subida => novo episódio
    _motion_ttl=MOTION_TTL;
}
void BleIds::tick(){
    if(_motion_ttl>0) _motion_ttl--;
    for(int i=0;i<CAT_N;i++){
        _spam[i]*=0.75f;                       // decaimento
        if(_spam_flag[i] && _spam[i]<SPAM_CLEAR) _spam_flag[i]=false;  // histerese
    }
}

// ---- stalker helpers ----
static StalkEntry* stalkSlot(const uint8_t addr[6]){
    int free_i=-1;
    for(int i=0;i<STALK_MAX;i++){ if(_stalk[i].used&&macEq(_stalk[i].addr,addr)) return &_stalk[i]; if(!_stalk[i].used&&free_i<0) free_i=i; }
    if(free_i>=0){ memset(&_stalk[free_i],0,sizeof(StalkEntry)); memcpy(_stalk[free_i].addr,addr,6); _stalk[free_i].used=true; return &_stalk[free_i]; }
    return nullptr;
}

// ===========================================================================
void BleIds::onEvent(const BleDevice* e, const BleParsed* p){
    // foxhunt
    if(_fox_active && macEq(e->addr,_fox_addr)){ _fox_rssi=e->rssi; _fox_seen=true; }

    // HID inesperado
    if(p->hid && !seenHas(_hid_seen,_hid_n,e->addr)){
        seenAdd(_hid_seen,_hid_n,e->addr);
        char m[80],mac[18]; macToStr(e->addr,mac);
        snprintf(m,sizeof(m),"HID/HOGP anunciado por %s (%s)",mac,e->ad.has_name?e->ad.name:"?");
        _emit(m,0xFFB400);
    }
    // Flipper
    if(p->vendor==VND_FLIPPER && !seenHas(_flp_seen,_flp_n,e->addr)){
        seenAdd(_flp_seen,_flp_n,e->addr);
        char m[64],mac[18]; macToStr(e->addr,mac);
        snprintf(m,sizeof(m),"Flipper Zero detectado (%s)",mac);
        _emit(m,0xFFB400);
    }
    // spam por categoria
    int cat=-1;
    switch(p->vendor){
        case VND_MS_SWIFTPAIR:    cat=CAT_SWIFTPAIR; break;
        case VND_GOOGLE_FASTPAIR: cat=CAT_FASTPAIR; break;
        case VND_SAMSUNG:         cat=CAT_SAMSUNG; break;
        case VND_APPLE_CONTINUITY:
            cat=(p->cont_type==APPLE_CONT_NEARBY_ACTION||p->cont_type==APPLE_CONT_NEARBY_INFO)?CAT_APPLE_NEARBY:CAT_APPLE_OTHER; break;
        default: break;
    }
    if(cat>=0){
        _spam[cat]+=1.0f;
        if(!_spam_flag[cat] && _spam[cat]>=SPAM_THRESH){
            _spam_flag[cat]=true;
            char m[64]; snprintf(m,sizeof(m),"Spam BLE: %s (flood de anúncios)",CAT_NAME[cat]);
            _emit(m,0xFF3232);
        }
    }
    // stalker: tag Find My / Tile / random-static, não-conhecida, persistente em movimento
    bool taggy = (p->vendor==VND_APPLE_FINDMY||p->vendor==VND_TILE||e->addr_kind==BLE_ADDR_RANDOM_STATIC);
    if(taggy && !isKnown(e->addr)){
        StalkEntry* s=stalkSlot(e->addr);
        if(s){
            s->rssi=e->rssi;
            if(_motion_ttl>0 && _episode>s->last_episode){   // visto em novo episódio de movimento
                s->last_episode=_episode; s->persistence++;
                if(!s->alerted && s->persistence>=STALK_THRESH){
                    s->alerted=true;
                    char m[96],mac[18]; macToStr(e->addr,mac);
                    snprintf(m,sizeof(m),"STALKER? tag %s te acompanha (%d janelas, rssi %d)",mac,s->persistence,(int)e->rssi);
                    _emit(m,0xFF3232);
                }
            }
        }
    }
}

// ---- foxhunt ----
void BleIds::foxhuntStart(const uint8_t addr[6]){ memcpy(_fox_addr,addr,6); _fox_active=true; _fox_seen=false; _fox_rssi=0; }
void BleIds::foxhuntStop(){ _fox_active=false; }
bool BleIds::foxhuntActive(){ return _fox_active; }
int  BleIds::foxhuntRssi(){ return _fox_seen?_fox_rssi:0; }
int  BleIds::beepIntervalForRssi(int rssi){
    if(rssi>-30) rssi=-30; if(rssi<-100) rssi=-100;    // clamp
    // -100 dBm -> 1500 ms ; -30 dBm -> 90 ms (linear)
    float t=(float)(rssi+100)/70.0f;                    // 0..1
    return (int)(1500 - t*(1500-90));
}
int  BleIds::foxhuntBeepIntervalMs(){ return _fox_seen?beepIntervalForRssi(_fox_rssi):0; }
