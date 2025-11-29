# Guia Completo de Display e Touch – Mini Lele (Waveshare ESP32‑S3 AMOLED)

Este documento explica, em detalhes, como o display AMOLED e o touchscreen da placa **Waveshare ESP32‑S3‑Touch‑AMOLED‑1.8** são usados pelo **Mini Lele**.

- Como o display é ligado e inicializado
- Pinagem usada pelo firmware
- Requisitos de tensão (PMIC AXP2101)
- Como integrar com `Arduino_GFX` e `TouchLib`
- Dicas rápidas de debug (tela preta, touch não responde)

---

## Índice

1. [Display AMOLED 1.8"]( #1-display-amoled-18)
   - [Especificações técnicas](#especificações-técnicas)
   - [Pinagem do display](#pinagem-do-display)
   - [Exemplo de inicialização com Arduino_GFX](#exemplo-de-inicialização-com-arduino_gfx)
   - [Sequência de reset via TCA9554](#sequência-de-reset-via-tca9554)
2. [Touchscreen capacitivo](#2-touchscreen-capacitivo)
   - [Pinagem do touch](#pinagem-do-touch)
   - [Endereços I²C](#endereços-i2c)
   - [Exemplo de uso com TouchLib](#exemplo-de-uso-com-touchlib)
3. [Gerenciamento de energia (AXP2101)](#3-gerenciamento-de-energia-pmic-axp2101)
4. [Dicas de debug](#4-dicas-de-debug)
5. [Links e referências](#5-links-e-referências)

---

## 1. Display AMOLED (1.8")

O painel AMOLED da placa utiliza o controlador **SH8601** (Sino Wealth) operando em interface **QSPI** (Quad SPI).  
Diferente de displays TFT comuns (SPI ou paralelo), ele exige:

- Sequência de inicialização específica
- Tensão controlada pelo PMIC AXP2101
- Reset via expansor de IO (TCA9554) e não diretamente pelo ESP32‑S3

### Especificações técnicas

- **Resolução:** 368 x 448 pixels
- **Controlador:** SH8601
- **Interface:** QSPI (4 linhas de dados + clock + chip select)
- **Formato de pixel:** RGB565 (16‑bit) – usado pelo firmware
- **Backlight:** AMOLED (auto emissivo) – sem LED de backlight dedicado  
  A alimentação do painel é controlada pela linha **BLDO1** do AXP2101.

### Pinagem do display

Mapeamento típico na placa Waveshare (verifique também `FULL_HARDWARE.md`):

| Sinal   | GPIO (ESP32‑S3) | Função                                      |
|--------|------------------|---------------------------------------------|
| LCD_CS | 12               | Chip Select (QSPI CS)                       |
| LCD_SCK| 11               | QSPI Clock                                  |
| LCD_D0 | 4                | QSPI Data 0                                 |
| LCD_D1 | 5                | QSPI Data 1                                 |
| LCD_D2 | 6                | QSPI Data 2                                 |
| LCD_D3 | 7                | QSPI Data 3                                 |
| LCD_RST| Expansor (P0)    | Reset do painel via **TCA9554** (pino 0)    |

> Observação: no firmware do Mini Lele, o reset é feito escrevendo no pino 0 do expansor I²C TCA9554 (endereço `0x20`), e não diretamente num GPIO.

### Exemplo de inicialização com Arduino_GFX

No Mini Lele, o display é controlado por `Arduino_GFX`.  
Abaixo um exemplo mínimo baseado na pinagem acima:

```cpp
#include <Arduino_GFX_Library.h>

// Barramento QSPI
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    12 /* CS */, 11 /* SCK */,
    4  /* D0 */, 5  /* D1 */, 6  /* D2 */, 7 /* D3 */
);

// Driver SH8601
Arduino_GFX *gfx = new Arduino_SH8601(
    bus,
    -1,      // RST físico é controlado via TCA9554 (não ligado direto)
    0,       // Rotação (0, 90, 180, 270)
    false,   // Invert (usar false na maioria dos casos)
    368,     // Largura do painel
    448      // Altura do painel
);

void setup() {
    // Aqui deve entrar a sequência de reset via TCA9554 (ver próxima seção)
    // ...
    gfx->begin();
    gfx->fillScreen(BLACK);
    gfx->setCursor(10, 10);
    gfx->setTextColor(WHITE);
    gfx->println("Mini Lele!");
}

void loop() {
}
```

### Sequência de reset via TCA9554

Como o pino de `RESET` do SH8601 está pendurado no **TCA9554**, a sequência típica é:

1. Inicializar o barramento I²C (SDA/SCL compartilhados)
2. Configurar o TCA9554 no endereço `0x20`
3. Definir o pino 0 como **saída**
4. Aplicar `LOW` por 10–20 ms (reset ativo)
5. Aplicar `HIGH` por 50–100 ms (sair de reset)
6. Só então chamar `gfx->begin()`

Pseudo‑código:

```cpp
// Exemplo genérico (a implementação real pode usar uma lib de IO Expander)
void resetDisplayViaTCA9554() {
    // 1) I2C.begin(SDA, SCL);
    // 2) Configurar direção do pino 0 como saída
    // 3) Escrever 0 (LOW) no pino 0
    delay(20);
    // 4) Escrever 1 (HIGH) no pino 0
    delay(80);
}
```

> Se esta sequência não for executada, o display geralmente fica totalmente preto, mesmo com o firmware rodando.

---

## 2. Touchscreen capacitivo

O touch pode vir com:

- **FT3168** (FocalTech), ou
- **GT1151** (Goodix)

Ambos compartilham o mesmo barramento I²C do restante dos periféricos (PMU, IMU, expansor, etc.).

### Pinagem do touch

| Sinal | GPIO          | Notas                                      |
|------|---------------|---------------------------------------------|
| SDA  | 15            | I²C SDA (compartilhado)                     |
| SCL  | 14            | I²C SCL (compartilhado)                     |
| INT  | 21            | Interrupção (active low) / pode ser polling|
| RST  | Expansor (P1) | Reset via **TCA9554** pino 1                |

### Endereços I²C

- **FT3168:** `0x38`
- **GT1151:** `0x14` (em alguns módulos também `0x5D`)

No Mini Lele, a lógica costuma:

- Tentar primeiro o endereço do FT3168 (`0x38`)
- Se falhar, testar o endereço do GT1151 (`0x14`)

### Exemplo de uso com TouchLib

A biblioteca recomendada é a [`TouchLib`](https://github.com/mmMicky/TouchLib), que já trata vários controladores de touch.

```cpp
#include <Wire.h>
#include <TouchLib.h>

// Exemplo: tentar FT3168 em 0x38
TouchLib *touch = new TouchLib(Wire, 15 /* SDA */, 14 /* SCL */, 0x38);

void setup() {
    Wire.begin(15, 14);
    if (!touch->init()) {
        // Se falhar, tentar GT1151 em 0x14
        delete touch;
        touch = new TouchLib(Wire, 15, 14, 0x14);
        if (!touch->init()) {
            // Tratar erro: touch não inicializou
        }
    }
}

void loop() {
    touch->read();
    if (touch->isTouched()) {
        int16_t x = touch->getX();
        int16_t y = touch->getY();
        // Processar toque (exibir, enviar para LVGL etc.)
    }
}
```

---

## 3. Gerenciamento de energia (PMIC AXP2101)

O display e o touch dependem fortemente do PMIC **AXP2101**, acessado via I²C (endereço `0x34`).  
Sem configurar corretamente os *LDOs* e *buck converters*, a tela não liga ou fica instável.

### Linhas típicas usadas para o display/touch

- **ALDO1 (1,8 V):** lógica do AMOLED e do touch
- **ALDO2 (2,8 V):** analógico do touch (referência)
- **ALDO3 (3,0 V):** áudio e periféricos auxiliares
- **BLDO1 (3,3 V):** alimentação principal do painel AMOLED (**crucial**)

No código do Mini Lele, o módulo de energia:

- Inicializa o AXP2101
- Liga e ajusta as tensões necessárias
- Monitora bateria, corrente de carga, etc.

Caso estas saídas não sejam habilitadas:

- O display permanecerá completamente preto
- Em alguns casos o touch também não responde

---

## 4. Dicas de debug

Problemas comuns ao trabalhar com display/touch:

### Tela completamente preta

- Verifique se:
  - O AXP2101 está sendo inicializado
  - **BLDO1** está habilitado em 3,3 V
  - A sequência de reset via TCA9554 (pino 0) está sendo executada
  - A resolução passada para `Arduino_SH8601` é `368x448`
- Confirme a pinagem usada no firmware (compare `pin_config.h` com esta documentação e com o esquemático da Waveshare).

### Touch não responde

- Confira:
  - SDA/SCL (normalmente 15/14) estão corretos
  - O endereço I²C tentado (`0x38` ou `0x14`) corresponde ao chip da sua placa
  - A linha de reset do touch (TCA9554 pino 1) está sendo liberada corretamente
- Verifique no barramento I²C com um scanner (ex.: exemplo `i2c_scanner` do Arduino) se o dispositivo aparece.

### Artefatos estranhos na tela

- Confirme:
  - Formato de pixel (`RGB565`)
  - Rotação e offsets de início de área de desenho usados na lib
  - Clocks dentro dos limites recomendados
- Reduza temporariamente otimizações e taxas de atualização para isolar o problema.

---

## 5. Links e referências

- **Placa Waveshare ESP32‑S3‑Touch‑AMOLED‑1.8**  
  Wiki oficial:  
  https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8

- **Arduino_GFX** (driver gráfico usado pelo Mini Lele)  
  https://github.com/moononournation/Arduino_GFX

- **TouchLib** (controladores FT3168, GT1151, etc.)  
  https://github.com/mmMicky/TouchLib

- **XPowersLib** (AXP2101 e outros PMICs)  
  https://github.com/lewisxhe/XPowersLib

Para detalhes adicionais de hardware (incluindo IMU, áudio, SD e demais periféricos), consulte também:

- `FULL_HARDWARE.md`
- `HARDWARE.md`
