# Contribuição

Este repositório tem **dois firmwares** para a mesma placa. Siga a convenção do
que você for tocar.

## Mini Lele v2.1 (`src/`, `include/`) — atual
- **Arduino** (pioarduino core 3.x) + LVGL 9 + Arduino_GFX + AsyncWebServer
- Classes `PascalCase` com métodos `static`; comentários em pt-BR
- Build: `pio run -e waveshare-esp32-s3-amoled [-t upload]`
- Caminhos de SD relativos à raiz do cartão (o `SD_MMC` prefixa o mountpoint)
- Recursos ofensivos novos devem, de preferência, seguir o padrão do WPS RED
  (ARM + allowlist + auditoria); não inclua payloads ofensivos prontos nem
  clones de marca em VID/PID

## SENTINEL-S3 (`sentinels3/`) — reescrita IDF (Fase 0)
- **ESP-IDF 5.5** puro, C11 + FreeRTOS; `snake_case`, `ALL_CAPS`, structs `PascalCase`
- Use `gpio_set_direction()`, `vTaskDelay()`, `ESP_LOGx()` — **não** Arduino
- Toda feature RED passa por ARM + PIN + allowlist
- Build: `cd sentinels3 && idf.py build && idf.py -p PORTA flash monitor`

## Documentos
- [README.md](README.md) · [INSTALACAO.md](INSTALACAO.md) · [HARDWARE.md](HARDWARE.md)
- [docs/LIMITATIONS.md](docs/LIMITATIONS.md) · [SENTINEL-S3-FASES.txt](SENTINEL-S3-FASES.txt)
