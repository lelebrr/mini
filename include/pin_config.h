#pragma once

/**
 * pin_config.h
 * PwnTamagotchi BR - Edição Lendária
 * Hardware Target: ESP32-S3-WROOM-1-N16R8 Custom Board
 */

// --- Display ST7701S (Interface RGB 8-bit / Parallel) ---
// Note: User prompt listed "RGB 8-bit" but provided pins for a full RGB interface.
// We map them to standard 16-bit RGB 565 logic where possible or 8-bit generic.
// Groups: 45-48, 38-41, 15-18, 11-14, 21, 10, 9.

// Control Signals
#define LCD_CS     8
#define LCD_DC     40 // Usually unused in pure RGB, but ST7701 needs it for SPI init
#define LCD_RST    39
#define LCD_WR     47 // SPI SCL for Init
#define LCD_RD     -1

// RGB Interface Signals (Mapped to prompt's groups)
#define LCD_DE     21
#define LCD_VSYNC  10
#define LCD_HSYNC  9
#define LCD_PCLK   14 // From 11-14 group

// Data Pins (R0-R4, G0-G5, B0-B4 for RGB565)
// Group 45-48
#define LCD_R0     45
#define LCD_R1     46
#define LCD_R2     48 // 47 is WR
#define LCD_R3     38

// Group 38-41 (38 used above, 39=RST, 40=DC, 41 available)
#define LCD_R4     41

// Group 15-18
#define LCD_G0     15
#define LCD_G1     16
#define LCD_G2     17
#define LCD_G3     18

// Group 11-14 (14 is PCLK)
#define LCD_G4     11
#define LCD_G5     12
#define LCD_B0     13

// Spares or re-using high bits if 8-bit mode intended (Driver will ignore unused)
#define LCD_B1     -1
#define LCD_B2     -1
#define LCD_B3     -1
#define LCD_B4     -1

#define LCD_WIDTH  320
#define LCD_HEIGHT 240

// --- Touchscreen (GT1151 I2C) ---
#define TOUCH_SDA  6
#define TOUCH_SCL  7
#define TOUCH_INT  5
#define TOUCH_RST  4
#define GT1151_ADDR 0x14

// --- Áudio (ES8311 I2S + I2C) ---
#define ES8311_ADDR 0x18
#define I2S_BCLK   1
#define I2S_LRCK   2
#define I2S_DIN    3
#define I2S_DOUT   42

// --- IMU (QMI8658 I2C) ---
#define IMU_SDA    6
#define IMU_SCL    7
#define IMU_INT1   0
#define QMI8658_ADDR 0x6B

// --- SD Card (SPI) ---
#define SD_CLK     36
#define SD_CMD     35
#define SD_D0      37
#define SD_D1      34
#define SD_D2      33
#define SD_D3      38
#define SD_CS      21

// --- System ---
#define PIN_BOOT   0
#define COLOR_NEON_PURPLE 0x801F
#define COLOR_NEON_GREEN  0x07E0
