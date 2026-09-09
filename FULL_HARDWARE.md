# Hardware Completo — Waveshare ESP32-S3-Touch-AMOLED-1.8

Complementa [HARDWARE.md](HARDWARE.md).

## Display
- SH8601 (V1) ou CO5300 (V2) via QSPI (Mini Lele: Arduino_GFX `Arduino_SH8601`)
- Resolucao 368x448, RGB565
- Reset via TCA9554 P0
- BLDO1 do AXP2101 essencial (3.3V)

## Touch
- FT3168 (V1, 0x38) ou CST820 (V2, 0x15)
- INT GPIO21, RST TCA9554 P1

## PMU AXP2101
- I2C 0x34, gerencia carga e reguladores
- ALDO1 (1.8V logica), ALDO2 (2.8V analogico), ALDO3 (3.0V audio), BLDO1 (3.3V display)

## Audio ES8311
- I2C 0x18, I2S, PA GPIO46, TCA9554 P6

## IMU QMI8658 - I2C 0x6B
## RTC PCF85063 - I2C 0x51

## SD
- SDMMC 1-bit: CLK=2, CMD=1, D0=3, CS=EXIO7 (HIGH antes de init)
- Montado via SD_MMC (arquivos abertos relativos à raiz do cartão)

## TCA9554 - I2C 0x20
P0=LCD_RST, P1=TP_RST, P2=perifericos, P6=audio, P7=SD_CS

## LED RGB - GPIO38 Neopixel WS2812 via RMT

## Referencias
- [HARDWARE.md](HARDWARE.md)
- [docs/PINOUT_AND_REV.md](docs/PINOUT_AND_REV.md)
- [DISPLAY_GUIDE.md](DISPLAY_GUIDE.md)
