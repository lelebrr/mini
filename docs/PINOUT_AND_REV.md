# Pinagem e Revisoes - SENTINEL-S3

## Revisoes da placa

A Waveshare ESP32-S3-Touch-AMOLED-1.8 tem duas revisoes:

| Revisao | Display (QSPI) | Touch (I2C) |
|--------|----------------|-------------|
| V1 (ate 05/2026) | SH8601 | FT3168 @ 0x38 |
| V2 (apos 05/2026) | CO5300 | CST820 @ 0x15 |

Autodetect: tentativa SH8601+FT3168; se falhar, CO5300+CST820.

## Ordem de inicializacao (CRITICO)

1. I2C init (GPIO 15/14)
2. PMU AXP2101 (0x34): ligar ALDO1-4 + BLDO1
3. TCA9554 (0x20): P0=LCD_RST, P1=TP_RST, P7=SD_CS
4. Display: RST via TCA9554 P0 LOW->HIGH; init Arduino_GFX (Mini Lele) ou BSP (IDF)
5. Touch: RST via TCA9554 P1 LOW->HIGH, init via I2C (FT3168 @0x38)
6. SD: EXIO7 HIGH, SD_MMC (Mini Lele) ou sdmmc_card_init (IDF)

## Mapa I2C

| Endereco | Componente |
|----------|-----------|
| 0x34 | AXP2101 (PMU) |
| 0x38 | FT3168 (touch V1) |
| 0x15 | CST820 (touch V2) |
| 0x51 | PCF85063 (RTC) |
| 0x6B | QMI8658 (IMU) |
| 0x20 | TCA9554 (expansor) |
| 0x18 | ES8311 (audio) |

## Pinagem rapida

Display QSPI: CS=12, SCK=11, D0=4, D1=5, D2=6, D3=7
Touch: SDA=15, SCL=14, INT=21
SD: CLK=2, CMD=1, D0=3, CS=EXIO7
Audio: MCLK=16, BCLK=9, WS=45, DO=8, DI=10, PA=46
LED: GPIO38 (Neopixel)
ARM: GPIO9 (contrato SENTINEL-S3/IDF)
BOOT: GPIO0

> Nota (build Arduino "Mini Lele"): GPIO9 e usado por I2S_BCLK, entao o gate
> ARM do WPS RED usa **GPIO17** por padrao (ajustavel via `-DWPS_ARM_GPIO=<pino>`
> em `platformio.ini`). Ver [WPS.md](../docs/WPS.md).
