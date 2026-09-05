#ifndef WEB_HANDLER_H
#define WEB_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <Update.h>
#include <SD_MMC.h>

#include "core/ConfigManager.h"
#include "WebAssets.h"
#include "core/PwnPet.h"
#include "core/PwnAttack.h"
#include "core/PwnPower.h"
#include "core/PwnBLE.h"
#include "WiFiTools.h"
#include "drivers/PwnRTC.h"

class WebHandler {
private:
    static AsyncWebServer  server;
    static AsyncWebSocket  ws;
    static ConfigManager  *config;
    static DNSServer       dns;
    static bool            dns_active;

public:
    static void init() {
        config = ConfigManager::getInstance();

        // Setup WiFi a partir da config
        String mode = config->getString("sys_wifi_mode");
        if (mode == "AP" || mode == "AP_STA") {
            WiFi.softAP(
                config->getString("sys_ap_ssid").c_str(),
                config->getString("sys_ap_pass").c_str()
            );
            Serial.print("[Web] AP Started: ");
            Serial.println(WiFi.softAPIP());
            // Portal cativo: qualquer domínio cai na WebUI (facilita 1ª config).
            dns.start(53, "*", WiFi.softAPIP());
            dns_active = true;
        }
        if (mode == "STA" || mode == "AP_STA") {
            WiFi.begin(
                config->getString("sys_sta_ssid").c_str(),
                config->getString("sys_sta_pass").c_str()
            );
            Serial.print("[Web] Connecting STA...");
        }

        // Página principal
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!checkAuth(request)) return request->requestAuthentication();
            request->send(200, "text/html", index_html);
        });

        // API: Config Get (senhas mascaradas)
        server.on("/api/config", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!checkAuth(request)) return request->requestAuthentication();
            String json;
            config->getJSONRedacted(json);
            request->send(200, "application/json", json);
        });

        // API: Config Set
        server.on(
            "/api/config",
            HTTP_POST,
            [](AsyncWebServerRequest *request) {},
            nullptr,
            [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
                if (!checkAuth(request)) return request->requestAuthentication();
                String json;
                json.reserve(len + 1);
                for (size_t i = 0; i < len; i++) {
                    json += static_cast<char>(data[i]);
                }
                config->updateFromJSON(json);
                request->send(200, "text/plain", "Config Updated");
            }
        );

        // API: Status
        // (Serializa com ArduinoJson direto para uma String — evita depender da
        //  API do AsyncJsonResponse, que varia entre versões do ESPAsyncWebServer.)
        server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
            JsonDocument doc;

            PetStats pet = PwnPet::getStats();
            doc["pet_name"]      = pet.name;
            doc["pet_hunger"]    = pet.hunger;
            doc["pet_happiness"] = pet.happiness;
            doc["pet_level"]     = pet.level;
            doc["pet_xp"]        = pet.xp;
            doc["pet_stage"]     = (int)pet.stage;
            doc["pet_mood"]      = PwnPet::getMoodStr();
            doc["pet_handshakes"] = pet.handshakes_total;

            doc["pwr_battery"] = PwnPower::getBatteryPercent();
            doc["pwr_current"] = PwnPower::getSystemCurrent();
            doc["pwr_voltage"] = PwnPower::getBatteryVoltage();
            doc["pwr_charging"] = PwnPower::isCharging();
            doc["pwr_vbus"]     = PwnPower::isVbusIn();
            doc["pwr_charge_status"] = PwnPower::getChargeStatusStr();
            doc["pwr_hours"]    = PwnPower::getEstimatedHours();
            doc["pwr_capacity"] = PwnPower::getBatteryCapacity();
            doc["wifi_mac"]    = WiFi.macAddress();
            doc["wifi_devices"] = (int)WiFiTools::nearby_devices.size();
            doc["wifi_handshakes"] = (uint32_t)WiFiTools::getHandshakeCount();
            doc["ble_devices"] = PwnBLE::getLastCount();
            doc["rtc_time"]    = PwnRTC::getTimestamp();
            doc["heap_free"]   = ESP.getFreeHeap();
            doc["uptime"]      = (uint32_t)(millis() / 1000);

            String out; serializeJson(doc, out);
            request->send(200, "application/json", out);
        });

        // API: File Manager (Listagem)
        server.on("/api/files", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!checkAuth(request)) return request->requestAuthentication();
            String path = request->hasParam("path") ? request->getParam("path")->value() : "/";

            JsonDocument doc;
            JsonArray root = doc.to<JsonArray>();

            File dir = SD_MMC.open(path);
            if (dir) {
                File file = dir.openNextFile();
                while (file) {
                    JsonObject item = root.add<JsonObject>();
                    item["name"]  = String(file.name());
                    item["size"]  = file.size();
                    item["isDir"] = file.isDirectory();
                    file.close();             // fecha explicitamente (boa prática)
                    file = dir.openNextFile();
                }
                dir.close();
            }

            String out; serializeJson(doc, out);
            request->send(200, "application/json", out);
        });

        // API: Download simples
        server.on("/api/download", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!checkAuth(request)) return request->requestAuthentication();
            if (!request->hasParam("file")) {
                request->send(400, "text/plain", "Missing file parameter");
                return;
            }
            String filepath = request->getParam("file")->value();
            request->send(SD_MMC, filepath, "application/octet-stream");
        });

        // API: Reboot
        server.on("/api/reboot", HTTP_POST, [](AsyncWebServerRequest *request) {
            if (!checkAuth(request)) return request->requestAuthentication();
            request->send(200, "text/plain", "Rebooting");
            delay(200);
            ESP.restart();
        });

        // API: OTA Update
        server.on(
            "/update",
            HTTP_POST,
            [](AsyncWebServerRequest *request) {
                if (!checkAuth(request)) return request->requestAuthentication();
                bool ok = !Update.hasError();
                AsyncWebServerResponse *response =
                    request->beginResponse(200, "text/plain", ok ? "OK" : "FAIL");
                response->addHeader("Connection", "close");
                request->send(response);
                if (ok) ESP.restart();
            },
            [](AsyncWebServerRequest *request, String filename, size_t index,
               uint8_t *data, size_t len, bool final) {
                if (!checkAuth(request)) return request->requestAuthentication();
                if (!index) {
                    size_t size = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
                    if (!Update.begin(size)) {
                        Update.printError(Serial);
                    }
                }
                if (Update.write(data, len) != len) {
                    Update.printError(Serial);
                }
                if (final) {
                    if (!Update.end(true)) {
                        Update.printError(Serial);
                    }
                }
            }
        );

        // Redireciona URLs desconhecidas para a WebUI (portal cativo).
        server.onNotFound([](AsyncWebServerRequest *request) {
            request->redirect("/");
        });

        // WebSocket
        ws.onEvent(onWsEvent);
        server.addHandler(&ws);

        server.begin();
        Serial.println("[Web] Server started on port 80");
    }

    // Processa o DNS do portal cativo (chamar no loop principal).
    static void loop() {
        if (dns_active) dns.processNextRequest();
    }

    // Encerra o servidor da WebUI. Necessário antes de subir o Evil Portal, que
    // também usa a porta 80 (os dois não podem coexistir).
    static void stop() {
        if (dns_active) { dns.stop(); dns_active = false; }
        ws.closeAll();
        server.end();
        Serial.println("[Web] Server stopped.");
    }

    static bool checkAuth(AsyncWebServerRequest *request) {
        String user = config->getString("sys_web_user");
        String pass = config->getString("sys_web_pass");
        return request->authenticate(user.c_str(), pass.c_str());
    }

    static void onWsEvent(AsyncWebSocket *server,
                          AsyncWebSocketClient *client,
                          AwsEventType type,
                          void *arg,
                          uint8_t *data,
                          size_t len) {
        (void)server;
        (void)arg;
        (void)data;
        (void)len;

        if (type == WS_EVT_CONNECT) {
            client->text("Connected to Mini Lele");
        }
    }

    static void broadcastLog(const String &msg) {
        if (config->get<bool>("web_live_logs")) {
            ws.textAll("LOG: " + msg);
        }
    }
};

inline AsyncWebServer  WebHandler::server(80);
inline AsyncWebSocket  WebHandler::ws("/ws");
inline ConfigManager  *WebHandler::config = nullptr;
inline DNSServer       WebHandler::dns;
inline bool            WebHandler::dns_active = false;

#endif
