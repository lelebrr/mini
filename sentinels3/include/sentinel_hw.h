// SENTINEL-S3 Hardware pin definitions
#pragma once

// Display QSPI
#define PIN_LCD_CS          12
#define PIN_LCD_SCK         11
#define PIN_LCD_D0          4
#define PIN_LCD_D1          5
#define PIN_LCD_D2          6
#define PIN_LCD_D3          7

// Touch I2C
#define PIN_TOUCH_SDA       15
#define PIN_TOUCH_SCL       14
#define PIN_TOUCH_INT       21
#define ADDR_FT3168         0x38
#define ADDR_CST820         0x15

// I2C bus
#define PIN_I2C_SDA         15
#define PIN_I2C_SCL         14
#define I2C_FREQ            400000

// PMU AXP2101
#define ADDR_AXP2101        0x34

// Audio I2S
#define PIN_I2S_MCLK        16
#define PIN_I2S_BCLK        9
#define PIN_I2S_WS          45
#define PIN_I2S_DOUT        8
#define PIN_I2S_DIN         10
#define PIN_PA_EN           46

// IMU QMI8658
#define ADDR_QMI8658        0x6B

// RTC PCF85063
#define ADDR_PCF85063       0x51

// IO Expander TCA9554
#define ADDR_TCA9554        0x20
#define TCA_PIN_LCD_RST     0
#define TCA_PIN_TP_RST      1
#define TCA_PIN_PERIPH      2
#define TCA_PIN_AUDIO       6
#define TCA_PIN_SD_CS       7

// SDMMC
#define PIN_SD_CLK          2
#define PIN_SD_CMD          1
#define PIN_SD_D0           3

// LED
#define PIN_LED_RGB         38

// ARM
#define PIN_ARM             9

// BOOT
#define PIN_BOOT            0

// Display resolution
#define LCD_WIDTH           368
#define LCD_HEIGHT          448
