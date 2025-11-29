#pragma once

// Mini Lele / Waveshare ESP32-S3-Touch-AMOLED-1.8 pin mapping

// --- QSPI Display (SH8601) ---
#define LCD_CS     4
#define LCD_SCLK   5
#define LCD_SDIO0  6
#define LCD_SDIO1  7
#define LCD_SDIO2  8
#define LCD_SDIO3  9
// LCD reset is controlled by the IO expander
#define LCD_RST    -1

#define LCD_WIDTH  368
#define LCD_HEIGHT 448

// --- I2C Bus (PMU, Touch, IO Expander, IMU) ---
#define IIC_SDA    38
#define IIC_SCL    39
#define D_IIC_SDA  IIC_SDA
#define D_IIC_SCL  IIC_SCL

// I2C device addresses
#define AXP2101_SLAVE_ADDRESS                 0x34  // PMU
#define FT3168_DEVICE_ADDRESS                 0x38  // Touch
#define ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000 0x20 // IO expander
#define QMI8658_L_SLAVE_ADDRESS               0x6B  // IMU
#define ES8311_ADDRRES_0                      0x18  // Audio codec

// Touch (FT3168)
#define TP_INT   -1
#define TOUCH_CS -1

// --- Audio (ES8311 + I2S) ---
#define BCLKPIN  41
#define WSPIN    42
#define DIPIN    1   // Mic data in
#define DOPIN    40  // Speaker data out
#define MCLKPIN  2   // Master clock

// --- SD Card (SDMMC, 1-bit mode) ---
#define SDMMC_CLK   12
#define SDMMC_CMD   11
#define SDMMC_DATA  13

// --- RGB565 Colors ---
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
