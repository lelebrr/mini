#pragma once

// Waveshare ESP32-S3-Touch-AMOLED-1.8 Pinout
// Based on provided documentation, example code, and code review.

// QSPI Display (SH8601)
// Confirmed from src/user_setup.h and example .ino usage
#define LCD_CS     4
#define LCD_SCLK   5
#define LCD_SDIO0  6
#define LCD_SDIO1  7
#define LCD_SDIO2  8
#define LCD_SDIO3  9
// LCD Reset is handled by IO Expander Pin 0, not a GPIO.
// We define it as -1 for the GFX driver.
#define LCD_RST    -1

// Display resolution
#define LCD_WIDTH  368
#define LCD_HEIGHT 448

// I2C Bus (Touch FT3168, PMIC, etc.)
#define IIC_SDA    38
#define IIC_SCL    39
#define D_IIC_SDA  38 // Alias
#define D_IIC_SCL  39 // Alias

// Touch (FT3168)
#define FT3168_DEVICE_ADDRESS 0x38
#define TP_INT     -1 // Interrupt pin (check if connected to IO Expander or GPIO)
#define TOUCH_CS   -1

// IO Expander (TCA9554)
// Pin 0: LCD_RST
// Pin 1: TOUCH_RST
// Pin 2: LCD_PWR (AVDD/AVEE?)
// Pin 6: AUDIO_PWR? (Or maybe PA)
#define ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000 0x20

// Audio (ES8311) - I2S
// Pins for Waveshare ESP32-S3-Touch-AMOLED-1.8 (Based on review)
#define BCLKPIN    41
#define WSPIN      42
#define DIPIN      1  // DIN (Data In to ESP, from Mic)
#define DOPIN      40 // DOUT (Data Out from ESP, to Speaker)
#define MCLKPIN    2  // Master Clock
#define PA         -1 // Power Amplifier Enable (Likely on Expander Pin)

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
