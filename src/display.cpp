#include "display.h"
#include "pin_config.h"
#include <Arduino_GFX_Library.h>

// Declarações de display
Arduino_DataBus *bus = nullptr;
Arduino_GFX *gfx_display = nullptr;

// Implementações para o display Arduino_GFX
namespace Display {
    void startScreen() {
        Serial.println("=== Minigotchi Display Init ===");
        
        // Inicializa o bus QSPI - EXATAMENTE como nos exemplos funcionais
        Serial.println("Creating QSPI bus...");
        bus = new Arduino_ESP32QSPI(
            4 /* CS */, 5 /* SCK */, 6 /* SDIO0 */, 7 /* SDIO1 */,
            8 /* SDIO2 */, 9 /* SDIO3 */);
        
        // Inicializa o display SH8601 - SEGUINDO EXATAMENTE OS EXEMPLOS
        Serial.println("Creating SH8601 display...");
        gfx_display = new Arduino_SH8601(bus, -1 /* RST */,
                                      0 /* rotation */, false /* IPS */, LCD_WIDTH, LCD_HEIGHT);
        
        Serial.println("Beginning display...");
        if (!gfx_display->begin()) {
            Serial.println("gfx->begin() failed!");
            return;
        }
        
        Serial.println("Display started!");
        
        // Teste EXATO do exemplo 1 + cores do exemplo 2
        gfx_display->fillScreen(BLACK);
        delay(500);
        
        Serial.println("Testing colors...");
        gfx_display->fillScreen(RED);
        delay(1000);
        gfx_display->fillScreen(GREEN);
        delay(1000);
        gfx_display->fillScreen(BLUE);
        delay(1000);
        gfx_display->fillScreen(WHITE);
        delay(1000);
        gfx_display->fillScreen(BLACK);
        
        // Teste de texto final
        gfx_display->setTextColor(RED);
        gfx_display->setCursor(10, 20);
        gfx_display->println("Minigotchi S3");
        gfx_display->setTextColor(WHITE);
        gfx_display->setCursor(10, 50);
        gfx_display->println("Display Working!");
        gfx_display->setTextColor(GREEN);
        gfx_display->setCursor(10, 80);
        gfx_display->println("Amoled OK!");
        
        Serial.println("=== Display Test Complete ===");
        delay(3000);
    }

    void updateDisplay(String face) {
        if (gfx_display) {
            gfx_display->fillScreen(BLACK);
            gfx_display->setCursor(10, 20);
            gfx_display->setTextColor(WHITE);
            gfx_display->println(face);
            delay(2000); // Dar tempo para ver o display
        }
    }

    void updateDisplay(String face, String text) {
        if (gfx_display) {
            gfx_display->fillScreen(BLACK);
            gfx_display->setCursor(10, 20);
            gfx_display->setTextColor(RED);
            gfx_display->println(face);
            gfx_display->setCursor(10, 50);
            gfx_display->setTextColor(GREEN);
            gfx_display->println(text);
            delay(2000); // Dar tempo para ver o display
        }
    }
}
