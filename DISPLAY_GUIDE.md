# Guia Completo de Display e Touch - Mini Lele (Waveshare S3 AMOLED)

## 1. Display AMOLED (1.8")
O painel AMOLED utiliza o controlador **SH8601** (Sino Wealth) operando em interface **QSPI** (Quad SPI). Diferente de displays LCD comuns, este painel requer uma sequência de inicialização complexa enviada via QSPI e tensões específicas gerenciadas pelo PMIC AXP2101.

### Especificações Técnicas
*   **Resolução:** 368 x 448 pixels
*   **Driver:** SH8601
*   **Interface:** QSPI (4-bit data + Clock + CS)
*   **Formato de Pixel:** RGB565 (16-bit)
*   **Backlight:** Não aplicável (AMOLED emite luz própria), mas o rail de alimentação do painel é controlado via AXP2101 (BLDO1).

### Pinagem (Hardware Físico)
| Sinal | GPIO (ESP32-S3) | Função |
| :--- | :--- | :--- |
| LCD_CS | 12 | Chip Select |
| LCD_SCK | 11 | Serial Clock |
| LCD_D0 | 4 | Data 0 |
| LCD_D1 | 5 | Data 1 |
| LCD_D2 | 6 | Data 2 |
| LCD_D3 | 7 | Data 3 |
| LCD_RST | Ext. 0 | Reset (Controlado via TCA9554 Pino 0) |

### Requisitos de Software (Drivers)
Para funcionar corretamente no ambiente Arduino/PlatformIO, utilizamos a biblioteca **Arduino_GFX** (MoonOnOurNation).

**Construtor Recomendado:**
```cpp
#include <Arduino_GFX_Library.h>

// Barramento de Dados QSPI
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    12 /* CS */, 11 /* SCK */,
    4 /* D0 */, 5 /* D1 */, 6 /* D2 */, 7 /* D3 */
);

// Driver SH8601
Arduino_GFX *gfx = new Arduino_SH8601(
    bus,
    -1 /* RST - controlado manualmente via TCA9554 */,
    0 /* Rotação */,
    false /* IPS/Invert */,
    368 /* Largura */,
    448 /* Altura */
);
```

### Sequência de Inicialização (Reset)
O pino de Reset **não** está conectado diretamente ao ESP32. Ele é controlado pelo pino 0 do Expansor de IO (TCA9554).
1.  Inicializar TCA9554 (I2C `0x20`).
2.  Configurar Pino 0 como OUTPUT.
3.  Jogar LOW (Reset Ativo) por 10-20ms.
4.  Jogar HIGH (Operação) por 50-100ms.
5.  Chamar `gfx->begin()`.

---

## 2. Touchscreen Capacitivo
O painel de toque utiliza o controlador **FT3168** (FocalTech) ou **GT1151** (Goodix) dependendo do lote. Ambos operam via I2C no mesmo barramento dos periféricos.

### Pinagem
| Sinal | GPIO | Notas |
| :--- | :--- | :--- |
| SDA | 15 | I2C Data (Compartilhado) |
| SCL | 14 | I2C Clock (Compartilhado) |
| INT | 21 | Interrupção (Active Low) |
| RST | Ext. 1 | Reset (Controlado via TCA9554 Pino 1) |

### Endereços I2C
*   **FT3168:** `0x38`
*   **GT1151:** `0x14`

### Driver (TouchLib)
Recomendamos a biblioteca `TouchLib` (mmMicky) que suporta ambos os chips e autodetecta via endereço.

**Inicialização:**
```cpp
#include <TouchLib.h>
TouchLib *touch = new TouchLib(Wire, 15, 14, 0x38); // Tenta FT3168 primeiro
// Se falhar, tentar 0x14
```

---

## 3. Gerenciamento de Energia (PMIC AXP2101)
O display AMOLED exige tensões muito específicas para funcionar. Se o AXP2101 não for configurado, a tela permanecerá preta.

### Configuração de Tensão (Via XPowersLib)
*   **ALDO1 (1.8V):** Lógica do AMOLED e Touch.
*   **ALDO2 (2.8V):** Tensão analógica do Touch.
*   **ALDO3 (3.0V):** Áudio e Periféricos.
*   **BLDO1 (3.3V):** Alimentação principal do painel AMOLED. **Crucial!**

---

## 4. Downloads e Referências
*   **Wiki Oficial:** [Waveshare ESP32-S3-Touch-AMOLED-1.8](https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8)
*   **Arduino_GFX:** [GitHub Link](https://github.com/moononournation/Arduino_GFX)
*   **TouchLib:** [GitHub Link](https://github.com/mmMicky/TouchLib)
*   **XPowersLib:** [GitHub Link](https://github.com/lewisxhe/XPowersLib)
