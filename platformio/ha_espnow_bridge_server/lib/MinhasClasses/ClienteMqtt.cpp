
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
char SAVE_SERVER_MAC[18];
Generica ObjGenerica;
boolean set_server_pin = false;
uint8_t server_pin = 0;

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
  Payload pld = {}; 
  uint8_t pin_state = 0;
  
  // TODO: Melhhorar logica, tornar generica, para qualquer topico
  uint8_t lastSlash = aux_topic.lastIndexOf('/'); // Encontrar a posição da última "/"
  uint8_t secondLastSlash = aux_topic.lastIndexOf('/', lastSlash - 1); // Encontrar a posição do penúltimo "/"

  String dev_mac = aux_topic.substring(secondLastSlash + 1, lastSlash); 
  String command = aux_topic.substring(lastSlash + 1);

  if( command == "set" ){
    
    char payloadStr[length + 1];
    memcpy(payloadStr, payload, length);
    payloadStr[length] = '\0';

    //Serial.println(aux_ayload);
    if( strcmp(payloadStr, "ON") == 0 ){
      pin_state = 1;
    }
    else if( strcmp(payloadStr, "OFF") == 0 ){
      pin_state = 0;
    }

    // Actualiza estado do PIN no HA
    char state_topic[TOPICS_SIZE];
    
    uint8_t var_len = dev_mac.length() + 1;    
    char AUX_CLIENT_MAC[var_len];
    dev_mac.toCharArray(AUX_CLIENT_MAC, var_len);

    sprintf(state_topic, "bridge/%s/%s/state", DEVICE_CLASS, AUX_CLIENT_MAC);    
    clientMqtt.publish(state_topic, pin_state ? "ON" : "OFF"); 



    // Actualiza estado do PIN no dispositivo
    pld.tipo_msg = CMD_SET;
    pld.estado_pin = pin_state;
    memcpy(pld.nome_dispositivo, DEVICE_NAME, sizeof(DEVICE_NAME));


    // Converte a MAC HEX, sem delimitadores ("58bf254d301f") , em bytes array
    uint8_t macBytes[6]; 
    for (int i = 0; i < 6; i++) {
      char byteStr[3]; // Armazena cada par HEX (ex: "58")
      strncpy(byteStr, AUX_CLIENT_MAC + (i * 2), 2); // Copia 2 caracteres
      byteStr[2] = '\0'; // Garante terminação nula
      macBytes[i] = strtol(byteStr, NULL, 16); // Converte para byte
    }

    // Retira os caracteres ':' do MAC
    ObjGenerica.RemoveMacDelimiters(':', SAVE_SERVER_MAC, SAVE_SERVER_MAC);
    
    // Envia o estado do PIN
    if( strcmp(AUX_CLIENT_MAC, SAVE_SERVER_MAC) == 0 ){ // Servidor
      set_server_pin = true;
      server_pin = pin_state;
    }
    else{ // Cliente
      esp_now_send(macBytes, (uint8_t *)&pld, sizeof(pld));
    }
    
  }
  else if( command == "state" ){

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
        //delay(100);
      #elif defined(ESP32) 
        //vTaskDelay(pdMS_TO_TICKS(100));
      #endif

      ReconnectarMqtt(mqtt_user, mqtt_pwd);

    }
  }
}