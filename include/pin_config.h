#pragma once

/**
 * pin_config.h
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
 * Mapeamento de Pinos para Waveshare ESP32-S3-Touch-AMOLED-1.8
 *
 * Hardware:
 * - MCU: ESP32-S3 (N16R8)
 * - Tela: 1.8" AMOLED (SH8601) - QSPI
 * - Touch: FT3168 ou GT1151 - I2C
 * - PMU: AXP2101 - I2C
 * - Áudio: ES8311 - I2S
 * - IMU: QMI8658 - I2C
 * - Expansor IO: TCA9554 - I2C
 */

// --- Display QSPI (SH8601) ---
// Resolução: 368x448 pixels
#define LCD_CS     4   // Chip Select
#define LCD_SCLK   5   // Clock
#define LCD_SDIO0  6   // Data 0
#define LCD_SDIO1  7   // Data 1
#define LCD_SDIO2  8   // Data 2
#define LCD_SDIO3  9   // Data 3
#define LCD_RST    -1  // Controlado pelo Expander IO Pin 0
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified

#define LCD_WIDTH  368
#define LCD_HEIGHT 448

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
// --- Barramento I2C (Interno) ---
#define IIC_SDA    38
#define IIC_SCL    39
#define D_IIC_SDA  38
#define D_IIC_SCL  39

// Endereços I2C dos Dispositivos
#define AXP2101_SLAVE_ADDRESS 0x34
#define FT3168_DEVICE_ADDRESS 0x38
#define GT1151_DEVICE_ADDRESS 0x14 // Endereço comum para GT1151 (ou 0x5D)
#define ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000 0x20
#define QMI8658_L_SLAVE_ADDRESS 0x6B
#define ES8311_ADDRRES_0      0x18

// --- Áudio (ES8311 + I2S) ---
#define BCLKPIN    41
#define WSPIN      42
#define DIPIN      1
#define DOPIN      40
#define MCLKPIN    2

// --- Cartão SD (SDMMC Slot 1) ---
#define SDMMC_CLK  12
#define SDMMC_CMD  11
#define SDMMC_DATA 13

// --- Cores Cyberpunk BR ---
#define COLOR_NEON_PURPLE 0x801F // Roxo
#define COLOR_NEON_GREEN  0x07E0 // Verde Matrix
#define COLOR_NEON_YELLOW 0xFFE0 // Amarelo
#define COLOR_DARK_BG     0x0000
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/merge-ready-mini-lele-v2
=======
>>>>>>> origin/mini-lele-v2-complete-verified
=======
>>>>>>> origin/mini-lele-v2-final-verified
