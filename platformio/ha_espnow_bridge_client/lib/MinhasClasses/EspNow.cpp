
#include "EspNow.h"

#define DEBUG 1

#if DEBUG == 1
  #define imprime(x) Serial.print(x)
  #define imprimeln(x) Serial.println(x)
#else
  #define imprime(x)
  #define imprimeln(x)
#endif

boolean device_paired = false; 

#if defined(ESP32) 
  esp_now_peer_info_t broadcastPeer;
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

  Pairing pld = {};
  strncpy(pld.comando, "PAIRING_REQUEST", sizeof(pld.comando));
  uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  // Regista callbacks para Rx/Tx de mensagens esp-now
  esp_now_register_send_cb(callback_tx_esp_now);
  esp_now_register_recv_cb(callback_rx_esp_now);  

  // Enviar Broad cast
  #if defined(ESP8266) 
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

    for (int channel = 1; channel <= 13 && (device_paired == false); channel++) {
      wifi_promiscuous_enable(true);
      wifi_set_channel(channel);
      wifi_promiscuous_enable(false);

      // Envia a mensagem de broadcast
      uint8_t result = esp_now_send(broadcastAddress, (uint8_t *)&pld, sizeof(pld));
      if (result == 0) {
        imprime(F("Mensagem enviada no canal: "));
        imprimeln(channel);
      } else {
        imprime(F("Erro ao enviar no canal: "));
        imprimeln(channel);
      }

      delay(1000);  
    } 
    
  #elif defined(ESP32) 
    for (int channel = 1; channel <= 13  && (device_paired == false); channel++) {

      esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
      broadcastPeer.channel = 0; // 0 = any
      broadcastPeer.encrypt = false; // can't encrypt when broadcasting
      memcpy(broadcastPeer.peer_addr, broadcastAddress, 6); // copy broadcast address

      bool exists = esp_now_is_peer_exist(broadcastAddress);      
      if(exists) esp_now_del_peer(broadcastAddress);

      if (esp_now_add_peer(&broadcastPeer) != ESP_OK){
          Serial.println("Can't register espnow broadcast peer, rebooting");
          return;        
      }   

      // Envia a mensagem de broadcast
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&pld, sizeof(pld));
      if (result == ESP_OK) {
        imprime(F("Mensagem enviada no canal: "));
        imprimeln(channel);
      } else {
        imprime(F("Erro ao enviar no canal: "));
        imprimeln(channel);
      }
      delay(1000);
    } 
  #endif    

}


//////////////////////////////////////////
// Callback, ESP-NOW, when data is sent //
//////////////////////////////////////////
#if defined(ESP8266) 
  void callback_tx_esp_now(uint8_t *mac_addr, uint8_t status) { 
    /*char macStr[18];
    //imprime("Destino: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.println(macStr);
    imprime(" Status:\t");
    //imprime(status == ESP_NOW_SEND_SUCCESS ? "Successo." : "Falha: ");*/
    
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
    
    /*Serial.print("Last Packet Send Status: ");
    if (status == 0){
      Serial.println("Delivery success");
    }
    else{
      Serial.println("Delivery fail");
    } */
  }  
#endif


//////////////////////////////////////////////
// Callback, ESP-NOW, when data is received //
//////////////////////////////////////////////
#if defined(ESP8266) 
  void callback_rx_esp_now(uint8_t * mac, uint8_t *incomingData, uint8_t len) { // Para ESP8266
  
    Pairing pld = {};   
    memcpy(&pld, incomingData, sizeof(pld));

    imprimeln(pld.comando);
    imprimeln(pld.canal_wifi);

    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             pld.mac[0], pld.mac[1], pld.mac[2], pld.mac[3], pld.mac[4], pld.mac[5]);
    Serial.println(macStr);

    if(strcmp(pld.comando, "PAIRING_CONFIRM") == 0){
      device_paired = true;     
    }
    
  
  }// end callback_rx_esp_now(...)
#elif defined(ESP32) 
  void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len){ 
    
    Pairing pld = {};   
    memcpy(&pld, incomingData, sizeof(pld));

    imprimeln(pld.comando);
    imprimeln(pld.canal_wifi);

    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             pld.mac[0], pld.mac[1], pld.mac[2], pld.mac[3], pld.mac[4], pld.mac[5]);
    Serial.println(macStr);

    if(strcmp(pld.comando, "PAIRING_CONFIRM") == 0){
      device_paired = true;
    }
    
  }// end callback_rx_esp_now(...)
#endif


////////////////////////////////////////
// Soliciata emparelhamento ao Master //
////////////////////////////////////////
boolean SolicitarEmparelhamento(uint8_t wifi_channel){ 

// ESP-NOW
#if defined(ESP8266)
  /*wifi_promiscuous_enable(true);
  wifi_set_channel(wifi_channel);
  wifi_promiscuous_enable(false);*/
#elif defined(ESP32)    
  // Define o canal do Wi-Fi
    esp_wifi_set_channel(wifi_channel, WIFI_SECOND_CHAN_NONE);

    Pairing pld = {};
    strncpy(pld.comando, "PAIRING_REQUEST", sizeof(pld.comando));

    // Mensagem a ser enviada
    //esp_now_message msg;
    //snprintf(msg.message, sizeof(msg.message), "Canal %d - Broadcast", channel);

    uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    // Envia a mensagem de broadcast
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&pld, sizeof(pld));
    if (result == ESP_OK) {
        Serial.printf("Mensagem enviada no canal %d\n", wifi_channel);
    } else {
        Serial.printf("Erro ao enviar no canal %d\n", wifi_channel);
    }
#endif 
 
  //esp_now_register_send_cb(callback_tx_esp_now);
  //esp_now_register_recv_cb(callback_rx_esp_now);
 
  /*Pairing pld = {};
  strncpy(pld.comando, "PAIRING_REQUEST", sizeof(pld.comando));
  esp_now_send(broadcastAddress, (uint8_t *)&pld, sizeof(Pairing));*/

  return device_paired;
}

