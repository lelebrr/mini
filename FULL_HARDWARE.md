# Documentação Completa de Hardware - Mini Lele (Waveshare ESP32-S3-Touch-AMOLED-1.8)

Este documento detalha as especificações técnicas, pinagem e componentes do dispositivo **Waveshare ESP32-S3-Touch-AMOLED-1.8**, utilizado como base para o projeto **Mini Lele**.

## 1. Visão Geral
*   **Fabricante:** Waveshare
*   **Modelo:** ESP32-S3-Touch-AMOLED-1.8
*   **MCU:** ESP32-S3R8 (Dual-Core Xtensa LX7, 240MHz)
*   **Memória:** 16MB Flash (Externo) + 8MB PSRAM (Octal) + 512KB SRAM (Interno)
*   **Conectividade:** WiFi 2.4GHz (802.11 b/g/n) + Bluetooth 5.0 LE
*   **Alimentação:** USB-C (5V) ou Bateria Li-Po (3.7V via conector MX1.25)

## 2. Display AMOLED
*   **Painel:** 1.8 polegadas
*   **Resolução:** 368 x 448 pixels
*   **Driver:** SH8601
*   **Interface:** QSPI (Quad SPI)
*   **Cores:** 16.7 Milhões (RGB565 / RGB888)

### Pinagem do Display (QSPI)
| Sinal | GPIO | Função |
| :--- | :--- | :--- |
| LCD_CS | 12 | Chip Select |
| LCD_SCK | 11 | Clock |
| LCD_D0 | 4 | Data 0 |
| LCD_D1 | 5 | Data 1 |
| LCD_D2 | 6 | Data 2 |
| LCD_D3 | 7 | Data 3 |
| LCD_RST | - | Reset (Controlado via Software/Power?) |

## 3. Touchscreen Capacitivo
*   **Controlador:** FT3168 (FocalTech)
*   **Interface:** I2C
*   **Endereço I2C:** `0x38`

### Pinagem do Touch
| Sinal | GPIO | Notas |
| :--- | :--- | :--- |
| I2C_SDA | 15 | Compartilhado com PMU, Audio, IMU |
| I2C_SCL | 14 | Compartilhado com PMU, Audio, IMU |
| TP_INT | 21 | Interrupção |
| TP_RST | - | Reset (Geralmente via AXP ou RC) |

## 4. Gerenciamento de Energia (PMU)
*   **Chip:** AXP2101
*   **Interface:** I2C (`0x34`)
*   **Funções:**
    *   Carregamento de Bateria
    *   Regulação de Voltagem (ALDO1, ALDO2, BLDO1, etc.)
    *   Monitoramento de Bateria (Voltagem/Corrente)
    *   Botão Power (PEK)

## 5. Áudio
*   **Codec:** ES8311
*   **Interface:** I2S (Dados) + I2C (Controle `0x18`)
*   **Microfone:** Microfone Digital (PDM ou I2S)
*   **Speaker:** Saída amplificada onboard

### Pinagem de Áudio
| Sinal | GPIO | Função |
| :--- | :--- | :--- |
| MCLK | 16 | Master Clock |
| BCLK | 9 | Bit Clock |
| WS | 45 | Word Select (LRCK) |
| DOUT | 10 | Data Out (Speaker) |
| DIN | 8 | Data In (Mic) |
| PA_EN | 46 | Enable Amplificador |

## 6. Sensores (IMU)
*   **Chip:** QMI8658
*   **Tipo:** 6-Eixos (Acelerômetro + Giroscópio)
*   **Interface:** I2C (`0x6B`)
*   **Interrupção:** GPIO 0 (Compartilhado com BOOT) ou dedicado? (Verificar esquemático específico, mas comum ser via I2C polling ou INT dedicado)

## 7. Armazenamento Externo
*   **Slot:** MicroSD (TF Card) onboard
*   **Interface:** SDMMC (1-bit ou 4-bit)

### Pinagem SD Card
| Sinal | GPIO | Notas |
| :--- | :--- | :--- |
| SD_CLK | 2 | Clock |
| SD_CMD | 1 | Command |
| SD_D0 | ? | Data 0 (Geralmente GPIO 40 ou similar se slot 1) |

## 8. Interfaces Adicionais
*   **Botão BOOT:** GPIO 0
*   **Botão PWR:** Conectado ao AXP2101
*   **USB:** USB-C (Serial/JTAG via GPIO 19/20 + USB D+/D- via GPIO 20/19)

## 9. Observação sobre "Edição Lendária"
O firmware atual do **Mini Lele v2.0** foi configurado para suportar uma variação de hardware customizada ("Edição Lendária") que utiliza uma pinagem alternativa (RGB 8-bit ST7701S e GT1151). Para utilizar este firmware no hardware Waveshare original descrito acima, é necessário alterar o arquivo `include/pin_config.h` para refletir os pinos QSPI (SH8601) listados na seção 2.

---
*Pesquisa realizada em bases de dados técnicas e documentação oficial da Waveshare.*
