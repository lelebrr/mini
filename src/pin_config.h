#pragma once

// Waveshare ESP32-S3-Touch-AMOLED-1.8 pin mapping
// MCU: ESP32-S3-N16R8, Display: SH8601 (QSPI), Touch: FT3168 (I2C)

// QSPI display (SH8601)
#define LCD_CS     4   // Chip Select
#define LCD_SCLK   5   // Clock
#define LCD_SDIO0  6   // Data 0
#define LCD_SDIO1  7   // Data 1
#define LCD_SDIO2  8   // Data 2
#define LCD_SDIO3  9   // Data 3
// Reset is controlled by IO expander (TCA9554) pin 0
#define LCD_RST    -1

#define LCD_WIDTH  368
#define LCD_HEIGHT 448

// I2C bus (internal)
// Shared by: touch, PMU (AXP2101), IO expander (TCA9554), IMU (QMI8658)
#define IIC_SDA    38
#define IIC_SCL    39
#define D_IIC_SDA  IIC_SDA
#define D_IIC_SCL  IIC_SCL

// I2C device addresses
#define AXP2101_SLAVE_ADDRESS                    0x34  // Power management
#define FT3168_DEVICE_ADDRESS                    0x38  // Touch controller
#define ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000  0x20  // IO expander
#define QMI8658_L_SLAVE_ADDRESS                  0x6B  // IMU
#define ES8311_ADDRRES_0                         0x18  // Audio codec

// Touch (FT3168)
#define TP_INT     -1   // We use polling in the LVGL driver
#define TOUCH_CS   -1

// Audio (ES8311 + I2S)
#define BCLKPIN    41   // I2S BCLK
#define WSPIN      42   // I2S LRCK/WS
#define DIPIN      1    // I2S data in (from mic)
#define DOPIN      40   // I2S data out (to speaker)
#define MCLKPIN    2    // I2S master clock

// SD card (SDMMC slot 1, 1‑bit mode)
#define SDMMC_CLK  12
#define SDMMC_CMD  11
#define SDMMC_DATA 13

// Common RGB565 colors
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN  0x07E0
#define BLUE   0x001F