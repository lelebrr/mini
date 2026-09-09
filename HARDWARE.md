# Hardware — Waveshare ESP32-S3-Touch-AMOLED-1.8

Mesma placa para os dois firmwares (Mini Lele/Arduino e SENTINEL-S3/IDF).
Resumo de pinagem.
Detalhes: [FULL_HARDWARE.md](FULL_HARDWARE.md), [docs/PINOUT_AND_REV.md](docs/PINOUT_AND_REV.md).

| Componente | Pinos |
|-----------|-------|
| Display QSPI V1 (SH8601) | CS=12, SCK=11, D0/D1/D2/D3=4/5/6/7, RST=TCA9554 P0 |
| Display QSPI V2 (CO5300) | Mesmos pinos (autodetect) |
| Touch V1 (FT3168) | I2C 0x38, SDA=15, SCL=14, INT=21, RST=TCA9554 P1 |
| Touch V2 (CST820) | I2C 0x15, mesmos pinos (autodetect) |
| PMU AXP2101 | I2C 0x34 (ALDO1-4 + BLDO1 OBRIGATORIO) |
| Audio ES8311 | I2S: MCLK=16, BCLK=9, WS=45, DO=8, DI=10, PA=46 |
| IMU QMI8658 | I2C 0x6B |
| RTC PCF85063 | I2C 0x51 |
| SD (SDMMC 1-bit) | CLK=2, CMD=1, D0=3, CS=TCA9554 EXIO7 |
| Expansor TCA9554 | I2C 0x20 (P0=LCD_RST, P1=TP_RST, P6=audio, P7=SD_CS) |
| LED RGB | GPIO38 (Neopixel WS2812 via RMT) |
| ARM físico (WPS RED) | GPIO17 padrão no build Arduino (GPIO9 é I2S_BCLK); ajustável via `-DWPS_ARM_GPIO` |
| BOOT | GPIO0 |
