
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


#if defined(ESP8266) 
  #define PIN_LED 12//2//23 
#elif defined(ESP32) 
  #define PIN_LED 23//19//2
#endif

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
Generica objGenerica;

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

  pinMode(PIN_LED, OUTPUT); 

 
  // Criar AP 
  //RedeWifi wifi_obj("ESPNOW", "123456789", "WIFI_AP", true, macAP);  
  //wifi_obj.CriaRedeWifi("");
  

  // Conectar-se a uma rede WiFi
#if defined(ESP8266) 
  RedeWifi wifi_obj(ssid, password, "WIFI_STA", false, macSTA);     //modo: WIFI_AP_STA | WIFI_STA | WIFI_AP
  wifi_obj.ConectaRedeWifi("");                                   // STA_IP_MODE = [DHCP | STATIC]
#elif defined(ESP32) 
  // Para ESP32 deve ser "WIFI_AP_STA"
  RedeWifi wifi_obj(ssid, password, "WIFI_AP_STA", false, macSTA);   //modo: WIFI_AP_STA | WIFI_STA | WIFI_AP
  wifi_obj.ConectaRedeWifi("");                                     // STA_IP_MODE = [DHCP | STATIC]
#endif


  // Obter MAC   
  memcpy(macaddr, wifi_obj.GetMacAddress(), sizeof(macaddr));

  // Iniciar ESP-NOW  
  EspNow objespnow(&macaddr[0]);   
  objmqtt = new ClienteMqtt(mqtt_server, mqtt_port, mqtt_user, mqtt_pwd);


   
  // Guarda o nome do dispositivo
#if defined(ESP32) 
  memcpy(DEVICE_NAME, WiFi.getHostname(), sizeof(DEVICE_NAME));
#elif defined(ESP8266)  
  memcpy(DEVICE_NAME, WiFi.hostname().c_str(), sizeof(DEVICE_NAME));
#endif

  // TDOD: Manusear aqui o nome do dev, em vez do hostname
  // Eliminar o caracter '-'
  objGenerica.RemoveMacDelimiters('-', DEVICE_NAME, DEVICE_NAME); 


  // Guardar, como byte array, o MAC do dispositivo.
  char buffer[18];
  memcpy(buffer, WiFi.macAddress().c_str(), sizeof(buffer));
  memcpy(SAVE_SERVER_MAC, buffer, sizeof(SAVE_SERVER_MAC));// Guarda o MAC do Server
  
  // Eliminar o caracter ':'
  objGenerica.RemoveMacDelimiters(':', buffer, buffer);
  
  // Guardar dados do dispositivo Servidor
  memcpy(DEVICE_NAME, DEVICE_NAME, sizeof(DEVICE_NAME));
  memcpy(DEVICE_CLASS, "switch", sizeof(DEVICE_CLASS)); // TODO: Rever logica p/atribuir classe do dev
  memcpy(CLIENT_MAC, buffer, sizeof(CLIENT_MAC)); 

  // Auto discovery do Se5rvidor
  send_auto_discovery = true;  
 
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
    char friedly_name[FRIENDLY_NAME_SIZE];
    char command_topic[TOPICS_SIZE];
    char state_topic[TOPICS_SIZE];
    char availability_topic[TOPICS_SIZE];
    char payload[PAYLOAD_SIZE];
    
    memcpy(friedly_name, DEVICE_NAME, sizeof(friedly_name)); // TODO: Rever logica, nos clients
    sprintf(discoveryTopic, "homeassistant/%s/%s/config", DEVICE_CLASS, CLIENT_MAC);
    sprintf(command_topic, "bridge/%s/%s/set", DEVICE_CLASS, CLIENT_MAC);
    sprintf(state_topic, "bridge/%s/%s/state", DEVICE_CLASS, CLIENT_MAC);
    sprintf(availability_topic, "bridge/%s/%s/available", DEVICE_CLASS, CLIENT_MAC);
    DynamicJsonDocument doc(512);    
  
    doc["name"] = friedly_name;
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

    // Subescrever o topico para obtencao do estado dos dispositivos   
    clientMqtt.subscribe(command_topic);
       
    send_auto_discovery = false;
   
  } 


  // Alterar estado do PIN do Servidor
  if( set_server_pin == true ){
    set_server_pin = false;
    digitalWrite(PIN_LED, server_pin);
  }

  // TODO: Rever esta logica, faz bloquear o processo de reemparelhamento 
  // quando o cliente é ESP32 e o master é eSP8266
  #if defined(ESP8266) 
    //delay(50);
  #elif defined(ESP32) 
    //vTaskDelay(pdMS_TO_TICKS(50));
  #endif
 
}// loop()



