#include "web/WebHandler.h"
#include "core/ConfigManager.h"
#include "web/WebAssets.h"
#include "core/PwnPower.h"
#include "core/PwnPet.h"
#include "WiFiTools.h"
#include "EvilPortal.h"
#include <SD_MMC.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <Update.h>
#include <ArduinoJson.h>

static AsyncWebServer* server = nullptr;
static DNSServer* dns = nullptr;
static AsyncWebSocket* ws = nullptr;
static bool running = false;

void WebHandler::init() {
    running = false;
    Serial.println("[WebHandler] Inicializado");
}

// Registra rotas (uma unica vez) e sobe o servidor quando o WiFi estiver pronto.
// Nao conflita com o EvilPortal: se o portal de ataque estiver ativo, a WebUI
// desliga (porta 80 unica); quando o portal para, a WebUI volta automaticamente.
void WebHandler::loop() {
    if (dns) dns->processNextRequest();

    if (EvilPortal::isActive()) {
        if (running) { stop(); Serial.println("[WebHandler] Pausado (EvilPortal ativo)"); }
        return;
    }
    if (running) return;
    if (WiFi.status() != WL_CONNECTED && WiFi.softAPgetStationNum() == 0) return;

    if (!server) {
        server = new AsyncWebServer(80);
        ws = new AsyncWebSocket("/ws");

        // ---- UI (o HTML otimizado e autocontido, com favicon inline) ----
        server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(200, "text/html", index_html);
        });
        server->on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(200, "text/html", index_html);
        });
        // Favicon ja embutido via data-URI; 204 evita 404 ruidoso.
        server->on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(204);
        });

        // ---- API ----
        server->on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(200, "application/json", getStatusJSON());
        });
        server->on("/api/config", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(200, "application/json", getConfigJSON());
        });
        server->on("/api/config", HTTP_POST, [](AsyncWebServerRequest *request) {
            request->send(200, "application/json", "{\"ok\":true}");
        }, nullptr, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            (void)index; (void)total; // body chega em pedacos; acumula e parseia no ultimo
            static char cfgBuf[1024];
            static size_t cfgLen = 0;
            if (index == 0) cfgLen = 0;
            size_t room = sizeof(cfgBuf) - 1 - cfgLen;
            size_t n = len < room ? len : room;
            memcpy(cfgBuf + cfgLen, data, n);
            cfgLen += n;
            cfgBuf[cfgLen] = 0;
            if (index + len >= total) updateConfig(cfgBuf);
        });
        server->on("/api/reboot", HTTP_POST, [](AsyncWebServerRequest *request) {
            request->send(200, "application/json", "{\"ok\":true}");
            delay(300);
            ESP.restart();
        });

        // ---- Arquivos do SD (lista + download) ----
        server->on("/api/files", HTTP_GET, [](AsyncWebServerRequest *request) {
            String path = "/";
            if (request->hasParam("path")) path = request->getParam("path")->value();
            if (!path.startsWith("/")) path = "/" + path;
            File dir = SD_MMC.open(path);
            if (!dir || !dir.isDirectory()) {
                request->send(404, "application/json", "[]");
                return;
            }
            JsonDocument doc;
            JsonArray arr = doc.to<JsonArray>();
            for (File f = dir.openNextFile(); f; f = dir.openNextFile()) {
                JsonObject o = arr.add<JsonObject>();
                o["name"] = f.name();
                o["size"] = (uint32_t)f.size();
                o["isDir"] = (bool)f.isDirectory();
                f.close();
            }
            dir.close();
            String out;
            serializeJson(doc, out);
            request->send(200, "application/json", out);
        });
        server->on("/api/download", HTTP_GET, [](AsyncWebServerRequest *request) {
            if (!request->hasParam("file")) { request->send(400, "text/plain", "parametro 'file' ausente"); return; }
            String path = request->getParam("file")->value();
            if (!path.startsWith("/")) path = "/" + path;
            if (!SD_MMC.exists(path)) { request->send(404, "text/plain", "arquivo nao encontrado"); return; }
            String mime = "application/octet-stream";
            if (path.endsWith(".json")) mime = "application/json";
            else if (path.endsWith(".txt") || path.endsWith(".md")) mime = "text/plain";
            request->send(SD_MMC, path, mime, true);
        });

        // ---- OTA ----
        server->on("/update", HTTP_POST,
            [](AsyncWebServerRequest *request) {
                bool ok = !Update.hasError();
                request->send(200, "text/plain", ok ? "OK" : "FAIL");
                if (ok) { delay(500); ESP.restart(); }
            },
            [](AsyncWebServerRequest *request, String filename, size_t index,
               uint8_t *data, size_t len, bool final) {
                if (index == 0 && !Update.begin(UPDATE_SIZE_UNKNOWN)) {
                    Update.printError(Serial);
                }
                if (len && Update.write(data, len) != len) {
                    Update.printError(Serial);
                }
                if (final && !Update.end(true)) {
                    Update.printError(Serial);
                }
            });

        // ---- WebSocket (logs ao vivo) + fallback ----
        server->addHandler(ws);
        server->onNotFound([](AsyncWebServerRequest *request) {
            // Rota /api/* inexistente responde 404 JSON. Antes devolviamos o
            // index_html para tudo, o que fazia a WebUI achar que estava
            // "offline (preview)" e mostrar dados MOCK no lugar dos reais.
            if (request->url().startsWith("/api/")) {
                request->send(404, "application/json", "{\"error\":\"not_found\"}");
                return;
            }
            request->send(200, "text/html", index_html);
        });
    }

    server->begin();
    dns = new DNSServer();
    IPAddress apip = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP() : WiFi.softAPIP();
    dns->start(53, "*", apip);
    running = true;
    Serial.printf("[WebHandler] WebUI ativa: http://%s/\n", apip.toString().c_str());
}

void WebHandler::stop() {
    if (server) { server->end(); delete server; server = nullptr; }
    if (dns) { dns->stop(); delete dns; dns = nullptr; }
    if (ws) { delete ws; ws = nullptr; }
    running = false;
}

bool WebHandler::isRunning() { return running; }

void WebHandler::restart() {
    stop();
    init();
}

String WebHandler::getStatusJSON() {
    // Telemetria real disponivel no firmware (todos os campos usados pela WebUI).
    uint32_t up = millis() / 1000UL;
    float vbat = PwnPower::pmu_ok ? PwnPower::getBatteryVoltage() : 0.0f;
    int pct = PwnPower::pmu_ok ? PwnPower::getBatteryPercent() : 0;
    bool charging = PwnPower::pmu_ok && PwnPower::isCharging();
    int wifi_devices = WiFiTools::getAPCount() + WiFiTools::getDeviceCount();

    // Humor derivado da felicidade do pet
    int hap = PwnPet::stats.happiness;
    const char *mood = "TRISTE";
    if (hap > 80) mood = "FELIZ";
    else if (hap > 50) mood = "CONTENTE";
    else if (hap > 20) mood = "ENTEDIADO";

    // Autonomia estimada (dreno medio ~150 mA)
    float hours = 0;
    if (PwnPower::pmu_ok && !charging && PwnPower::battery_capacity_mah > 0)
        hours = (PwnPower::battery_capacity_mah * pct / 100.0f) / 150.0f;

    char buf[640];
    snprintf(buf, sizeof(buf),
        "{\"uptime\":%lu,\"heap_free\":%u,\"wifi_mac\":\"%s\","
        "\"pet_name\":\"%s\",\"pet_mood\":\"%s\",\"pet_stage\":%d,"
        "\"pet_stage_name\":\"%s\",\"pet_charging\":%s,"
        "\"pet_level\":%lu,\"pet_xp\":%lu,\"pet_hunger\":%lu,"
        "\"pet_happiness\":%lu,\"pet_handshakes\":%lu,"
        "\"wifi_devices\":%d,"
        "\"pwr_battery\":%d,\"pwr_voltage\":%.2f,\"pwr_charging\":%s,"
        "\"pwr_vbus\":%s,\"pwr_hours\":%.1f,\"pwr_charge_status\":\"%s\"}",
        (unsigned long)up, (unsigned)ESP.getFreeHeap(), WiFi.macAddress().c_str(),
        PwnPet::stats.name.c_str(), mood, (int)PwnPet::stats.stage,
        PwnPet::getStageName(), charging ? "true" : "false",
        (unsigned long)PwnPet::stats.level, (unsigned long)PwnPet::stats.xp,
        (unsigned long)PwnPet::stats.hunger, (unsigned long)PwnPet::stats.happiness,
        (unsigned long)PwnPet::stats.total_handshakes,
        wifi_devices,
        pct, vbat, charging ? "true" : "false",
        PwnPower::last_vbus ? "true" : "false",
        hours, charging ? "Carregando" : "Bateria");
    return String(buf);
}

String WebHandler::getConfigJSON() {
    ConfigManager *cfg = ConfigManager::getInstance();
    return cfg->getJSONRedacted().as<String>();
}

bool WebHandler::updateConfig(const char* json) {
    return ConfigManager::getInstance()->updateFromJSON(json);
}

void WebHandler::handleUpload() {
    // Upload de arquivos
}

void WebHandler::sendWebSocket(const char* msg) {
    if (ws) ws->textAll(msg);
}

