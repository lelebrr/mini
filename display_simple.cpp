// Exemplo simplificado baseado no código funcional fornecido pelo usuário
// display_simple.cpp

#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"

// Configurações de hardware - usando exatamente como nos exemplos
#define LCD_CS     4    // D_LCD_CS
#define LCD_SCLK   5    // D_LCD_SCLK  
#define LCD_SDIO0  6    // D_LCD_SDIO0
#define LCD_SDIO1  7    // D_LCD_SDIO1
#define LCD_SDIO2  8    // D_LCD_SDIO2
#define LCD_SDIO3  9    // D_LCD_SDIO3

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
  LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */, LCD_SDIO1 /* SDIO1 */,
  LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);

Arduino_GFX *gfx = new Arduino_SH8601(bus, -1 /* RST */,
                                      0 /* rotation */, false /* IPS */, 320, 240);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=== Display Test Simplificado ===");
  
  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
    return;
  }
  
  Serial.println("Display iniciado com sucesso!");
  
  // Teste básico de cores
  gfx->fillScreen(BLACK);
  delay(500);
  
  gfx->fillScreen(RED);
  delay(1000);
  
  gfx->fillScreen(GREEN);
  delay(1000);
  
  gfx->fillScreen(BLUE);
  delay(1000);
  
  gfx->fillScreen(WHITE);
  delay(1000);
  
  gfx->fillScreen(BLACK);
  delay(500);
  
  // Teste de texto
  gfx->setTextColor(RED);
  gfx->setCursor(10, 10);
  gfx->println("Minigotchi S3");
  
  gfx->setTextColor(WHITE);
  gfx->setCursor(10, 30);
  gfx->println("Display OK!");
  
  gfx->setTextColor(GREEN);
  gfx->setCursor(10, 50);
  gfx->println("Amoled Working");
  
  Serial.println("Teste completo!");
  delay(3000);
}

void loop() {
  // Loop vazio - apenas teste de display
}