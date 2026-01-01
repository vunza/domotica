#include <Arduino.h>
#include <headers.h>


/*
          PIN_RELAY   PIN_BUTTON    PIN_LED   MCU
--------------------------------------------------------  
SLAMPHER  GPIO12      GPIO0         GPIO13    ESP8285 1M
BASIC     GPIO12      GPIO0         GPIO13    ESP8266 1M
ESP12E    GPIO12      GPIO0         GPIO2     ESP8266 4M
T4EU1C    GPIO12      GPIO0         GPIO13    ESP8285 1M 
T0EU1C    GPIO12      GPIO0         GPIO13    ESP8285 1M 
SHELLY1L  GPIO5       GPIO4         GPIO0     ESP8285 1M 
M51C      GPIO23      GPIO0         GPIO19    ESP32   4M

*/


// Variáveis globais para cache dos sensores
float g_voltage = 0.0;
float g_current = 0.0;
float g_temperature = 0.0;
float g_humidity = 0.0;
DeviceData dados_dispositivo;


WiFiManager wifiManager;

// Servidor Web
AsyncWebServer servidorHTTP(80);
WebServer webServer(&servidorHTTP);

unsigned long lastUpdate = 0;

EspNowManager espnow;
EspNowData dados_espnow;
uint8_t broadcastMac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

// EEPROM
EEPROMManager eeprom;

// CLIENTE MQTT
WiFiClient wifiClient;
MQTTClient mqttClient(wifiClient, "192.168.0.5");

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("MQTT: ");
  Serial.println(topic);

  static char msg[MQTT_BUFFER_SIZE];   // ajuste ao teu buffer
  if (length >= sizeof(msg)) {
    imprimeln(F("Payload demasiado grande"));
    return;
  }

  memcpy(msg, payload, length);
  msg[length] = '\0';     // termina a string

  Serial.print(" Payload: ");
  Serial.println(msg);
}



//////////////////////////
// Configuração inicial //
//////////////////////////
void setup() {
    // Iniciar comunicação serial
    Serial.begin(115200);    

    while (!Serial) {
        ; // Aguarda porta serial iniciar (normal no ESP32 USB)
    }

    imprimeln();
    imprimeln(F("Iniciando..."));   
 
    // Desactiva Debug
    #if defined(ESP32)
        esp_log_level_set("*", ESP_LOG_NONE);
    #elif defined(ESP8266)
        Serial.setDebugOutput(false);
    #endif



    // Inicializa a EEPROM com 512 bytes
    eeprom.begin(EEPROM_SIZE);   
    
    // Obtem Nome do Dispositivo e o guarda no array "DeviceData"
    eeprom.EEPROM_readStruct(EEPROM_START_ADDR, dados_dispositivo);

    imprime(F("Nome do Dispositivo: "));
    imprimeln(dados_dispositivo.device_name);

    
    // Iniciar ESP-NOW    
    espnow.begin(); // canal = 1 e modo = WIFI_STA por defeito

    // broadcast (tudo que enviar vai para todos)
    espnow.addPeer(broadcastMac); // canal = 1 por defeito
    

    // Callback de recepção ESP-NOW
    espnow.onReceive([](const uint8_t *mac, const uint8_t *data, int len){

        // Obter MAC do Servidor
        char server_mac[18]; 
        String macStr = WiFi.macAddress();
        macStr.toCharArray(server_mac, 18);

        // Obter MAC do Cliente
        char client_mac[18]; 
        snprintf(client_mac, 18, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        
        // Passa dados recebidos para a estructura
        memcpy(&dados_espnow, data, sizeof(EspNowData));

        // Checa e processa o tipo de mensagens (Se nao for para retransmissao)
        if( strcmp(dados_espnow.msg_type, "DATA") == 0 && strcmp(dados_espnow.state, "RTx") != 0) {   

            // Converte em json os dados recebidos dos sensores           
            JsonBuilder json;
            json.add("server_mac", server_mac);
            json.add("client_mac", client_mac);
            json.add("state", dados_espnow.state);
            json.add("ups1_current", dados_espnow.u1_current, 2);
            json.add("ups1_voltage", dados_espnow.u1_voltage, 2);
            json.add("ups1_temperature", dados_espnow.u1_temperature, 2);
            json.add("ups1_humidity", dados_espnow.u1_humidity);             
            String dados = json.build();

            // evita enviar JSON quebrado
            if (!dados.startsWith("{") || !dados.endsWith("}")) {
                return; 
            }
            
            // Evia pela serial os dados processados            
            Serial.println(dados.c_str());
            json.reset();
        }
    });


    // Callback de envio ESP-NOW
    espnow.onSend([](const uint8_t *mac, bool ok){
        
    });

    // TDOD: Guardar/Recuperar dados da WiFi na EEPROM
    // Credeciais de acesso a rede WiFi
    const char* ssid = "TPLINK";
    const char* password = "gregorio@2012";

    // Rede WiFi
    wifiManager.connect(ssid, password);


    // CLIENTE MQTT
    mqttClient.setClientId("esp32_001");
    mqttClient.setCallback(mqttCallback);
    mqttClient.setBufferSize(MQTT_BUFFER_SIZE);
    mqttClient.connect();
    mqttClient.subscribe("wemos/C8C9A338D2C3/state");
    
}


////////////////////
// loop principal //
////////////////////
void loop() {
    // NOTA: AsyncElegantOTA não precisa de nada no loop   
    
    // CLIENTE MQTT
    mqttClient.loop();

    // Se recebeu dados pela Serial
    if (Serial.available()) {

        String recebido = Serial.readStringUntil('\n');
        recebido.trim();         
        
        // Converte String para char array
        char buffer[JS_UART_DATA_SIZE];
        recebido.toCharArray(buffer, sizeof(buffer));

        // Extrai mensagem e MAC do formato <MSG|MAC>
        char command[16];
        char dts_mac[18];

        if (buffer[0] == '<' && buffer[strlen(buffer) - 1] == '>') {
            sscanf(buffer + 1, "%15[^|]|%17[^>]", command, dts_mac);            
        }

        if ( strcmp(command, "NORMAL_MODE") == 0) {
            
            // Veriifca a quem destina-se o comando
            char mac_addr[18];            
            WiFi.macAddress().toCharArray(mac_addr, 18);

            if ( strcmp(mac_addr, dts_mac) == 0) {
                ESP.restart(); // Rinicia o ESP para iniciar o modo e operação normal
            }
            else{ // Reassa a mensagem para todos os nós ESP-NOW  
                strcpy(dados_espnow.state, "");     
                strcpy(dados_espnow.mac_server, mac_addr);
                strcpy(dados_espnow.mac_client, dts_mac);
                strcpy(dados_espnow.msg_type, "NORMAL_MODE");
                espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
            }
            
        }
        else if (strcmp(command, "OTA_MODE") == 0) { 
            
            // Rede WiFi
            //wifiManager.criar_ap(ssid, password);

            // Iniciar servidor web
            webServer.begin();

            // Inicializar OTA elegante
            ElengantOTA::begin(&servidorHTTP);       
        }

    }
   
    // Envia um "ping" a cada 5 segundos
    /*static unsigned long lastPing = 0;
    if (millis() - lastPing > 10000) {   
        lastPing = millis();       
    }*/

}

