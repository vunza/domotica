
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
  esp_now_peer_info_t broadcastPeer, peerInfo;    
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
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);   
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
    /*Serial.print("Last Packet Send Status: ");
    if (status == 0){
      Serial.println("Delivery success");
    }
    else{
      Serial.println("Delivery fail");
    }*/
  }
#endif
//////////////////////////////////////////
// Callback, ESP-NOW, when data is sent //
//////////////////////////////////////////
#if defined(ESP32) 
  void callback_tx_esp_now(const uint8_t *mac_addr, esp_now_send_status_t status) {    
    /*Serial.print("Last Packet Send Status: ");
    if (status == 0){
      Serial.println("Delivery success");
    }
    else{
      Serial.println("Delivery fail");
    }*/
  }  
#endif



//////////////////////////////////////////////
// Callback, ESP-NOW, when data is received //
//////////////////////////////////////////////
#if defined(ESP8266) 
  void callback_rx_esp_now(uint8_t * mac, uint8_t *incomingData, uint8_t len) { // Para ESP8266
    
    EmparelharDispositivos(mac, incomingData);
    
    /*Pairing pld = {};   
    memcpy(&pld, incomingData, sizeof(pld));   

    bool exists = esp_now_is_peer_exist(mac);

    // Emparelha dispositivo requerente e envia resposta
    if( !exists && strcmp(pld.comando, "PAIRING_REQUEST") == 0){
      //ESP_NOW_ROLE_SLAVE
      uint8_t result = esp_now_add_peer(mac, ESP_NOW_ROLE_COMBO, WiFi.channel(), NULL, 0); 
      if (result != 0) {
        imprimeln(F("Failed to add peer"));
        return;
      }   

      strcpy(pld.comando, "PAIRING_CONFIRM");
      pld.canal_wifi = WiFi.channel();
      memcpy(&pld.mac, SERVER_MAC, sizeof(SERVER_MAC));
      esp_now_send(mac, (uint8_t *)&pld, sizeof(pld));
    }
    else{
      strcpy(pld.comando, "PAIRING_CONFIRM");
      pld.canal_wifi = WiFi.channel();
      memcpy(&pld.mac, SERVER_MAC, sizeof(SERVER_MAC));
      esp_now_send(mac, (uint8_t *)&pld, sizeof(pld));
    } */    
  
  }// end callback_rx_esp_now(...)
#elif defined(ESP32) 
  void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len){ 
   
   EmparelharDispositivos(mac, incomingData);

    /*Pairing pld = {};   
    memcpy(&pld, incomingData, sizeof(pld));

    bool exists = esp_now_is_peer_exist(mac);

    // Add receiver's MAC address  
    if( !exists  && strcmp(pld.comando, "PAIRING_REQUEST") == 0){
      memcpy(peerInfo.peer_addr, mac, 6);
      peerInfo.channel = WiFi.channel();
      peerInfo.encrypt = false;
      if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
      }

      strcpy(pld.comando, "PAIRING_CONFIRM");
      pld.canal_wifi = WiFi.channel();
      memcpy(&pld.mac, SERVER_MAC, sizeof(SERVER_MAC));
      esp_now_send(mac, (uint8_t *)&pld, sizeof(pld));
    }  
    else{
      strcpy(pld.comando, "PAIRING_CONFIRM");
      pld.canal_wifi = WiFi.channel();
      memcpy(&pld.mac, SERVER_MAC, sizeof(SERVER_MAC));
      esp_now_send(mac, (uint8_t *)&pld, sizeof(pld));
    }*/

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

  /*char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    //imprimeln(macStr);
    //imprimeln( (const char*) incomingData); */ 

    Pairing pld = {};   
    memcpy(&pld, incomingData, sizeof(pld));
    bool exists = esp_now_is_peer_exist(mac);
 
    // Emparelha dispositivo requerente e envia resposta
    if( !exists && strcmp(pld.comando, "PAIRING_REQUEST") == 0){
      
      #if defined(ESP8266) 
        uint8_t result = esp_now_add_peer(mac, ESP_NOW_ROLE_COMBO, WiFi.channel(), NULL, 0); 
        if (result != 0) {
          imprimeln(F("Failed to add peer"));
          return;
        }   
      #elif defined(ESP32) 
        memcpy(peerInfo.peer_addr, mac, 6);
        peerInfo.channel = WiFi.channel();
        peerInfo.encrypt = false;
        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
          Serial.println("Failed to add peer");
          return;
        }
      #endif

      strcpy(pld.comando, "PAIRING_CONFIRM");
      pld.canal_wifi = WiFi.channel();
      memcpy(&pld.mac, SERVER_MAC, sizeof(SERVER_MAC));
      esp_now_send(mac, (uint8_t *)&pld, sizeof(pld));
    }
    else{
      strcpy(pld.comando, "PAIRING_CONFIRM");
      pld.canal_wifi = WiFi.channel();
      memcpy(&pld.mac, SERVER_MAC, sizeof(SERVER_MAC));
      esp_now_send(mac, (uint8_t *)&pld, sizeof(pld));
    }      
}

