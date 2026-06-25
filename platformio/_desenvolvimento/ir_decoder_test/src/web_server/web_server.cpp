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
    server->on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {    
        char dta[] = ("data:image/x-icon;base64,AAAB AAAAA");
        request->send(200, "image/x-icon", dta );   
    });


    // Route for root / web page
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/index.html", "text/html"); });  
              
    // Route to load onfig.html file          
    server->on("/config", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/config.html", "text/html"); });

    // Route to load onfig.html file          
    server->on("/configex", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/configex.html", "text/html"); });          
                        

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
            // Carrega dados evitando cópias desnecessárias na Stack
            lerDadosEEPROM(esp_cfg_data); 

            // Actualizar Array com os dados novos
            strncpy(esp_cfg_data.device_name, nome.c_str(), DEVICE_NAME_SIZE); 
            esp_cfg_data.device_name[DEVICE_NAME_SIZE] = '\0'; 
            salvarDadosEEPROM(esp_cfg_data);
                                  
            imprime(F("Novo nome guardado: "));
            imprimeln(nome);
            request->send(200, "text/plain", "OK: Nome atualizado para " + nome);
        } else {
            request->send(400, "text/plain", "Erro: JSON invalido");
        }
    });

    
    
    // Rota para obter o nome do ESP
    server->on("/api/get_esp_name", HTTP_GET, [](AsyncWebServerRequest *request){  
        
        // Carrega dados evitando cópias desnecessárias na Stack
        lerDadosEEPROM(esp_cfg_data); 

        // Envia o nome do Dispositivo mais o tamanho do Nome, separados por virgula
        request->send(200, "text/plain",  String(esp_cfg_data.device_name) + "," + String(DEVICE_NAME_SIZE)); 
    });


    // Route serve dados de configuracoes
    server->on("/api/configex/data", HTTP_GET, [](AsyncWebServerRequest *request){   
        
        // Carrega dados evitando cópias desnecessárias na Stack
        lerDadosEEPROM(esp_cfg_data);       
        
        JsonBuilder json;
        json.add("device_name", esp_cfg_data.device_name);
        json.add("wifiSSID", esp_cfg_data.wifiSSID);
        json.add("wifiPass", esp_cfg_data.wifiPass);
        json.add("mqtt_server_ip", esp_cfg_data.mqtt_server_ip);  
        json.add("mqtt_user", esp_cfg_data.mqtt_user);  
        json.add("mqtt_password", esp_cfg_data.mqtt_password);    
        
        String dados = json.build();        

        request->send(200, "application/json", dados);     
    }); 
    
    
    // Recebe dados a serem guardados
    // ROTA POST: Recebe JSON puro de forma nativa e ultraestável
    server->on("/api/configex/save", HTTP_POST, [](AsyncWebServerRequest *request) {
        // Retorna erro se o corpo estiver vazio
        request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Corpo vazio\"}");
        }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            
            // Garante que processamos o JSON apenas quando todos os bytes chegarem
            if (index + len == total) {
                StaticJsonDocument<512> doc;
                DeserializationError error = deserializeJson(doc, data, total);

                if (!error) {
                    // 1. LER OS DADOS DA EEPROM PRIMEIRO para preservar os bytes de pareamento na RAM
                    lerDadosEEPROM(esp_cfg_data);

                    // 2. Copia os dados usando snprintf (Proteção absoluta da RAM contra corrupção)
                    snprintf(esp_cfg_data.device_name, sizeof(esp_cfg_data.device_name), "%s", doc["device_name"] | "");
                    snprintf(esp_cfg_data.wifiSSID, sizeof(esp_cfg_data.wifiSSID), "%s", doc["wifiSSID"] | "");
                    snprintf(esp_cfg_data.wifiPass, sizeof(esp_cfg_data.wifiPass), "%s", doc["wifiPass"] | "");
                    snprintf(esp_cfg_data.mqtt_server_ip, sizeof(esp_cfg_data.mqtt_server_ip), "%s", doc["mqtt_server_ip"] | "");
                    snprintf(esp_cfg_data.mqtt_user, sizeof(esp_cfg_data.mqtt_user), "%s", doc["mqtt_user"] | "");
                    snprintf(esp_cfg_data.mqtt_password, sizeof(esp_cfg_data.mqtt_password), "%s", doc["mqtt_password"] | "");

                    // 3. Grava na Flash mantendo o pareamento intacto
                    salvarDadosEEPROM(esp_cfg_data);

                    // Envia a resposta de sucesso
                    request->send(200, "application/json", "{\"status\":\"success\"}");
                } else {
                    request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Falha no parse do JSON\"}");
                }
            }
    });


    // Rota do estado do dispositivo
    server->on("/api/configex/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        
        lerDadosEEPROM(esp_cfg_data);

        // 1. Cria o documento JSON (alocado na Stack para maior velocidade)
        StaticJsonDocument<128> doc;

        // 2. Preenche os campos com o estado real das variáveis do seu firmware
        doc["configurado"] = esp_cfg_data.configurado; // true ou false

        // Lógica para definir se o pareamento está em curso ou parado (idle)        
        if (!paired && !esp_cfg_data.configurado) {
            doc["status_pareamento"] = "em_curso";
        } else {
            doc["status_pareamento"] = "idle";
        }

        // 3. Serializa o JSON para uma String
        String jsonResposta;
        serializeJson(doc, jsonResposta);

        // 4. Envia a resposta para o navegador com o cabeçalho correto (application/json)
        request->send(200, "application/json", jsonResposta);
    });


    // Rota para limpar EEPROM
    server->on("/api/configex/clear", HTTP_POST, [](AsyncWebServerRequest *request) {
        
        // 1. Limpa a estrutura global na RAM preenchendo-a com zeros
        memset(&esp_cfg_data, 0, sizeof(ConfigDados));
        
        // 2. Grava a estrutura vazia na EEPROM/Flash para apagar o pareamento e o Wi-Fi
        salvarDadosEEPROM(esp_cfg_data);
        
        // 3. Reseta a variável de controle local
        paired = false;
        
        // 4. CRÍTICO: Responde ao navegador PRIMEIRO antes de reiniciar!
        // Se reiniciar antes desta linha, o JavaScript vai dar erro.
        request->send(200, "application/json", "{\"status\":\"cleared\"}");
        
        // 5. Agenda o reinício do chip para aplicar a limpeza (dá 500ms para o pacote web sair)
        if (request->client()) {
            delay(500); 
            ESP.restart(); // Opcional: Se quiser que o ESP reinicie sozinho
        }      
        
    });

   
    
    // Route serve sensor INA226 data as JSON
    server->on("/api/sensores", HTTP_GET, [](AsyncWebServerRequest *request){          
        
        JsonBuilder json;
        json.add("ups1_current", 0.00, 2);
        json.add("ups1_voltage", 0.00, 2);
        json.add("ups1_temperature", 0.00, 2);
        json.add("ups1_humidity", 0, 2);    
        
        String dados = json.build();        

        request->send(200, "application/json", dados);     
    });         

    // Iniciar o servidor web          
    server->begin();

    imprimeln(F("Servidor HTTP iniciado"));
}
