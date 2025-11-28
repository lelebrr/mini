#pragma once

/**
 * pin_config.h
 * Mapeamento de Pinos para Waveshare ESP32-S3-Touch-AMOLED-1.8
 *
 * Hardware:
 * - MCU: ESP32-S3 (N16R8)
 * - Tela: 1.8" AMOLED (SH8601) - QSPI
 * - Touch: FT3168 - I2C
 * - PMU: AXP2101 - I2C
 * - Áudio: ES8311 - I2S
 * - IMU: QMI8658 - I2C
 * - Expansor IO: TCA9554 - I2C
 */

// --- Display QSPI (SH8601) ---
// Resolução: 368x448 pixels
#define LCD_CS     4   // Chip Select
#define LCD_SCLK   5   // Clock
#define LCD_SDIO0  6   // Data 0
#define LCD_SDIO1  7   // Data 1
#define LCD_SDIO2  8   // Data 2
#define LCD_SDIO3  9   // Data 3
// O Reset do LCD é controlado pelo Pino 0 do Expansor IO (TCA9554)
#define LCD_RST    -1

#define LCD_WIDTH  368
#define LCD_HEIGHT 448

// --- Barramento I2C (Interno) ---
// Compartilhado por: Touch, PMU, Expansor IO, IMU
#define IIC_SDA    38
#define IIC_SCL    39
#define D_IIC_SDA  38
#define D_IIC_SCL  39

// Endereços I2C dos Dispositivos
#define AXP2101_SLAVE_ADDRESS 0x34         // Gerenciamento de Energia
#define FT3168_DEVICE_ADDRESS 0x38         // Controlador de Touch
#define ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000 0x20 // Expansor de IO
#define QMI8658_L_SLAVE_ADDRESS 0x6B       // Acelerômetro/Giroscópio
#define ES8311_ADDRRES_0      0x18         // Codec de Áudio

// --- Touch (FT3168) ---
// Reset controlado pelo Pino 1 do Expansor IO
#define TP_INT     -1  // Interrupção (usamos polling)
#define TOUCH_CS   -1

// --- Áudio (ES8311 + I2S) ---
// Microfone (Entrada) e Alto-falante (Saída)
#define BCLKPIN    41
#define WSPIN      42
#define DIPIN      1   // Data In (Do Mic)
#define DOPIN      40  // Data Out (Para Speaker)
#define MCLKPIN    2   // Master Clock (Gerado pelo ESP32)

// --- Cartão SD (SDMMC Slot 1) ---
// Modo 1-Bit (Padrão da placa)
#define SDMMC_CLK  12
#define SDMMC_CMD  11
#define SDMMC_DATA 13

// --- Cores (Formato RGB565) ---
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
