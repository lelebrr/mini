#pragma once

/**
 * pin_config.h
 * Minimal, cleaned mapping for Waveshare ESP32-S3-Touch-AMOLED-1.8
 *
 * MCU:   ESP32-S3 (N16R8)
 * LCD:   1.8" AMOLED (SH8601) - QSPI
 * Touch: FT3168 / GT1151 - I2C
 * PMU:   AXP2101 - I2C
 * Audio: ES8311 - I2S
 * IMU:   QMI8658 - I2C
 * IOX:   TCA9554 - I2C
 */

#define XPOWERS_CHIP_AXP2101

// ------------------------
// Display QSPI (SH8601)
// ------------------------
#define LCD_CS     12
#define LCD_SCLK   11
#define LCD_SDIO0  4
#define LCD_SDIO1  5
#define LCD_SDIO2  6
#define LCD_SDIO3  7
// LCD reset is driven by IO expander (pin 0)
#define LCD_RST    -1

#define LCD_WIDTH  368
#define LCD_HEIGHT 448

// ------------------------
// Shared internal I2C bus
// (Touch, PMU, IO expander, IMU, codec ctrl)
// ------------------------
#define IIC_SDA    15
#define IIC_SCL    14

// Touch
#define FT3168_DEVICE_ADDRESS 0x38
#define GT1151_DEVICE_ADDRESS 0x14
#define TP_INT                21
#define TOUCH_SDA             IIC_SDA
#define TOUCH_SCL             IIC_SCL

// PMU
#define AXP2101_SLAVE_ADDRESS 0x34

// IO expander TCA9554
#define ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000 0x20

// IMU
#define QMI8658_L_SLAVE_ADDRESS 0x6B

// ------------------------
// Audio (ES8311 + I2S)
// ------------------------
#define ES8311_ADDRRES_0  0x18

#define MCLKPIN 16
#define BCLKPIN 9
#define WSPIN   45
#define DOPIN   10
#define DIPIN   8
// On most Waveshare boards PA is a direct GPIO
#define PA_EN   46

// ------------------------
// SD card (SDMMC, 1‑bit mode)
// ------------------------
#define SDMMC_CLK   2
#define SDMMC_CMD   1
#define SDMMC_DATA  3
#define SDMMC_D0    SDMMC_DATA

// ------------------------
// System / colors
// ------------------------
#define PIN_BOOT   0

#define COLOR_NEON_PURPLE 0x801F
#define COLOR_NEON_GREEN  0x07E0
#define COLOR_NEON_YELLOW 0xFFE0
#define COLOR_DARK_BG     0x0000

// Basic RGB565
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
