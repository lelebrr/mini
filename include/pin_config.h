#pragma once

/**
 * pin_config.h
	 * Mini Lele - Waveshare ESP32-S3-Touch-AMOLED-1.8
	 * Hardware: SH8601 QSPI, FT3168 I2C, AXP2101 I2C, ES8311 I2S
	 */
	
	// --- Display QSPI (SH8601) ---
	#define LCD_CS     12
	#define LCD_SCK    11
	#define LCD_D0     4
	#define LCD_D1     5
	#define LCD_D2     6
	#define LCD_D3     7
	#define LCD_RST    -1 // Reset via TCA9554 Pin 0

#define LCD_WIDTH  368
#define LCD_HEIGHT 448

// --- I2C (Shared Bus: Touch, PMU, Audio, IMU, Expander, RTC) ---
	#define IIC_SDA    15
	#define IIC_SCL    14
	
	// --- Touchscreen (FT3168) ---
	#define TOUCH_SDA  15
	#define TOUCH_SCL  14
	#define TOUCH_INT  21
	#define TOUCH_RST  -1 // Via TCA9554 Pin 1
	#define FT3168_ADDR 0x38
	
	// --- PMU (AXP2101) ---
	#define PMU_SDA    15
	#define PMU_SCL    14
	#define AXP2101_SLAVE_ADDRESS 0x34
	
	// --- Audio (ES8311) ---
	#define ES8311_ADDR 0x18
	#define I2S_MCLK   16
	#define I2S_BCLK   9
	#define I2S_LRCK   45
	#define I2S_DOUT   10
	#define I2S_DIN    8
	#define PA_EN      46
	
	// --- IMU (QMI8658) ---
	#define IMU_SDA    15
	#define IMU_SCL    14
	#define QMI8658_ADDR 0x6B
	
	// --- RTC (PCF85063) ---
	#define RTC_ADDR   0x51
	
	// --- SD Card (SDMMC Slot 0 Remapped) ---
	// Using 1-bit mode. Pinos 1 e 2 são UART0 TX/RX.
	// Requer ARDUINO_USB_CDC_ON_BOOT=1.
	#define SDMMC_CLK  2
	#define SDMMC_CMD  1
	#define SDMMC_D0   42 // Safe fallback for 1-bit mode
	
	// --- System ---
	#define PIN_BOOT   0
	
	// --- Cores Cyberpunk BR ---
	#define COLOR_NEON_PURPLE 0x801F // Roxo
	#define COLOR_NEON_GREEN  0x07E0 // Verde Matrix
	#define COLOR_NEON_YELLOW 0xFFE0 // Amarelo
	#define COLOR_DARK_BG     0x0000
