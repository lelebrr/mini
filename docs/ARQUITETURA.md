# Arquitetura — Mini Lele v2.1

Firmware **Arduino** (cooperativo, `setup()/loop()`) para Waveshare
ESP32-S3-Touch-AMOLED-1.8. Não é multitarefa dedicada por subsistema (isso é a
meta da reescrita IDF SENTINEL-S3); tudo roda no loop principal + callbacks.

## Módulos (`src/`, `include/`)

| Módulo | Papel |
|--------|-------|
| `ConfigManager` | Config em `/config.json` (+ NVS), get/set/JSON |
| `PwnPower` | AXP2101: bateria, carga, temperatura, deep sleep |
| `PwnPet` + `Gamification` | Pet virtual (XP/nível/humor/evolução), stats |
| `PwnUI` + `FaceHandler` | Tiles LVGL, barra de status, notificações, rosto |
| `WiFiTools` | Sniffer promíscuo + PCAP + fila de captura + EAPOL count |
| `WpsBlue` / `WpsRed` | WPS: inventário BLUE / enrollee RED de laboratório |
| `EspNowScan` | Inventário ESP-NOW (BLUE, passivo): detector + peers + relatório |
| `PwnBLE` | Scan GAP BLE (Bluedroid); alimenta `BleBlue` com o AD cru |
| `BleBlue` | Inventário BLE BLUE: parser de AD, vendor, endereço, relatório |
| `BleIds` | IDS BLE (BLUE): spam, HID, stalker+IMU, Flipper, baseline, foxhunt |
| `BleGatt` | GATT audit BLE (BLUE, só allowlist): enumerar + write exposta |
| `PwnAttack` | Orquestra scan/deauth/portal/alvo |
| `EvilPortal` | SoftAP + captive portal + captura de credenciais |
| `OnlineCrack` | Upload de handshakes p/ serviço externo |
| `AudioHandler` / `PwnVoice` | ES8311 (beep/wav/record) / voz |
| `PwnSleep` | Dim/off/deep-sleep por timeout |
| `WebHandler` (+ `web_assets`) | WebUI Async + DNS de portal cativo |
| `drivers/TCA9554`, `TouchFT3168`, `PwnRTC` | Expansor IO, touch, RTC |

## Sequência de boot (`setup()`)

1. `Serial` (USB CDC), `Wire` (SDA15/SCL14 @400 kHz)
2. `initIOExpander()` — TCA9554: power-on rails, solta LCD_RST/TP_RST, SD_CS HIGH
3. `PwnPower::init()` — AXP2101
4. `initDisplay()` — Arduino_GFX SH8601 via QSPI (368×448)
5. `touchDev.begin()` — FT3168 @0x38
6. `initIMU()` — QMI8658
7. `initSD()` — SD_MMC 1-bit, cria diretórios
8. `initLVGL()` — LVGL 9, double buffer em PSRAM, tema escuro
9. Módulos: Config, RTC, Gamification, Pet, Audio, UI, Attack, Voice, Sleep, Web
10. **WPS**: `WpsBlue::init()` + `WpsRed::init()` + allowlist
11. Watchdog de tarefa (opcional), sniffer automático (se `atk_auto_scan`)

## Loop principal

`lv_timer_handler` → `WiFiTools::flush` → **`WpsBlue::poll`** → **`wpsSerialPoll`**
→ WebUI/portal → botão/shake → tarefas por segundo (`PwnPet`, `Gamification`,
`PwnPower`, `PwnAttack`, **`WpsRed::tick`**, `PwnSleep`, `PwnUI`) → scan BLE
periódico → persistência 60 s → `esp_task_wdt_reset`.

## Persistência (raiz do cartão SD)

```
/config.json               config
/pwn_pet_save.json         pet
/game_stats.bin            gamification
/devices.json             inventário de dispositivos
/portal_creds.json         credenciais do Evil Portal
/capturas/*.pcap           capturas Wi-Fi
/reports/wps_inventory.md  inventário WPS (BLUE)
/wps/baseline.jsonl        baseline WPS por BSSID
/logs/wps_audit.jsonl      auditoria RED WPS
```
