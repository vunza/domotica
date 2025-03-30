
#include "ClienteMqtt.h"

#define DEBUG 1


#if DEBUG == 1
    #define imprime(x) Serial.print(x)
    #define imprimeln(x) Serial.println(x)
#else
    #define imprime(x)
    #define imprimeln(x)
#endif


WiFiClient espClient;
PubSubClient clientMqtt(espClient);
char hostname[16];


/////////////////
// Construtor //
////////////////
ClienteMqtt::ClienteMqtt(const char* mqtt_server, uint16_t mqtt_port, const char* mqtt_user, const char* mqtt_pwd){
  
  // Inicializar atributos
  _mqtt_port = mqtt_port;
  strcpy(_mqtt_server, mqtt_server);  
  strcpy(_mqtt_user, mqtt_user);
  strcpy(_mqtt_pwd, mqtt_pwd);

  // Inializar MQTT
  clientMqtt.setServer(_mqtt_server, _mqtt_port);
  clientMqtt.setCallback(callback); 
  
  // Obter Nome do Dispositivo
  #if defined(ESP8266) 
    strcpy(hostname, WiFi.hostname().c_str());
  #elif defined(ESP32) 
    strcpy(hostname, WiFi.getHostname());
  #endif
}
   

/////////////////////////////////////////////////
// Função de callback para mensagens recebidas //
/////////////////////////////////////////////////
void callback(char* topic, byte* payload, unsigned int length) {  

  String aux_topic = String(topic);
  char aux_ayload[8];
  
  uint8_t lastSlash = aux_topic.lastIndexOf('/'); // Encontrar a posição da última "/"
  uint8_t secondLastSlash = aux_topic.lastIndexOf('/', lastSlash - 1); // Encontrar a posição do penúltimo "/"

  String deviceID = aux_topic.substring(secondLastSlash + 1, lastSlash); 
  String command = aux_topic.substring(lastSlash + 1);

  if( command == "set" ){
    
    for (uint i = 0; i < length; i++) {
      aux_ayload[i] = (char)payload[i];
    }

    Serial.println(aux_ayload);
  }
  
}


/////////////////////////////
// Reconectar Cliente MQTT //
/////////////////////////////
void ClienteMqtt::ReconnectarMqtt(const char* mqtt_user, const char* mqtt_pwd){  
  while (!clientMqtt.connected()) {
    imprimeln(F("Conectando ao MQTT..."));
    if (clientMqtt.connect(hostname, mqtt_user, mqtt_pwd)) {
      
      imprimeln(F("Conectado!"));
      
      // Publica o dispositivo para o autodiscovery
      /*clientMqtt.publish("homeassistant/switch/esp8266_switch/config",
                     "{\"name\": \"teste_switch\", \"command_topic\": \"homeassistant/switch/esp8266_switch/set\", \"state_topic\": \"homeassistant/switch/esp8266_switch/state\"}",
                     true);*/
    } 
    else {
      imprime(F("Falha ao conectar. Erro: "));
      imprimeln(clientMqtt.state());      
      
      #if defined(ESP8266) 
        delay(5000);
      #elif defined(ESP32) 
        vTaskDelay(pdMS_TO_TICKS(5000));
      #endif
    }
  }
}