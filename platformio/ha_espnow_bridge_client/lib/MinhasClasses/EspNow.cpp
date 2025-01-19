
#include "EspNow.h"

#define DEBUG 1


#if DEBUG == 1
    #define imprime(x) Serial.print(x)
    #define imprimeln(x) Serial.println(x)
#else
    #define imprime(x)
    #define imprimeln(x)
#endif



// Construtor
EspNow::EspNow(){ 

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    imprimeln(F("Error initializing ESP-NOW"));
    return;
  }
  else  {
    imprimeln(F("\nESP-NOW inicializado com sucesso!"));
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

}


//////////////////////////////////////////
// Callback, ESP-NOW, when data is sent //
//////////////////////////////////////////
#if defined(ESP8266) 
  void callback_tx_esp_now(uint8_t *mac_addr, uint8_t status) { 
    /* char macStr[18];
    //imprime("Destino: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.println(macStr);
    imprime(" Status:\t");
   imprime(status == ESP_NOW_SEND_SUCCESS ? "Successo." : "Falha: ");*/
}
#endif


//////////////////////////////////////////
// Callback, ESP-NOW, when data is sent //
//////////////////////////////////////////
#if defined(ESP32) 
  void callback_tx_esp_now(const uint8_t *mac_addr, esp_now_send_status_t status) {
    
    /*char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.println(macStr);*/

    /*char macStr[18];
    imprime("Destino: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    imprime(macStr);
    imprime(" Status:\t");    
    // imprime(status == ESP_NOW_SEND_SUCCESS ? "Successo." : "Falha: ");*/    
  }  
#endif


//////////////////////////////////////////////
// Callback, ESP-NOW, when data is received //
//////////////////////////////////////////////
#if defined(ESP8266) 
  void callback_rx_esp_now(uint8_t * mac, uint8_t *incomingData, uint8_t len) { // Para ESP8266
  
    imprimeln( (const char*) incomingData);
  
  }// end callback_rx_esp_now(...)
#elif defined(ESP32) 
  void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len){ 
    
    imprimeln( (const char*) incomingData);
    
  }// end callback_rx_esp_now(...)
#endif


////////////////////////////////////////
// Soliciata emparelhamento ao Master //
////////////////////////////////////////
void SolicitarEmparelhamento(uint8_t wifi_channel){
  
  // clean esp now  
  esp_now_deinit();

#if defined(ESP32)
  // Force espnow to use a specific channel
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
#elif defined(ESP8266)
  wifi_promiscuous_enable(true);
  wifi_set_channel(wifi_channel);
  wifi_promiscuous_enable(false);
#endif

  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0)
  {
    imprimeln(F("Error initializing ESP-NOW"));
  }

// ESP-NOW
#if defined(ESP8266)
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
#endif

  esp_now_register_send_cb(callback_tx_esp_now);
  esp_now_register_recv_cb(callback_rx_esp_now);

  uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 

  char command[] = "PAIRING_REQUEST";  
  esp_now_send(broadcastAddress, (uint8_t *)command, sizeof(command));
}

    
    
  
    
    



