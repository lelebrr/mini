#include "core/PwnBLE.h"
#include "BleBlue.h"
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

static BLEScan* pScan = nullptr;
static std::vector<String> device_names;
static std::vector<String> device_addrs;
static std::vector<int> device_rssi;

class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice dev) override {
        PwnBLE::total_seen++;
        // BLUE: alimenta o parser/inventário BLE com o AD cru.
        uint8_t addr[6];
        BLEAddress ba = dev.getAddress();
        memcpy(addr, ba.getNative(), 6);
        const uint8_t* pl = dev.getPayload();
        size_t pll = dev.getPayloadLength();
        if (pl && pll)
            BleBlue::onAdv(pl, (uint16_t)pll, addr,
                           (uint8_t)dev.getAddressType(), (int8_t)dev.getRSSI());
    }
};

void PwnBLE::init() {
    BLEDevice::init("MiniLele");
    pScan = BLEDevice::getScan();
    pScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
    last_count = 0;
    total_seen = 0;
    BleBlue::init();
    Serial.println("[PwnBLE] BLE inicializado");
}

void PwnBLE::deinit() {
    BLEDevice::deinit();
}

int PwnBLE::scan(int timeout_sec) {
    if (!pScan) PwnBLE::init();   // init preguiçoso
    if (!pScan) return 0;
    device_names.clear();
    device_addrs.clear();
    device_rssi.clear();

    BLEScanResults* results = pScan->start(timeout_sec, false);
    last_count = results->getCount();

    for (int i = 0; i < last_count && i < 50; i++) {
        BLEAdvertisedDevice dev = results->getDevice(i);
        device_names.push_back(dev.getName().c_str());
        device_addrs.push_back(dev.getAddress().toString().c_str());
        device_rssi.push_back(dev.getRSSI());
    }
    pScan->clearResults();
    return last_count;
}

int PwnBLE::getDeviceCount() { return last_count; }

String PwnBLE::getDeviceName(int index) {
    if (index >= 0 && index < (int)device_names.size()) return device_names[index];
    return "";
}

String PwnBLE::getDeviceAddress(int index) {
    if (index >= 0 && index < (int)device_addrs.size()) return device_addrs[index];
    return "";
}

int PwnBLE::getDeviceRSSI(int index) {
    if (index >= 0 && index < (int)device_rssi.size()) return device_rssi[index];
    return 0;
}

void PwnBLE::clear() {
    device_names.clear();
    device_addrs.clear();
    device_rssi.clear();
    last_count = 0;
}
