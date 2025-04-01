
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

#define DEBUG 1

#if DEBUG == 1
#define imprime(x) Serial.print(x)
#define imprimeln(x) Serial.println(x)
#else
#define imprime(x)
#define imprimeln(x)
#endif

 

uint8_t macSTA[6] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};
uint8_t macAP[6]  = {0x02, 0x00, 0x00, 0x00, 0x00, 0x02}; 
uint8_t macaddr[6];  
const char* ssid = "TPLINK";  
const char* password = "gregorio@2012"; 

ClienteMqtt* objmqtt;
const char* mqtt_server = "192.168.0.5";
uint16_t mqtt_port = 1883;
const char* mqtt_user = "";
const char* mqtt_pwd = "";


/////////////
// setup() //
/////////////
void setup() {

  #if DEBUG == 1
    // Inicializar Serial
    Serial.begin(115200);
    
    // Aguardar pela inicializacao da Serual
    while (!Serial); 
  #endif 

 
  // Criar AP 
  //RedeWifi wifi_obj("ESPNOW", "123456789", "WIFI_AP", true, macAP);  
  //wifi_obj.CriaRedeWifi("");
  

  // Conectar-se a uma rede WiFi
#if defined(ESP8266) 
  RedeWifi wifi_obj(ssid, password, "WIFI_STA", true, macSTA);     //modo: WIFI_AP_STA | WIFI_STA | WIFI_AP
  wifi_obj.ConectaRedeWifi("");                                   // STA_IP_MODE = [DHCP | STATIC]
#elif defined(ESP32) 
  // Para ESP32 deve ser "WIFI_AP_STA"
  RedeWifi wifi_obj(ssid, password, "WIFI_AP_STA", true, macSTA);   //modo: WIFI_AP_STA | WIFI_STA | WIFI_AP
  wifi_obj.ConectaRedeWifi("");                                     // STA_IP_MODE = [DHCP | STATIC]
#endif


  // Obter MAC   
  memcpy(macaddr, wifi_obj.GetMacAddress(), sizeof(macaddr));

  // Iniciar ESP-NOW  
  EspNow objespnow(&macaddr[0]);   
  objmqtt = new ClienteMqtt(mqtt_server, mqtt_port, mqtt_user, mqtt_pwd);
 
}// setup()



////////////
// loop() //
////////////
void loop() {

  // Cliente MQTT
  if (!clientMqtt.connected()) {
    objmqtt->ReconnectarMqtt(mqtt_user, mqtt_pwd);
  }

  clientMqtt.loop();    
  
  if(send_auto_discovery == true){
   
    char discoveryTopic[TOPICS_SIZE];  
    char friedly_name[FRIENDLY_NAME_SIZE] = "wemos_d1_mini"; // TODO: Rever logica
    char command_topic[TOPICS_SIZE];
    char state_topic[TOPICS_SIZE];
    char availability_topic[TOPICS_SIZE];
    char payload[PAYLOAD_SIZE];
    
    sprintf(discoveryTopic, "homeassistant/%s/%s/config", DEVICE_CLASS, CLIENT_MAC);
    sprintf(command_topic, "bridge/%s/%s/set", DEVICE_CLASS, CLIENT_MAC);
    sprintf(state_topic, "bridge/%s/%s/state", DEVICE_CLASS, CLIENT_MAC);
    sprintf(availability_topic, "bridge/%s/%s/available", DEVICE_CLASS, CLIENT_MAC);
    DynamicJsonDocument doc(512);    
  
    doc["name"] = DEVICE_NAME;//friedly_name;
    doc["unique_id"] = CLIENT_MAC;
    doc["command_topic"] = command_topic;
    doc["state_topic"] = state_topic;       
    doc["device_class"] = DEVICE_CLASS;    // Opcional (pode ser "light" se for uma luz)
    //doc["optimistic"] = false;           // Força atualização via state_topic
    //doc["retain"] = true;                // Mantém estado após reinício 
    //doc["availability_topic"] = availability_topic;
    
    serializeJson(doc, payload);

    //imprimeln(discoveryTopic);
    //imprimeln(payload);

    // Publica o dispositivo para o autodiscovery
    clientMqtt.publish(discoveryTopic, payload, true);

    // Subescrever nos topicos para obtencao do estados dos dispositivos
    //clientMqtt.subscribe(state_topic);
    clientMqtt.subscribe(command_topic);

    send_auto_discovery = false;
   
  } 

  // Publica o estado do dispositivo
  if( true ){ // TODO: Rever logica
    char state_topic[TOPICS_SIZE];
    sprintf(state_topic, "bridge/%s/%s/state", DEVICE_CLASS, CLIENT_MAC);
    //imprimeln(state_topic);
    clientMqtt.publish(state_topic, pin_state ? "ON" : "OFF");    
  }
  

  #if defined(ESP8266) 
    delay(50);
  #elif defined(ESP32) 
    vTaskDelay(pdMS_TO_TICKS(50));
  #endif
 
}// loop()



