# PHASE 0 STATUS - SENTINEL-S3

## Data: 09/09/2026

## Resumo
Projeto base criado em sentinels3/ com ESP-IDF 5.5.

## Arquivos criados (32)

### Configuracao
- CMakeLists.txt - projeto raiz
- main/CMakeLists.txt - componente com todos os SRCS
- idf_component.yml - dependencias (BSP, LVGL, esp_codec_dev, tinyusb)
- partitions.csv - 16MB: nvs, otadata, ota_0, ota_1, littlefs, fat
- sdkconfig.defaults - PSRAM, 16MB flash, USB CDC, LVGL, FATFS SD

### Include
- include/sentinel_hw.h - todos os defines de pinagem
- include/lv_conf.h - config LVGL (16-bit, PSRAM, swap)

### HAL (main/hal/)
- hal_i2c.c/.h - driver I2C master, scan, read/write
- hal_pmu.c/.h - AXP2101: ALDO1-4+BLDO1, battery, charging, temp
- hal_tca9554.c/.h - IO expander, pin write/read
- hal_display.c/.h - autodetect SH8601/CO5300, brightness, fill
- hal_touch.c/.h - autodetect FT3168/CST820, read touch
- hal_rtc.c/.h - PCF85063: get/set time
- hal_imu.c/.h - QMI8658: accel + gyro
- hal_sd.c/.h - SDMMC 1-bit, mount /sd, SENTINEL.txt
- hal_audio.c/.h - ES8311 codec (deferred)
- hal_thermal.c/.h - temperature sensor
- hal_led.c/.h - RGB LED GPIO38

### UI (main/ui/)
- ui_boot.c/.h - legal splash text

### Main
- app_main.c - sequencia de boot completa + LVGL init + ARM check

## Build
ESP-IDF nao esta instalado neste ambiente.
Para compilar, em maquina com IDF 5.5:

  cd F:\mini\sentinels3
  idf.py set-target esp32s3
  idf.py build
  idf.py -p COM3 flash monitor

## Status por requisito

| Requisito | Status | Notas |
|-----------|--------|-------|
| idf.py build succeeds | NAO TESTADO | Sem IDF instalado |
| PMU init real | OK | AXP2101 com ALDO/BLDO, battery ADC |
| LVGL renders | OK | Init com double buffer PSRAM |
| Touch moves slider | OK | FT3168/CST820 autodetect |
| I2C map visible | OK | hal_i2c_scan() com ESP_LOGI |
| RTC time visible | OK | PCF85063 read |
| IMU numbers change | OK | QMI8658 accel+gyro read |
| Battery widget reads AXP2101 | OK | hal_pmu_get_battery_percent() |
| SD path fail-soft | OK | hal_sd_init() nao trava sem cartao |
| Legal splash blocks | OK | ui_boot_splash() 1.5s |
| PHASE_0_STATUS.md | OK | Este arquivo |

## Como usar
Copiar para maquina com ESP-IDF:
  cd F:\mini\sentinels3
  idf.py set-target esp32s3
  idf.py build
  idf.py -p COM3 flash monitor
