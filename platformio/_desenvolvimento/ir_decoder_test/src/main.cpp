/*
    ESP8266:
    ========
    GPIO5 --> BC7215A TX | GPIO16 -> BC7215A RX
    GPIO14 -> BC7215A MOD | GPIO4 --> BC7215A BUSY
    3.3V ---> BC7215A VCC

    ESP32:
    ======
    GPIO25 -> BC7215A TX | GPIO33 -> BC7215A RX
    GPIO27 -> BC7215A MOD | GPIO26 -> BC7215A BUSY
    3.3V ---> BC7215A VCC
 */

#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
//#include <bc7215.h>
//#include <bc7215ac.h>
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

const uint8_t MOD_PIN  = 27;
const uint8_t BUSY_PIN = 26;
const uint8_t LED_PIN  = 2;

#if defined(ESP32)
  HardwareSerial bc7215Serial(1);        
#elif defined(ESP8266)
  #define bc7215Serial Serial1 
#endif

BC7215 bc7215Board(bc7215Serial, MOD_PIN, BUSY_PIN);
BC7215AC ac(bc7215Board);

const char* MODES[]      = {"Auto", "Cool", "Heat", "Dry", "Fan", "Keep", "N/A"};
const char* FANSPEED[]   = {"Auto", "Low", "Med", "High", "Keep", "N/A"};
const char* PWR_STATUS[] = {"OFF", "ON", "TOGGLE", "N/A"};
//bool paired = false;

// Protótipos de função
void emparelharAc();
void descodificarSinalAc();
void callback_mqtt_rx(char* topic, byte* payload, unsigned int length);
void setup_wifi();
void reconnectMQTT();
void enviaDadosAc(int temp, int mode_index, int fan_index, int power_index);

void setup() {
    Serial.begin(115200);
    
#if defined(ESP32)
    bc7215Serial.begin(19200, SERIAL_8N2, 25, 33);
#elif defined(ESP8266)
    bc7215Serial.begin(19200, SERIAL_8N2);
#endif

    delay(100);
    bc7215Board.setRx();
    delay(50);

    pinMode(LED_PIN, OUTPUT);
#if defined(ESP32)        
    digitalWrite(LED_PIN, LOW); 
#elif defined(ESP8266)        
    digitalWrite(LED_PIN, HIGH);
#endif 

    setup_wifi();

    mqttClient.setServer(mqtt_server, 1883);
    mqttClient.setCallback(callback_mqtt_rx); 
   
    // Carrega dados evitando cópias desnecessárias na Stack
    lerDadosEEPROM(esp_cfg_data); 
    ac.startCapture();

    // Iniciar servidor web
    webServer.begin();
    // Inicializar OTA elegante
    ElengantOTA::begin(&servidorHTTP); 

    if (!paired && !esp_cfg_data.configurado) {  
        imprimeln(F("\nPAREAMENTO DO PROTOCOLO DO AC"));
        imprimeln(F("Ligue o controlo remoto no Modo COOL a 25°C e pressione FAN.\n"));  
        imprimeln(F("Esperando o sinal para o Pareamento..."));
    }

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
       
    if (!paired && !esp_cfg_data.configurado) {  
        emparelharAc();
        return;
    }
    else if (!paired && esp_cfg_data.configurado) {
        if (ac.init(esp_cfg_data.dataPkt, esp_cfg_data.formatPkt)) {
            paired = true;
            imprimeln(F(">>> Sucesso: Configuração restaurada! Pronto para operar."));
            return;
        } else {
            imprimeln(F("Falha ao inicializar com dados salvos. Forçando novo pareamento..."));
            esp_cfg_data.configurado = false;
            emparelharAc();
        }
    }
    else if (paired && esp_cfg_data.configurado) {
        descodificarSinalAc();
    }   

    // Monitor Serial Commands
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();

        if (input == "d") {        
            memset(&esp_cfg_data, 0, sizeof(ConfigDados));
            salvarDadosEEPROM(esp_cfg_data);
            paired = false;            
        }
        if (input == "r") {        
            static uint8_t contador = 0; 
            char nome[DEVICE_NAME_SIZE];
            sprintf(nome, "NOME-%d", ++contador);
            strncpy(esp_cfg_data.device_name, nome, DEVICE_NAME_SIZE);
            esp_cfg_data.device_name[DEVICE_NAME_SIZE] = '\0';
            salvarDadosEEPROM(esp_cfg_data);                      
        }
    }

}// Fim de loop()



void emparelharAc() {   

    if (ac.signalCaptured()) {
        ac.stopCapture();
        imprimeln(F("Sinal recebido."));

        if (ac.init()) {
            paired = true;
            imprimeln(F("Pareamento OK!"));

            esp_cfg_data.configurado = true;                       
            esp_cfg_data.formatPkt = *ac.getFormatPkt(); 
            esp_cfg_data.dataPkt = *((bc7215DataMaxPkt_t*)ac.getDataPkt());
            
            salvarDadosEEPROM(esp_cfg_data);
            digitalWrite(LED_PIN, LOW);             
            ac.startCapture();
        } else {
            imprimeln(F("Falha no pareamento. Tente novamente."));
            ac.startCapture();
        }
    } else {
        static unsigned long ultimoPisca = 0;
        if (millis() - ultimoPisca > 250) {
            ultimoPisca = millis();
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));            
        }
    }
}

void descodificarSinalAc() {
    if (ac.signalCaptured()) {
        ac.stopCapture();

        int temp = -1, mode = -1, fan = -1, power = -1;

        if (ac.parse(temp, mode, fan, power)) {
            StaticJsonDocument<JSON_SIZE> doc;           

            if (power >= 0 && power <= 2) {                 
                doc["power"] = power;
                mqttClient.publish(MQTT_TOPIC_POWER_STATE, PWR_STATUS[power], true);
            }            
            
            if (temp >= 16 && temp <= 30) {
                doc["temp"] = temp;
                mqttClient.publish(MQTT_TOPIC_TEMP_STATE, String(temp).c_str(), true);
            }

            if (mode >= 0 && mode <= 4) {
                doc["mode"] = mode;
                mqttClient.publish(MQTT_TOPIC_MODE_STATE, MODES[mode], true);
            }
            
            if (fan >= 0 && fan <= 3) {
                doc["fan"] = fan;
                mqttClient.publish(MQTT_TOPIC_FAN_STATE, FANSPEED[fan], true); 
            }

            doc["for_tx"] = false;

            char payload[JSON_SIZE];
            serializeJson(doc, payload);
            mqttClient.publish(MQTT_TOPIC_AC_STATES, payload, true); 
            
            imprimef("POWER: %s TEP: %d MODE: %s FAN: %s\n", PWR_STATUS[power],temp,MODES[mode],FANSPEED[fan]);

        } else {
            imprimeln(F("Falha ao decodificar sinal."));
        }
        ac.startCapture();
    }
}

/*void salvarDadosEEPROM(const ConfigDados &dados) {
#if defined(ESP32)
    Preferences prefs;
    prefs.begin("esp32cfg_mem", false);
    prefs.putBytes("esp32cfg", &dados, sizeof(ConfigDados));
    prefs.end();
    digitalWrite(LED_PIN, LOW); 
    imprimeln(F("Dados Gravados via NVS Preferences (ESP32)."));
#elif defined(ESP8266)
    EEPROM.begin(EEPROM_TAMANHO);
    EEPROM.put(0, dados);
    EEPROM.commit();
    EEPROM.end();
    imprimeln(F("Dados Gravados via Flash EEPROM (ESP8266)."));
#endif
}

void lerDadosEEPROM(ConfigDados &dadosDestino) { 
    memset(&dadosDestino, 0, sizeof(ConfigDados));      
#if defined(ESP32)
    Preferences prefs;
    prefs.begin("esp32cfg_mem", true);
    if (prefs.isKey("esp32cfg")) {
        prefs.getBytes("esp32cfg", &dadosDestino, sizeof(ConfigDados));
    }
    prefs.end();
#elif defined(ESP8266)
    EEPROM.begin(EEPROM_TAMANHO);
    EEPROM.get(0, dadosDestino);
    EEPROM.end();
#endif
}
*/


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
}

void callback_mqtt_rx(char* topic, byte* payload, unsigned int length) {
    if (strcmp(topic, MQTT_TOPIC_AC_STATES) == 0) {  
        StaticJsonDocument<JSON_SIZE> doc;
        DeserializationError error = deserializeJson(doc, payload, length);
        if (error) {
            imprime(F("Erro JSON: "));
            imprimeln(error.c_str());
            return;
        }

        int8_t temp = doc["temp"];
        int8_t mode_index = doc["mode"];
        int8_t fan_index = doc["fan"];
        int8_t power_index = doc["power"];
        bool for_tx = doc["for_tx"];

        if (for_tx) {       
            if (power_index == 1) {
                enviaDadosAc(temp, mode_index, fan_index, -1);  
            }
            else if (power_index == 0) {
                unsigned long startTime = millis();            
                bc7215Board.setTx();
                delay(50);   
                ac.off(); 
                while (ac.isBusy() && (millis() - startTime < 3000)) {
                    delay(10);
                }
                bc7215Board.setRx();
            }
        }   
    }
}

void reconnectMQTT() {
    while (!mqttClient.connected()) {
        imprime(F("[MQTT] Conectando..."));
        String clientID = "ESP32-TCL-AC-" + String(random(0xffff), HEX);
        
        if (mqttClient.connect(clientID.c_str(), mqtt_user, mqtt_pass)) {
            imprimeln(F("Conectado!"));
            mqttClient.subscribe(MQTT_TOPIC_POWER_STATE);
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
        } else {
            delay(5000);
        }
    }
}

void enviaDadosAc(int temp, int mode_index, int fan_index, int power_index) {
    unsigned long startTime = millis();
    bc7215Board.setTx();
    delay(50);
    ac.setTo(temp, mode_index, fan_index, power_index);
    while (ac.isBusy() && (millis() - startTime < 3000)) {
        delay(10);
    }
    bc7215Board.setRx();
}