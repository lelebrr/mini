#ifndef WPS_BLUE_H
#define WPS_BLUE_H
//
// WpsBlue.h — WPS BLUE (defensivo): parser do IE WPS, inventario, chip de
// higiene, baseline e relatorio. NAO transmite nada. NAO ataca.
//
// Fluxo: WiFiTools (modo promiscuo) chama WpsBlue::onFrame() para cada beacon/
// probe-response. onFrame() faz um check rapido do OUI e enfileira o quadro
// cru; WpsBlue::poll() (chamado no loop) faz o parse pesado dos TLVs e atualiza
// o inventario, a higiene e o baseline.
//
#include "WpsCommon.h"

class WpsBlue {
public:
    static const int MAX_APS = 48;

    static void init();

    // Chamado do callback promiscuo do Wi-Fi (contexto da task de Wi-Fi).
    // Faz apenas: filtro beacon/probe-resp + presenca do OUI + copia limitada
    // para um ring. O parse acontece em poll().
    static void onFrame(const uint8_t* payload, uint16_t len, int8_t rssi, uint8_t channel);

    // Chamado no loop principal: drena o ring e faz o parse -> inventario.
    static void poll();

    // ---- Parser puro (tambem usado em testes) --------------------------------
    // Faz o parse de um quadro de gerenciamento 802.11 (beacon/probe-resp).
    // Preenche 'out' (WpsInfo) e, se fornecidos, bssid/ssid/channel. Retorna
    // true se o quadro continha um IE WPS valido.
    static bool parseMgmtFrame(const uint8_t* frame, uint16_t len,
                               WpsInfo* out, uint8_t bssid_out[6],
                               char* ssid_out, size_t ssid_cap,
                               int* channel_out);
    // Faz o parse apenas do conteudo do IE WPS (apos OUI+type). Uso interno/testes.
    static bool parseWpsIe(const uint8_t* data, uint16_t len, WpsInfo* out);

    // ---- Inventario ----------------------------------------------------------
    static int  count();
    static const WpsApEntry* get(int i);
    static const WpsApEntry* findByBssid(const uint8_t bssid[6]);
    static void clear();

    // ---- Higiene -------------------------------------------------------------
    static WpsHygiene hygiene(const uint8_t bssid[6]);

    // ---- Baseline ------------------------------------------------------------
    // Baseline = mapa BSSID -> wps_estava_ligado, persistido no SD.
    // Alerta quando um AP conhecido (no baseline) passa a expor WPS.
    static void loadBaseline(const char* path = "/wps/baseline.jsonl");
    static bool saveBaseline(const char* path = "/wps/baseline.jsonl");
    // Registra o estado atual do inventario como novo baseline.
    static void snapshotBaseline();

    // ---- Relatorio -----------------------------------------------------------
    // Escreve a secao "WPS inventory" (markdown) no caminho dado.
    static bool writeReport(const char* path = "/reports/wps_inventory.md");
    // Gera a secao de relatorio numa String (para UI/serial).
    static String reportSection();

    // ---- Alertas -------------------------------------------------------------
    // Callback chamado em eventos BLUE (baseline flip, novo WPS-ON, etc).
    typedef void (*AlertCb)(const char* msg, uint32_t color);
    static void setAlertCallback(AlertCb cb);

    // Estatisticas rapidas
    static int countWpsOn();
    static int countWpsLocked();

private:
    static WpsApEntry _aps[MAX_APS];
    static portMUX_TYPE _mux;
    static AlertCb _alert;
    static void _emit(const char* msg, uint32_t color);
    static WpsApEntry* _slotFor(const uint8_t bssid[6]);  // acha/cria slot
    static void _applyPixieHeuristic(WpsApEntry* e);
    static void _checkBaseline(WpsApEntry* e, bool was_present, bool now_present);
};

#endif // WPS_BLUE_H
