#include <Arduino.h>
#include <lvgl.h>
// Core Headers
#include "core/PwnPet.h"
#include "core/PwnAttack.h"
#include "core/PwnUI.h"
#include "core/PwnVoice.h"
// Drivers
#include "pin_config.h"
#include "XPowersLib.h"
#include "ESP_IOExpander_Library.h"
#include <SD_MMC.h>
#include "WiFiTools.h"

// Globals
XPowersPMU power;
ESP_IOExpander *expander;
SemaphoreHandle_t gui_mutex;

void setup() {
    Serial.begin(115200);
    gui_mutex = xSemaphoreCreateMutex();

    // 1. Power
    Wire.begin(IIC_SDA, IIC_SCL);
    power.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
    power.enableALDO1(); power.enableALDO2(); power.enableALDO3(); power.enableALDO4();
    power.enableBLDO1(); power.enableBLDO2();

    // 2. IO Expander
    expander = new ESP_IOExpander_TCA95xx_8bit((i2c_port_t)0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, IIC_SCL, IIC_SDA);
    expander->init(); expander->begin();
    expander->pinMode(0, OUTPUT); expander->digitalWrite(0, HIGH); // LCD RST

    // 3. SD Card
    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
    SD_MMC.begin("/sdcard", true);

    // 4. Core Init
    PwnPet::init();
    PwnAttack::init();

    // 5. UI Init
    // (Assume LVGL driver setup code is reused from previous main.cpp snippet here for brevity,
    // real impl would strictly copy the display/touch driver code)
    // ... Display Driver Init ...
    lv_init();
    // ... Buffer Init ...

    PwnUI::init();

    Serial.println("PwnTamagotchi BR Iniciado!");
}

void loop() {
    // 1. LVGL Logic
    if (xSemaphoreTake(gui_mutex, 5)) {
        lv_timer_handler();
        PwnUI::update();
        xSemaphoreGive(gui_mutex);
    }

    // 2. Game Logic
    PwnPet::tick();

    // 3. Attack Logic
    PwnAttack::tick();

    delay(5);
}
