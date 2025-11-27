/*
 * PlatformIO specific definitions for Minigotchi ESP32
 * Copyright (C) 2024 dj1ch
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PLATFORMIO_H
#define PLATFORMIO_H

#ifdef ARDUINO
  #ifdef ESP32
    #define PLATFORM_ESP32 1
    #define PLATFORM_ESP32_S3 1
  #endif
#endif

// PlatformIO specific includes
#ifdef PLATFORMIO
  #include <Arduino.h>
  #include <Wire.h>
  #include <WiFi.h>
  #include <esp_wifi.h>
  #include <nvs_flash.h>
  #include <freertos/FreeRTOS.h>
  #include <freertos/task.h>
  #include <freertos/queue.h>
  
  // PlatformIO specific optimizations
  #define PLATFORMIO_OPTIMIZED 1
  #define PLATFORMIO_DEBUG 1
#endif

// Platform specific definitions
#ifdef PLATFORM_ESP32_S3
  #define ESP32_S3_SPECIFIC 1
  #define HAS_QSPI_DISPLAY 1
  #define HAS_I2C_PMU 1
#endif

// Display specific definitions for PlatformIO
#ifdef PLATFORMIO
  #ifdef HAS_QSPI_DISPLAY
    // Bibliotecas de display e PMU - serão incluídas via PlatformIO
    // #include <Arduino_GFX_Library.h>
    // #include <XPowersLib.h>
    // Biblioteca de display - será incluída via PlatformIO
    // #include <Arduino_SH8601.h>
    // Biblioteca de PMU - será incluída via PlatformIO
    // #include <XPowersAXP2101.h>
  #endif
#endif

// Memory optimization for PlatformIO
#ifdef PLATFORMIO
  #define PLATFORMIO_MEMORY_OPTIMIZATION 1
#endif

// PlatformIO specific functions
#ifdef PLATFORMIO
  #ifdef __cplusplus
    extern "C" {
  #endif
  
  void platformio_init();
  void platformio_cleanup();
  void platformio_debug(const char* message);
  
  #ifdef __cplusplus
    }
  #endif
#endif

#endif // PLATFORMIO_H