// User setup file for TFT_eSPI

// Waveshare ESP32-S3-Touch-AMOLED-1.8 Configuration
// Based on official examples

// Display size
#define LCD_WIDTH 320
#define LCD_HEIGHT 480

// QSPI Display pins (SH8601)
#define LCD_CS     4
#define LCD_SCLK   5
#define LCD_SDIO0  6
#define LCD_SDIO1  7
#define LCD_SDIO2  8
#define LCD_SDIO3  9
#define LCD_RST    10

// I2C pins
#define D_IIC_SDA  38
#define D_IIC_SCL  39

// Touch CS (not used)
#define TOUCH_CS -1

// Display rotation
#define TFT_ROTATION 1

// Colors
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F


// Background color
#define TFT_BLACK 0x0000
#define TFT_WHITE 0xFFFF
#define TFT_RED   0xF800
#define TFT_GREEN 0x07E0
#define TFT_BLUE  0x001F