#ifndef ONLINE_CRACK_H
#define ONLINE_CRACK_H

/**
 * OnlineCrack.h
 * Upload manual de capturas para wpa-sec.stanev.org.
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
        cfg.ssid        = "";
        cfg.pass        = "";
        cfg.wpa_sec_key = "";

        // Mantém compatibilidade com o script: /arquivos_cartao_sd/wifi_config.txt
        if (SD_MMC.exists("/arquivos_cartao_sd/wifi_config.txt")) {
            File f = SD_MMC.open("/arquivos_cartao_sd/wifi_config.txt");
            if (f) {
                while (f.available()) {
                    String line = f.readStringUntil('\n');
                    line.trim();
                    if (line.startsWith("SSID="))      cfg.ssid        = line.substring(5);
                    else if (line.startsWith("PASS="))  cfg.pass        = line.substring(5);
                    else if (line.startsWith("KEY="))   cfg.wpa_sec_key = line.substring(4);
                }
                f.close();
            }
        }
        return cfg;
    }

    static int uploadAll() {
        WiFiConfig cfg = loadConfig();

        if (cfg.ssid.isEmpty() || cfg.wpa_sec_key.isEmpty()) {
            Serial.println("[Upload] Erro: Configure /arquivos_cartao_sd/wifi_config.txt (SSID/PASS/KEY).");
            return -1;
        }

        WiFi.begin(cfg.ssid.c_str(), cfg.pass.c_str());
        int retries = 0;
        while (WiFi.status() != WL_CONNECTED && retries < 20) {
            delay(500);
            retries++;
        }
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[Upload] Não conectou ao WiFi.");
            return -2;
        }

        // Fila de envio (novo layout na raiz do SD)
        File root = SD_MMC.open("/fila_envio");
        if (!root) {
            // Fallback: diretório de capturas
            root = SD_MMC.open("/capturas");
        }
        if (!root) {
            Serial.println("[Upload] Nenhum diretório /fila_envio ou /capturas encontrado.");
            WiFi.disconnect();
            return 0;
        }

        int count = 0;
        File file = root.openNextFile();
        while (file) {
            if (!file.isDirectory()) {
                String filename = String(file.name());
                if (filename.endsWith(".pcap") || filename.endsWith(".hccapx")) {
                    Serial.printf("[Upload] Enviando %s\n", filename.c_str());
                    if (uploadFile(file, cfg.wpa_sec_key)) {
                        count++;
                    }
                }
            }
            file = root.openNextFile();
        }

        root.close();
        WiFi.disconnect();
        return count;
    }

private:
    static bool uploadFile(File &file, const String &key) {
        WiFiClientSecure client;
        client.setInsecure();

        if (!client.connect("wpa-sec.stanev.org", 443)) {
            Serial.println("[Upload] Conexão TLS falhou.");
            return false;
        }

        String boundary = "------------------------Esp32Boundary";
        String fileName = String(file.name());
        size_t fileSize = file.size();

        String head =
            "--" + boundary +
            "\r\nContent-Disposition: form-data; name=\"key\"\r\n\r\n" +
            key + "\r\n";

        String fileHead =
            "--" + boundary +
            "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"" +
            fileName + "\"\r\nContent-Type: application/octet-stream\r\n\r\n";

        String tail = "\r\n--" + boundary + "--\r\n";

        size_t totalLen = head.length() + fileHead.length() + fileSize + tail.length();

        client.println("POST /?api&dl=1 HTTP/1.1");
        client.println("Host: wpa-sec.stanev.org");
        client.println("Content-Type: multipart/form-data; boundary=" + boundary);
        client.print("Content-Length: ");
        client.println(totalLen);
        client.println("Connection: close");
        client.println();

        client.print(head);
        client.print(fileHead);

        uint8_t buf[512];
        while (file.available()) {
            size_t read = file.read(buf, sizeof(buf));
            client.write(buf, read);
        }

        client.print(tail);

        long timeout = millis();
        while (client.connected() && !client.available()) {
            if (millis() - timeout > 10000) {
                Serial.println("[Upload] Timeout aguardando resposta.");
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
        Serial.println(success ? "[Upload] OK" : "[Upload] Falha");
        return success;
    }
};

#endif
