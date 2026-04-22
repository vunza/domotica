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


// TDOD: Guardar/Recuperar, na EEPROM, dados da WiFi e  do Servidor Broker MQTT, para evitar reconfiguração após reinicio do dispositivo.
// ========== CONFIGURAÇÕES ==========
const char* WIFI_SSID = "TPLINK";
const char* WIFI_PASSWORD = "gregorio@2012";

const char* MQTT_BROKER = "192.168.0.5";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "wnr";
const char* MQTT_PASSWORD = "123";


//bool ledState = false;
const uint8_t LED_PIN = 2; // GPIO2 - LED embutido (LOW = ligado)


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
//volatile bool newDataReady = false;

EspNowManager espnow;
EspNowData dados_espnow;
uint8_t broadcastMac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

// EEPROM
EEPROMManager eeprom;

// CLIENTE MQTT
WiFiClient wifiClient;
// TODO: IP do broker dinamico
MQTTClient mqttClient(wifiClient, MQTT_BROKER);

// Variável para controlar tentativas de reconexão MQTT
unsigned long lastReconnectAttempt = 0;


// DISPLAY OLED
OLEDDisplay oled;

// CallBack MQTT 
void mqttCallback(char* topic, byte* payload, unsigned int length);

// Função para conectar ao broker MQTT e publicar a entidade do dispositivo
void conect2MqttBroker();

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
    delay(1000);
    
    // TODO: Melhorar este trecho
    pinMode(LED_PIN, OUTPUT);

    #if defined(ESP32)        
        digitalWrite(LED_PIN, LOW); // Lógica direta (LOW = ligado)
    #elif defined(ESP8266)        
        digitalWrite(LED_PIN, HIGH); // Lógica invertida (HIGH = desligado)
    #endif 


    // while (!Serial) {
    //     ; // Aguarda porta serial iniciar (normal no ESP32 USB)
    // }

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

   
    // Obter canal WiFi
    uint8_t channel = WiFi.channel();
    
    // Iniciar ESP-NOW    
    espnow.begin(channel, false); // canal e modo = WIFI_STA por defeito

    // broadcast (tudo que enviar vai para todos)
    espnow.addPeer(broadcastMac, channel);    
    
    //////////////////////////////////
    // Callback de recepção ESP-NOW //
    //////////////////////////////////
    espnow.onReceive([](const uint8_t *mac, const uint8_t *data, int len){

        // Verifica se o pacote tem o tamanho esperado
        if (len == sizeof(EspNowData)) {
             // Passa dados recebidos para a estructura              
            memcpy((void*)&dados_espnow, data, sizeof(EspNowData)); 
            //newDataReady = true;  // sinaliza para o loop
        }
        else{
            return; // Ignora pacotes de tamanho inesperado
        }       

        // Descarta Mensagens de Retransmissão
        if( strcmp(dados_espnow.mac_server, "RTx") == 0) {
            /*imprime(F("Msg RTx descartada: "));
            imprime(dados_espnow.msg_type);
            imprime(F(" | MAC Cliente: "));
            imprimeln(dados_espnow.mac_client);*/
            return;
        }

        if( strcmp(dados_espnow.mac_server, "RTx") != 0 && strcmp(dados_espnow.msg_type, "TELEMETRY") != 0 ) {
            imprime(F("Msg Recebida: "));
            imprime(dados_espnow.msg_type);
            imprime(F(" | MAC Cliente: "));
            imprimeln(dados_espnow.mac_client);
        }
        

        // Obter MAC do Servidor
        char server_mac[18]; 
        String macStr = WiFi.macAddress();
        macStr.toCharArray(server_mac, 18);

        // Obter MAC do Cliente
        const char* str = dados_espnow.mac_client;
        uint8_t CLIENT_MAC[6];
        sscanf(str, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
                                &CLIENT_MAC[0], &CLIENT_MAC[1], &CLIENT_MAC[2],
                                &CLIENT_MAC[3], &CLIENT_MAC[4], &CLIENT_MAC[5]);  


        char client_mac[18]; 
        snprintf(client_mac, 18, "%02X:%02X:%02X:%02X:%02X:%02X", CLIENT_MAC[0], CLIENT_MAC[1], CLIENT_MAC[2], CLIENT_MAC[3], CLIENT_MAC[4], CLIENT_MAC[5]);
        char client_mac_without_colon[13]; 
        snprintf(client_mac_without_colon, 13, "%02x%02x%02x%02x%02x%02x", CLIENT_MAC[0], CLIENT_MAC[1], CLIENT_MAC[2], CLIENT_MAC[3], CLIENT_MAC[4], CLIENT_MAC[5]);
             

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
            const char* component  = MQTT_ENTITY_DOMAIN;                
            strcpy(mac_colon, client_mac); // wifiManager.getMacAddress(mac_colon, true);
            strcpy(node_id, client_mac_without_colon); // wifiManager.getMacAddress(node_id, false);                 
            const char* device_name = client_mac_without_colon;
            const char* entity_name = ""; //"Interruptor Sala"; 
            const char* base_topic = MQTT_BASE_TOPIC;
            snprintf(unique_id, sizeof(unique_id), "%s%s", mqttClient.entity_id_prefix, node_id);            
            const char* manufacturer = MQTT_ENTITY_MANUFACTURER;            
            const char* model = client_host_name; // (*J*) Deve ser unico
            const char* extra_config;

            // Publicar Entidade MQTT do Cliente
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
    
        } else if( strcmp(dados_espnow.msg_type, "STATE_SETED") == 0 ) { 
            // Publicar Estado do PIN/LED
            String str_topico = MQTT_BASE_TOPIC + String(client_mac_without_colon) + String("/state");
            mqttClient.publish(str_topico.c_str(), dados_espnow.state, true);

        } else if( strcmp(dados_espnow.msg_type, "ASK_STATE") == 0 ) {            
            // Buscar estado do PIN/LED no Array de Estados MQTT
            const char* estado = mqttClient.buscarEstado(client_mac_without_colon);
                        
            // Respode a mensagem "CHANNEL_REQ"
            String str_mac_formatado = wifiManager.formatarMac(String(client_mac_without_colon)); // Formata string MAC
            strcpy(dados_espnow.msg_type, "SET_STATE");
            strcpy(dados_espnow.state, estado);  
            strcpy(dados_espnow.mac_server, server_mac);
            strcpy(dados_espnow.mac_client, str_mac_formatado.c_str());    
            
            // Envia SET_STATE aos clientes esp-now   
            espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));  

        }
        // Checa e processa o tipo de mensagens (Se nao for para retransmissao)
        else if( strcmp(dados_espnow.msg_type, "TELEMETRY") == 0 ) {   

            // Converte em json os dados recebidos dos sensores           
            JsonBuilder json;
            //json.add("server_mac", server_mac);
            //json.add("state", dados_espnow.state);
            json.add("mac_address", client_mac);
            json.add("basic_topic", MQTT_BASE_TOPIC);
            json.add("identificador", client_mac);
            json.add("ups1_current", dados_espnow.u1_current, 2);
            json.add("ups1_voltage", dados_espnow.u1_voltage, 2);
            json.add("ups1_temperature", dados_espnow.u1_temperature, 2);
            json.add("ups1_humidity", dados_espnow.u1_humidity);             
            String dados = json.build();            

            // evita enviar JSON quebrado
            if (!dados.startsWith("{") || !dados.endsWith("}")) {
                return; 
            }           

            // Evia telemetria para o MQTT Broker
            char attributes_topic[64]; 
            snprintf(attributes_topic, sizeof(attributes_topic), "%s%s/attributes", MQTT_BASE_TOPIC, client_mac_without_colon);
            mqttClient.publish(attributes_topic, dados.c_str(), true);
            //imprimeln(attributes_topic);
            //imprimeln(dados.c_str());

            json.reset();
        }     

    }); // FIM de Callback de recepção ESP-NOW


    ///////////////////////////////
    // Callback de envio ESP-NOW //
    //////////////////////////////
    espnow.onSend([](const uint8_t *mac, bool ok){       
        
    }); // FIM de Callback de envio ESP-NOW
    

    // Rede WiFi
    wifiManager.connect(WIFI_SSID, WIFI_PASSWORD);    

    // Conectar CLIENTE MQTT   
    conect2MqttBroker();    

    // INICIALIZA DISPLAY OLED
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
        
    
    // Envia mensagem "REPAIR_MSG" aos clientes para Forçar reemparelhamento
    strcpy(dados_espnow.msg_type, "REPAIR_MSG");    
    espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));       
}




////////////////////
// loop principal //
////////////////////
void loop() {
    // NOTA: AsyncElegantOTA não precisa de nada no loop     
       
    // CLIENTE MQTT       
    if (!mqttClient.connected()) {

        unsigned long now = millis();

        if (now - lastReconnectAttempt > 5000) {

            imprimeln(F("Tentando reconectar MQTT..."));
            lastReconnectAttempt = now;              
            // Conectar CLIENTE MQTT
            conect2MqttBroker();

            // Envia mensagem "REPAIR_MSG" aos clientes para Forçar reemparelhamento
            strcpy(dados_espnow.msg_type, "REPAIR_MSG");    
            espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData)); 
        }

    } 
    else {
        mqttClient.loop();  // processa mensagens recebidas
    }


    // DISPLAY OLED
    static unsigned long lastUpdate = 0;
    static uint8_t bat = 0;
    if (millis() - lastUpdate > 2000) { 
        bat = (bat + 5) % 101;
        oled.clear();
        oled.printText(0, 0, "Bat.", 2);
        oled.showBattery(bat);
        oled.update();
        lastUpdate = millis();
    }


    // Processa dados recebidos via ESP-NOW (se houver)
    /*if(newDataReady){
        newDataReady = false; // reseta o flag para aguardar novos dados
    }
    else{
        return; // Se não há novos dados, sai do loop para economizar recursos
    }*/
      

    // Envia um "ping", aos clientes esp-now, a cada X segundos
    static unsigned long lastPing = 0;
    if (millis() - lastPing > 5000) {   
        lastPing = millis();         

        // Converte inteiro a char []
        uint8_t channel = espnow.getCurrentWiFiChannel();;
        uint8_t len = String(channel).length();
        char cz_channel[len + 1];  
        String(channel).toCharArray(cz_channel, sizeof(cz_channel));
        
        // Envia mensagem "ALIVE_MSG" aos clientes
        strcpy(dados_espnow.msg_type, "ALIVE_MSG");    
        strcpy(dados_espnow.state, cz_channel);
        espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
    }    

}


///////////////////
// CallBack MQTT //
///////////////////
void mqttCallback(char* topic, byte* payload, unsigned int length) {

  static char msg[MQTT_BUFFER_SIZE];  
  if (length >= sizeof(msg)) {
    imprimeln(F("Payload demasiado grande"));
    return;
  }

  memcpy(msg, payload, length);
  msg[length] = '\0';     
  
  /*imprime(F("Topico: "));
  imprimeln(topic);
  imprime(F("Payload: "));
  imprimeln(msg);*/
    
  // Obtem MAC contido no toico
  String str_mac = wifiManager.extrairIdDoTopico(String(topic));
  // Obtem MAC do Servidor
  char server_mac[13];
  wifiManager.getMacAddress(server_mac, false); 

  
  if( strcmp(msg, "ON") != 0 && strcmp(msg, "OFF") != 0 ){ // MENSAGEM OTA

    char command[32];
    char mac[18];    

    if (mqttClient.parseOtaMessageOptimized(msg, command, mac) && strcmp(server_mac, str_mac.c_str()) == 0 ){

        if( strcmp(command, "OTA_MODE") != 0 ){
            return; // Se a mensagem recebida não for "OTA_MODE", ignora.
        }
        
        // Rede WiFi
        //wifiManager.criar_ap(ssid, password);
        // Iniciar servidor web
        webServer.begin();
        // Inicializar OTA elegante
        ElengantOTA::begin(&servidorHTTP);    
    }
    else if (mqttClient.parseOtaMessageOptimized(msg, command, mac) && strcmp(server_mac, str_mac.c_str()) != 0 ){
        
        if( strcmp(command, "OTA_MODE") != 0 ){
            return; // Se a mensagem recebida não for "OTA_MODE", ignora.
        }

        // Envia mensagem "OTA_MODE" aos clientes
        strcpy(dados_espnow.msg_type, command);    
        strcpy(dados_espnow.mac_client, mac);        
        espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
    }

  } else if ( strcmp(server_mac, str_mac.c_str()) == 0 && strstr(topic, "/set")) { // ON/OFF SERVIDOR ESP-NOW

    

#if defined(ESP32)
    // Ligar/Desligar PIN/LED (Logica directa)
    if (strcmp(msg, "OFF") == 0) {      
      digitalWrite(LED_PIN, LOW);
    } else if (strcmp(msg, "ON") == 0) {      
      digitalWrite(LED_PIN, HIGH);
    }
#elif defined(ESP8266)
    // Ligar/Desligar PIN/LED (Logica invertida)
    if (strcmp(msg, "ON") == 0) {      
      digitalWrite(LED_PIN, LOW);
    } else if (strcmp(msg, "OFF") == 0) {      
      digitalWrite(LED_PIN, HIGH);
    }
#endif

    // Publicar Estado do PIN/LED
    String str_topico = MQTT_BASE_TOPIC + str_mac + String("/state");
    mqttClient.publish(str_topico.c_str(), msg, true);
    
  } else if ( strcmp(server_mac, str_mac.c_str()) != 0 && strstr(topic, "/set")){ // ON/OFF CLIENTES ESP-NOW 
    
    // Respode a mensagem "CHANNEL_REQ"
    String str_mac_formatado = wifiManager.formatarMac(str_mac); // Formata string MAC
    strcpy(dados_espnow.msg_type, "SET_STATE");
    strcpy(dados_espnow.state, msg);  
    strcpy(dados_espnow.mac_server, server_mac);
    strcpy(dados_espnow.mac_client, str_mac_formatado.c_str());    
    
    // Envia SET_STATE aos clientes esp-now   
    espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));  

    // Atualiza Estado do PIN/LED no Array de Estados MQTT
    mqttClient.atualizarEstado(str_mac.c_str(), msg);

  }


}// mqttCallback(char* topic, byte* payload, unsigned int length)


// Função para conectar ao broker MQTT e publicar a entidade do dispositivo
void conect2MqttBroker(){

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
    const char* component  = MQTT_DEVICE_DOMAIN; 
    wifiManager.getMacAddress(mac_colon, true);   
    wifiManager.getMacAddress(node_id, false);    
    snprintf(unique_id, sizeof(unique_id), "%s%s", mqttClient.entity_id_prefix, node_id);      
    const char* device_name = node_id;
    const char* entity_name = "";
    const char* base_topic = MQTT_BASE_TOPIC;  
    const char* manufacturer = MQTT_DEVICE_MANUFACTURER;    
    const char* model = host_name; // (*J*) Deve ser único.
    const char* extra_config;

    // Guarda node_id do dispositivo principal para ser usado nas entidades dependentes
    strcpy(mqttClient.main_device_node_id, node_id);
    
    mqttClient.publishDiscoveryEntity(
        mqttClient, "device", component, node_id, entity_name, base_topic, unique_id,       
        mac_colon, device_name, manufacturer, model, extra_config = nullptr 
    );    

}