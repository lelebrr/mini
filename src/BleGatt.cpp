#include "BleGatt.h"
#include <SD_MMC.h>

#if defined(ARDUINO)
#include <BLEDevice.h>
#include <BLEClient.h>
#include <map>
#endif

GattChar BleGatt::_chars[BleGatt::MAX_CHARS];
int      BleGatt::_n = 0;

// allowlist
#define GATT_ALLOW_MAX 32
static uint8_t _allow[GATT_ALLOW_MAX][6];
static int     _allow_n = 0;

static bool macEq(const uint8_t*a,const uint8_t*b){ return memcmp(a,b,6)==0; }
static void macToStr(const uint8_t b[6],char*o){ snprintf(o,18,"%02X:%02X:%02X:%02X:%02X:%02X",b[0],b[1],b[2],b[3],b[4],b[5]); }
static void macToFlat(const uint8_t b[6],char*o){ snprintf(o,13,"%02X%02X%02X%02X%02X%02X",b[0],b[1],b[2],b[3],b[4],b[5]); }

void BleGatt::init(){ _n=0; loadAllowlist(); }

// ---- allowlist ----
int BleGatt::loadAllowlist(const char* path){
    _allow_n=0;
    File f=SD_MMC.open(path, FILE_READ);
    if(!f){ Serial.println("[BleGatt] allowlist BLE ausente"); return 0; }
    while(f.available() && _allow_n<GATT_ALLOW_MAX){
        String line=f.readStringUntil('\n'); line.trim();
        if(line.length()<17 || line[0]=='#') continue;
        int h=line.indexOf('#'); String mp=(h>=0)?line.substring(0,h):line; mp.trim();
        uint8_t b[6];
        if(sscanf(mp.c_str(),"%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",&b[0],&b[1],&b[2],&b[3],&b[4],&b[5])==6)
            memcpy(_allow[_allow_n++],b,6);
    }
    f.close();
    Serial.printf("[BleGatt] allowlist: %d addr(s)\n",_allow_n);
    return _allow_n;
}
bool BleGatt::isAllowed(const uint8_t addr[6]){ for(int i=0;i<_allow_n;i++) if(macEq(_allow[i],addr)) return true; return false; }

// ---- puros ----
bool BleGatt::isOpenWrite(uint8_t props){ return (props & (GATT_PROP_WRITE|GATT_PROP_WRITE_NR)) != 0; }
void BleGatt::propsString(uint8_t props, char* out, size_t cap){
    out[0]=0; char t[24]=""; 
    if(props&GATT_PROP_READ)     strcat(t,"R ");
    if(props&GATT_PROP_WRITE)    strcat(t,"W ");
    if(props&GATT_PROP_WRITE_NR) strcat(t,"Wnr ");
    if(props&GATT_PROP_NOTIFY)   strcat(t,"N ");
    if(props&GATT_PROP_INDICATE) strcat(t,"I ");
    if(props&GATT_PROP_SIGNED_WR)strcat(t,"Sw ");
    size_t L=strlen(t); if(L && t[L-1]==' ') t[L-1]=0;
    strncpy(out,t,cap-1); out[cap-1]=0;
}
String BleGatt::formatReport(const uint8_t addr[6], const GattChar* chars, int n){
    char mac[18]; macToStr(addr,mac);
    String s="# GATT audit `"; s+=mac; s+="`\n";
    s+="_BLUE — enumeração autorizada (allowlist). Read-only, sem escrita._\n\n";
    s+="- characteristics: "; s+=String(n); s+="\n";
    int ow=0; for(int i=0;i<n;i++) if(chars[i].open_write) ow++;
    s+="- writes expostas: "; s+=String(ow); s+="\n";
    String cur="";
    for(int i=0;i<n;i++){
        if(cur!=String(chars[i].svc_uuid)){ cur=chars[i].svc_uuid; s+="\n## service "; s+=cur; s+="\n"; }
        char ps[24]; propsString(chars[i].props, ps, sizeof(ps));
        s+="- `"; s+=chars[i].chr_uuid; s+="`  ["; s+=ps; s+="]";
        if(chars[i].open_write) s+="  ⚠ WRITE exposta (revisar)";
        s+="\n";
    }
    if(n==0) s+="\n_(nenhuma characteristic enumerada)_\n";
    return s;
}

// ---- auditoria + log ----
void BleGatt::_auditLog(const uint8_t addr[6], const char* event, const char* detail){
    if(!SD_MMC.exists("/logs")) SD_MMC.mkdir("/logs");
    File f=SD_MMC.open("/logs/ble_gatt.jsonl", FILE_APPEND);
    if(!f) return;
    char mac[18]; macToStr(addr,mac);
    f.printf("{\"t\":%lu,\"target\":\"%s\",\"event\":\"%s\",\"detail\":\"%s\"}\n",
             (unsigned long)millis(), mac, event, detail?detail:"");
    f.close();
}
bool BleGatt::_writeReport(const uint8_t addr[6]){
    if(!SD_MMC.exists("/reports")) SD_MMC.mkdir("/reports");
    char flat[13]; macToFlat(addr,flat);
    char path[48]; snprintf(path,sizeof(path),"/reports/gatt-%s.md",flat);
    File f=SD_MMC.open(path, FILE_WRITE);
    if(!f){ Serial.println("[BleGatt] falha ao gravar relatorio"); return false; }
    f.print(formatReport(addr,_chars,_n)); f.close();
    Serial.printf("[BleGatt] relatorio em %s\n",path);
    return true;
}

int BleGatt::count(){ return _n; }
const GattChar* BleGatt::get(int i){ return (i>=0 && i<_n)?&_chars[i]:nullptr; }

#if defined(ARDUINO)
// ---- corpo real (Bluedroid central) ----
int BleGatt::audit(const uint8_t addr[6]){
    if(!isAllowed(addr)){ _auditLog(addr,"gate","REFUSED_NOT_ALLOWLISTED");
        Serial.println("[BleGatt] RECUSA: alvo fora da allowlist"); return -1; }
    _n=0;
    esp_bd_addr_t na; memcpy(na,addr,6);
    BLEAddress ba(na);
    _auditLog(addr,"connect","START");
    BLEClient* c = BLEDevice::createClient();
    bool ok = c->connect(ba);
    if(!ok){ _auditLog(addr,"connect","FAIL"); delete c;
        Serial.println("[BleGatt] falha de conexao"); return -2; }
    _auditLog(addr,"connect","OK");

    std::map<std::string, BLERemoteService*>* svcs = c->getServices();
    if(svcs){
        for(auto& sv : *svcs){
            BLERemoteService* s = sv.second;
            String suuid = s->getUUID().toString().c_str();
            std::map<std::string, BLERemoteCharacteristic*>* chs = s->getCharacteristics();
            if(!chs) continue;
            for(auto& ch : *chs){
                if(_n>=MAX_CHARS) break;
                BLERemoteCharacteristic* rc = ch.second;
                uint8_t props=0;
                if(rc->canRead())            props|=GATT_PROP_READ;
                if(rc->canWrite())           props|=GATT_PROP_WRITE;
                if(rc->canWriteNoResponse()) props|=GATT_PROP_WRITE_NR;
                if(rc->canNotify())          props|=GATT_PROP_NOTIFY;
                if(rc->canIndicate())        props|=GATT_PROP_INDICATE;
                GattChar* g=&_chars[_n++];
                strncpy(g->svc_uuid, suuid.c_str(), sizeof(g->svc_uuid)-1); g->svc_uuid[sizeof(g->svc_uuid)-1]=0;
                String cu = rc->getUUID().toString().c_str();
                strncpy(g->chr_uuid, cu.c_str(), sizeof(g->chr_uuid)-1); g->chr_uuid[sizeof(g->chr_uuid)-1]=0;
                g->props=props; g->open_write=isOpenWrite(props);
            }
        }
    }
    c->disconnect();
    delete c;
    _writeReport(addr);
    char det[32]; snprintf(det,sizeof(det),"%d chars",_n);
    _auditLog(addr,"done",det);
    Serial.printf("[BleGatt] %d characteristic(s) enumerada(s)\n",_n);
    return _n;
}
#else
// ---- host: sem Bluedroid; audit não roda (a lógica pura é testada) ----
int BleGatt::audit(const uint8_t addr[6]){ (void)addr; return -99; }
#endif
