#ifndef ONLINE_CRACK_H
#define ONLINE_CRACK_H

#include <Arduino.h>
#include <WiFi.h>
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
#include <HTTPClient.h>
#include <SD_MMC.h>
#include "FS.h"

// Simple config loader for WiFi
<<<<<<< HEAD
=======
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
#include <WiFiClientSecure.h>
#include <SD_MMC.h>
#include "FS.h"

// Config loader
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
struct WiFiConfig {
    String ssid;
    String pass;
    String wpa_sec_key;
};

class OnlineCrack {
public:
    static WiFiConfig loadConfig() {
        WiFiConfig cfg;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        cfg.ssid = "SSID_AQUI";
        cfg.pass = "SENHA_AQUI";
        cfg.wpa_sec_key = "KEY_AQUI";
=======
        cfg.ssid = "";
        cfg.pass = "";
        cfg.wpa_sec_key = "";
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
        cfg.ssid = "SSID_AQUI";
        cfg.pass = "SENHA_AQUI";
        cfg.wpa_sec_key = "KEY_AQUI";
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
        cfg.ssid = "";
        cfg.pass = "";
        cfg.wpa_sec_key = "";
>>>>>>> origin/waveshare-s3-amoled-review-complete

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

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
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
<<<<<<< HEAD
=======
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
    static int uploadAll() {
        WiFiConfig cfg = loadConfig();

        if (cfg.ssid == "" || cfg.wpa_sec_key == "") {
            Serial.println("Erro: Configure SSID e KEY em /wifi_config.txt");
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
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

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        Serial.println("\nWiFi Conectado! Iniciando Upload...");

        File root = SD_MMC.open("/upload_queue");
=======
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
        Serial.println("\nWiFi Conectado! Iniciando Real Upload (wpa-sec)...");

        File root = SD_MMC.open("/upload_queue");
        if (!root) {
             root = SD_MMC.open("/handshakes"); // Fallback to handshakes folder if queue empty
        }

<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
        Serial.println("\nWiFi Conectado! Iniciando Upload...");

        File root = SD_MMC.open("/upload_queue");
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
        File file = root.openNextFile();
        int count = 0;

        while (file) {
            if (!file.isDirectory()) {
                String filename = String(file.name());
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
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
<<<<<<< HEAD
=======
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
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
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
            }
            file = root.openNextFile();
        }

        WiFi.disconnect();
        return count;
    }
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete

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
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-manual-upload
=======
>>>>>>> origin/waveshare-s3-amoled-review-complete
};

#endif
