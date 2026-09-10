#include "EspNowScan.h"
#include <SD_MMC.h>
#include <string.h>

EspNowPeer EspNowScan::_peers[EspNowScan::MAX_PEERS];

// OUI Espressif usado no action frame ESP-NOW
static const uint8_t ESPNOW_OUI[3] = { 0x18, 0xFE, 0x34 };

// ring cru (onFrame -> poll)
struct EnRec { uint8_t src[6]; int8_t rssi; uint8_t channel; uint16_t paylen; bool bcast; };
#define EN_RING 8
static EnRec         s_ring[EN_RING];
static volatile int  s_head = 0, s_tail = 0;
static portMUX_TYPE  s_mux = portMUX_INITIALIZER_UNLOCKED;

static bool macEq(const uint8_t*a,const uint8_t*b){ return memcmp(a,b,6)==0; }
static void macToStr(const uint8_t b[6],char*o){ snprintf(o,18,"%02X:%02X:%02X:%02X:%02X:%02X",b[0],b[1],b[2],b[3],b[4],b[5]); }

void EspNowScan::init(){ memset(_peers,0,sizeof(_peers)); s_head=0; s_tail=0; Serial.println("[EspNow] inventário ESP-NOW (BLUE)"); }

// ===========================================================================
// Detector puro. Layout do frame ESP-NOW:
//  [0..23]  header 802.11 (FC=0xD0 action; addr1 dest 4..9; addr2 src 10..15)
//  [24]     category = 0x7F (vendor specific action)
//  [25..27] OUI = 18 FE 34
//  [28..31] random
//  [32]     element id = 0xDD
//  [33]     element length
//  [34..36] OUI = 18 FE 34   [37] type=0x04   [38] version   [39..] body
// ===========================================================================
bool EspNowScan::isEspNow(const uint8_t* p, uint16_t len,
                          uint8_t src_out[6], bool* bcast_out, uint16_t* paylen_out){
    if(!p || len < 39) return false;
    if(p[0] != 0xD0) return false;                     // action (mgmt)
    if(p[24] != 0x7F) return false;                    // vendor specific action
    if(memcmp(&p[25], ESPNOW_OUI, 3) != 0) return false;
    if(p[32] != 0xDD) return false;                    // vendor element
    if(memcmp(&p[34], ESPNOW_OUI, 3) != 0) return false;
    if(p[37] != 0x04) return false;                    // ESP-NOW type
    if(src_out) memcpy(src_out, &p[10], 6);            // addr2 = src
    if(bcast_out){ bool b=true; for(int i=4;i<10;i++) if(p[i]!=0xFF){b=false;break;} *bcast_out=b; }
    if(paylen_out){ uint8_t el=p[33]; *paylen_out = (el>=5)?(uint16_t)(el-5):0; }  // -OUI3 -type1 -ver1
    return true;
}

void EspNowScan::onFrame(const uint8_t* payload, uint16_t len, int8_t rssi, uint8_t channel){
    uint8_t src[6]; bool bcast=false; uint16_t pl=0;
    if(!isEspNow(payload, len, src, &bcast, &pl)) return;
    portENTER_CRITICAL(&s_mux);
    int next=(s_head+1)%EN_RING;
    if(next!=s_tail){ memcpy(s_ring[s_head].src,src,6); s_ring[s_head].rssi=rssi;
        s_ring[s_head].channel=channel; s_ring[s_head].paylen=pl; s_ring[s_head].bcast=bcast; s_head=next; }
    portEXIT_CRITICAL(&s_mux);
}

void EspNowScan::poll(){
    for(;;){
        EnRec r;
        portENTER_CRITICAL(&s_mux);
        if(s_tail==s_head){ portEXIT_CRITICAL(&s_mux); break; }
        r=s_ring[s_tail]; s_tail=(s_tail+1)%EN_RING;
        portEXIT_CRITICAL(&s_mux);

        EspNowPeer* e=_slotFor(r.src);
        if(!e) continue;
        if(!e->used){ memset(e,0,sizeof(*e)); e->used=true; memcpy(e->src,r.src,6); e->first_seen=millis(); }
        e->rssi=r.rssi; e->channel=r.channel; e->last_len=r.paylen; e->broadcast=r.bcast;
        e->last_seen=millis(); e->count++;
    }
}

EspNowPeer* EspNowScan::_slotFor(const uint8_t src[6]){
    int free_i=-1;
    for(int i=0;i<MAX_PEERS;i++){ if(_peers[i].used&&macEq(_peers[i].src,src)) return &_peers[i]; if(!_peers[i].used&&free_i<0) free_i=i; }
    if(free_i>=0) return &_peers[free_i];
    int old=0; for(int i=1;i<MAX_PEERS;i++) if(_peers[i].last_seen<_peers[old].last_seen) old=i;
    return &_peers[old];
}

int EspNowScan::count(){ int n=0; for(int i=0;i<MAX_PEERS;i++) if(_peers[i].used) n++; return n; }
const EspNowPeer* EspNowScan::get(int idx){ int n=0; for(int i=0;i<MAX_PEERS;i++){ if(!_peers[i].used)continue; if(n==idx)return &_peers[i]; n++; } return nullptr; }
void EspNowScan::clear(){ memset(_peers,0,sizeof(_peers)); }

String EspNowScan::reportSection(){
    String s="## ESP-NOW inventory\n_BLUE — observação passiva. "; s+=String(count()); s+=" peer(s)._\n";
    char mac[18];
    for(int i=0;i<MAX_PEERS;i++){ const EspNowPeer*e=&_peers[i]; if(!e->used) continue; macToStr(e->src,mac);
        s+="\n- `"; s+=mac; s+="`  ch "; s+=String((int)e->channel);
        s+=" | rssi "; s+=String((int)e->rssi); s+=" dBm";
        s+=" | payload "; s+=String((int)e->last_len); s+="B";
        s+=e->broadcast?" | broadcast":" | unicast";
        s+=" | "; s+=String((unsigned)e->count); s+="x\n"; }
    if(count()==0) s+="\n_(nenhum peer ESP-NOW observado)_\n";
    return s;
}
bool EspNowScan::writeReport(const char* path){
    if(!SD_MMC.exists("/reports")) SD_MMC.mkdir("/reports");
    File f=SD_MMC.open(path, FILE_WRITE);
    if(!f){ Serial.println("[EspNow] falha ao gravar relatorio"); return false; }
    f.print(reportSection()); f.close();
    Serial.printf("[EspNow] relatorio em %s\n",path); return true;
}
