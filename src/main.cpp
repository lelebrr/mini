#include <Arduino.h>
#include <Wire.h>
#include <lvgl.h>

#include <Arduino_GFX_Library.h>
#include <ESP_IOExpander_Library.h>
#include <SensorQMI8658.hpp>
#include <TouchLib.h>
#include "esp_heap_caps.h"

#include "pin_config.h"

// Simple, clean LVGL + hardware bring-up for Waveshare ESP32-S3 Touch AMOLED.
// This main.cpp is self-contained and does not depend on the conflicted legacy
// PwnTamagotchi code.
//
// Nota: o arquivo lvgl_smoke_test.cpp fornece uma variante mínima de teste de
// LVGL. Para usá-lo, defina a macro LVGL_SMOKE_TEST no build e o setup/loop
// deste arquivo serão desativados (veja os #ifndef LVGL_SMOKE_TEST mais abaixo).

// -----------------------------------------------------------------------------
// Hardware globals
// -----------------------------------------------------------------------------
ESP_IOExpander *expander = nullptr;
SensorQMI8658 qmi;
TouchLib *touch = nullptr;

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS,      // CS
    LCD_SCLK,    // SCK
    LCD_SDIO0,   // SDIO0
    LCD_SDIO1,   // SDIO1
    LCD_SDIO2,   // SDIO2
    LCD_SDIO3    // SDIO3
);

Arduino_GFX *gfx = new Arduino_SH8601(
    bus,
    LCD_RST,     // RST (-1: controlled by IO expander)
    0,           // rotation
    false,       // IPS
    LCD_WIDTH,
    LCD_HEIGHT
);

// -----------------------------------------------------------------------------
// LVGL globals
// -----------------------------------------------------------------------------
static lv_display_t *lv_display = nullptr;
static lv_indev_t   *lv_indev   = nullptr;

static lv_color_t *buf1 = nullptr;
static lv_color_t *buf2 = nullptr;

static lv_obj_t *label_title = nullptr;
static lv_obj_t *label_imu   = nullptr;

static bool imu_ok = false;

// -----------------------------------------------------------------------------
// LVGL callbacks
// -----------------------------------------------------------------------------
static void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;

#if (LV_COLOR_16_SWAP != 0)
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1,
                              reinterpret_cast<uint16_t *>(px_map), w, h);
#else
    gfx->draw16bitRGBBitmap(area->x1, area->y1,
                            reinterpret_cast<uint16_t *>(px_map), w, h);
#endif

    lv_display_flush_ready(disp);
}

static void my_touch_read(lv_indev_t *indev, lv_indev_data_t *data) {
    (void)indev;

    if (!touch) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }

    if (touch->read()) {
        TP_Point p = touch->getPoint(0);
        data->point.x = p.x;
        data->point.y = p.y;
        data->state   = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// -----------------------------------------------------------------------------
// Initialization helpers
// -----------------------------------------------------------------------------
static void initIOExpander() {
    Serial.println("[IO] Initializing TCA9554 IO expander...");

    expander = new ESP_IOExpander_TCA95xx_8bit(
        (i2c_port_t)0,
        ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000,
        IIC_SCL,
        IIC_SDA
    );
    expander->init();
    expander->begin();

    // Pin mapping (board specific, inferred from demos):
    // 0: LCD_RST
    // 1: TOUCH_RST
    // 2: peripheral power
    // 6: audio power
    expander->pinMode(0, OUTPUT);
    expander->pinMode(1, OUTPUT);
    expander->pinMode(2, OUTPUT);
    expander->pinMode(6, OUTPUT);

    expander->digitalWrite(0, LOW);
    expander->digitalWrite(1, LOW);
    expander->digitalWrite(2, LOW);
    expander->digitalWrite(6, LOW);
    delay(20);
    expander->digitalWrite(0, HIGH);
    expander->digitalWrite(1, HIGH);
    expander->digitalWrite(2, HIGH);
    expander->digitalWrite(6, HIGH);

    Serial.println("[IO] IO expander ready.");
}

static void initDisplay() {
    Serial.println("[DISPLAY] Initializing SH8601 display...");

    if (!gfx->begin()) {
        Serial.println("[DISPLAY] gfx->begin() failed!");
        return;
    }

    gfx->fillScreen(BLACK);
    gfx->Display_Brightness(200);

    Serial.printf("[DISPLAY] Resolution: %d x %d\n",
                  gfx->width(), gfx->height());
}

static void initTouch() {
    Serial.println("[TOUCH] Initializing FT3168 touch...");

    touch = new TouchLib(Wire, IIC_SDA, IIC_SCL, FT3168_DEVICE_ADDRESS);
    if (!touch->init()) {
        Serial.println("[TOUCH] Touch init failed.");
    } else {
        Serial.println("[TOUCH] Touch ready.");
    }
}

static void initIMU() {
    Serial.println("[IMU] Initializing QMI8658...");

    if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
        Serial.println("[IMU] QMI8658 not found.");
        imu_ok = false;
        return;
    }

    qmi.configAccelerometer(
        SensorQMI8658::ACC_RANGE_4G,
        SensorQMI8658::ACC_ODR_1000Hz,
        SensorQMI8658::LPF_MODE_0
    );
    qmi.enableAccelerometer();
    imu_ok = true;

    Serial.println("[IMU] QMI8658 ready.");
}

static void initLVGL() {
    Serial.println("[LVGL] Initializing LVGL...");

    lv_init();

    uint32_t buffer_pixels = (LCD_WIDTH * LCD_HEIGHT) / 10;
    buf1 = static_cast<lv_color_t *>(
        heap_caps_malloc(buffer_pixels * sizeof(lv_color_t),
                         MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA));
    buf2 = static_cast<lv_color_t *>(
        heap_caps_malloc(buffer_pixels * sizeof(lv_color_t),
                         MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA));

    if (!buf1 || !buf2) {
        Serial.println("[LVGL] Failed to allocate draw buffers.");
        while (true) {
            delay(1000);
        }
    }

    lv_display = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_flush_cb(lv_display, my_disp_flush);
    lv_display_set_buffers(
        lv_display,
        buf1,
        buf2,
        buffer_pixels * sizeof(lv_color_t),
        LV_DISPLAY_RENDER_MODE_PARTIAL
    );

    lv_indev = lv_indev_create();
    lv_indev_set_type(lv_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(lv_indev, my_touch_read);

    // Simple UI: title + IMU info
    lv_obj_t *scr = lv_screen_active();

    label_title = lv_label_create(scr);
    lv_label_set_text(label_title, "Waveshare ESP32-S3 AMOLED");
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 10);

    label_imu = lv_label_create(scr);
    lv_label_set_text(label_imu, "IMU: ---");
    lv_obj_align(label_imu, LV_ALIGN_BOTTOM_MID, 0, -10);

    Serial.println("[LVGL] LVGL ready.");
}

// -----------------------------------------------------------------------------
// Arduino setup/loop
// -----------------------------------------------------------------------------
// A versão padrão usa IMU, touch etc. Para um teste mínimo de LVGL, use
// lvgl_smoke_test.cpp com a macro LVGL_SMOKE_TEST definida no build.
#ifndef LVGL_SMOKE_TEST
void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println();
    Serial.println("=== Waveshare ESP32-S3 AMOLED minimal LVGL demo ===");

    // I2C for all I2C devices
    Wire.begin(IIC_SDA, IIC_SCL);

    initIOExpander();
    initDisplay();
    initTouch();
    initIMU();
    initLVGL();

    Serial.println("[MAIN] Setup complete.");
}

void loop() {
    static uint32_t last_tick_ms = 0;
    uint32_t now = millis();

    // LVGL tick + handler
    uint32_t elapsed = now - last_tick_ms;
    if (elapsed >= 5) {
        lv_tick_inc(elapsed);
        last_tick_ms = now;
        lv_timer_handler();
    }

    // Periodically read IMU and update label
    static uint32_t last_imu_ms = 0;
    if (imu_ok && (now - last_imu_ms) > 200) {
        last_imu_ms = now;

        if (qmi.getDataReady()) {
            float ax, ay, az;
            if (qmi.getAccelerometer(ax, ay, az)) {
                char buf[64];
                snprintf(buf, sizeof(buf),
                         "AX: %.2f  AY: %.2f  AZ: %.2f", ax, ay, az);
                lv_label_set_text(label_imu, buf);
            }
        }
    }

    delay(5);
}
#endif  // LVGL_SMOKE_TEST