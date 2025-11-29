# Documentação de Hardware – Mini Lele

O **Mini Lele** é projetado especificamente para o kit **Waveshare ESP32‑S3‑Touch‑AMOLED‑1.8**.  
O uso em outros hardwares é possível, mas exige adaptações importantes em `pin_config.h` e nos drivers.

Este arquivo é um **resumo rápido** dos principais pinos e componentes.  
Para uma descrição completa, consulte `FULL_HARDWARE.md`.

---

## 1. Dispositivo alvo

- **Modelo:** Waveshare ESP32‑S3‑Touch‑AMOLED‑1.8  
- **MCU:** ESP32‑S3R8 (Dual Core, 240 MHz, 8 MB PSRAM, 16 MB Flash)  
- **Display:** 1.8" AMOLED (368x448, controlador SH8601, interface QSPI)  
- **Touch:** Capacitivo (FT3168 ou GT1151, interface I²C)  
- **PMU:** AXP2101 (gerenciamento completo de energia, I²C)  
- **Áudio:** Codec ES8311 + microfone e amplificador (I²S)  
- **IMU:** QMI8658 (acelerômetro/giroscópio, I²C)  
- **RTC:** PCF85063 (relógio de tempo real, I²C)  
- **Expansor IO:** TCA9554 (I²C)

---

## 2. Resumo de pinagem principal

> Os valores abaixo são o alvo principal do projeto.  
> Sempre confira `include/pin_config.h` e o esquemático da Waveshare em caso de dúvida.

### 2.1 Display (QSPI – SH8601)

| Sinal | GPIO / Origem    | Função                              |
|-------|------------------|-------------------------------------|
| CS    | 12               | Chip Select do display              |
| SCK   | 11               | Clock QSPI                          |
| D0    | 4                | Dados 0                             |
| D1    | 5                | Dados 1                             |
| D2    | 6                | Dados 2                             |
| D3    | 7                | Dados 3                             |
| RST   | TCA9554 (P0)     | Reset do display (via expansor I²C) |

> O reset do display não vai direto ao ESP32‑S3; ele é controlado através do expansor TCA9554.

### 2.2 Touch (I²C – FT3168 / GT1151)

| Sinal | GPIO / Origem    | Função                                        |
|-------|------------------|-----------------------------------------------|
| SDA   | 15               | SDA I²C compartilhado                         |
| SCL   | 14               | SCL I²C compartilhado                         |
| INT   | 21               | Interrupção (active low)                      |
| RST   | TCA9554 (P1)     | Reset do touch (via expansor I²C)            |

Endereços típicos:

- FT3168: `0x38`
- GT1151: `0x14` (ou `0x5D` em versões específicas)

### 2.3 Áudio (I²S – ES8311)

| Sinal | GPIO | Função                    |
|-------|------|---------------------------|
| MCLK  | 16   | Master Clock              |
| BCLK  | 9    | Bit Clock                 |
| LRCK  | 45   | Word Select / WS / LRCK   |
| DOUT  | 10   | Saída de áudio (speaker)  |
| DIN   | 8    | Entrada de áudio (mic)    |
| PA_EN | 46   | Enable do amplificador    |

Codec ES8311 via I²C:

- Endereço: `0x18`

### 2.4 Barramento I²C compartilhado

O barramento principal I²C é compartilhado pelos seguintes dispositivos:

- AXP2101 (PMU)
- TCA9554 (expansor IO)
- FT3168 / GT1151 (touch)
- QMI8658 (IMU)
- PCF85063 (RTC)
- ES8311 (controle)

Pinos utilizados:

- **SDA:** 15  
- **SCL:** 14  

Endereços I²C mais importantes:

- **AXP2101:** `0x34` (PMU)
- **TCA9554:** `0x20` (expansor IO)
- **FT3168:** `0x38` (touch)
- **GT1151:** `0x14` (touch – variante)
- **ES8311:** `0x18` (codec de áudio)
- **QMI8658:** `0x6B` (IMU)
- **PCF85063:** `0x51` (RTC)

### 2.5 Cartão microSD (SDMMC)

No Mini Lele, o cartão SD é usado para:

- Handshakes `.pcap`
- Logs (`macs_detectados.txt`, `credenciais_capturadas.txt`, etc.)
- Arquivos TTS/voz e templates de portal cativo

Configuração típica (modo 1‑bit):

| Sinal | GPIO | Função/Notas                                       |
|-------|------|----------------------------------------------------|
| SD_CLK| 2    | Clock SDMMC (compartilhado com bootstrap/UART0)    |
| SD_CMD| 1    | Comando (também pino da UART0 em modo alternativo) |
| SD_D0 | 42   | Dados (modo 1‑bit)                                 |

> Por usar os pinos 1 e 2, o debug é feito via **USB CDC**, e não pela UART0 tradicional.

---

## 3. Considerações de energia (AXP2101)

O Mini Lele depende do AXP2101 para ligar os periféricos na ordem correta.

Reguladores mais importantes:

- **ALDO1:** lógica do AMOLED e touch (1,8 V)
- **ALDO2:** analógico do touch (≈ 2,8 V)
- **ALDO3:** áudio e periféricos (≈ 3,0 V)
- **ALDO4:** bias de microfone, em algumas configurações
- **BLDO1:** alimentação principal do display (3,3 V – **essencial**)

Se essas saídas não forem habilitadas:

- O display pode ficar permanentemente preto
- O touch pode não responder
- O áudio pode permanecer mudo

---

## 4. Expansor de IO – TCA9554

Endereço I²C: `0x20`.

Exemplos de uso no Mini Lele:

- **P0:** Reset do display (LCD_RST)
- **P1:** Reset do touch (TP_RST)
- **P6:** Pode ser usado para PA_EN (amplificador), dependendo da revisão

Através do TCA9554, o firmware:

- Garante a sequência de inicialização correta (display/touch)
- Consegue ligar/desligar alguns periféricos sem consumir GPIOs valiosos do ESP32‑S3

---

## 5. Especificações rápidas (resumo)

- **MCU:** ESP32‑S3 (N16R8, com PSRAM)
- **Flash:** 16 MB
- **PSRAM:** 8 MB
- **Tela:** 1.8" AMOLED 368x448, QSPI, SH8601
- **Touch:** FT3168/GT1151 (I²C)
- **PMU:** AXP2101
- **Áudio:** ES8311 + mic + speaker
- **IMU:** QMI8658
- **RTC:** PCF85063
- **SD:** microSD, interface SDMMC (modo 1‑bit)

---

## 6. Recomendações para quem for modificar hardware

Se você pretende adaptar o Mini Lele para outro hardware ou revisão de placa:

1. Comece ajustando `include/pin_config.h`
2. Atualize:
   - `HARDWARE.md` (resumo de pinos)
   - `FULL_HARDWARE.md` (detalhamento)
   - `DISPLAY_GUIDE.md` (display/touch)
3. Documente:
   - Qual placa exata está sendo usada
   - Quais pinos foram trocados
   - Quais recursos foram removidos/ativados (ex.: sem IMU, sem áudio, etc.)

Isso facilita a vida de quem for compilar o firmware para outra revisão no futuro.

---

Para detalhes completos de tensões, tabelas e observações adicionais, consulte:

- `FULL_HARDWARE.md`
- `DISPLAY_GUIDE.md`
- `pin_config.h` (fonte da verdade usada pelo código)
