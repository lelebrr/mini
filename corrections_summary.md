# Resumo de Correcoes - SENTINEL-S3

O SENTINEL-S3 e codigo novo (do zero). Nao ha correcoes incrementais.

## Drivers reescritos

| Driver | Mini Lele | SENTINEL-S3 |
|--------|-----------|-------------|
| GPIO | pinMode/digitalWrite | gpio_set_direction/gpio_set_level |
| Delay | delay(ms) | vTaskDelay(pdMS_TO_TICKS) |
| I2C | Wire.begin/endTransmission | i2c_master_write/read |
| Display | Arduino_GFX (SH8601) | BSP panel API |
| Touch | TouchLib | BSP touch API |
| SD | SD_MMC.begin | sdmmc_card_init + FATFS VFS |
| PMU | XPowersLib | I2C direct registers |
| Audio | AudioHandler I2S | esp_codec_dev |
| BLE | NimBLE | esp_ble_gap |
| USB | Serial (CDC) | TinyUSB (CDC/HID/MSC/NCM) |
| WiFi | WiFi.mode | esp_wifi_set_mode |

## Pinagem mantida

Display QSPI: CS=12, SCK=11, D0/D1/D2/D3=4/5/6/7, RST=TCA9554 P0
Touch I2C: SDA=15, SCL=14, INT=21, RST=TCA9554 P1
SDMMC: CLK=2, CMD=1, D0=3, CS=EXIO7
Audio I2S: MCLK=16, BCLK=9, WS=45, DO=8, DI=10, PA=46
LED: GPIO38 Neopixel

## Correcoes recentes (build Arduino "Mini Lele")

| Item | Correcao |
|------|----------|
| Touch FT3168 | `read()`/`isTouched()` liam o registrador errado (`TD_STATUS & 0x80`, sempre 0 -> toque nunca detectado). Corrigido para o mapa FocalTech `TD_STATUS,XH,XL,YH,YL` a partir de 0x02. |
| WPS | Novo modulo BLUE (inventario/higiene/baseline/relatorio) + RED (enrollee PBC/PIN de laboratorio com ARM+allowlist+auditoria, *soft-target*). Ver docs/WPS.md. |
