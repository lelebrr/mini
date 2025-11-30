#ifndef WEB_HANDLER_H
#define WEB_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <Update.h>
#include <SD_MMC.h>

#include "core/ConfigManager.h"
#include "WebAssets.h"
#include "core/PwnPet.h"
#include "core/PwnAttack.h"
#include "core/PwnPower.h"

class WebHandler {
private:
    static AsyncWebServer  server;
    static AsyncWebSocket  ws;
    static ConfigManager  *config;

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
            request->send(200, "text/html", index_html_raw);
        });

        // API: Config Get
        server.on("/api/config", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!checkAuth(request)) return request->requestAuthentication();
            String json;
            config->getJSON(json);
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
        server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
            auto *response = new AsyncJsonResponse();
            JsonObject root = response->getRoot();

            PetStats pet = PwnPet::getStats();
            root["pet_hunger"]    = pet.hunger;
            root["pet_happiness"] = pet.happiness;
            root["pet_level"]     = pet.level;

            root["pwr_battery"] = PwnPower::getBatteryPercent();
            root["pwr_current"] = PwnPower::getSystemCurrent();
            root["wifi_mac"]    = WiFi.macAddress();
            root["heap_free"]   = ESP.getFreeHeap();

            response->setLength();
            request->send(response);
        });

        // API: File Manager (Listagem)
        server.on("/api/files", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!checkAuth(request)) return request->requestAuthentication();
            String path = request->hasParam("path") ? request->getParam("path")->value() : "/";

            auto *response = new AsyncJsonResponse(8192);
            JsonArray root = response->getRoot().to<JsonArray>();

            File dir = SD_MMC.open(path);
            if (dir) {
                File file = dir.openNextFile();
                while (file) {
                    JsonObject item = root.createNestedObject();
                    item["name"]  = String(file.name());
                    item["size"]  = file.size();
                    item["isDir"] = file.isDirectory();
                    file = dir.openNextFile();
                }
                dir.close();
            }

            response->setLength();
            request->send(response);
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

        // WebSocket
        ws.onEvent(onWsEvent);
        server.addHandler(&ws);

        server.begin();
        Serial.println("[Web] Server started on port 80");
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

#endif
