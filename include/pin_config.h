#pragma once

/*
 * Mini Lele / Waveshare ESP32-S3-Touch-AMOLED-1.8
 * Hardware: SH8601 QSPI, FT3168/GT1151 I2C, AXP2101 I2C, ES8311 I2S,
 *           QMI8658 I2C, PCF85063 I2C, TCA9554 I2C.
 *
 * Fonte da verdade de mapeamento de pinos para o firmware.
 */

// -----------------------------------------------------------------------------
// Display QSPI (SH8601)
// -----------------------------------------------------------------------------

#define LCD_CS     12  // Chip Select
#define LCD_SCLK   11  // Clock
#define LCD_SDIO0  4   // Data 0
#define LCD_SDIO1  5   // Data 1
#define LCD_SDIO2  6   // Data 2
#define LCD_SDIO3  7   // Data 3

// Reset controlado via expansor TCA9554 (P0), não ligado direto ao ESP32-S3
#define LCD_RST    -1

#define LCD_WIDTH  368
#define LCD_HEIGHT 448

// -----------------------------------------------------------------------------
// Barramento I2C principal (Touch, PMU, Expansor IO, IMU, RTC, ES8311 ctrl)
// -----------------------------------------------------------------------------

#define IIC_SDA    15
#define IIC_SCL    14
#define D_IIC_SDA  IIC_SDA
#define D_IIC_SCL  IIC_SCL

// -----------------------------------------------------------------------------
// Endereços I2C
// -----------------------------------------------------------------------------

#define AXP2101_SLAVE_ADDRESS                     0x34
#define FT3168_DEVICE_ADDRESS                     0x38
#define GT1151_DEVICE_ADDRESS                     0x14   // Variante comum (ou 0x5D)
#define ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000   0x20
#define QMI8658_L_SLAVE_ADDRESS                   0x6B
#define ES8311_ADDRRES_0                          0x18

// Aliases convenientes usados pelo código
#define ES8311_ADDR                               0x18
#define FT3168_ADDR                               FT3168_DEVICE_ADDRESS
#define QMI8658_ADDR                              QMI8658_L_SLAVE_ADDRESS

// -----------------------------------------------------------------------------
// Touch (FT3168 / GT1151)
// -----------------------------------------------------------------------------

#define TP_INT     21   // Interrupção (active low)

#define TOUCH_SDA  IIC_SDA
#define TOUCH_SCL  IIC_SCL
#define TOUCH_INT  TP_INT
// Reset via TCA9554 P1
#define TOUCH_RST  -1

// -----------------------------------------------------------------------------
// Áudio (ES8311 + I2S)
// -----------------------------------------------------------------------------

// Pinos I2S conforme documentação da Waveshare
#define I2S_MCK_IO 16
#define I2S_BCK_IO 9
#define I2S_DI_IO  10
#define I2S_WS_IO  45
#define I2S_DO_IO  8

// Aliases compatíveis com exemplos antigos
#define MCLKPIN    16
#define BCLKPIN    9
#define WSPIN      45
#define DOPIN      10
#define DIPIN      8

// Enable do amplificador de áudio (PA)
#define PA_EN      46
#define PA         46

// -----------------------------------------------------------------------------
// Cartão SD (SDMMC, modo 1-bit – usado para logs, handshakes, etc.)
// -----------------------------------------------------------------------------

// Usa os pinos 1 e 2, por isso o debug é feito via USB CDC.
#define SDMMC_CLK  2
#define SDMMC_CMD  1
#define SDMMC_D0   42
#define SDMMC_DATA SDMMC_D0

// -----------------------------------------------------------------------------
// Sistema / botões
// -----------------------------------------------------------------------------

#define PIN_BOOT   0   // BOOT button (GPIO0)

// -----------------------------------------------------------------------------
// Cores RGB565 básicas
//   (só definimos se ainda não existirem para evitar conflitos com
//    bibliotecas gráficas como Arduino_GFX)
// -----------------------------------------------------------------------------

#ifndef BLACK
#define BLACK   0x0000
#endif

#ifndef WHITE
#define WHITE   0xFFFF
#endif

#ifndef RED
#define RED     0xF800
#endif

#ifndef GREEN
#define GREEN   0x07E0
#endif

#ifndef BLUE
#define BLUE    0x001F
#endif
