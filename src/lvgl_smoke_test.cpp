#include <Arduino.h>
#include <Wire.h>
#include <lvgl.h>

#include <Arduino_GFX_Library.h>
#include "drivers/TCA9554.h"   // expansor TCA9554 próprio (via Wire/driver_ng)
#include "esp_heap_caps.h"

#include "pin_config.h"

// Teste mínimo de LVGL 9 para o Waveshare ESP32-S3 Touch AMOLED.
// Inicializa apenas:
//   - IO Expander (para energizar periféricos / reset)
//   - Display (via Arduino_GFX)
//   - LVGL com um único label
//
// Para usar este teste, defina LVGL_SMOKE_TEST no build (por exemplo,
// em platformio.ini) para desativar o setup/loop do main.cpp e ativar
// apenas este arquivo.

// -----------------------------------------------------------------------------
// Globais de hardware (mínimas para display)
// -----------------------------------------------------------------------------
static TCA9554 smoke_expander;

static Arduino_DataBus *smoke_bus = new Arduino_ESP32QSPI(
    LCD_CS,      // CS
    LCD_SCLK,    // SCK
    LCD_SDIO0,   // SDIO0
    LCD_SDIO1,   // SDIO1
    LCD_SDIO2,   // SDIO2
    LCD_SDIO3    // SDIO3
);

static Arduino_GFX *smoke_gfx = new Arduino_SH8601(
    smoke_bus,
    LCD_RST,     // RST (-1: via IO expander)
    0,           // rotation
    LCD_WIDTH,
    LCD_HEIGHT
);
// Construtor da GFX Library >= 1.6.x: (bus, rst, rotation, w, h, offsets...).
// O parâmetro 'ips' não existe mais; a chamada antiga passava 'false' como
// largura e LCD_HEIGHT como col_offset (truncado de 448 para 192).

// -----------------------------------------------------------------------------
// Globais LVGL
// -----------------------------------------------------------------------------
static lv_display_t *smoke_display = nullptr;
static lv_color_t *smoke_buf1 = nullptr;
static lv_color_t *smoke_buf2 = nullptr;

// -----------------------------------------------------------------------------
// Callback de flush para LVGL 9
// -----------------------------------------------------------------------------
static void smoke_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;

#if (LV_COLOR_16_SWAP != 0)
    smoke_gfx->draw16bitBeRGBBitmap(
        area->x1, area->y1,
        reinterpret_cast<uint16_t *>(px_map),
        w, h
    );
#else
    smoke_gfx->draw16bitRGBBitmap(
        area->x1, area->y1,
        reinterpret_cast<uint16_t *>(px_map),
        w, h
    );
#endif

    lv_display_flush_ready(disp);
}

// -----------------------------------------------------------------------------
// Inicializações mínimas
// -----------------------------------------------------------------------------
static void smoke_init_ioexpander() {
    Serial.println("[SMOKE][IO] Initializing TCA9554 IO expander...");

    // Driver próprio (drivers/TCA9554.h) via Wire (driver_ng). A lib externa
    // ESP32_IO_Expander usava o driver I2C LEGADO (i2c_driver_install) e
    // conflitava com o Wire -> abort() no boot.
    bool ok = false;
    for (int attempt = 1; attempt <= 3 && !ok; ++attempt) {
        ok = smoke_expander.begin(Wire, TCA9554_ADDR);
        if (!ok) { delay(10); }
    }
    if (!ok) {
        Serial.println("[SMOKE][IO] TCA9554 NÃO respondeu no I2C!");
    }

    // Pinos principais: 0/LCD_RST, 1/TOUCH_RST, 2/power, 6/audio power
    for (uint8_t p : {0, 1, 2, 6}) smoke_expander.pinMode(p, OUTPUT);

    smoke_expander.digitalWrite(0, LOW);
    smoke_expander.digitalWrite(1, LOW);
    smoke_expander.digitalWrite(2, LOW);
    smoke_expander.digitalWrite(6, LOW);
    delay(20);
    smoke_expander.digitalWrite(0, HIGH);
    smoke_expander.digitalWrite(1, HIGH);
    smoke_expander.digitalWrite(2, HIGH);
    smoke_expander.digitalWrite(6, HIGH);

    Serial.println("[SMOKE][IO] IO expander ready.");
}

static void smoke_init_display() {
    Serial.println("[SMOKE][DISPLAY] Initializing SH8601 display...");

    if (!smoke_gfx->begin()) {
        Serial.println("[SMOKE][DISPLAY] gfx->begin() failed!");
        return;
    }

    smoke_gfx->fillScreen(RGB565_BLACK);
    static_cast<Arduino_OLED *>(smoke_gfx)->setBrightness(200);

    Serial.printf("[SMOKE][DISPLAY] Resolution: %d x %d\n",
                  smoke_gfx->width(), smoke_gfx->height());
}

static void smoke_init_lvgl() {
    Serial.println("[SMOKE][LVGL] Initializing LVGL...");

    lv_init();

    // Buffer pequeno, suficiente para validar pipeline (1/10 da tela)
    uint32_t buffer_pixels = (LCD_WIDTH * LCD_HEIGHT) / 10;

    smoke_buf1 = static_cast<lv_color_t *>(
        heap_caps_malloc(buffer_pixels * sizeof(lv_color_t),
                         MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    smoke_buf2 = static_cast<lv_color_t *>(
        heap_caps_malloc(buffer_pixels * sizeof(lv_color_t),
                         MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));

    if (!smoke_buf1 || !smoke_buf2) {
        // PSRAM não é DMA-capable no S3; se falhar, reinicia (não trava o boot).
        Serial.println("[SMOKE][LVGL] Failed to allocate draw buffers. Restarting...");
        delay(3000);
        ESP.restart();
    }

    smoke_display = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_flush_cb(smoke_display, smoke_disp_flush);
    lv_display_set_buffers(
        smoke_display,
        smoke_buf1,
        smoke_buf2,
        buffer_pixels * sizeof(lv_color_t),
        LV_DISPLAY_RENDER_MODE_PARTIAL
    );

    // Tela mínima: um label central
    lv_obj_t *scr = lv_screen_active();
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "LVGL 9 Smoke Test");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    Serial.println("[SMOKE][LVGL] LVGL ready.");
}

// -----------------------------------------------------------------------------
// Arduino setup/loop – apenas ativa quando LVGL_SMOKE_TEST está definido
// -----------------------------------------------------------------------------
#ifdef LVGL_SMOKE_TEST
void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println();
    Serial.println("=== LVGL 9 Smoke Test (minimal) ===");

    // I2C principal
    Wire.begin(IIC_SDA, IIC_SCL);

    smoke_init_ioexpander();
    smoke_init_display();
    smoke_init_lvgl();

    Serial.println("[SMOKE][MAIN] Setup complete.");
}

void loop() {
    static uint32_t last_tick_ms = 0;
    uint32_t now = millis();

    uint32_t elapsed = now - last_tick_ms;
    if (elapsed >= 5) {
        lv_tick_inc(elapsed);
        last_tick_ms = now;
        lv_timer_handler();
    }

    delay(5);
}
#endif  // LVGL_SMOKE_TEST