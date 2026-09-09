#ifndef WPS_RED_H
#define WPS_RED_H
//
// WpsRed.h — WPS RED (laboratorio). Estritamente contratual:
//   * ARM fisico (GPIO) obrigatorio  -> senao RECUSA
//   * allowlist de BSSID obrigatoria -> vazia ou alvo fora dela = RECUSA
//   * auditoria de TODA tentativa
//
// So implementa fluxo de ENROLLEE via esp_wps:
//   * PBC por ate 120 s (contagem na UI)
//   * PIN enrollee: ate 5 PINs (device-PIN) de /sd/allowlist/wps_pins.txt, para
//   * PIN vendor: 1 candidato calculado do BSSID, 1 tiro (opcional)
//
// NAO implementa (proibido): brute-force online de PIN / maquina de estados
// Reaver / registrar-brute, Pixie Dust (sem extracao M1-M3), DoS/lockout de WPS
// como recurso, WPS por NFC/USB, WPS coringa (nao-allowlist).
//
// LIMITE DE HARDWARE: esp_wps NAO aceita BSSID alvo. O alvo e verificado por
// "soft targeting" (pos-associacao: se o AP associado != alvo -> aborta e
// audita VIOLATION). Ver docs/WPS.md.
//
#include "WpsCommon.h"

// ---- Configuracao (override via build_flags) ------------------------------
#ifndef WPS_ARM_GPIO
#define WPS_ARM_GPIO          17      // jumper/chave de ARM fisico (nivel ALTO = armado)
#endif
#ifndef WPS_ARM_ACTIVE_HIGH
#define WPS_ARM_ACTIVE_HIGH   1
#endif
#ifndef WPS_THERMAL_MAX_C
#define WPS_THERMAL_MAX_C     80.0f
#endif
#ifndef WPS_ENABLE_VENDOR_PIN
#define WPS_ENABLE_VENDOR_PIN 0       // 0 = gerador de PIN vendor desativado
#endif
#define WPS_PBC_TIMEOUT_MS         120000
#define WPS_PIN_MAX_ATTEMPTS       5
#define WPS_PIN_ATTEMPT_TIMEOUT_MS 35000
#define WPS_NACK_FLOOD_LIMIT       3
#define WPS_ALLOW_MAX              32

class WpsRed {
public:
    enum State {
        IDLE = 0, RUNNING_PBC, RUNNING_PIN, VERIFY,
        DONE_OK, DONE_FAIL, ABORTED, REFUSED
    };

    static void init();

    // ---- Gates ------------------------------------------------------------
    static bool isArmed();                                     // le o GPIO de ARM
    static int  loadAllowlist(const char* path = "/allowlist/bssid.txt");
    static bool isAllowlisted(const uint8_t bssid[6]);
    static int  allowlistCount();

    // ---- Operacoes RED (todas passam por ARM + allowlist + auditoria) -----
    // target_bssid DEVE estar na allowlist; alvo coringa e recusado.
    static bool redPbc(const uint8_t target_bssid[6]);
    static bool redPin(const uint8_t target_bssid[6]);        // ate 5 PINs do arquivo
    static bool redVendorPinOneShot(const uint8_t target_bssid[6]);
    static void abort(const char* reason);

    // ---- Loop -------------------------------------------------------------
    static void tick();                 // chame ~1x/s: timeouts, verificacao, stops
    static State state();
    static const char* stateStr();
    static int  secondsRemaining();     // p/ contagem na UI
    static int  pinAttempt();           // indice da tentativa PIN corrente

    // ---- Auditoria --------------------------------------------------------
    static bool audit(const char* mode, const uint8_t bssid[6],
                      const char* pin, const char* event, const char* result);

    // ---- Utilitario -------------------------------------------------------
    // Gera 1 candidato de PIN (8 digitos, checksum WPS) a partir do BSSID.
    // Heuristica generica (24-bit serial). NAO e um banco de PINs de fabricante.
    static void computeVendorPin(const uint8_t bssid[6], char out[9]);

    // Callback opcional de status (para UI/toast).
    typedef void (*StatusCb)(const char* msg, uint32_t color);
    static void setStatusCallback(StatusCb cb);

private:
    static bool _startCommon(const uint8_t target[6], const char* mode);  // gates
    static void _beginPbc();
    static void _beginPinAttempt();
    static void _stopWps();
    static bool _thermalTrip();
    static bool _targetLocked();
    static void _finish(State st, const char* result);
    static void _emit(const char* msg, uint32_t color);
};

#endif // WPS_RED_H
