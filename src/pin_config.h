#pragma once

// Waveshare ESP32-S3-Touch-AMOLED-1.8 Pinout
// Corrected based on functional demos and analysis.

// QSPI Display (SH8601)
#define LCD_CS     4
#define LCD_SCLK   5
#define LCD_SDIO0  6
#define LCD_SDIO1  7
#define LCD_SDIO2  8
#define LCD_SDIO3  9
// LCD Reset is handled by IO Expander Pin 0, not a GPIO.
// But some demos define it as 10? We will stick to -1 for driver and manual handling if needed,
// OR if the driver expects a pin, we might need to verify.
// In 13_LVGL_Widgets, -1 is passed to constructor.
#define LCD_RST    -1

// Display resolution
// Confirmed 1.8 inch AMOLED is typically 368x448.
#define LCD_WIDTH  368
#define LCD_HEIGHT 448

// I2C Bus (Touch FT3168, PMIC AXP2101, IO Expander TCA9554)
#define IIC_SDA    38
#define IIC_SCL    39
#define D_IIC_SDA  38
#define D_IIC_SCL  39

// Devices I2C Addresses
#define AXP2101_SLAVE_ADDRESS 0x34
#define FT3168_DEVICE_ADDRESS 0x38
#define ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000 0x20

// Touch (FT3168)
#define TP_INT     -1 // Polling used in demos
#define TOUCH_CS   -1

// Audio (ES8311) - I2S
// Pins for Waveshare ESP32-S3-Touch-AMOLED-1.8
#define BCLKPIN    41
#define WSPIN      42
#define DIPIN      1  // DIN (Mic)
#define DOPIN      40 // DOUT (Speaker)
#define MCLKPIN    2  // Master Clock
#define PA         -1 // Power Amplifier controlled via Expander or PMIC?

// SD MMC
// Standard S3 SDMMC Slot 1 pins:
#define SDMMC_CLK  12
#define SDMMC_CMD  11
#define SDMMC_DATA 13

// Colors
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
