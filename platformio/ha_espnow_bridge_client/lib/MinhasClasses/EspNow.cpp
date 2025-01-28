
#include "EspNow.h"

#define DEBUG 1

#if DEBUG == 1
  #define imprime(x) Serial.print(x)
  #define imprimeln(x) Serial.println(x)
#else
  #define imprime(x)
  #define imprimeln(x)
#endif

uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
boolean device_paired = false; 
uint8_t SERVER_MAC[6];
unsigned long ctrl_server_alive = 0;

#if defined(ESP32) 
  esp_now_peer_info_t broadcastPeer;
#endif
 
// Construtor
EspNow::EspNow(){ 

  //RestartEspNow();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    imprimeln(F("Error initializing ESP-NOW"));
    return;
  }
  else  {
    imprimeln(F("\nESP-NOW inicializado com sucesso!"));
  }

  Payload pld = {};
  strncpy(pld.comando, "ASK_CHANNEL", sizeof(pld.comando));  
  
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
        imprime(F(" MAC Destino: "));
        char macStr[18];    
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                broadcastAddress[0], broadcastAddress[1], broadcastAddress[2], broadcastAddress[3], broadcastAddress[4], broadcastAddress[5]);
        imprimeln(macStr);
      } else {
        imprime(F("Erro ao enviar no canal: "));
        imprimeln(channel);
      }
      
      vTaskDelay(pdMS_TO_TICKS(1000));
    } 
  #endif  
   

} // FIM



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
    imprime(WiFi.channel());
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
    //memcpy(pld.mac_origem, mac, sizeof(pld.mac_origem));
    ProcessarPayload(pld);   
  
  }// end callback_rx_esp_now(...)
#elif defined(ESP32) 
  void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len){ 
    
    Payload pld = {};   
    memcpy(&pld, incomingData, sizeof(pld));
    memcpy(pld.mac_origem, mac, sizeof(pld.mac_origem));
    ProcessarPayload(pld);  
    
  }// end callback_rx_esp_now(...)
#endif


///////////////////////////////////////////
// Processar mensagens esp-now recebidas //
///////////////////////////////////////////
void ProcessarPayload(Payload pld){

  // Guardar MAC do Servidor
  memcpy(SERVER_MAC, pld.mac_origem, sizeof(SERVER_MAC));
  uint8_t wifi_channel = pld.canal_wifi;

  // Procerssar mensagem de emparelhamento
  if(strcmp(pld.comando, "ASW_CHANNEL") == 0){   
    
    #if defined(ESP8266)      
      // ALterar Canal WiFi          
      wifi_promiscuous_enable(true);
      wifi_set_channel(wifi_channel);
      wifi_promiscuous_enable(false);  
    #elif defined(ESP32) 
      esp_wifi_set_channel(wifi_channel, WIFI_SECOND_CHAN_NONE);      
    #endif

    device_paired = true;
    ctrl_server_alive = millis();
  }
  else if(strcmp(pld.comando, "PING_RESPONSE") == 0){
    // Reinivializa variael de controlo do Servidor
    device_paired = false;
    ctrl_server_alive = millis();
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

} // FIM de ProcessarPayload(...)


///////////////////////////////////////
// Obter canal, esp-now, do Servidor //
///////////////////////////////////////
void ReEmparelhar(){

  imprimeln(F("Servidor Indisponível!"));

  Payload pld = {};
  strncpy(pld.comando, "ASK_CHANNEL", sizeof(pld.comando));  
     
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
        imprime(F(" MAC Destino: "));
        char macStr[18];    
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                broadcastAddress[0], broadcastAddress[1], broadcastAddress[2], broadcastAddress[3], broadcastAddress[4], broadcastAddress[5]);
        imprimeln(macStr);
      } else {
        imprime(F("Erro ao enviar no canal: "));
        imprimeln(channel);
      }
      
      vTaskDelay(pdMS_TO_TICKS(1000));
    } 
  #endif

  ctrl_server_alive = millis();

}// FIM de ReEmparelhar()
