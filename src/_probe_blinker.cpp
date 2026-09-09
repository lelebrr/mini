#include <Arduino.h>
#include "USB.h"
#include "USBCDC.h"
// Sonda v4 (env:probe) — ARQUIVO TEMPORÁRIO.
// Troca o console p/ USB-OTG (GPIO19/20, USBCDC) em vez do USB-Serial/JTAG.
// Se esta sonda FALAR e a v3 ficou muda, o problema é o caminho HWCDC/host.
USBCDC USBSerial;
void setup() {
    USBSerial.begin();
    USB.begin();
    uint32_t t0 = millis();
    while (!USBSerial && (millis() - t0) < 4000) { delay(50); }
    delay(300);
    USBSerial.println();
    USBSerial.println("[PROBE4-OTG] boot ok via USB-OTG (GPIO19/20)");
    USBSerial.printf("[PROBE4-OTG] chip=%s rev=%d cpu=%luMHz\n",
                     ESP.getChipModel(), ESP.getChipRevision(),
                     (unsigned long)ESP.getCpuFreqMHz());
    USBSerial.flush();
}
void loop() {
    static uint32_t n = 0;
    USBSerial.printf("[PROBE4-OTG] tick %lu millis=%lu\n",
                     (unsigned long)n++, (unsigned long)millis());
    USBSerial.flush();
    delay(500);
}
