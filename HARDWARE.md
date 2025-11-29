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
| CS | 12 | Chip Select |
| SCK | 11 | Clock |
| D0 | 4 | Data 0 |
| D1 | 5 | Data 1 |
| D2 | 6 | Data 2 |
| D3 | 7 | Data 3 |
=======
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
| CS | 4 | Chip Select |
| SCK | 5 | Clock |
| D0 | 6 | Data 0 |
| D1 | 7 | Data 1 |
| D2 | 8 | Data 2 |
| D3 | 9 | Data 3 |
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
| RST | Ext. 0 | Controlado via TCA9554 (Pino 0) |

### Touch (I2C)
| Sinal | GPIO | Notas |
| :--- | :--- | :--- |
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
| RST | Ext. 1 | Controlado via TCA9554 (Pino 1) |

### Áudio (I2S)
| Sinal | GPIO | Notas |
| :--- | :--- | :--- |
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
| MCLK | 16 | Master Clock |
| BCLK | 9 | Bit Clock |
| LRCK | 45 | Word Select (WS) |
| DOUT | 10 | Speaker Out |
| DIN | 8 | Mic In |
| PA_EN | 46 | Enable Amplificador (TCA9554) |
=======
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
| MCLK | 2 | Master Clock |
| BCLK | 41 | Bit Clock |
| LRCK | 42 | Word Select (WS) |
| DOUT | 40 | Speaker Out |
| DIN | 1 | Mic In |
| PA_EN | Ext. 6 | Enable Amplificador (TCA9554) |
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified

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
