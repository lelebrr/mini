#pragma once

// Waveshare ESP32-S3-Touch-AMOLED-1.8 Pinout
// Configuration based on Hardware v1.0 / v1.1
// ESP32-S3-N16R8 Module: 16MB Flash, 8MB PSRAM (Octal)
// Connectivity: Wi-Fi 2.4GHz (802.11 b/g/n) & Bluetooth 5 (LE)

// --- QSPI Display (SH8601) ---
// Resolution: 368x448 px (AMOLED)
#define LCD_CS     4   // Chip Select
#define LCD_SCLK   5   // Clock
#define LCD_SDIO0  6   // Data 0
#define LCD_SDIO1  7   // Data 1
#define LCD_SDIO2  8   // Data 2
#define LCD_SDIO3  9   // Data 3
// LCD Reset is controlled by TCA9554 IO Expander Pin 0 (Output)
#define LCD_RST    -1

#define LCD_WIDTH  368
#define LCD_HEIGHT 448

// --- I2C Bus (Internal) ---
// Used for: Touch, PMU (AXP2101), IO Expander (TCA9554), IMU (QMI8658)
#define IIC_SDA    38
#define IIC_SCL    39
#define D_IIC_SDA  38
#define D_IIC_SCL  39

// Device I2C Addresses
#define AXP2101_SLAVE_ADDRESS 0x34         // Power Management Unit
#define FT3168_DEVICE_ADDRESS 0x38         // Touch Controller
#define ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000 0x20 // IO Expander
#define QMI8658_L_SLAVE_ADDRESS 0x6B       // 6-Axis IMU (Check 0x6A or 0x6B)
#define ES8311_ADDRRES_0      0x18         // Audio Codec

// --- Touch (FT3168) ---
// Reset is controlled by TCA9554 IO Expander Pin 1 (Output)
#define TP_INT     -1  // Polling is often used, but interrupt pin might be connected to a GPIO
#define TOUCH_CS   -1

// --- Audio (ES8311 + I2S) ---
// Microphone (Input) and Speaker (Output)
#define BCLKPIN    41
#define WSPIN      42
#define DIPIN      1   // DIN (Mic Data In)
#define DOPIN      40  // DOUT (Speaker Data Out)
#define MCLKPIN    2   // Master Clock
// Audio Power/Enable might be controlled by Expander Pin 6

// --- SD Card (SDMMC Slot 1) ---
// 1-Bit Mode
#define SDMMC_CLK  12
#define SDMMC_CMD  11
#define SDMMC_DATA 13

// --- IO Expander (TCA9554) Pin Map ---
// Pin 0: LCD Reset
// Pin 1: Touch Reset
// Pin 2: Peripheral Power Gate (Main)
// Pin 3: Unknown / Unused
// Pin 4: Backlight Control (or related)
// Pin 5: PMU Interrupt / Input
// Pin 6: Audio Power / Enable
// Pin 7: Unknown

// --- Colors (RGB565) ---
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F

// --- Exposed Header (Approximate) ---
// 3.3V, GND, TX, RX, I2C, etc. available on side connector.
