# Instalação — Mini Lele v2.1 (Arduino/PlatformIO)

Firmware atual: **Arduino** (pioarduino core 3.x) via **PlatformIO**.
Placa: **Waveshare ESP32-S3-Touch-AMOLED-1.8**.

> Para a reescrita ESP-IDF (`sentinels3/`, em Fase 0) veja o bloco no fim.

## Pré-requisitos

- [PlatformIO](https://platformio.org/) (CLI ou extensão VS Code)
- Cartão microSD FAT32
- Cabo USB-C
- Bateria Li-Po MX1.25 (opcional)

A plataforma e as libs são baixadas no primeiro build (requer internet uma vez;
depois ficam em cache). As libs principais (LVGL, Arduino_GFX, ArduinoJson,
XPowersLib, AsyncTCP/ESPAsyncWebServer) estão fixadas no `platformio.ini`.

## Build

```
pio run -e waveshare-esp32-s3-amoled
```

Ambientes disponíveis: `waveshare-esp32-s3-amoled` (firmware completo),
`waveshare-esp32-s3-amoled-smoke` (teste LVGL mínimo), `probe` (diagnóstico).

## Flash e monitor

```
pio run -e waveshare-esp32-s3-amoled -t upload   # esptool via USB CDC (COM/tty)
pio device monitor -b 115200
```

O upload usa `esptool` (CDC-ACM). Se a porta não aparecer, segure BOOT (GPIO0)
ao conectar para entrar no bootloader.

## Estrutura do cartão SD

Arquivos são abertos **relativos à raiz do cartão** (o `SD_MMC` prefixa o
mountpoint). Copie o esqueleto de [sd_skel/](sd_skel/) para o cartão:

```
/config.json
/allowlist/bssid.txt        # BSSIDs permitidos (RED WPS)
/allowlist/ssid.txt         # SSIDs permitidos
/allowlist/wps_pins.txt     # PINs de laboratório WPS (máx 5)
/wps/baseline.jsonl         # baseline WPS por BSSID
/reports/                   # relatórios (wps_inventory.md)
/logs/                      # auditoria (wps_audit.jsonl)
/portals/lab_basic/         # HTML do captive portal
/scripts/hid/               # scripts Ducky
/capturas/                  # PCAPs
```

## Documentação relacionada

- [README.md](README.md) — visão geral
- [MANUAL.md](MANUAL.md) — operação
- [HARDWARE.md](HARDWARE.md) / [FULL_HARDWARE.md](FULL_HARDWARE.md) — pinagem
- [docs/WPS.md](docs/WPS.md) — WPS BLUE/RED

---

## SENTINEL-S3 (ESP-IDF 5.5, `sentinels3/`) — reescrita em andamento

Ainda em Fase 0 (só boot/HAL). Quando aplicável:

```
cd sentinels3
idf.py set-target esp32s3
idf.py build
idf.py -p PORTA flash monitor
```
