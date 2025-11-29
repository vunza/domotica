
#include "EspNow.h"

#define DEBUG 1


#if DEBUG == 1
    #define imprime(x) Serial.print(x)
    #define imprimeln(x) Serial.println(x)
#else
    #define imprime(x)
    #define imprimeln(x)
#endif

uint8_t pin_state = 0;
uint8_t WIFI_CH = 0;
char SERVER_MAC[18];
char CLIENT_MAC[18];
char DEVICE_NAME[DEVICE_NAME_SIZE];
char DEVICE_CLASS[DEVICE_CLASS_SIZE];
boolean send_auto_discovery = false;
Generica obj_generica;


#if defined(ESP32) 
  esp_now_peer_info_t peerInfo;   
#endif


// Construtor
EspNow::EspNow(const uint8_t *mac){  

  WIFI_CH = WiFi.channel();  
  memcpy(SERVER_MAC, mac, sizeof(SERVER_MAC));  

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    imprimeln(F("Error initializing ESP-NOW"));
    return;
  }
  else  {
    imprimeln(F("ESP-NOW inicializado com sucesso!"));
  }

  // ESP-NOW
  #if defined(ESP8266) 
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);   
    wifi_set_channel(WIFI_CH);
  #elif defined(ESP32)
    esp_wifi_set_channel(WIFI_CH, WIFI_SECOND_CHAN_NONE);         
  #endif 

  esp_now_register_send_cb(callback_tx_esp_now);
  esp_now_register_recv_cb(callback_rx_esp_now);

  imprime(F("Canal WiFi: "));
  imprimeln(WIFI_CH);

}



//////////////////////////////////////////
// Callback, ESP-NOW, when data is sent //
//////////////////////////////////////////
#if defined(ESP8266) 
  void callback_tx_esp_now(uint8_t *mac_addr, uint8_t status) { 
    /*char macStr[18];    
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    imprime("Destino: ");
    imprime(macStr);
    imprime(" Status: ");
    imprimeln(status == 0 ? "Successo." : "Falha: "); */
  }  
#elif defined(ESP32) 
  void callback_tx_esp_now(const uint8_t *mac_addr, esp_now_send_status_t status) {    
    /*char macStr[18];    
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    imprime("Destino: ");
    imprime(macStr);
    imprime(" Status: ");
    imprimeln(status == ESP_NOW_SEND_SUCCESS ? "Successo." : "Falha: ");*/
  }
#endif
  


//////////////////////////////////////////////
// Callback, ESP-NOW, when data is received //
//////////////////////////////////////////////
#if defined(ESP8266) 
  void callback_rx_esp_now(uint8_t * mac, uint8_t *incomingData, uint8_t len) {

    Payload pld = {};   
    memcpy(&pld, incomingData, sizeof(pld));   

    // Emparelha dispositivo requerente e envia resposta
    if(pld.tipo_msg == ASK_PAIRING){
      EmparelharDispositivos(mac, incomingData);
    }  
    else if(pld.tipo_msg == DATA){          
      ProcessarPayload(mac, incomingData);
    }
    else if(pld.tipo_msg == PING_REQUEST){
      ProcessarPayload(mac, incomingData);
    }
  }// end callback_rx_esp_now(...)  
#elif defined(ESP32) 
  void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len){ 
  
    Payload pld = {};   
    memcpy(&pld, incomingData, sizeof(pld)); 

    // Emparelha dispositivo requerente e envia resposta
    if(pld.tipo_msg == ASK_PAIRING){
      EmparelharDispositivos(mac, incomingData);
    }    
    else if(pld.tipo_msg == DATA){
      ProcessarPayload(mac, incomingData);
    }
    else if(pld.tipo_msg == PING_REQUEST){
      ProcessarPayload(mac, incomingData);
    }

  }// end callback_rx_esp_now(...)
#endif
 
  

////////////////////////////
// Emparelha dispositivos //
////////////////////////////
#if defined(ESP8266) 
  void EmparelharDispositivos(uint8_t * mac, uint8_t* incomingData){  
#elif defined(ESP32) 
  void EmparelharDispositivos(const uint8_t * mac, const uint8_t* incomingData){
#endif

  // Guarda o MAC do Clinte, após retirar o caracter ':' e conversão de Byte Array para const char
  const uint8_t maxLength = 18;  
  char buffer[maxLength];  
  snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        
  // Eliminar o caracter ':'
  obj_generica.RemoveMacDelimiters(':', buffer, buffer);   
    
  memcpy(CLIENT_MAC, buffer, sizeof(CLIENT_MAC));   

  // Registo dos Clientes/Pares
  uint8_t wifi_channel = WIFI_CH;
  Payload pld = {};   
  memcpy(&pld, incomingData, sizeof(pld));
  bool exists = esp_now_is_peer_exist(mac);

  // Guarda o Nome e a Classe do Clinte
  memcpy(DEVICE_NAME, pld.nome_dispositivo, sizeof(DEVICE_NAME));
  memcpy(DEVICE_CLASS, pld.classe_dispositivo, sizeof(DEVICE_CLASS));

 
   // Se o par nao existe, Emparelha dispositivo requerente e envia resposta
  if( !exists && pld.tipo_msg == ASK_PAIRING){         
    #if defined(ESP8266) 
      uint8_t result = esp_now_add_peer(mac, ESP_NOW_ROLE_SLAVE, wifi_channel, NULL, 0); 
      if (result != 0) {
        imprimeln(F("Failed to add peer"));
        return;
      }  
      else{
        pld.tipo_msg = CONFIRM_PAIRING;
        pld.canal_wifi = wifi_channel;
        memcpy(&pld.mac_servidor, SERVER_MAC, sizeof(SERVER_MAC));
        memcpy(&pld.mac_cliente, mac, sizeof(pld.mac_cliente));
        esp_now_send(mac, (uint8_t *)&pld, sizeof(pld));               
      } 
    #elif defined(ESP32)     
      memcpy(peerInfo.peer_addr, mac, 6);
      peerInfo.channel = wifi_channel;
      peerInfo.encrypt = false;
      if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        imprimeln(F("Failed to add peer"));
        return;
      }
      else{        
        pld.tipo_msg = CONFIRM_PAIRING;
        pld.canal_wifi = wifi_channel;
        memcpy(&pld.mac_servidor, SERVER_MAC, sizeof(SERVER_MAC));
        memcpy(&pld.mac_cliente, mac, sizeof(pld.mac_cliente));       
        esp_now_send(mac, (uint8_t *)&pld, sizeof(pld));       
      }
    #endif
    
  }
  else if( exists && pld.tipo_msg == ASK_PAIRING){ // Se o par existe, envia resposta
    pld.tipo_msg = CONFIRM_PAIRING;
    pld.canal_wifi = wifi_channel;
    memcpy(&pld.mac_servidor, SERVER_MAC, sizeof(SERVER_MAC));
    memcpy(&pld.mac_cliente, mac, sizeof(pld.mac_cliente));
    esp_now_send(mac, (uint8_t *)&pld, sizeof(pld));    
  }

  // Controlo do envio do topico auto discovery
  send_auto_discovery = true; 
  
}// End



///////////////////////
// Processar Payload //
///////////////////////
#if defined(ESP8266) 
void ProcessarPayload(uint8_t * mac, uint8_t* incomingData){
#elif defined(ESP32) 
void ProcessarPayload(const uint8_t * mac, const uint8_t* incomingData){
#endif

  Payload pld = {};   
  memcpy(&pld, incomingData, sizeof(pld));  

  /*char macStr1[18];
  imprime("Cliete: ");
  snprintf(macStr1, sizeof(macStr1), "%02x:%02x:%02x:%02x:%02x:%02x",
    pld.mac_cliente[0], pld.mac_cliente[1], pld.mac_cliente[2], pld.mac_cliente[3], pld.mac_cliente[4], pld.mac_cliente[5]);
  imprimeln(macStr1);
  imprime("Comando: ");
  imprimeln(pld.tipo_msg);   
  imprime("Estado do PIN: ");
  imprimeln(pld.estado_pin);   
  imprimeln("-------------------");*/

  if(pld.tipo_msg == DATA){
    pin_state = pld.estado_pin;
  }
  else if(pld.tipo_msg == PING_REQUEST){
    pld.tipo_msg = PING_RESPONSE;
    memcpy(pld.nome_dispositivo, DEVICE_NAME, sizeof(DEVICE_NAME));
    memcpy(&pld.mac_servidor, SERVER_MAC, sizeof(SERVER_MAC));
    memcpy(&pld.mac_cliente, mac, sizeof(pld.mac_cliente));    
    esp_now_send(mac, (uint8_t *)&pld, sizeof(pld));
  }

}


////////////////////////////////////////////
// Converter MAC (string para Byte array) //
////////////////////////////////////////////
uint8_t* ConverteMacString2Byte(const char* cz_mac) {

  //static uint8_t MAC[6];
  uint8_t* MAC = (uint8_t*)calloc(6, sizeof(uint8_t));
  char* ptr;

  MAC[0] = strtol(cz_mac, &ptr, HEX );
  for ( uint8_t i = 1; i < 6; i++ ) {
    MAC[i] = strtol(ptr + 1, &ptr, HEX );
  }

  return MAC;
}// converteMacString2Byte(const char* cz_mac)

