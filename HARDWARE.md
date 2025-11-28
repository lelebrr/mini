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
