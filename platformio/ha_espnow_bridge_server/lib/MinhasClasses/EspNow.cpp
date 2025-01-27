
#include "EspNow.h"

#define DEBUG 1


#if DEBUG == 1
    #define imprime(x) Serial.print(x)
    #define imprimeln(x) Serial.println(x)
#else
    #define imprime(x)
    #define imprimeln(x)
#endif

uint8_t WIFI_CH = 0;
char SERVER_MAC[6];

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
  #elif defined(ESP32)
           
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
    char macStr[18];    
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    imprime("Destino: ");
    imprime(macStr);
    imprime(" Status: ");
    imprimeln(status == 0 ? "Successo." : "Falha: "); 
  }  
#elif defined(ESP32) 
  void callback_tx_esp_now(const uint8_t *mac_addr, esp_now_send_status_t status) {    
    char macStr[18];    
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    imprime("Destino: ");
    imprime(macStr);
    imprime(" Status: ");
    imprimeln(status == ESP_NOW_SEND_SUCCESS ? "Successo." : "Falha: ");
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
    if(strcmp(pld.comando, "ASK_CHANNEL") == 0){
      EmparelharDispositivos(mac, incomingData);
    }  
    else{
      ProcessarPayload(mac, incomingData);
    }
  
  }// end callback_rx_esp_now(...)
#elif defined(ESP32) 
  void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len){ 
   
    Payload pld = {};   
    memcpy(&pld, incomingData, sizeof(pld)); 
    // Emparelha dispositivo requerente e envia resposta
    if(strcmp(pld.comando, "ASK_CHANNEL") == 0){
      EmparelharDispositivos(mac, incomingData);
    }    
    else{
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

  uint8_t wifi_channel = WIFI_CH;
  Payload pld = {};   
  memcpy(&pld, incomingData, sizeof(pld));
  bool exists = esp_now_is_peer_exist(mac);
 
  // Se o par nao existe, Emparelha dispositivo requerente e envia resposta
  if( !exists && strcmp(pld.comando, "ASK_CHANNEL") == 0){    
      
    #if defined(ESP8266) 
      uint8_t result = esp_now_add_peer(mac, ESP_NOW_ROLE_SLAVE, wifi_channel, NULL, 0); 
      if (result != 0) {
        imprimeln(F("Failed to add peer"));
        return;
      }  
      else{
        strcpy(pld.comando, "ASW_CHANNEL");
        pld.canal_wifi = wifi_channel;
        memcpy(&pld.mac_origem, SERVER_MAC, sizeof(SERVER_MAC));
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
        strcpy(pld.comando, "ASW_CHANNEL");
        pld.canal_wifi = wifi_channel;
        memcpy(&pld.mac_origem, SERVER_MAC, sizeof(SERVER_MAC));
        esp_now_send(mac, (uint8_t *)&pld, sizeof(pld));       
      }
    #endif
    
  }
  else{ // Se o par existe, envia resposta
    strcpy(pld.comando, "ASW_CHANNEL");
    pld.canal_wifi = wifi_channel;
    memcpy(&pld.mac_origem, SERVER_MAC, sizeof(SERVER_MAC));
    esp_now_send(mac, (uint8_t *)&pld, sizeof(pld));
  } 

}// End



///////////////////////
// Processar Payload //
//////////////////////
#if defined(ESP8266) 
void ProcessarPayload(uint8_t * mac, uint8_t* incomingData){
#elif defined(ESP32) 
void ProcessarPayload(const uint8_t * mac, const uint8_t* incomingData){
#endif

  Payload pld = {};   
  memcpy(&pld, incomingData, sizeof(pld));

  if(strcmp(pld.comando, "PING_REQUEST") == 0){    
    memcpy(pld.comando, "PING_RESPONSE", sizeof(pld.comando));
    memcpy(pld.mac_origem, SERVER_MAC, sizeof(pld.mac_destino));
    memcpy(pld.mac_destino, mac, sizeof(pld.mac_destino));
    pld.canal_wifi = WIFI_CH;
      
    esp_now_send(mac, (uint8_t *)&pld, sizeof(pld));
  }

  char macStr[18];
  imprime("Origem: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
    pld.mac_origem[0], pld.mac_origem[1], pld.mac_origem[2], pld.mac_origem[3], pld.mac_origem[4], pld.mac_origem[5]);
  imprimeln(macStr);
  char macStr1[18];
  imprime("Destino: ");
  snprintf(macStr1, sizeof(macStr1), "%02x:%02x:%02x:%02x:%02x:%02x",
    pld.mac_destino[0], pld.mac_destino[1], pld.mac_destino[2], pld.mac_destino[3], pld.mac_destino[4], pld.mac_destino[5]);
  imprimeln(macStr1);
  imprime("Comando: ");
  imprimeln(pld.comando);    
  imprime("Canal: ");
  imprimeln(pld.canal_wifi);
  imprimeln("-------------------");

}


