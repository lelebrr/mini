# Mini Lele v2.1 + SENTINEL-S3

Ferramenta de análise e segurança Wi-Fi/BLE para a placa
**Waveshare ESP32-S3-Touch-AMOLED-1.8** (ESP32-S3R8, 16 MB flash, 8 MB PSRAM OPI).

Este repositório contém **dois firmwares** para a mesma placa:

| Firmware | Pasta | Stack | Estado |
|----------|-------|-------|--------|
| **Mini Lele v2.1** | `src/` + `include/` | **Arduino** (pioarduino core 3.x) + LVGL 9 + Arduino_GFX + AsyncWebServer | **Atual — é o que compila e roda** (`pio run`) |
| **SENTINEL-S3** | `sentinels3/` | **ESP-IDF 5.5** puro + LVGL 9 + TinyUSB | Reescrita do zero, **em andamento (Fase 0**: boot/HAL; sem rádio ainda) |

> A maior parte da documentação histórica descreve a *visão* SENTINEL-S3 (o
> contrato BLUE/RED trancado por ARM+PIN+allowlist). Esse contrato é a meta da
> reescrita IDF. O firmware que hoje roda é o **Mini Lele**, e este README
> descreve o que ele realmente faz. Onde os dois divergem, está sinalizado.

---

## Mini Lele v2.1 — o que existe hoje

Firmware cooperativo (`setup()/loop()` Arduino), UI LVGL em tiles, com:

- **UI / Pet**: tiles HOME / WiFi / BLE / Ataques / Captura / Config; pet virtual
  com XP/nível/humor (gamification), rosto animado, IMU (shake), RTC, áudio
  (ES8311), gestão de energia (AXP2101, dim/sleep/deep-sleep).
- **BLUE (padrão)**:
  - Wi-Fi: sniffer passivo em modo promíscuo + captura **PCAP** (linktype 105),
    lista de dispositivos, contagem de EAPOL observada (não crackeia).
  - BLE: scan GAP (Bluedroid), lista de dispositivos.
  - **ESP-NOW inventory (novo)**: observação passiva de peers ESP-NOW (2.4 GHz).
  - **BLE inventory + IDS (novo)**: parser de AD à prova de truncar, classificação
    de vendor (Apple/AirPods/FindMy, Swift Pair, Fast Pair, Tile, Flipper, Eddystone,
    iBeacon, Xiaomi, Improv, BluFi, HID, Mesh, NUS), tipo de endereço; IDS: spam por
    tipo, alerta HID, stalker (Find My/Tile + IMU), Flipper, baseline, foxhunt;
    GATT audit read-only só-allowlist (enumerar + marcar write exposta).
  - **WPS (novo)**: parser do IE WPS, inventário, chip de higiene
    **WPS-ON/WPS-LOCKED**, baseline com alerta, relatório `wps_inventory.md`.
- **Ofensivo (Ataques)**:
  - Evil Portal: SoftAP + captive portal (HTML do cartão) com captura de
    credenciais.
  - Deauth (toggle; depende de TX 802.11 do driver).
  - OnlineCrack: upload de handshakes para serviço externo de quebra.
  - **WPS RED (novo)**: enrollee PBC/PIN **de laboratório**, com **ARM +
    allowlist + auditoria** e *soft-target* (ver abaixo).
- **WebUI**: servidor Async + DNS de portal cativo (`web_assets.cpp`).

### Postura de segurança (honesta) do build Arduino
No Mini Lele, **apenas o caminho WPS RED** aplica o contrato ARM + allowlist +
auditoria (ver [docs/WPS.md](docs/WPS.md)). Os demais recursos ofensivos
(Evil Portal, deauth, OnlineCrack) **não** passam por ARM/PIN/allowlist neste
firmware — essa é uma diferença conhecida em relação à visão SENTINEL-S3, onde
*toda* operação RED é trancada. Use com responsabilidade e só em redes próprias
ou autorizadas.

---

## Build & flash (Mini Lele)

```
pio run -e waveshare-esp32-s3-amoled            # compila
pio run -e waveshare-esp32-s3-amoled -t upload  # grava (esptool via USB CDC)
pio device monitor -b 115200                    # monitor serial
```

Detalhes em [INSTALACAO.md](INSTALACAO.md) e [README_PlatformIO.md](README_PlatformIO.md).

## Estrutura do cartão SD

O firmware monta o cartão e abre arquivos **relativos à raiz do cartão**
(o `SD_MMC` prefixa o mountpoint automaticamente). Layout usado:

```
/config.json              config do sistema
/pwn_pet_save.json        estado do pet
/game_stats.bin           gamification
/devices.json             último inventário de dispositivos
/portal_creds.json        credenciais capturadas pelo Evil Portal
/capturas/                PCAPs
/allowlist/bssid.txt      BSSIDs permitidos (RED WPS)
/allowlist/ssid.txt       SSIDs permitidos
/allowlist/wps_pins.txt   PINs de laboratório WPS (máx 5)
/wps/baseline.jsonl       baseline WPS por BSSID
/reports/wps_inventory.md relatório de inventário WPS
/logs/wps_audit.jsonl     auditoria de toda tentativa RED WPS
/portals/lab_basic/       HTML do captive portal
/scripts/hid/             scripts Ducky (uso futuro/HID)
```

Esqueleto pronto em [sd_skel/](sd_skel/).

## Documentos

| Documento | Descrição |
|-----------|-----------|
| [INSTALACAO.md](INSTALACAO.md) | Build PlatformIO, flash, cartão SD |
| [README_PlatformIO.md](README_PlatformIO.md) | Ambientes PlatformIO |
| [MANUAL.md](MANUAL.md) | Operação (UI, BLUE, ofensivo, WPS) |
| [docs/WPS.md](docs/WPS.md) | WPS: inventário BLUE + enrollee RED de laboratório |
| [docs/BLE.md](docs/BLE.md) | BLE: inventário BLUE (parser AD + vendor) + IDS + GATT audit |
| [docs/ESPNOW.md](docs/ESPNOW.md) | ESP-NOW: inventário BLUE (passivo) |
| [HARDWARE.md](HARDWARE.md) / [FULL_HARDWARE.md](FULL_HARDWARE.md) | Pinagem |
| [DISPLAY_GUIDE.md](DISPLAY_GUIDE.md) | Display e touch (inclui fix do FT3168) |
| [docs/ARQUITETURA.md](docs/ARQUITETURA.md) | Módulos, loop, boot, persistência |
| [docs/LIMITATIONS.md](docs/LIMITATIONS.md) | Limitações técnicas |
| [docs/THREAT_MODEL.md](docs/THREAT_MODEL.md) | O que detecta / faz / não faz |
| [docs/DEVICE_TEST.md](docs/DEVICE_TEST.md) | Checklist de teste |
| [docs/PINOUT_AND_REV.md](docs/PINOUT_AND_REV.md) | Pinagem e revisões da placa |
| [CONTRIBUTING.md](CONTRIBUTING.md) | Guia de contribuição (ambos os firmwares) |

### Reescrita SENTINEL-S3 (ESP-IDF, `sentinels3/`)
Projeto paralelo, ainda em **Fase 0** (bring-up de hardware; sem Wi-Fi/BLE).
A visão completa (contrato de segurança, fases, módulos) está em
[SENTINEL-S3-MASTER-PROMPT.md](SENTINEL-S3-MASTER-PROMPT.md) e
[SENTINEL-S3-FASES.txt](SENTINEL-S3-FASES.txt).

**Subsistema BLE** (NimBLE; sem Bluetooth Classic — BLE 5.0 completo): prompt
mestre em [SENTINEL-S3-BLE-MASTER-PROMPT.md](SENTINEL-S3-BLE-MASTER-PROMPT.md),
fases A–E em [SENTINEL-S3-BLE-FASES.txt](SENTINEL-S3-BLE-FASES.txt).

**EXTRAS** (ataques nativos do S3 além do catálogo — LAN pós-associação,
provisionamento IoT, DoS 2.4 GHz capped, físico/sensor; tudo lab/allowlist):
[SENTINEL-S3-EXTRAS-MASTER-PROMPT.md](SENTINEL-S3-EXTRAS-MASTER-PROMPT.md) e
[SENTINEL-S3-EXTRAS-FASES.txt](SENTINEL-S3-EXTRAS-FASES.txt).
