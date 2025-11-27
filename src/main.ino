#include "config.h"
#include "minigotchi.h"
#ifdef PLATFORMIO
  #include "platformio.h"
#endif

// === DISPLAY AMOLED + AXP2101 ===
// Bibliotecas serão configuradas via PlatformIO
#include "pin_config.h"

// Variáveis globais
Config config;
Minigotchi minigotchi;

// Variáveis de display (serão inicializadas após a instalação das bibliotecas)
void* display = nullptr;
void* pmu = nullptr;

void setup() {
  Serial.begin(115200);
  delay(1000); // Aguarda o Serial estar pronto
  
  // while (!Serial); // Wait for serial connection

#ifdef PLATFORMIO
  platformio_init();
#endif

  Serial.println("\n=== Minigotchi S3 AMOLED v3 - Booting ===");

  // Inicialização básica - versão simplificada para PlatformIO
  Serial.println("Initializing basic components...");
  
  // Inicializa o I2C para sensores
  Wire.begin(D_IIC_SDA, D_IIC_SCL);

  minigotchi.boot();
}

void loop() {
  // Loop principal simplificado para PlatformIO
  minigotchi.epoch();
  minigotchi.cycle();
  minigotchi.detect();
  minigotchi.advertise();
  minigotchi.deauth();
  minigotchi.spam();
  
  delay(100); // Pequeno delay para não sobrecarregar o sistema
}