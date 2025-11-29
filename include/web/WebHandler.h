#ifndef WEB_HANDLER_H
#define WEB_HANDLER_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include <SD_MMC.h>

#include "core/ConfigManager.h"
#include "WebAssets.h"
#include "core/PwnPet.h"
#include "core/PwnAttack.h"
#include "core/PwnPower.h"

class WebHandler {
private:
    static AsyncWebServer server;
    static AsyncWebSocket ws;
    static ConfigManager* config;

    static bool checkAuth(AsyncWebServerRequest *request) {
        String user = config->getString("sys_web_user");
        String pass = config->getString("sys_web_pass");
        return request->authenticate(user.c_str(), pass.c_str());
    }

    static void handlePetAction(AsyncWebServerRequest *request, uint8_t *data, size_t len) {
        if (!checkAuth(request)) {
            request->requestAuthentication();
            return;
        }

        DynamicJsonDocument doc(512);
        DeserializationError err = deserializeJson(doc, data, len);
        if (err) {
            request->send(400, "application/json", "{\"error\":\"invalid json\"}");
            return;
        }

        const char *action = doc["action"] | "";
        int value = doc["value"] | 0;

        if (strcmp(action, "feed") == 0) {
            PwnPet::feed(value > 0 ? value : 10);
        } else if (strcmp(action, "sleep") == 0) {
            PwnPet::setSleeping(true);
        } else if (strcmp(action, "wake") == 0) {
            PwnPet::setSleeping(false);
        } else if (strcmp(action, "attack_start") == 0) {
            PwnAttack::start();
        } else if (strcmp(action, "attack_stop") == 0) {
            PwnAttack::stop();
        }

        DynamicJsonDocument resp(128);
        resp["ok"] = true;
        String out;
        serializeJson(resp, out);
        request->send(200, "application/json", out);
    }

    static void onWsEvent(AsyncWebSocket *serverPtr, AsyncWebSocketClient *client,
                          AwsEventType type, void *arg, uint8_t *data, size_t len) {
        if (type == WS_EVT_CONNECT) {
            client->text("Connected to PwnTamagotchi BR");
        }
    }

public:
    static void init() {
        config = ConfigManager::getInstance();

        String mode = config->getString("sys_wifi_mode");
        if (mode == "AP" || mode == "AP_STA") {
            WiFi.softAP(config->getString("sys_ap_ssid").c_str(),
                        config->getString("sys_ap_pass").c_str());
            Serial.print("[Web] AP Started: ");
            Serial.println(WiFi.softAPIP());
        }
        if (mode == "STA" || mode == "AP_STA") {
            WiFi.begin(config->getString("sys_sta_ssid").c_str(),
                       config->getString("sys_sta_pass").c_str());
            Serial.println("[Web] Connecting STA...");
        }

        // Root UI
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!checkAuth(request)) return request->requestAuthentication();
            request->send(200, "text/html", index_html_raw);
        });

        // Config API
        server.on("/api/config", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!checkAuth(request)) return request->requestAuthentication();
            String json;
            config->getJSON(json);
            request->send(200, "application/json", json);
        });

        server.on("/api/config", HTTP_POST,
                  [](AsyncWebServerRequest *request) {},
                  nullptr,
                  [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
                     size_t index, size_t total) {
                      if (!checkAuth(request)) return request->requestAuthentication();
                      String json;
                      json.reserve(len);
                      for (size_t i = 0; i < len; i++) json += (char)data[i];
                      config->updateFromJSON(json);
                      request->send(200, "text/plain", "Config Updated");
                  });

        // Status API
        server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
            DynamicJsonDocument doc(512);
            JsonObject root = doc.to<JsonObject>();

            PetStats s = PwnPet::getStats();
            root["pet_hunger"] = s.hunger;
            root["pet_happiness"] = s.happiness;
            root["pet_level"] = s.level;
            root["pwr_battery"] = PwnPower::getBatteryPercent();
            root["pwr_current"] = PwnPower::getSystemCurrent();
            root["wifi_mac"] = WiFi.macAddress();
            root["heap_free"] = ESP.getFreeHeap();

            String out;
            serializeJson(doc, out);
            request->send(200, "application/json", out);
        });

        // Pet control API
        server.on("/api/pet/action", HTTP_POST,
                  [](AsyncWebServerRequest *request) {},
                  nullptr,
                  [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
                     size_t index, size_t total) {
                      handlePetAction(request, data, len);
                  });

        // Simple gamification stats
        server.on("/api/pet/stats", HTTP_GET, [](AsyncWebServerRequest *request) {
            DynamicJsonDocument doc(512);
            JsonObject root = doc.to<JsonObject>();

            PetStats s = PwnPet::getStats();
            root["name"] = s.name;
            root["level"] = s.level;
            root["xp"] = s.xp;
            root["hunger"] = s.hunger;
            root["happiness"] = s.happiness;
            root["handshakes"] = s.handshakes_total;

            String out;
            serializeJson(doc, out);
            request->send(200, "application/json", out);
        });

        // File list API
        server.on("/api/files", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!checkAuth(request)) return request->requestAuthentication();
            String path = request->hasParam("path") ? request->getParam("path")->value() : "/";

            DynamicJsonDocument doc(4096);
            JsonArray root = doc.to<JsonArray>();

            if (SD_MMC.begin("/sdcard", true, true)) {
                File dir = SD_MMC.open(path);
                if (dir) {
                    File file = dir.openNextFile();
                    while (file) {
                        JsonObject item = root.createNestedObject();
                        item["name"] = String(file.name());
                        item["size"] = file.size();
                        item["isDir"] = file.isDirectory();
                        file = dir.openNextFile();
                    }
                }
            }

            String out;
            serializeJson(doc, out);
            request->send(200, "application/json", out);
        });

        // File download
        server.on("/api/download", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!checkAuth(request)) return request->requestAuthentication();
            if (!request->hasParam("file")) {
                request->send(400, "text/plain", "missing file");
                return;
            }
            String filepath = request->getParam("file")->value();
            if (!SD_MMC.begin("/sdcard", true, true)) {
                request->send(500, "text/plain", "sd not ready");
                return;
            }
            request->send(SD_MMC, filepath, "application/octet-stream");
        });

        // WebSocket
        ws.onEvent(onWsEvent);
        server.addHandler(&ws);

        server.begin();
        Serial.println("[Web] Server started on port 80");
    }

    static void broadcastLog(const String &msg) {
        if (config && config->get<bool>("web_live_logs")) {
            ws.textAll("LOG: " + msg);
        }
    }
};

inline AsyncWebServer WebHandler::server(80);
inline AsyncWebSocket WebHandler::ws("/ws");
inline ConfigManager* WebHandler::config = nullptr;

#endif
