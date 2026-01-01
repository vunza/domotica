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
              { request->send(LittleFS, "/index.html", "text/html"); });

    server->on("/painel", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/painel.html", "text/html"); });           

    // Route to load onfig.html file          
    server->on("/config", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/config.html", "text/html"); });

    // Route to load style.css file
    server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/style.css", "text/css"); });

    // Route to load script.js file
    server->on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/script.js", "text/javascript"); });

    // Rota para reiniciar o ESP
    server->on("/api/restart_device", HTTP_GET, [](AsyncWebServerRequest *request){        
        ESP.restart();
        request->send(200, "text/plain", "OK");     
    });   
    
    // Rota para renonmear o ESP, recebe dados atravez da requisicao POST
    server->on("/api/rename_esp", HTTP_POST, [](AsyncWebServerRequest *request){},NULL,[](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {

        String body = "";
        for (size_t i = 0; i < len; i++) body += (char)data[i];

        imprimeln(F("JSON Recebido:"));
        imprimeln(body);

        // Extrair o campo "name"
        String nome = "";
        uint8_t p1 = body.indexOf("\"name\"");
        if (p1 >= 0) {
            uint8_t aspas1 = body.indexOf("\"", p1 + 6);
            uint8_t aspas2 = body.indexOf("\"", aspas1 + 1);
            nome = body.substring(aspas1 + 1, aspas2);
        }

        if (nome.length() > 0) {
            // Guardar no Array os dados do dispositivo que estao na EEPROM (para preservar os dados antigos necessarios)
            eeprom.EEPROM_readStruct(EEPROM_START_ADDR, dados_dispositivo);

            // Actualizar Array com os dados novos
            strcpy(dados_dispositivo.device_name, nome.c_str()); 
            
            // Guarda os dados novos na EEPROM
            eeprom.EEPROM_writeStruct(EEPROM_START_ADDR, dados_dispositivo);            

            imprime(F("Novo nome salvo: "));
            imprimeln(nome);
            request->send(200, "text/plain", "OK: Nome atualizado para " + nome);
        } else {
            request->send(400, "text/plain", "Erro: JSON invalido");
        }
    });

    
    
    // Rota para obter o nome do ESP
    server->on("/api/get_esp_name", HTTP_GET, [](AsyncWebServerRequest *request){  
        
        // Ler, na EEPROM, os dados do Dispositivo e os guarda no Array "DeviceData"
        eeprom.EEPROM_readStruct(EEPROM_START_ADDR, dados_dispositivo);

        // Envia o nome do Dispositivo mais o tamanho do Nome, separados por virgula
        request->send(200, "text/plain",  String(dados_dispositivo.device_name) + "," + String(DEVICE_NAME_SIZE));     
    });


    // Route serve sensor INA226 data as JSON
    server->on("/api/sensores", HTTP_GET, [](AsyncWebServerRequest *request){            

        JsonBuilder json;
        json.add("voltage", g_voltage, 3);
        json.add("current", g_current, 3);
        json.add("temperature", g_temperature, 2);
        json.add("humidity", g_humidity, 2);

        request->send(200, "application/json", json.build());     
    }); 

    
    // Iniciar o servidor web          
    server->begin();

    imprimeln(F("Servidor HTTP iniciado"));
}
