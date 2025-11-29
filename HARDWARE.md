<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
# Documentação de Hardware - Mini Lele

O **Mini Lele** é projetado especificamente para o kit de desenvolvimento abaixo. O uso de outro hardware requer adaptação significativa dos drivers.

## Dispositivo Alvo
*   **Modelo:** Waveshare ESP32-S3-Touch-AMOLED-1.8
*   **MCU:** ESP32-S3R8 (Dual Core, 240MHz, 8MB PSRAM, 16MB Flash)
*   **Display:** 1.8" AMOLED (368x448)
*   **Touch:** Capacitivo (GT1151 ou FT3168)
*   **PMU:** AXP2101 (Gerenciamento de Energia)
*   **Áudio:** Codec ES8311 + Microfone Digital
*   **IMU:** QMI8658 (Acelerômetro/Giroscópio)
*   **Expansor IO:** TCA9554

## Mapeamento de Pinos (Pinout)

### Display (Interface QSPI)
| Sinal | GPIO | Notas |
| :--- | :--- | :--- |
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
| CS | 12 | Chip Select |
| SCK | 11 | Clock |
| D0 | 4 | Data 0 |
| D1 | 5 | Data 1 |
| D2 | 6 | Data 2 |
| D3 | 7 | Data 3 |
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
| CS | 4 | Chip Select |
| SCK | 5 | Clock |
| D0 | 6 | Data 0 |
| D1 | 7 | Data 1 |
| D2 | 8 | Data 2 |
| D3 | 9 | Data 3 |
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
| RST | Ext. 0 | Controlado via TCA9554 (Pino 0) |

### Touch (I2C)
| Sinal | GPIO | Notas |
| :--- | :--- | :--- |
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
| SDA | 15 | Barramento I2C compartilhado |
| SCL | 14 | Barramento I2C compartilhado |
| INT | 21 | Polling ou Interrupção (Configurável) |
=======
| SDA | 38 | Barramento I2C compartilhado |
| SCL | 39 | Barramento I2C compartilhado |
| INT | - | Polling ou Interrupção (Configurável) |
>>>>>>> origin/merge-ready-mini-lele-v2
=======
| SDA | 38 | Barramento I2C compartilhado |
| SCL | 39 | Barramento I2C compartilhado |
| INT | - | Polling ou Interrupção (Configurável) |
>>>>>>> origin/mini-lele-v2-complete-verified
=======
| SDA | 38 | Barramento I2C compartilhado |
| SCL | 39 | Barramento I2C compartilhado |
| INT | - | Polling ou Interrupção (Configurável) |
>>>>>>> origin/mini-lele-v2-final-verified
=======
| SDA | 38 | Barramento I2C compartilhado |
| SCL | 39 | Barramento I2C compartilhado |
| INT | - | Polling ou Interrupção (Configurável) |
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
| SDA | 15 | Barramento I2C compartilhado |
| SCL | 14 | Barramento I2C compartilhado |
| INT | 21 | Polling ou Interrupção (Configurável) |
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
| SDA | 38 | Barramento I2C compartilhado |
| SCL | 39 | Barramento I2C compartilhado |
| INT | - | Polling ou Interrupção (Configurável) |
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
| SDA | 38 | Barramento I2C compartilhado |
| SCL | 39 | Barramento I2C compartilhado |
| INT | - | Polling ou Interrupção (Configurável) |
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
| SDA | 15 | Barramento I2C compartilhado |
| SCL | 14 | Barramento I2C compartilhado |
| INT | 21 | Polling ou Interrupção (Configurável) |
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
| SDA | 38 | Barramento I2C compartilhado |
| SCL | 39 | Barramento I2C compartilhado |
| INT | - | Polling ou Interrupção (Configurável) |
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
| SDA | 38 | Barramento I2C compartilhado |
| SCL | 39 | Barramento I2C compartilhado |
| INT | - | Polling ou Interrupção (Configurável) |
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
| RST | Ext. 1 | Controlado via TCA9554 (Pino 1) |

### Áudio (I2S)
| Sinal | GPIO | Notas |
| :--- | :--- | :--- |
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
| MCLK | 16 | Master Clock |
| BCLK | 9 | Bit Clock |
| LRCK | 45 | Word Select (WS) |
| DOUT | 10 | Speaker Out |
| DIN | 8 | Mic In |
| PA_EN | 46 | Enable Amplificador (TCA9554) |
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
| MCLK | 2 | Master Clock |
| BCLK | 41 | Bit Clock |
| LRCK | 42 | Word Select (WS) |
| DOUT | 40 | Speaker Out |
| DIN | 1 | Mic In |
| PA_EN | Ext. 6 | Enable Amplificador (TCA9554) |
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
=======
>>>>>>> origin/mini-lele-v2-legendary-final-drivers
=======
>>>>>>> origin/mini-lele-v2-legendary-final-release
=======
>>>>>>> origin/mini-lele-v2-legendary-final-sync
=======
>>>>>>> origin/mini-lele-v2-legendary-fixed-hardware
=======
>>>>>>> origin/mini-lele-v2-legendary-missing-assets
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final

### Periféricos I2C (Endereços)
*   **AXP2101 (PMU):** `0x34`
*   **TCA9554 (Expansor):** `0x20`
*   **GT1151 (Touch):** `0x14`
*   **FT3168 (Touch):** `0x38` (Alternativo)
*   **ES8311 (Codec):** `0x18`
*   **QMI8658 (IMU):** `0x6B`

## Considerações de Energia
O sistema utiliza o AXP2101 para gerenciar as tensões. O driver `PwnPower` configura automaticamente:
*   **ALDO1:** AMOLED Logic
*   **ALDO2:** Touch Logic
*   **ALDO3:** Audio Logic
*   **ALDO4:** Microfone Bias
*   **BLDO1:** AMOLED Power
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/pwn-tamagotchi-br-release
# ⚙️ Hardware e Pinos

## Especificações Técnicas

*   **MCU:** ESP32-S3 (N16R8)
*   **Flash:** 16MB
*   **PSRAM:** 8MB
*   **Tela:** 1.8" AMOLED (368x448) - Controlador SH8601
*   **PMU:** AXP2101 (Gerenciamento de Energia I2C)
*   **Touch:** FT3168 (I2C)
*   **Áudio:** Codec ES8311 + Microfone Analógico (I2S)
*   **IMU:** QMI8658 (Acelerômetro/Giroscópio I2C)
*   **RTC:** PCF85063 (Relógio Real I2C)

## Mapa de Pinos (Pinout)

### Display (QSPI)
| Função | Pino |
| :--- | :--- |
| CS | 4 |
| SCLK | 5 |
| D0 | 6 |
| D1 | 7 |
| D2 | 8 |
| D3 | 9 |
| RST | *IO Expander Pin 0* |

### Áudio (I2S)
| Função | Pino |
| :--- | :--- |
| MCLK | 2 |
| BCLK | 41 |
| WS | 42 |
| DOUT | 40 (Speaker) |
| DIN | 1 (Mic) |

### I2C (Sensores/Touch/PMU)
*   **SDA:** 38
*   **SCL:** 39

### IO Expander (TCA9554)
Endereço 0x20. Controla Reset e Energia dos periféricos.
<<<<<<< HEAD
>>>>>>> origin/mini-lele-v2-rebrand
=======
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
