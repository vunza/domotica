
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
#if defined(ESP32) 
  esp_now_peer_info_t peerInfo;  
#endif

// Construtor
EspNow::EspNow(){

  WIFI_CH = WiFi.channel();   

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
  #endif

  #if defined(ESP32) 
    //Force espnow to use a specific channel
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(WIFI_CH, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
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
    
  }
#endif


//////////////////////////////////////////
// Callback, ESP-NOW, when data is sent //
//////////////////////////////////////////
#if defined(ESP32) 
  void callback_tx_esp_now(const uint8_t *mac_addr, esp_now_send_status_t status) {    
     
  }  
#endif


//////////////////////////////////////////////
// Callback, ESP-NOW, when data is received //
//////////////////////////////////////////////
#if defined(ESP8266) 
  void callback_rx_esp_now(uint8_t * mac, uint8_t *incomingData, uint8_t len) { // Para ESP8266
  
    imprimeln( (const char*) incomingData);   

    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    imprimeln(macStr);

    bool exists = esp_now_is_peer_exist(mac);

    if( !exists ){
      esp_now_add_peer(mac, ESP_NOW_ROLE_SLAVE, WiFi.channel(), NULL, 0);
    }
    else{
      uint8_t data[] = "PAIRING_ANSWER";
      esp_now_send(mac, data, sizeof(data));
    }
       
  
  }// end callback_rx_esp_now(...)
#elif defined(ESP32) 
  void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len){ 
   
    //imprimeln( (const char*) incomingData);    
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    imprimeln(macStr);  

    bool exists = esp_now_is_peer_exist(mac);

    // Add receiver's MAC address  
    if( !exists ){
      memcpy(peerInfo.peer_addr, mac, 6);
      peerInfo.channel = WiFi.channel();
      peerInfo.encrypt = false;
      if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
      }
    }  
    else{
      uint8_t data[] = "Hello, ESP-NOW!";
      esp_err_t result = esp_now_send(mac, data, sizeof(data));
    }    
    
  }// end callback_rx_esp_now(...)
#endif
  

