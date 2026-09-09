# Display e Touch — Waveshare ESP32-S3-Touch-AMOLED-1.8

## Display

- SH8601 (V1) ou CO5300 (V2), 368x448, QSPI
- Ordem: PMU AXP2101 (BLDO1) -> TCA9554 P0 LOW->HIGH -> BSP init
- Mini Lele (Arduino): `Arduino_ESP32QSPI` + `Arduino_SH8601` (Arduino_GFX)
- SENTINEL-S3 (IDF): BSP waveshare/esp32_s3_touch_amoled_1_8

## Touch

- FT3168 (V1, 0x38) ou CST820 (V2, 0x15)
- INT GPIO21 active low, RST TCA9554 P1 LOW->HIGH
- Gestos: swipe, tap, tap duplo

## Debug
- Tela preta: verificar AXP2101 BLDO1 (3.3V)
- Touch nao responde:
  - Verificar I2C (FT3168 @0x38 / CST820 @0x15)
  - Driver FT3168 le o mapa FocalTech a partir de 0x02:
    `TD_STATUS, XH, XL, YH, YL`. Toque = `TD_STATUS & 0x0F > 0`;
    X=`((XH&0x0F)<<8)|XL`, Y=`((YH&0x0F)<<8)|YL`.
  - **Bug corrigido**: a versao antiga testava `TD_STATUS & 0x80` (sempre 0),
    entao o toque nunca era reportado (tela "morta") e as coords vinham erradas.
  - Se ainda nao responder, a placa pode ser rev2 (touch CST820 @0x15) — este
    build so tem driver FT3168.
- Artefatos: verificar QSPI clock

## Referencias
- [HARDWARE.md](HARDWARE.md)
- [FULL_HARDWARE.md](FULL_HARDWARE.md)
- [docs/PINOUT_AND_REV.md](docs/PINOUT_AND_REV.md)
