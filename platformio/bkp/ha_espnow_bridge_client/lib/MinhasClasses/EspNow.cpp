
#include "EspNow.h"

#define DEBUG 1

#if DEBUG == 1
  #define imprime(x) Serial.print(x)
  #define imprimeln(x) Serial.println(x)
#else
  #define imprime(x)
  #define imprimeln(x)
#endif

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t broadcastAddress1[] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};
boolean device_paired = false; 
uint8_t SERVER_MAC[6];

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

  Payload pld = {};
  strncpy(pld.comando, "PAIRING_REQUEST", sizeof(pld.comando));  
  
  // Regista callbacks para Rx/Tx de mensagens esp-now
  esp_now_register_send_cb(callback_tx_esp_now);
  esp_now_register_recv_cb(callback_rx_esp_now);  

   
  // Enviar Broad cast
  #if defined(ESP8266) 
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    //esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    for (int channel = 1; channel <= 13 && (device_paired == false); channel++) {
      wifi_promiscuous_enable(true);
      wifi_set_channel(channel);
      wifi_promiscuous_enable(false);

      // Envia a mensagem de broadcast
      uint8_t result = esp_now_send(broadcastAddress, (uint8_t *)&pld, sizeof(pld));
      if (result == 0) {
        imprime(F("Mensagem enviada no canal: "));
        imprimeln(channel);
        imprime(F(" MAC Destino: "));
        char macStr[18];    
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                broadcastAddress[0], broadcastAddress[1], broadcastAddress[2], broadcastAddress[3], broadcastAddress[4], broadcastAddress[5]);
        imprimeln(macStr);
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
    char macStr[18];    
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    imprime("Destino: ");
    imprime(macStr);
    imprime(" Canal: ");
    imprime(wifi_get_channel());
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
    imprime(" Canal: ");
    imprime(wifi_get_channel());
    imprime(" Status: ");
    imprimeln(status == 0 ? "Successo." : "Falha: ");  
  }  
#endif


//////////////////////////////////////////////
// Callback, ESP-NOW, when data is received //
//////////////////////////////////////////////
#if defined(ESP8266) 
  void callback_rx_esp_now(uint8_t * mac, uint8_t *incomingData, uint8_t len) { 
  
    Payload pld = {};   
    memcpy(&pld, incomingData, sizeof(pld));
    memcpy(pld.mac, mac, sizeof(pld.mac));
    ProcessarPayload(pld);
  
    char macStr[18];
    imprime("Remetente: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    imprimeln(macStr);
    imprime("CMD: ");
    imprimeln(pld.comando);    
    imprime("Canal: ");
    imprimeln(pld.canal_wifi);
    imprimeln("-------------------");
  
  }// end callback_rx_esp_now(...)
#elif defined(ESP32) 
  void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len){ 
    
    Payload pld = {};   
    memcpy(&pld, incomingData, sizeof(pld));
    ProcessarPayload(pld);    
    
  }// end callback_rx_esp_now(...)
#endif


///////////////////////////////////////////
// Processar mensagens esp-now recebidas //
///////////////////////////////////////////
void ProcessarPayload(Payload pld){

  // Guardar MAC do Servidor
  memcpy(SERVER_MAC, pld.mac, sizeof(SERVER_MAC));
  uint8_t wifi_channel = pld.canal_wifi;

  // Procerssar mensagem de emparelhamento
  if(strcmp(pld.comando, "PAIRING_CONFIRM") == 0){    

    // Adiciona par, servidor
    bool exists = esp_now_is_peer_exist(SERVER_MAC);    
    #if defined(ESP8266)      
      if(!exists){     

        // Regista callbacks para Rx/Tx de mensagens esp-now        
        esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
        wifi_promiscuous_enable(true);
        wifi_set_channel(wifi_channel);
        wifi_promiscuous_enable(false);

        /*uint8_t rslt = esp_now_add_peer(SERVER_MAC, ESP_NOW_ROLE_SLAVE, wifi_channel, NULL, 0);
        if (rslt != 0){
            Serial.println("Can't register espnow broadcast peer, rebooting");
            return;        
        }
        else{         

          //if(esp_now_is_peer_exist(SERVER_MAC)) imprimeln(F("Emparelhado!"));
          imprime("Canal local:");
          imprimeln(wifi_get_channel());
          imprime("Canal remoto:");
          imprimeln(wifi_channel);
          char macStr[18];
          imprime("Remetente: ");
          snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            SERVER_MAC[0], SERVER_MAC[1], SERVER_MAC[2], SERVER_MAC[3], SERVER_MAC[4], SERVER_MAC[5]);
          imprimeln(macStr);
          imprimeln("--------------------");
        } */        
      }    
    #elif defined(ESP32) 
      if(!exists){
        esp_wifi_set_channel(wifi_channel, WIFI_SECOND_CHAN_NONE);
        broadcastPeer.channel = 0; // 0 = any
        broadcastPeer.encrypt = false; // can't encrypt when broadcasting
        memcpy(broadcastPeer.peer_addr, SERVER_MAC, 6); // copy broadcast address

        bool exists = esp_now_is_peer_exist(SERVER_MAC);      
        if(exists) esp_now_del_peer(SERVER_MAC);

        if (esp_now_add_peer(&broadcastPeer) != ESP_OK){
            Serial.println("Can't register espnow broadcast peer, rebooting");
            return;        
        }
        else{
          //if(esp_now_is_peer_exist(SERVER_MAC)) imprimeln(F("Emparelhado!"));
        }        
      }
    #endif

    device_paired = true;
  }

}

