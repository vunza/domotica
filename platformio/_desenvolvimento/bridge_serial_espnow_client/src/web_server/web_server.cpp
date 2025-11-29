#include "web_server.h"


// Constructor
WebServer::WebServer(AsyncWebServer* s) {
    server = s;
}



void WebServer::begin() {

    // ---- INICIAR SPIFFS ----
    if (!LittleFS.begin()) {
        imprimeln(F("Erro a montar SPIFFS"));
        return;
    }
    else{
        imprimeln(F("SPIFFS montado com sucesso"));
    }

    // ---- SERVE ARQUIVOS DO SPIFFS ----
    // Route for root / web page
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/painel.html", "text/html"); });

    // Route to load ota.html file          
    server->on("/ota", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/ota.html", "text/html"); });

    // Route to load style.css file
    server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/style.css", "text/css"); });

    // Route to load script.js file
    server->on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/script.js", "text/javascript"); });

    // Rota para reiniciar o ESP
    server->on("/api/reset_esp", HTTP_GET, [](AsyncWebServerRequest *request){        
        ESP.restart();
        request->send(200, "text/plain", "OK");     
    });     
    
    // Iniciar o servidor web          
    server->begin();

    imprimeln(F("Servidor HTTP iniciado"));
}
