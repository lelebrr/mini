#ifndef PWN_BLE_H
#define PWN_BLE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <vector>

/**
 * PwnBLE
 * ------
 * Scanner Bluetooth LE usando a pilha BLE embutida no core arduino-esp32 3.x
 * (não precisa de biblioteca extra). Faz varreduras SOB DEMANDA e libera a
 * memória do controlador BT entre as varreduras (init/deinit), para conviver
 * bem com o Wi-Fi (AP) na mesma rádio.
 *
 * Uso: os dispositivos BLE encontrados viram "comida" extra para o pet.
 * Tudo é opcional (config `atk_ble_scan`) e seguro: se algo falhar, retorna 0.
 */
class PwnBLE {
private:
    static int last_count;
    static int total_seen;
    inline static std::vector<String> last_list;

public:
    // Varredura passiva de `seconds` segundos. Retorna nº de dispositivos.
    static int scan(int seconds = 3) {
        int n = 0;
        BLEDevice::init("");
        BLEScan *s = BLEDevice::getScan();
        if (s) {
            s->setActiveScan(false);   // passivo = mais leve, menos intrusivo
            s->setInterval(100);
            s->setWindow(80);
            BLEScanResults *r = s->start(seconds, false);
            last_list.clear();
            if (r) {
                n = r->getCount();
                for (int i = 0; i < n; i++) {
                    BLEAdvertisedDevice d = r->getDevice(i);
                    String line = String(d.getAddress().toString().c_str());
                    if (d.haveName()) line += "  " + String(d.getName().c_str());
                    line += "  " + String(d.getRSSI()) + "dBm";
                    last_list.push_back(line);
                }
            }
            s->clearResults();
        }
        BLEDevice::deinit(true);        // libera RAM/coexistência
        last_count = n;
        total_seen += n;
        Serial.printf("[BLE] Varredura: %d dispositivos.\n", n);
        return n;
    }

    static String getDevicesText() {
        String out;
        for (auto &l : last_list) out += l + "\n";
        if (last_list.empty()) out = "(toque ESCANEAR)";
        return out;
    }
    static int getLastCount() { return last_count; }
    static int getTotalSeen() { return total_seen; }
};

// Definições estáticas em core_singletons.cpp
#endif
