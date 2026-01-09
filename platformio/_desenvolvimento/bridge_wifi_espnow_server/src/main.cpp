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


// TDOD: Guardar/Recuperar dados da WiFi na EEPROM
// ========== CONFIGURAÇÕES ==========
const char* WIFI_SSID = "TPLINK";
const char* WIFI_PASSWORD = "gregorio@2012";

const char* MQTT_BROKER = "192.168.0.5";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "wnr";
const char* MQTT_PASSWORD = "123";


bool ledState = false;
const int LED_PIN = 2; // GPIO2 - LED embutido (LOW = ligado)


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
// TODO: IP do broker dinamico
MQTTClient mqttClient(wifiClient, MQTT_BROKER);


// DISPLAY OLED
OLEDDisplay oled;


void publishState() {
  const char* state = ledState ? "ON" : "OFF";    
  mqttClient.publish("casa/c8c9a338d2e7/state", state, true);
  digitalWrite(LED_PIN, ledState ? LOW : HIGH);
}


// CallBack MQTT 
void mqttCallback(char* topic, byte* payload, unsigned int length);

uint8_t getWiFiChannelFromAP(const char* ssid) {
#if defined(ESP8266)
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; i++) {
        if (WiFi.SSID(i) == ssid) {
            return WiFi.channel(i);
        }
    }
#elif defined(ESP32)
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; i++) {
        if (WiFi.SSID(i) == ssid) {
            return WiFi.channel(i);
        }
    }
#endif
    return 0; // não encontrado
}


//////////////////////////
// Configuração inicial //
//////////////////////////
void setup() {
    // Iniciar comunicação serial
    Serial.begin(115200); 
    delay(200);   

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


    // TODO: Melhorar este trecho
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH); // LED desligado inicialmente


    // Inicializa a EEPROM com 512 bytes
    eeprom.begin(EEPROM_SIZE);   
    
    // Obtem Nome do Dispositivo e o guarda no array "DeviceData"
    eeprom.EEPROM_readStruct(EEPROM_START_ADDR, dados_dispositivo);

    imprime(F("Nome do Dispositivo: "));
    imprimeln(dados_dispositivo.device_name);

    // Obter canal WiFi
    uint8_t channel = WiFi.channel();
    
    // Iniciar ESP-NOW    
    espnow.begin(channel, false); // canal e modo = WIFI_STA por defeito

    // broadcast (tudo que enviar vai para todos)
    espnow.addPeer(broadcastMac, channel); 
    

    // Callback de recepção ESP-NOW
    espnow.onReceive([](const uint8_t *mac, const uint8_t *data, int len){

        // Obter MAC do Servidor
        char server_mac[18]; 
        String macStr = WiFi.macAddress();
        macStr.toCharArray(server_mac, 18);

        // Obter MAC do Cliente
        char client_mac[18]; 
        snprintf(client_mac, 18, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        char client_mac_without_colon[13]; 
        snprintf(client_mac_without_colon, 13, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        
        // Passa dados recebidos para a estructura
        memcpy(&dados_espnow, data, sizeof(EspNowData));        

        // Processa Requisição de Canal
        if( strcmp(dados_espnow.msg_type, "CHANNEL_REQ") == 0) {

            // Guarda o hostname do cliente, para CHANNEL_REQ vem atrave de "dados_espnow.mac_server"
            const char* client_host_name = dados_espnow.mac_server;            

            // Converte inteiro a char []
            uint8_t channel = espnow.getCurrentWiFiChannel();;
            uint8_t len = String(channel).length();
            char cz_channel[len + 1];  
            String(channel).toCharArray(cz_channel, sizeof(cz_channel));
           
            // Criar Entidade MQTT                        
            char node_id[13];  
            char mac_colon[18]; 
            char unique_id[64];    
            const char* component  = "switch";                
            strcpy(mac_colon, client_mac); // wifiManager.getMacAddress(mac_colon, true);
            strcpy(node_id, client_mac_without_colon); // wifiManager.getMacAddress(node_id, false);    
            snprintf(unique_id, sizeof(unique_id), "%s%s", mqttClient.entity_id_prefix, node_id);   
            const char* device_name = client_mac_without_colon;
            const char* entity_name = ""; //"Interruptor Sala";
            const char* base_topic = "casa/";  
            const char* manufacturer = "Espressif";            
            const char* model = client_host_name; // (*J*) Deve ser unico
            const char* extra_config;            

            mqttClient.publishDiscoveryEntity(
                mqttClient, "entity", component, node_id, entity_name, base_topic, unique_id,       
                mac_colon, device_name, manufacturer, model, extra_config = nullptr 
            );  


            // Respode a mensagem "CHANNEL_REQ"
            strcpy(dados_espnow.msg_type, "CHANNEL_RSP");
            strcpy(dados_espnow.state, cz_channel);  
            strcpy(dados_espnow.mac_server, server_mac);
            strcpy(dados_espnow.mac_client, client_mac);          
           
            // Envia Resposta, CHANNEL_RSP, da petição CHANNEL_REQ
            espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
    
        }

        // Checa e processa o tipo de mensagens (Se nao for para retransmissao)
        else if( strcmp(dados_espnow.msg_type, "DATA") == 0 ) {   

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
    

    // Rede WiFi
    wifiManager.connect(WIFI_SSID, WIFI_PASSWORD);
    //wifiManager.criar_ap("ESP8266", "123456789", 3);


    // Conectar CLIENTE MQTT   
    #if defined(ESP32)
        const char* host_name = WiFi.getHostname();
    #elif defined(ESP8266)  
        String hostNameStr = WiFi.hostname();
        const char* host_name = hostNameStr.c_str();
    #endif  
    
    mqttClient.setClientId(host_name);
    mqttClient.setCallback(mqttCallback);
    mqttClient.setBufferSize(MQTT_BUFFER_SIZE);
    mqttClient.connect();         

    // Criar Dispositivo MQTT
    char node_id[13];  
    char mac_colon[18]; 
    char unique_id[64];    
    const char* component  = "switch"; 
    wifiManager.getMacAddress(mac_colon, true);   
    wifiManager.getMacAddress(node_id, false);    
    snprintf(unique_id, sizeof(unique_id), "%s%s", mqttClient.entity_id_prefix, node_id);      
    const char* device_name = node_id;
    const char* entity_name = "";//"Interruptor Cozinha";
    const char* base_topic = "casa/";  
    const char* manufacturer = "Espressif";    
    const char* model = host_name; // (*J*) Deve ser unico
    const char* extra_config;

    // Guarda node_id do dispositivo principal para ser usado nas entidades dependentes
    strcpy(mqttClient.main_device_node_id, node_id);
     
    mqttClient.publishDiscoveryEntity(
        mqttClient, "device", component, node_id, entity_name, base_topic, unique_id,       
        mac_colon, device_name, manufacturer, model, extra_config = nullptr 
    );  
    
    

    // DISPLAY OLED
    static unsigned long contador = 0;
    if (millis() - contador > 5000 && !oled.begin()) {   
        contador = millis();       
        imprimeln(F("Falha ao iniciar OLED"));        
    }
    else if(oled.begin()){
        oled.clear();
        oled.printText(0, 0, "Sistema OK", 2);
        oled.showBattery(75);
        oled.update();
    }     
    
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

    // DISPLAY OLED
    static uint8_t bat = 0;
    bat = (bat + 5) % 101;

    oled.clear();
    oled.printText(0, 0, "Bat.", 2);
    oled.showBattery(bat);
    oled.update();

    delay(2000);

}


///////////////////
// CallBack MQTT //
///////////////////
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  
  imprimeln(topic);

  static char msg[MQTT_BUFFER_SIZE];   // ajuste ao teu buffer
  if (length >= sizeof(msg)) {
    imprimeln(F("Payload demasiado grande"));
    return;
  }

  memcpy(msg, payload, length);
  msg[length] = '\0';     

  imprime(F(" Payload: "));
  imprimeln(msg);

     
  // Verificar se é comando
  if (strcmp(topic, "casa/c8c9a338d2e7/set") == 0) {
    if (strcmp(msg, "ON") == 0) {
      ledState = true;
    } else if (strcmp(msg, "OFF") == 0) {
      ledState = false;
    }
    publishState();
  }
}


