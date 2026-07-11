/*
    Para ESP32:
    VC-02 TX -- GPIO 16 (RX2)
    VC-02 RX -- GPIO 17 (TX2)

    Para ESP8266:
    VC-02 TX -- GPIO 5 (RX)
    VC-02 RX -- GPIO 4 (TX)
*/

/*
    LINKS:
    - https://www.youtube.com/watch?v=P0UDOAZBqr4&t=256s
    - https://github.com/techiesms/VC02-UART-exmaple-codes
    - https://www.youtube.com/watch?v=P0UDOAZBqr4
    - https://voice.ai-thinker.com/#/
*/

#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <async_elegant_ota/async_elegant_ota.h>
#include <web_server/web_server.h>
#include <eeprom_manager_pro/eeprom_manager_pro.h>
#include "ctrl_debug.h"

#if defined(ESP32)
  #include <WiFi.h>     
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>  
#endif


// Configurações de Rede de Backup (Caso não use os da EEPROM)
const char* ssid         = "TPLINK";
const char* password     = "gregorio@2012";
const char* mqtt_server  = "192.168.0.5"; 
const char* mqtt_user    = "";      
const char* mqtt_pass    = ""; 

// Servidor Web
AsyncWebServer servidorHTTP(80);
WebServer webServer(&servidorHTTP);

// Tópicos MQTT
#define MQTT_TOPIC_POWER_STATE    "homeassistant/escritorio/ac/power/state"
#define MQTT_TOPIC_MODE_STATE     "homeassistant/escritorio/ac/mode/state"
#define MQTT_TOPIC_TEMP_STATE     "homeassistant/escritorio/ac/temp/state"
#define MQTT_TOPIC_FAN_STATE      "homeassistant/escritorio/ac/fan/state"
#define MQTT_TOPIC_AC_STATES      "homeassistant/escritorio/ac/states"
#define MQTT_TOPIC_AC_ATTRIBUTES  "homeassistant/escritorio/ac/attributes"
#define JSON_SIZE 128

WiFiClient espClient;
PubSubClient mqttClient(espClient);

#if defined(ESP32)
  // Configuração nativa para ESP32 (Usa Hardware Serial 2)
  #define RX_PIN 16
  #define TX_PIN 17
  #define LED_PIN 2
  #define vc02Serial Serial2
  uint16_t receivedValue = 0;

#elif defined(ESP8266)
  // Configuração para ESP8266 (Usa Software Serial)
  #include <SoftwareSerial.h>
  #define RX_PIN 5  // Pino D1 no NodeMCU
  #define TX_PIN 4  // Pino D2 no NodeMCU
  #define LED_PIN 2 // LED integrado (GPIO2 / D4)
  
  SoftwareSerial vc02Serial(RX_PIN, TX_PIN);
#else
  #error "Placa não suportada! Selecione ESP32 ou ESP8266 na IDE Arduino."
#endif


// Protótipos de função
void callback_mqtt_rx(char* topic, byte* payload, unsigned int length);
void setup_wifi();
void reconnectMQTT();



//////////////
// setup() //
/////////////
void setup() {
  
    Serial.begin(115200);
    
    #if defined(ESP32)
        // Inicializa a Hardware Serial do ESP32
        Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
        imprimeln(F("Placa Detectada: ESP32 (Usando HardwareSerial2)"));
    #elif defined(ESP8266)
        // Inicializa a Software Serial do ESP8266
        vc02Serial.begin(115200);
        imprimeln(F("Placa Detectada: ESP8266 (Usando SoftwareSerial)"));
    #endif

    pinMode(LED_PIN, OUTPUT);
    
  
  imprimeln(F("Pronto! Aguardando comandos de voz do VC-02..."));


    setup_wifi();

    mqttClient.setServer(mqtt_server, 1883);
    mqttClient.setCallback(callback_mqtt_rx); 
   
    // Carrega dados evitando cópias desnecessárias na Stack
    //lerDadosEEPROM(esp_cfg_data); 
    

    // Iniciar servidor web
    webServer.begin();
    // Inicializar OTA elegante
    ElengantOTA::begin(&servidorHTTP);     

}// Fim de setup()



////////////
// loop() //
////////////
void loop() {

    if (WiFi.status() != WL_CONNECTED) {
        setup_wifi();
    }

    if (!mqttClient.connected()) {
        reconnectMQTT();
    }

    mqttClient.loop(); 
       
    // Verifica se há dados recebidos do VC-02
    if (Serial2.available()) {
        // Read the incoming bytes
        byte highByte = vc02Serial.read();
        byte lowByte = vc02Serial.read();

        // Combine the two bytes into a single 16-bit value
        receivedValue = (highByte << 8) | lowByte;

        // Print the received value in HEX format
        imprimef("Received HEX value: 0x%02X\n", receivedValue);
    }

    
}// Fim de loop()




////////////////////////////
// COnfigurações de WiFi //
///////////////////////////
void setup_wifi() {
    delay(10);
    imprime(F("\n[WiFi] Conectando-se a rede ")); 
    imprimeln(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        imprime(F("."));
    }

    imprimeln(F("\n[WiFi] Conectado!"));
    imprime(F("[WiFi] IP: "));
    imprimeln(WiFi.localIP());
    imprime(F("[WiFi] MAC: "));
    imprimeln(WiFi.macAddress());
    imprimeln();

}// Fim de setup_wifi()



//////////////////////////////////////////
// Callback para receber mensagens MQTT //
//////////////////////////////////////////
void callback_mqtt_rx(char* topic, byte* payload, unsigned int length) {

    if (strcmp(topic, MQTT_TOPIC_AC_STATES) == 0) {  
        StaticJsonDocument<JSON_SIZE> doc;
        DeserializationError error = deserializeJson(doc, payload, length);
        if (error) {
            imprime(F("Erro JSON: "));
            imprimeln(error.c_str());
            return;
        }        

    }// Fim de if (strcmp(topic, MQTT_TOPIC_AC_STATES) == 0)

}// Fim de callback_mqtt_rx()



///////////////////////////////////////////
// Função para reconectar ao broker MQTT //
///////////////////////////////////////////
void reconnectMQTT() {
    while (!mqttClient.connected()) {
        imprime(F("[MQTT] Conectando..."));
        String clientID = "ESP32-TCL-AC-" + String(random(0xffff), HEX);
        
        if (mqttClient.connect(clientID.c_str(), mqtt_user, mqtt_pass)) {
            imprimeln(F("Conectado!"));
            /*mqttClient.subscribe(MQTT_TOPIC_POWER_STATE);
            mqttClient.subscribe(MQTT_TOPIC_MODE_STATE);
            mqttClient.subscribe(MQTT_TOPIC_TEMP_STATE);
            mqttClient.subscribe(MQTT_TOPIC_FAN_STATE);
            mqttClient.subscribe(MQTT_TOPIC_AC_STATES);           

            StaticJsonDocument<JSON_SIZE> doc;                         
            doc["mac_address"] = WiFi.macAddress();
            doc["basic_topic"] = "homeassistant/";
            doc["identificador"] = WiFi.macAddress();            
            char payload[JSON_SIZE];
            serializeJson(doc, payload);
            mqttClient.publish(MQTT_TOPIC_AC_ATTRIBUTES, payload, true); 
            */
        } else {
            delay(5000);
        }
    }

}// Fim de reconnectMQTT()



