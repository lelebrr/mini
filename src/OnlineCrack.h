#ifndef ONLINE_CRACK_H
#define ONLINE_CRACK_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SD_MMC.h>
#include "FS.h"

// Simple config loader for WiFi
struct WiFiConfig {
    String ssid;
    String pass;
    String wpa_sec_key;
};

class OnlineCrack {
public:
    static WiFiConfig loadConfig() {
        WiFiConfig cfg;
        cfg.ssid = "SSID_AQUI";
        cfg.pass = "SENHA_AQUI";
        cfg.wpa_sec_key = "KEY_AQUI";

        if (SD_MMC.exists("/wifi_config.txt")) {
            File f = SD_MMC.open("/wifi_config.txt");
            if (f) {
                while (f.available()) {
                    String line = f.readStringUntil('\n');
                    line.trim();
                    if (line.startsWith("SSID=")) cfg.ssid = line.substring(5);
                    else if (line.startsWith("PASS=")) cfg.pass = line.substring(5);
                    else if (line.startsWith("KEY=")) cfg.wpa_sec_key = line.substring(4);
                }
                f.close();
            }
        }
        return cfg;
    }

    // Move a captured handshake/log to upload queue
    static void queueFile(String path) {
        // In a real scenario, this moves .pcap or .hccapx files
        // Here we just copy/rename to simulate queuing
        String fileName = path.substring(path.lastIndexOf('/') + 1);
        String dest = "/upload_queue/" + fileName;

        // Copy logic (SD_MMC doesn't support move directly easily across dirs sometimes, copy+delete)
        File src = SD_MMC.open(path);
        if (src) {
            File dst = SD_MMC.open(dest, FILE_WRITE);
            while (src.available()) dst.write(src.read());
            dst.close();
            src.close();
            // SD_MMC.remove(path); // Keep original for backup?
            Serial.printf("Crack queued: %s\n", dest.c_str());
        }
    }

    static int uploadAll() {
        WiFiConfig cfg = loadConfig();

        if (cfg.ssid == "SSID_AQUI") {
            Serial.println("Erro: Configure /wifi_config.txt no SD");
            return -1;
        }

        Serial.printf("Conectando a %s...\n", cfg.ssid.c_str());
        WiFi.begin(cfg.ssid.c_str(), cfg.pass.c_str());

        int retries = 0;
        while (WiFi.status() != WL_CONNECTED && retries < 20) {
            delay(500);
            Serial.print(".");
            retries++;
        }

        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("\nFalha no WiFi");
            return -2;
        }

        Serial.println("\nWiFi Conectado! Iniciando Upload...");

        File root = SD_MMC.open("/upload_queue");
        File file = root.openNextFile();
        int count = 0;

        while (file) {
            if (!file.isDirectory()) {
                String filename = String(file.name());
                // Upload logic for wpa-sec (POST)
                // This assumes standard wpa-sec API: https://wpa-sec.stanev.org/?help
                // curl -X POST -F [email protected] -F key=... https://wpa-sec.stanev.org/

                HTTPClient http;
                String url = "https://wpa-sec.stanev.org/?api&dl=1"; // Example endpoint

                // Real upload would need Multipart form data (simulated here)
                // For simplicity/demo, we just ping sending "Simulated Upload"
                http.begin("http://google.com"); // Ping to prove connection
                int code = http.GET();

                if (code > 0) {
                    Serial.printf("Arquivo %s 'enviado' (Simulado, HTTP %d)\n", filename.c_str(), code);
                    // Move to /uploaded/ or delete
                    // SD_MMC.remove(file.path());
                    count++;
                } else {
                    Serial.printf("Erro envio %s\n", filename.c_str());
                }
                http.end();
            }
            file = root.openNextFile();
        }

        WiFi.disconnect();
        return count;
    }
};

#endif
