# Build PlatformIO — Mini Lele v2.1

O firmware **Mini Lele** (pasta `src/`) é **Arduino via PlatformIO**.

## Ambientes (`platformio.ini`)

| Env | Uso |
|-----|-----|
| `waveshare-esp32-s3-amoled` | Firmware completo |
| `waveshare-esp32-s3-amoled-smoke` | Smoke test LVGL mínimo |
| `probe` | Sonda de diagnóstico (blink standalone) |

## Comandos

```
pio run -e waveshare-esp32-s3-amoled                 # build
pio run -e waveshare-esp32-s3-amoled -t upload       # flash
pio device monitor -b 115200                         # monitor
pio run -t clean                                     # limpar
```

## Configurações de placa (já no ini)

- Plataforma: pioarduino platform-espressif32 (core Arduino 3.x)
- Board: `esp32-s3-devkitc-1`, framework `arduino`
- Flash 16 MB QIO, PSRAM OPI (`memory_type = qio_opi`)
- Partições: `partitions.csv` (OTA A/B 7 MB + spiffs + coredump)
- `-DBOARD_HAS_PSRAM`, `-DARDUINO_USB_CDC_ON_BOOT=1`, `-DARDUINO_USB_MODE=1`

## Flags úteis do WPS RED

```
-DWPS_ARM_GPIO=17         # pino do jumper de ARM (padrão 17; GPIO9 é I2S neste build)
-DWPS_THERMAL_MAX_C=80    # limite térmico de parada
-DWPS_ENABLE_VENDOR_PIN=1 # habilita gerador vendor-PIN 1-shot (padrão off)
```

---

> A reescrita **SENTINEL-S3** (`sentinels3/`) usa **ESP-IDF 5.5** (`idf.py`),
> não PlatformIO. Está em Fase 0.
