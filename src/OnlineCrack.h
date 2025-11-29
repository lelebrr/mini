#ifndef ONLINE_CRACK_H
#define ONLINE_CRACK_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <SD_MMC.h>
#include "FS.h"

struct WiFiConfig {
    String ssid;
    String pass;
    String wpa_sec_key;
};

class OnlineCrack {
public:
    // Carrega SSID, senha e chave do wpa-sec de /wifi_config.txt
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
                    else if (line.startsWith("KEY="))  cfg.wpa_sec_key = line.substring(4);
                }
                f.close();
            }
        }
        return cfg;
    }

    // Envia todos os arquivos .pcap/.hccapx de /upload_queue (ou /handshakes) para wpa-sec
    static int uploadAll() {
        WiFiConfig cfg = loadConfig();

        if (cfg.ssid.isEmpty() || cfg.wpa_sec_key.isEmpty()) {
            Serial.println("[OnlineCrack] Configure SSID/PASS/KEY em /wifi_config.txt");
            return -1;
        }

        Serial.printf("[OnlineCrack] Conectando a %s...\n", cfg.ssid.c_str());
        WiFi.begin(cfg.ssid.c_str(), cfg.pass.c_str());

        int retries = 0;
        while (WiFi.status() != WL_CONNECTED && retries < 20) {
            delay(500);
            Serial.print(".");
            retries++;
        }
        Serial.println();

        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[OnlineCrack] Falha ao conectar WiFi");
            return -2;
        }

        File root = SD_MMC.open("/upload_queue");
        if (!root) {
            root = SD_MMC.open("/handshakes");
        }
        if (!root) {
            Serial.println("[OnlineCrack] Nenhuma pasta /upload_queue ou /handshakes encontrada");
            WiFi.disconnect(true);
            return 0;
        }

        int count = 0;
        File file = root.openNextFile();
        while (file) {
            if (!file.isDirectory()) {
                String filename = String(file.name());
                if (filename.endsWith(".pcap") || filename.endsWith(".hccapx")) {
                    if (uploadFile(file, cfg.wpa_sec_key)) {
                        Serial.printf("[OnlineCrack] SUCESSO: %s enviado\n", filename.c_str());
                        count++;
                    } else {
                        Serial.printf("[OnlineCrack] FALHA: %s\n", filename.c_str());
                    }
                }
            }
            file = root.openNextFile();
        }

        WiFi.disconnect(true);
        return count;
    }

private:
    static bool uploadFile(File& file, const String &key) {
        WiFiClientSecure client;
        client.setInsecure(); // ignora certificado

        if (!client.connect("wpa-sec.stanev.org", 443)) {
            Serial.println("[OnlineCrack] Falha ao conectar em wpa-sec.stanev.org");
            return false;
        }

        String boundary = "----MiniLeleBoundary";
        String fileName = String(file.name());
        size_t fileSize = file.size();

        String head = "--" + boundary +
                      "\r\nContent-Disposition: form-data; name=\"key\"\r\n\r\n" +
                      key + "\r\n";
        String fileHead = "--" + boundary +
                          "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"" +
                          fileName + "\"\r\nContent-Type: application/octet-stream\r\n\r\n";
        String tail = "\r\n--" + boundary + "--\r\n";

        size_t totalLen = head.length() + fileHead.length() + fileSize + tail.length();

        client.println("POST /?api&dl=1 HTTP/1.1");
        client.println("Host: wpa-sec.stanev.org");
        client.println("Connection: close");
        client.println("Content-Type: multipart/form-data; boundary=" + boundary);
        client.print("Content-Length: ");
        client.println(totalLen);
        client.println();

        client.print(head);
        client.print(fileHead);

        uint8_t buf[512];
        while (file.available()) {
            size_t read = file.read(buf, sizeof(buf));
            if (read == 0) break;
            client.write(buf, read);
        }

        client.print(tail);

        long timeout = millis();
        while (client.connected() && !client.available()) {
            if (millis() - timeout > 10000) {
                Serial.println("[OnlineCrack] Timeout aguardando resposta");
                client.stop();
                return false;
            }
            delay(10);
        }

        bool success = false;
        while (client.available()) {
            String line = client.readStringUntil('\n');
            if (line.indexOf("HTTP/1.1 200") >= 0) {
                success = true;
            }
        }

        client.stop();
        return success;
    }
};

#endif
