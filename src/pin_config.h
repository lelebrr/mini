#pragma once

// Waveshare ESP32-S3-Touch-AMOLED-1.8 Pinout

// QSPI Display (SH8601)
#define D_LCD_CS     4
#define D_LCD_SCLK   5
#define D_LCD_SDIO0  6
#define D_LCD_SDIO1  7
#define D_LCD_SDIO2  8
#define D_LCD_SDIO3  9
#define D_LCD_RST    10

// I2C for PMIC (AXP2101) and Touch (FT3168)
#define D_IIC_SDA    38
#define D_IIC_SCL    39

// AXP2101 I2C Address
#define AXP2101_SLAVE_ADDRESS 0x34

// Display dimensions
#define LCD_WIDTH  320
#define LCD_HEIGHT 480

// Display configuration
#define TFT_MISO 47
#define TFT_MOSI 48
#define TFT_SCLK 45
#define TFT_CS    4
#define TFT_DC    3
#define TFT_RST   10

// Colors
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
