# Transicao Mini Lele -> SENTINEL-S3

O SENTINEL-S3 e uma reescrita completa (do zero) do Mini Lele para ESP-IDF 5.5.

## Mudancas

| Mini Lele (Arduino) | SENTINEL-S3 (IDF) |
|--------------------|--------------------|
| Arduino framework | ESP-IDF 5.5 puro |
| loop() cooperativo | FreeRTOS multitarefa |
| Pet virtual + gamification | Removido |
| WebUI (AsyncWebServer) | LVGL + TinyUSB CDC |
| Arduino_GFX | BSP waveshare QSPI |
| TouchLib | BSP I2C touch |
| SD_MMC.begin() | sdmmc_card_init + FATFS |
| XPowersLib (AXP2101) | I2C driver IDF |
| I2S AudioHandler | esp_codec_dev |
| NimBLE | esp_ble_gap_*() |
| Serial CDC | TinyUSB (CDC+HID+MSC+NCM) |
| Sem seguranca | ARM fisico + PIN + allowlist |
| PlatformIO build | idf.py |

## Mantido

- Mesma placa Waveshare ESP32-S3-Touch-AMOLED-1.8
- Mesma pinagem (corrigida na v2.1)
- Display SH8601/CO5300, touch FT3168/CST820
- SD D0=GPIO3, CS=EXIO7 (correcao mantida)

## Adendos ao build Arduino atual

- **Touch FT3168 corrigido**: leitura passou a usar o mapa FocalTech
  (`TD_STATUS,XH,XL,YH,YL` @0x02); antes testava `& 0x80` e nunca detectava toque.
- **WPS adicionado** (BLUE inventario + RED enrollee de laboratorio, *soft-target*,
  ARM+allowlist+auditoria). Detalhes em [docs/WPS.md](docs/WPS.md).
