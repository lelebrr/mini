#pragma once

/**
 * pin_config.h
 * Mapeamento de Pinos para Waveshare ESP32-S3-Touch-AMOLED-1.8
 *
 * Hardware:
 * - MCU: ESP32-S3 (N16R8)
 * - Tela: 1.8" AMOLED (SH8601) - QSPI
 * - Touch: FT3168 ou GT1151 - I2C
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
#define LCD_RST    -1  // Controlado pelo Expander IO Pin 0

#define LCD_WIDTH  368
#define LCD_HEIGHT 448

// --- Barramento I2C (Interno) ---
#define IIC_SDA    38
#define IIC_SCL    39
#define D_IIC_SDA  38
#define D_IIC_SCL  39

// Endereços I2C dos Dispositivos
#define AXP2101_SLAVE_ADDRESS 0x34
#define FT3168_DEVICE_ADDRESS 0x38
#define GT1151_DEVICE_ADDRESS 0x14 // Endereço comum para GT1151 (ou 0x5D)
#define ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000 0x20
#define QMI8658_L_SLAVE_ADDRESS 0x6B
#define ES8311_ADDRRES_0      0x18

// --- Áudio (ES8311 + I2S) ---
#define BCLKPIN    41
#define WSPIN      42
#define DIPIN      1
#define DOPIN      40
#define MCLKPIN    2

// --- Cartão SD (SDMMC Slot 1) ---
#define SDMMC_CLK  12
#define SDMMC_CMD  11
#define SDMMC_DATA 13

// --- Cores Cyberpunk BR ---
#define COLOR_NEON_PURPLE 0x801F // Roxo
#define COLOR_NEON_GREEN  0x07E0 // Verde Matrix
#define COLOR_NEON_YELLOW 0xFFE0 // Amarelo
#define COLOR_DARK_BG     0x0000
