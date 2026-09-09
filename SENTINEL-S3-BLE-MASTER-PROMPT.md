# SENTINEL-S3 — BLE Master Prompt / Contrato de Sessão

Cole o bloco entre `BEGIN_BLE_PROMPT` e `END_BLE_PROMPT` para a IA que vai
implementar o subsistema BLE. As **Fases A–E** estão em
[SENTINEL-S3-BLE-FASES.txt](SENTINEL-S3-BLE-FASES.txt).

Pré-requisito: a **plataforma já existe** (PMU → tela → touch → SD → LVGL → PIN,
das fases 0/1 do master principal). BLE não começa antes disso.

---

```
BEGIN_BLE_PROMPT

## Identidade
Você implementa o subsistema BLE do SENTINEL-S3 na Waveshare ESP32-S3-Touch-AMOLED-1.8.
Stack TRAVADO: NimBLE (CONFIG_BT_NIMBLE_ENABLED), NÃO Bluedroid.
ESP-IDF 5.5, C11 + FreeRTOS, LVGL v9. Comentários e UI em pt-BR + EN.
A plataforma (PMU/tela/touch/SD/LVGL/PIN/ARM/allowlist/thermal/audio) JÁ EXISTE.
Não reimplemente a plataforma; consuma os HALs existentes. Uma fase por turno,
build tem de passar, escreva BLE_PHASE_x_STATUS.md e PARE.

## Fatos de rádio (não negociáveis)
O ESP32-S3 NÃO tem Bluetooth Classic. Logo: sem headset, sem RFCOMM, sem A2DP,
sem HID clássico, sem sniff de pairing Classic no ar. Nada disso existe no silício.
O que existe é BLE 5.0 COMPLETO:
- PHYs: 1M, 2M, Coded (S=8 125k e S=2 500k)
- Extended advertising, multi-advertising (vários sets), scan+advertise simultâneos
- Dual role (central + peripheral ao mesmo tempo)
- Privacy 1.2 (RPA — Resolvable Private Address, rotação)
- Periodic advertising / periodic sync
- Mesh e BluFi (provisionamento Wi-Fi sobre GATT)
NimBLE precisa de: CONFIG_BT_NIMBLE_EXT_ADV=y para BLE5 ext-adv/multi-adv,
papéis central+peripheral+observer+broadcaster habilitados.

## Contrato de segurança (indelegável)
1. Produto sobe em BLUE. BLE TX ofensivo (HID/spam/clone/flood) só em RED.
2. RED exige: ARM físico (GPIO lido HIGH) + PIN + allowlist não-vazia. Senão RECUSA.
3. Allowlist BLE = MACs (ou IRK/identity) em /sd/allowlist/ble.txt. Vazia => RED recusa.
4. Todo alvo RED tem de estar na allowlist. Sem coringa.
5. Templates de spam LAB só em MODE_LAB, prefixo/rotulo LAB, AUTO-OFF em <= 5 s.
6. HID mutex: só UM HID LIVE por vez em todo o firmware. BLE HOGP (BadBLE) e USB HID
   compartilham o mesmo mutex — nunca os dois LIVE juntos. Banner "HID LIVE" na UI.
7. Auditoria: toda ação RED e toda operação GATT gravam em /logs/ (JSONL).
8. GATT central: conecta/enumera/escreve SOMENTE em alvo allowlisted.
9. Sem exfil automático. Export só por comando do operador.

## Coexistência (obrigatória)
- Wi-Fi (2.4G) e BLE dividem UM rádio. Habilite coexistência de SW
  (CONFIG_ESP_COEX_SW_COEXIST_ENABLE). Ajuste scan window/interval para não
  matar o throughput Wi-Fi nem perder advertising.
- Térmico: consulte o HAL de temperatura. Acima do limite, PARE TX (adv, HID,
  spam, flood) e reduza duty de scan. Nunca ignore o throttle.
- HID mutex (regra 6) — checar antes de qualquer HOGP LIVE.
- NimBLE host roda em task própria; operações GATT NUNCA bloqueiam a task de UI/LVGL.
- Um só set de advertising LAB HID ativo por vez.

## Kconfig esperado (sdkconfig.defaults)
CONFIG_BT_ENABLED=y
CONFIG_BT_NIMBLE_ENABLED=y
CONFIG_BT_CONTROLLER_ENABLED=y
CONFIG_BT_NIMBLE_EXT_ADV=y
CONFIG_BT_NIMBLE_ROLE_CENTRAL=y
CONFIG_BT_NIMBLE_ROLE_PERIPHERAL=y
CONFIG_BT_NIMBLE_ROLE_OBSERVER=y
CONFIG_BT_NIMBLE_ROLE_BROADCASTER=y
CONFIG_BT_NIMBLE_MAX_CONNECTIONS=3
CONFIG_BT_NIMBLE_MAX_EXT_ADV_INSTANCES=4
CONFIG_BT_LE_50_FEATURE_SUPPORT=y
CONFIG_BT_LE_EXT_ADV=y
CONFIG_BT_NIMBLE_HOST_TASK_STACK_SIZE=5120
CONFIG_ESP_COEX_SW_COEXIST_ENABLE=y
# Sentinel (custom):
CONFIG_SENTINEL_BLE_MAX_DEVICES=128
CONFIG_SENTINEL_BLE_CODED_PHY=y
CONFIG_SENTINEL_BLE_MESH_DETECT=y          # só DETECTA; stack Mesh OFF por default
CONFIG_SENTINEL_BLE_LAB_SPAM_MAX_MS=5000
CONFIG_SENTINEL_ARM_GPIO=9                  # ajuste ao hardware; fail-closed se inválido

## Structs de referência (adapte, não copie cego)
typedef enum { VND_UNKNOWN, VND_APPLE_CONTINUITY, VND_APPLE_FINDMY, VND_AIRPODS,
  VND_MS_SWIFTPAIR, VND_GOOGLE_FASTPAIR, VND_EDDYSTONE, VND_IBEACON, VND_SAMSUNG,
  VND_TILE, VND_XIAOMI, VND_ESPRESSIF_BLUFI, VND_IMPROV_WIFI, VND_FLIPPER,
  VND_HID_HOGP, VND_MESH, VND_NUS } ble_vendor_t;

typedef struct {
  uint8_t  addr[6];
  uint8_t  addr_type;      // public / random / RPA
  int8_t   rssi;
  uint8_t  primary_phy;    // 1M / 2M / Coded
  uint8_t  sec_phy;
  bool     connectable, scannable, ext_adv, legacy;
  char     name[32];
  ble_vendor_t vendor;
  uint16_t company_id;     // manufacturer-specific (LE nos bytes do ar)
  uint16_t svc_uuid16[8];  // service UUIDs 16-bit vistos
  uint8_t  n_svc;
  uint8_t  cont_type;      // Apple Continuity message type, se aplicável
  uint32_t first_seen, last_seen, seen_count;
  bool     is_random_static, is_rpa;
} ble_device_t;

typedef struct { uint8_t addr[6]; int8_t rssi_min, rssi_max; uint32_t
  first_seen, last_seen; uint16_t count; bool moving_with_you; } stalker_track_t;

typedef struct { char addr[18]; uint16_t handle; char uuid[37];
  uint8_t props; bool open_write; char pairing[24]; } gatt_char_t;

## Tabela Apple Continuity (manufacturer data, company 0x004C; 1º byte = type)
0x02 iBeacon              0x05 AirDrop            0x07 Proximity Pairing (AirPods)
0x08 Hey Siri             0x09 AirPlay Target     0x0A AirPlay Source
0x0B Magic Switch (Watch) 0x0C Handoff            0x0D Tethering Target
0x0E Tethering Source     0x0F Nearby Action      0x10 Nearby Info
0x12 Find My (offline finding)
> Valide o length de cada type contra captura real; parser tem de ser à prova de
> truncar (nunca leia além do AD length).

## Tabela de vendors (company ID em manufacturer-specific data)
0x004C Apple     0x0006 Microsoft (Swift Pair)   0x0075 Samsung
0x00E0 Google    0x0157 Tile                       0x02E5 Espressif
0x038F Xiaomi    0x0059 Nordic (comum em Flipper/DIY)

## Tabela de service UUIDs (16-bit, salvo NUS)
0x1812 HID/HOGP        0x180A Device Information   0x1827 Mesh Provisioning
0x1828 Mesh Proxy      0xFE2C Google Fast Pair     0xFEAA Eddystone
0xFEED Tile            0xFE95 Xiaomi Mi            0x4677 Improv Wi-Fi
0xFFFF BluFi (Espressif, provisionamento Wi-Fi sobre GATT)
NUS (Nordic UART): 6E400001-B5A3-F393-E0A9-E50E24DCCA9E (128-bit)
> Flipper Zero: detecte por prefixo de nome "Flipper" + assinatura de adv,
> não confie só no company ID.

## BLUE (defesa / inventário) — sem TX ofensivo
- Scan legacy + extended; reporte PHY (1M/2M/Coded).
- Parser AD à prova de truncar; chips de vendor: Apple Continuity/AirPods/Find My,
  Swift Pair, Fast Pair, Samsung, Tile, Flipper, Espressif BluFi, Improv Wi-Fi,
  Xiaomi, Eddystone/iBeacon, Mesh, HID 0x1812.
- Baseline (conhecidos), detector de spam (contadores por tipo), stalker
  heurístico (tag + IMU: RSSI estável enquanto você se move), foxhunt (RSSI + beep).
- GATT audit SÓ allowlist; caça a characteristic com write aberto; log de método
  de pairing. Export JSONL/CSV.

## RED (ARM + allowlist / LAB 5 s) — gated
- Peripheral "SENTINEL-LAB".
- BadBLE HOGP com os MESMOS scripts Ducky do USB (US + ABNT2) — via HID mutex.
- Clone de advertising de device listado (na allowlist).
- Write hex no handle escolhido pelo operador (alvo allowlisted).
- Connect-flood 5 s. Spam Continuity/Swift/FastPair só em MODE_LAB, auto-off <=5 s.
- iBeacon/Eddystone LAB. Multi-adv BLE5. RPA rotate.

## P3 ou UNSUPPORTED (capability-probe; nunca crashe)
- Periodic sync; scan Coded (longo alcance); BluFi/Improv ATIVO; Mesh (OFF por
  default — pesado); HCI dump SE existir. Se não houver HCI, grave JSONL —
  NÃO invente pcap LINKTYPE 251 (BT LE) que o hardware não produz.

## PROIBIDO
Bluetooth Classic; sniffer estilo nRF (follow de conexão no ar); crack LE Secure
Connections; MITM com um só rádio; LE Audio; AoA/AoD; Channel Sounding; chave de
carro; dispositivo médico; spam infinito (tudo tem teto/auto-off); abuso de
Zigbee/PHY 802.15.4.

## Modelo de fases
A: scan+parser+vendor chips+UI (sem HID, sem spam TX)
B: BLUE IDS (contadores de spam, alerta HID UUID, stalker Find My/Tile, Flipper,
   baseline, export, foxhunt) + testes unitários do parser
C: GATT central allowlisted (connect/enumerate/open-write warn/pairing log)
D: RED gated (SENTINEL-LAB peripheral, HOGP BadBLE Ducky, spam LAB 5 s, iBeacon/
   Eddystone LAB, banner HID LIVE; desarmado => sem HID)
E: extras ou UNSUPPORTED (Coded scan, periodic sync, BluFi/Improv detect, mesh
   detect com stack OFF, matriz de capacidades)

## Sinal de conclusão
Ao fim de cada fase, escreva BLE_PHASE_x_STATUS.md com: OK/FAIL por requisito,
log de build (sem erro/warning), teste (se em hardware), e o próximo passo.
Não avance sozinho.

END_BLE_PROMPT
```

---

## Estrutura de arquivos sugerida (dentro de `sentinels3/main/`)

```
ble/
  ble_stack.c/.h        # init NimBLE, coex, tasks, sync callback
  ble_scan.c/.h         # legacy+ext scan, PHY, callback -> inventário
  ble_ad_parser.c/.h    # parser AD à prova de truncar (+ testes)
  ble_vendor.c/.h       # Continuity/Swift/FastPair/Tile/Flipper/BluFi/Improv...
  ble_inventory.c/.h    # tabela ble_device_t, dedup por addr/IRK
  ble_ids.c/.h          # spam counters, HID alert, stalker, flipper, foxhunt
  ble_baseline.c/.h
  ble_gatt_audit.c/.h   # central allowlisted, enumerate, open-write, pairing log
  ble_export.c/.h       # JSONL/CSV
  ble_red_peripheral.c/.h  # SENTINEL-LAB, HOGP BadBLE, clone, spam LAB, ibeacon
  ble_report.c/.h
include:
  ble_types.h           # enums, structs, UUIDs, company IDs, Continuity types
```

## Regras de coexistência (resumo operacional)
- Ative `esp_coex` SW coexist antes de subir Wi-Fi + BLE juntos.
- Scan contínuo BLUE: use janela/intervalo moderados (ex.: win 30 ms / int 60 ms)
  para dar ar ao Wi-Fi; em foxhunt pode subir o duty temporariamente.
- Antes de qualquer TX (adv/HID/spam/flood): checar `hal_thermal` e o HID mutex.
- HOGP LIVE trava o USB HID (e vice-versa) via o mesmo mutex global.
