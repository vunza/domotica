 /*
    ESP8266:
    ========
    GPIO5 --> BC7215A TX
    GPIO16 -> BC7215A RX
    GPIO14 -> BC7215A MOD
    GPIO4 --> BC7215A BUSY
    3.3V ---> BC7215A VCC


    ESP32:
    ======
    GPIO25 -> BC7215A TX
    GPIO33 -> BC7215A RX
    GPIO27 -> BC7215A MOD
    GPIO26 -> BC7215A BUSY
    3.3V ---> BC7215A VCC
 */

#include <Arduino.h>
#include <EEPROM.h>
#include "ctrl_debug.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <bc7215.h>
#include <bc7215ac.h>
//#include <eeprom_manager_pro/eeprom_manager_pro.h>
#include <async_elegant_ota/async_elegant_ota.h>
#include <web_server/web_server.h>

#if defined(ESP32)
  #include <WiFi.h>     
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

#if defined(ESP32)
  #include <Preferences.h>
  Preferences prefs;
#elif defined(ESP8266)
  #include <EEPROM.h>
  #define EEPROM_TAMANHO sizeof(ConfigDados) 
#else
  #error "Placa não suportada!"
#endif

typedef struct {
    char wifiSSID[25]; 
    char wifiPass[25];
    char mqtt_server_ip[16];
    char mqtt_user[25];
    char mqtt_password[25];
    char device_name[25];
    bool configurado; 
    bc7215FormatPkt_t formatPkt;    
    bc7215DataMaxPkt_t dataPkt;                       
}ConfigDados;


//EEPROMManagerPro eeprom_manager_pro;
ConfigDados esp_cfg_data; // (*J*) Nao crie objectos da estructura na funcao loo() 



// Servidor Web
AsyncWebServer servidorHTTP(80);
WebServer webServer(&servidorHTTP);


const char* ssid     = "TPLINK";
const char* password = "gregorio@2012";
const char* mqtt_server = "192.168.0.5"; 
const char* mqtt_user   = "";      
const char* mqtt_pass   = ""; 

/*
    TÓPICOS MQTT (Home Assistant Climate);
    Devem ser passados aos arquivos: 
    - /home/wnr/homeassistant/config/configuration.yaml
    - /home/wnr/homeassistant/config/www/my_cards/js/globals.js e aos arquivos do modulo que os precisem
    - main.cpp do codigo ESP32
*/ 
#define MQTT_TOPIC_POWER_STATE "homeassistant/escritorio/ac/power/state"
#define MQTT_TOPIC_MODE_STATE  "homeassistant/escritorio/ac/mode/state"
#define MQTT_TOPIC_TEMP_STATE  "homeassistant/escritorio/ac/temp/state"
#define MQTT_TOPIC_FAN_STATE   "homeassistant/escritorio/ac/fan/state"
#define MQTT_TOPIC_AC_STATES  "homeassistant/escritorio/ac/states"
#define MQTT_TOPIC_AC_ATTRIBUTES  "homeassistant/escritorio/ac/attributes"
#define JSON_SIZE 128


WiFiClient     espClient;
PubSubClient   mqttClient(espClient);

const uint8_t MOD_PIN  = 27;
const uint8_t BUSY_PIN = 26;
const uint8_t LED_PIN = 2; // GPIO2


// Compatibilidade de Hardware Serial para ESP32 e ESP8266
#if defined(ESP32)
  HardwareSerial bc7215Serial(1);        
#elif defined(ESP8266)
  // No ESP8266 a Serial1 é usada por padrão. Ajuste os pinos no seu circuito físico se necessário.
  #define bc7215Serial Serial1 
#endif
BC7215 bc7215Board(bc7215Serial, MOD_PIN, BUSY_PIN);
BC7215AC ac(bc7215Board);

const char* MODES[] = {"Auto", "Cool","Heat","Dry","Fan","Keep","N/A"};
const char* FANSPEED[] = {"Auto","Low","Med","High","Keep","N/A"};
const char* PWR_STATUS[] = {"OFF","ON","TOGGLE","N/A"};
bool paired = false;


void emparelharAc();
void descodificarSinalAc();
void callback_mqtt_rx(char* topic, byte* payload, unsigned int length);
void setup_wifi();
void reconnectMQTT();
void enviaDadosAc(int temp, int mode_index, int fan_index, int power_index);

void salvarDadosEEPROM(ConfigDados dados){
    #if defined(ESP32)
        prefs.begin("esp32cfg_mem", false);
        // Transforma a struct inteira em bytes brutos e salva no NVS
        prefs.putBytes("esp32cfg", &dados, sizeof(dados));
        prefs.end();                
        imprimeln(F("Dados do Emparelhamneto Gravados na EEPROM via Preferences (ESP32)."));
    #elif defined(ESP8266)
        EEPROM.begin(EEPROM_TAMANHO);
        // .put gerencia automaticamente a gravação sequencial dos arrays da struct
        EEPROM.put(0, dados);
        EEPROM.commit();
        EEPROM.end();
        imprimeln(F("Dados do Emparelhamneto Gravados na EEPROM via EEPROM (ESP8266)."));
    #endif
}// Fim de salvarDadosEEPROM(ConfigDados dados)

ConfigDados lerDadosEEPROM() { 
    ConfigDados dadosTemporarios;  // (*J*) Nao crie objectos da estructura na funcao loo()      
    #if defined(ESP32)
        prefs.begin("esp32cfg_mem", true);
        if(prefs.isKey("esp32cfg")) {
            prefs.getBytes("esp32cfg", &dadosTemporarios, sizeof(dadosTemporarios));
        }
        prefs.end();
    #elif defined(ESP8266)
        EEPROM.begin(EEPROM_TAMANHO);
        EEPROM.get(0, dadosTemporarios);
        EEPROM.end();
    #endif
        return dadosTemporarios;
}


///////////
// setup //
///////////
void setup(){
    Serial.begin(115200);
    bc7215Serial.begin(19200, SERIAL_8N2, 25, 33);
    delay(100);
    bc7215Board.setRx();
    delay(50);

    pinMode(LED_PIN, OUTPUT);

    #if defined(ESP32)        
        digitalWrite(LED_PIN, LOW); 
    #elif defined(ESP8266)        
        digitalWrite(LED_PIN, HIGH); // Lógica invertida (HIGH = desligado)
    #endif 

    setup_wifi();

    // Parametros MQTT
    mqttClient.setServer(mqtt_server, 1883);
    mqttClient.setCallback(callback_mqtt_rx); 

    imprimeln(F("\n=== PAREAMENTO AC ==="));
    imprimeln(F("Ligue controlo remoto e coloque-o no Modo COOL e TEPERATURA = 25°C"));    
    imprimeln(F("Depois pressione FAN no controlo remoto.\n"));

       
    // Ler Dados da EEPROM
    esp_cfg_data = lerDadosEEPROM(); 

    ac.startCapture();

    // Iniciar servidor web
    webServer.begin();

    // Inicializar OTA elegante
    ElengantOTA::begin(&servidorHTTP); 


} // Fim do setup()



//////////
// loop //
//////////
void loop(){

    // Garante estabilidade das conexões MQTT e Wi-Fi
    if (WiFi.status() != WL_CONNECTED) {
        setup_wifi();
    }

    if (!mqttClient.connected()) {
        reconnectMQTT();
    }

    // Processa a pilha MQTT ativa
    mqttClient.loop(); 
       
    // Processa Emparelhamento do AC
    if (!paired && esp_cfg_data.configurado == false){    
        emparelharAc();
        return;
    }
    else if(!paired && esp_cfg_data.configurado == true){
        if (ac.init(esp_cfg_data.dataPkt, esp_cfg_data.formatPkt)) {
            paired = true;
            imprimeln(F(">>> Sucesso: Configuração restaurada! Pronto para operar."));
            imprimeln(F("Aguardando comandos ou sinais do controlo remoto físico..."));
            return;

        } else {
            imprimeln(F("Nenhum dado prévio encontrado. Iniciando modo de emparelhamento obrigatório..."));
            imprimeln(F("Configure o controle remoto do AC para: <Modo Refrigeração (Cool), 25°C>"));
            imprimeln(F("Aponte para o receptor e pressione o botão <Controlo de Fan (Velocidade)>..."));
            //ac.startCapture();
            emparelharAc();
        }
    }
    else if(paired && esp_cfg_data.configurado == true){
        // Descoifica codigos de comandos fisicos e os publica no topico MQTT_TOPIC_AC_STATES
        descodificarSinalAc();
    }   


    // --- ENVIO DE COMANDOS VIA MONITOR SERIAL ---
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();

        unsigned long startTime = millis();

        if (input.length() > 0) {

            if(input == "d"){                        
                esp_cfg_data = {0};
                salvarDadosEEPROM(esp_cfg_data);
                paired = false;
                Serial.println("CLEAN");
            }

        }// if (input.length() > 0)
    }
    

}// Fim do loop()



///////////////////////////////
// Emparelha AC com o MODULO //
///////////////////////////////
void emparelharAc(){
    if (ac.signalCaptured()) {
        ac.stopCapture();

        imprimeln(F("Sinal recebido."));

        if (ac.init()) {
            paired = true;

            imprimeln(F("Pareamento OK!"));
            imprimeln(F("Agora envie comandos do controle."));
            imprimeln();

            // Guardar configuracoes na EEPROM                
            esp_cfg_data.configurado = true;                       

            // 1. Capturando o FORMAT PACKET (ac.getFormatPkt() retorna um ponteiro bc7215FormatPkt_t*)
            bc7215FormatPkt_t ptrFormat = *ac.getFormatPkt();          
            esp_cfg_data.formatPkt = ptrFormat; 

            // 2. Capturando o DATA PACKET (ac.getDataPkt() retorna um ponteiro bc7215DataMaxPkt_t*)
            bc7215DataMaxPkt_t* ptrData = (bc7215DataMaxPkt_t*)ac.getDataPkt();
            esp_cfg_data.dataPkt = *ptrData;
            
            salvarDadosEEPROM(esp_cfg_data);

            #if defined(ESP32)        
                digitalWrite(LED_PIN, LOW); 
            #elif defined(ESP8266)        
                digitalWrite(LED_PIN, HIGH); // Lógica invertida (HIGH = desligado)
            #endif 

            ac.startCapture();
        }
        else{
            imprimeln(F("Falha no pareamento."));
            imprimeln(F("Tente novamente."));
            ac.startCapture();
        }
    }
    else{
        imprimeln(F("Aguardando emparelhamento!"));
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        delay(250);
    }

}//Fim de emparelharAc()



/////////////////////////
// descodificarSinalAc //
/////////////////////////
void descodificarSinalAc(){

    if (ac.signalCaptured()){
        ac.stopCapture();

        int temp  = -1, mode  = -1, fan   = -1, power = -1;

        if (ac.parse(temp, mode, fan, power)){
            
            StaticJsonDocument<JSON_SIZE> doc;           

            if (power >= 0 && power <= 2){                 
                doc["power"] = power;
                mqttClient.publish(MQTT_TOPIC_POWER_STATE, PWR_STATUS[power], true);
            }            
            
            if(temp >= 16 && temp <= 30){
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
        }
        else{
            imprimeln(F("Falha ao decodificar sinal."));
        }

        ac.startCapture();
    }

}// Fim de descodificarSinalAc()




///////////////////////////
// Inicialização de Rede //
///////////////////////////
void setup_wifi() {

    delay(10);
    imprimeln();
    imprime(F("Conectando em ")); 
    imprimeln(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        imprime(F("."));
    }

    imprimeln(F("\n[WiFi] Conectado com sucesso!"));
    imprime(F("[WiFi] IP obtido: ")); 
    imprimeln(WiFi.localIP());

}// Fim de setup_wifi()


///////////////////////////////
// Recebe mensagens via MQTT //
///////////////////////////////
void callback_mqtt_rx(char* topic, byte* payload, unsigned int length) {
  
  // Converte o payload (array de bytes) em uma String amigável
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }    

  if (String(topic) == MQTT_TOPIC_AC_STATES) {  

    StaticJsonDocument<JSON_SIZE> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error){
        imprimeln("Erro JSON: ");
        imprimeln(error.c_str());
        return;
    }
    int8_t temp = doc["temp"];
    int8_t mode_index = doc["mode"];
    int8_t fan_index = doc["fan"];
    int8_t power_index = doc["power"];
    boolean for_tx = doc["for_tx"];

    if(for_tx == true || for_tx == 1){       

        if(power_index == 1){
            enviaDadosAc(temp, mode_index, fan_index, -1);  
        }
        else if(power_index == 0){
            unsigned long startTime = millis();            
            bc7215Board.setTx();
            delay(50);   
            ac.off(); 
            while (ac.isBusy() && (millis() - startTime < 3000)){
                delay(10);
            }
            bc7215Board.setRx();
        }
        
        //Serial.printf("POWER: %d TEMP: %d MODE: %d FAN: %d FOR_TX: %d\n", power_index, temp, mode_index, fan_index, for_tx);
    }   
  }
  else if (String(topic) == MQTT_TOPIC_MODE_STATE) {          
    
  }
  else if (String(topic) == MQTT_TOPIC_TEMP_STATE) {

  }
  else if (String(topic) == MQTT_TOPIC_FAN_STATE) {

  } 
  else if (String(topic) == MQTT_TOPIC_POWER_STATE) {
    
  }  
  
} // Fim de callback_mqtt_rx()


////////////////////////////////////////
// Reconectar/Conectar ao Broker MQTT //
////////////////////////////////////////
void reconnectMQTT() {
    while (!mqttClient.connected()) {
        imprime(F("[MQTT] Tentando conectar ao Broker..."));
        // Cria um ID único para o cliente MQTT baseado no endereço MAC
        String clientID = "ESP32-TCL-AC-" + String(random(0xffff), HEX);
        
        if (mqttClient.connect(clientID.c_str(), mqtt_user, mqtt_pass)) {
            imprimeln(F("Conectado!"));
            // Se você quiser receber comandos do HA para o ESP, adicione o subscribe aqui:            
            mqttClient.subscribe(MQTT_TOPIC_POWER_STATE);
            mqttClient.subscribe(MQTT_TOPIC_MODE_STATE);
            mqttClient.subscribe(MQTT_TOPIC_TEMP_STATE);
            mqttClient.subscribe(MQTT_TOPIC_FAN_STATE);
            mqttClient.subscribe(MQTT_TOPIC_AC_STATES);           

            // Publicar atrubutos (usados no modulo javascript do homeassistant para activar menu Programar)
            StaticJsonDocument<JSON_SIZE> doc;                         
            doc["mac_address"] = WiFi.macAddress();
            doc["basic_topic"] = "homeassistant/";
            doc["identificador"] = WiFi.macAddress();            
            char payload[JSON_SIZE];
            serializeJson(doc, payload);
            mqttClient.publish(MQTT_TOPIC_AC_ATTRIBUTES, payload, true); 

        } else {
            imprime(F("Falha, rc=")); 
            imprime(mqttClient.state());
            imprimeln(F(" Tentando novamente em 5 segundos."));
            delay(5000);
        }
    }
}// Fim de reconnectMQTT()


///////////////////////////////////
// Envia Dados ao Aparelho de AC //
///////////////////////////////////
void enviaDadosAc(int temp, int mode_index, int fan_index, int power_index){

    unsigned long startTime = millis();

    bc7215Board.setTx();
    delay(50);
    ac.setTo(temp, mode_index, fan_index, power_index);
    while (ac.isBusy() && (millis() - startTime < 3000)){
        delay(10);
    }
    bc7215Board.setRx();

}// Fim de enviaDadosAc(int temp, int mode_index, int fan_index, int power_index)
