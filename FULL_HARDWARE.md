# Documentação Completa de Hardware – Mini Lele (Waveshare ESP32‑S3‑Touch‑AMOLED‑1.8)

Este documento detalha as especificações técnicas, pinagem e componentes do dispositivo **Waveshare ESP32‑S3‑Touch‑AMOLED‑1.8**, utilizado como base para o projeto **Mini Lele**.

Ele é complementar ao resumo de pinos em `HARDWARE.md` e ao guia de display/touch em `DISPLAY_GUIDE.md`.

---

## 1. Visão geral da placa

- **Fabricante:** Waveshare  
- **Modelo:** ESP32‑S3‑Touch‑AMOLED‑1.8  
- **MCU:** ESP32‑S3R8 (Dual‑Core Xtensa LX7 @ 240 MHz)  
- **Memória:**
  - 16 MB Flash externa
  - 8 MB PSRAM (Octal)
  - 512 KB SRAM interna
- **Conectividade:**
  - Wi‑Fi 2.4 GHz (802.11 b/g/n)
  - Bluetooth 5.0 LE
- **Alimentação:**
  - USB‑C (5 V)
  - Bateria Li‑Po 3,7 V (conector MX1.25)
- **Principais periféricos:**
  - Tela AMOLED 1.8" com touch capacitivo
  - Codec de áudio + microfone + amplificador
  - IMU 6 eixos
  - PMIC AXP2101
  - Slot microSD

---

## 2. Display AMOLED

- **Painel:** 1.8" AMOLED
- **Resolução:** 368 x 448 pixels
- **Controlador:** SH8601
- **Interface:** QSPI (Quad SPI)
- **Cores:** até 16,7 milhões (usando RGB565/RGB888; o Mini Lele usa RGB565)

### 2.1 Pinagem do display (QSPI)

| Sinal   | GPIO | Função                                               |
|--------|------|------------------------------------------------------|
| LCD_CS | 12   | Chip Select (QSPI CS)                                |
| LCD_SCK| 11   | Clock (QSPI SCK)                                     |
| LCD_D0 | 4    | Linha de dados 0 (QSPI D0)                           |
| LCD_D1 | 5    | Linha de dados 1 (QSPI D1)                           |
| LCD_D2 | 6    | Linha de dados 2 (QSPI D2)                           |
| LCD_D3 | 7    | Linha de dados 3 (QSPI D3)                           |
| LCD_RST| Expansor (P0) | Reset do display (TCA9554 pino 0, via I²C) |

> No firmware do Mini Lele, o reset não é ligado diretamente em um GPIO do ESP32‑S3, e sim controlado pelo expansor de IO TCA9554.  
> Detalhes de inicialização: ver `DISPLAY_GUIDE.md`.

---

## 3. Touchscreen capacitivo

- **Controlador principal:** FT3168 (FocalTech)  
- **Possível variante:** GT1151 (Goodix), dependendo do lote da placa  
- **Interface:** I²C  
- **Endereços I²C típicos:**
  - FT3168: `0x38`
  - GT1151: `0x14` (em alguns módulos, `0x5D`)

### 3.1 Pinagem do touch

| Sinal   | GPIO | Notas                                                             |
|--------|------|-------------------------------------------------------------------|
| I2C_SDA| 15   | SDA compartilhado (PMU, áudio, IMU, RTC, expansor, etc.)          |
| I2C_SCL| 14   | SCL compartilhado                                                 |
| TP_INT | 21   | Interrupção do touch (active low)                                 |
| TP_RST | Expansor (P1) | Reset do touch (TCA9554 pino 1, controlado via I²C)      |

> Ambos os controladores usam o mesmo barramento I²C; a detecção é feita por tentativa de leitura dos endereços esperados.

---

## 4. Gerenciamento de energia (PMU AXP2101)

- **Chip:** AXP2101  
- **Interface:** I²C (`0x34`)  
- **Funções principais:**
  - Carregamento da bateria (charger integrado)
  - Reguladores de tensão (ALDO, BLDO, DCDC)
  - Medição de tensão/corrente de bateria e entrada
  - Controle do botão Power (PEK)
  - Monitoramento de temperatura em alguns cenários

O Mini Lele utiliza o AXP2101 para:

- Ligar e desligar trilhas específicas para display, touch e áudio
- Medir nível de bateria e estimar autonomia
- Implementar comportamentos de economia de energia (modo sono, etc.)

---

## 5. Áudio

- **Codec:** ES8311
- **Interface de controle:** I²C (`0x18`)
- **Interface de dados:** I²S
- **Microfone:** Digital (ligado ao ES8311 / I²S)
- **Saída:** Amplificador e alto‑falante onboard

### 5.1 Pinagem de áudio (I²S)

| Sinal | GPIO | Função                       |
|-------|------|-----------------------------|
| MCLK  | 16   | Master Clock (clock de áudio) |
| BCLK  | 9    | Bit Clock (I²S BCLK)        |
| WS    | 45   | Word Select / LRCK          |
| DOUT  | 10   | Saída de áudio (speaker)    |
| DIN   | 8    | Entrada de áudio (mic)      |
| PA_EN | 46   | Enable do amplificador      |

> Em alguns firmwares, o `PA_EN` pode ser ligado diretamente em um GPIO ou ser roteado via expansor. No Mini Lele, o valor padrão é GPIO 46.

---

## 6. Sensores e RTC

### 6.1 IMU – QMI8658

- **Chip:** QMI8658
- **Tipo:** IMU 6 eixos (acelerômetro + giroscópio)
- **Interface:** I²C (`0x6B`)
- **Uso no Mini Lele:**
  - Detecção de movimento
  - Animações e reações do Pet a “sacudidas”
  - Possíveis funções de economia de energia (acordar com movimento)

Pinagem compartilhada com o barramento principal I²C (SDA 15, SCL 14).

### 6.2 RTC – PCF85063

- **Chip:** PCF85063
- **Interface:** I²C (`0x51`)
- **Função:** relógio em tempo real (data/hora)
- **Uso típico:**
  - Registrar horário de capturas e logs
  - Eventos programados (se o firmware fizer uso)

---

## 7. Armazenamento – Cartão microSD

- **Slot:** microSD onboard
- **Interface:** SDMMC (geralmente em modo 1‑bit para simplificar pinagem)
- **Uso no Mini Lele:**
  - Armazenar:
    - Handshakes `.pcap`
    - Logs de sniffer e portal cativo
    - Arquivos de configuração
    - Assets de voz (WAV) e templates HTML

### 7.1 Pinagem do SD Card

Configuração típica usada no projeto:

| Sinal  | GPIO | Notas                                                          |
|--------|------|----------------------------------------------------------------|
| SD_CLK | 2    | Clock SDMMC (compartilhado com função de bootstrap/UART0)     |
| SD_CMD | 1    | Comando (pino UART0 RX/TX em modo alternativo)                |
| SD_D0  | 42   | Data 0 (modo 1‑bit)                                           |

> Como os pinos 1 e 2 são usados pelo SD, o Mini Lele utiliza **USB CDC** para logs de debug, evitando o uso da UART0 tradicional.

---

## 8. Expansor de IO – TCA9554

- **Chip:** TCA9554
- **Interface:** I²C (`0x20`)
- **Função:** disponibilizar IOs extras para controle de reset/enable de periféricos

Mapeamento típico de pinos:

- **P0:** Reset do display (LCD_RST)
- **P1:** Reset do touch (TP_RST)
- **P6:** Pode ser usado para controle de amplificador de áudio ou outro periférico (dependendo da revisão/firmware)

O expansor é essencial para:

- Garantir a sequência correta de power‑up do display/touch
- Isolar o ESP32‑S3 de limites de tensão/corrente de alguns periféricos

---

## 9. Observações de software

- O uso dos pinos `1` e `2` para SDMMC impede o uso da Serial0 padrão para debug quando o SD está ativo
- O projeto utiliza:
  - **USB CDC On Boot** (`ARDUINO_USB_CDC_ON_BOOT=1`) para debug via USB‑C (`Serial`)
  - Particionamento de 16 MB adequado para:
    - App grande (LVGL, WebUI, drivers, lógica de jogo)
    - Espaço para filesystem (SPIFFS/LittleFS, se usado)
- Recomendações:
  - Não usar `Serial0` nos pinos 1/2 para logs
  - Utilizar sempre `Serial` via USB nativa
  - Conferir `platformio.ini` para detalhes de flash, PSRAM e partições

---

## 10. Notas sobre variantes de hardware

O firmware do **Mini Lele v2.0** foi pensado para o kit Waveshare original descrito acima.

- Há referências no código (`pin_config.h`) a:
  - Possíveis variantes (“Edição Lendária”) com outros controladores de display (ex.: ST7701S)
  - Layouts alternativos de pinos
- Caso você esteja adaptando o Mini Lele para outro hardware:
  - Revise e ajuste `include/pin_config.h`
  - Atualize as seções relevantes deste documento
  - Deixe claro em comentários e documentação qual revisão de placa você está usando

---

## 11. Referências

- Wiki oficial da placa:  
  https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8
- `DISPLAY_GUIDE.md` – Detalhes de display/touch, drivers e debug
- `HARDWARE.md` – Resumo rápido de pinos para consulta diária
- `pin_config.h` – Fonte da verdade usada pelo firmware para mapeamento de pinos
