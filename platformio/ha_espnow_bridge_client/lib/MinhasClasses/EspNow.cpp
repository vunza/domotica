
#include "EspNow.h"

#define DEBUG 1
#define MAX_WIFI_CHANNELS 13

#if DEBUG == 1
  #define imprime(x) Serial.print(x)
  #define imprimeln(x) Serial.println(x)
#else
  #define imprime(x)
  #define imprimeln(x)
#endif

uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t localMac[6]; 
boolean device_paired = false; 
uint8_t SERVER_MAC[6];
char DEVICE_NAME[DEVICE_NAME_SIZE];
char DEVICE_CLASS[DEVICE_CLASS_SIZE];
unsigned long ctrl_time_ping_request = 0;
uint8_t remote_wifi_channel = 0; 
uint8_t local_wifi_channel = WiFi.channel(); 
uint8_t get_pin_state;
boolean set_device_pin = false; 
uint8_t ping_couter = 0; 
Generica objGenerica;


#if defined(ESP32) 
  esp_now_peer_info_t broadcastPeer;
#endif
 
// Construtor
EspNow::EspNow(){ 
 
  // Guarda o nome do dispositivo
#if defined(ESP32) 
  memcpy(DEVICE_NAME, WiFi.getHostname(), sizeof(DEVICE_NAME));
#elif defined(ESP8266)  
  memcpy(DEVICE_NAME, WiFi.hostname().c_str(), sizeof(DEVICE_NAME));
#endif

  // Atribuitr classe ao dispositivo  
  memcpy(DEVICE_CLASS, "switch", sizeof(DEVICE_CLASS)); // TODO: Rever logica para atribuicao da classe

  // TDOD: Manusear aqui o nome do dev, em vez do hostname
  // Eliminar o caracter '-'
  uint8_t j = 0;
  char AUX_DEVICE_NAME[DEVICE_NAME_SIZE];
  for (uint8_t i = 0; DEVICE_NAME[i] != '\0'; i++) {
    if (DEVICE_NAME[i] != '-') {     
      AUX_DEVICE_NAME[j] = DEVICE_NAME[i];
      j++;
    }
  }

  AUX_DEVICE_NAME[j] = '\0'; 
  memcpy(DEVICE_NAME, AUX_DEVICE_NAME, sizeof(DEVICE_NAME));

  uint8_t scan_wifi_ch = 0;

  Payload pld = {};
  pld.tipo_msg = ASK_PAIRING; 
  memcpy(pld.nome_dispositivo, DEVICE_NAME, sizeof(DEVICE_NAME));
  memcpy(pld.classe_dispositivo, DEVICE_CLASS, sizeof(DEVICE_CLASS)); 

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    imprimeln(F("Error initializing ESP-NOW"));
    return;
  }
  else  {
    imprimeln(F("\nESP-NOW inicializado com sucesso!"));
  }

  // Regista callbacks para Rx/Tx de mensagens esp-now
  esp_now_register_send_cb(callback_tx_esp_now);
  esp_now_register_recv_cb(callback_rx_esp_now);  

  // Guardar, como byte array, o MAC do dispositivo.
  memcpy(localMac, objGenerica.ConverteMacString2Byte(WiFi.macAddress().c_str()), sizeof(localMac));

//ConverteMacString2Byte(WiFi.macAddress().c_str())
  #if defined(ESP8266) 
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO); 

    while (device_paired == false) {
      wifi_promiscuous_enable(true);
      wifi_set_channel(scan_wifi_ch);
      wifi_promiscuous_enable(false);

      bool exists = esp_now_is_peer_exist(broadcastAddress);      
      if(exists) esp_now_del_peer(broadcastAddress);

      esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, scan_wifi_ch, NULL, 0); 

      // Envia a mensagem de broadcast
      uint8_t result = esp_now_send(broadcastAddress, (uint8_t *)&pld, sizeof(pld));
      if (result == 0) {
        imprime(F("Mensagem enviada no canal: "));
        imprimeln(scan_wifi_ch);
        imprime(F("MAC Destino: "));
        char macStr[18];    
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                broadcastAddress[0], broadcastAddress[1], broadcastAddress[2], broadcastAddress[3], broadcastAddress[4], broadcastAddress[5]);
        imprimeln(macStr);
      } else {
        imprime(F("Erro ao enviar no canal: "));
        imprimeln(scan_wifi_ch);
      }

      if(scan_wifi_ch > MAX_WIFI_CHANNELS){      
        scan_wifi_ch = 0;
      }
      else{
        scan_wifi_ch++;
      }

      delay(1000);
    }
  #elif defined(ESP32)   

    while (device_paired == false) {
      esp_wifi_set_channel(scan_wifi_ch, WIFI_SECOND_CHAN_NONE);   
      memset(&broadcastPeer, 0, sizeof(broadcastPeer));       
      memcpy(broadcastPeer.peer_addr, broadcastAddress, 6);
      broadcastPeer.channel = 0;
      broadcastPeer.encrypt = false;

      bool exists = esp_now_is_peer_exist(broadcastAddress);      
      if(exists) esp_now_del_peer(broadcastAddress);

      if (esp_now_add_peer(&broadcastPeer) != ESP_OK) {
        imprimeln(F("Erro ao adicionar peer"));
        return;
      }

      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&pld, sizeof(pld));

      if (result == ESP_OK) {
        imprime(F("Mensagem enviada no canal: "));
        imprimeln(scan_wifi_ch);
        imprime(F("MAC Destino: "));
        char macStr[18];    
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                broadcastAddress[0], broadcastAddress[1], broadcastAddress[2], broadcastAddress[3], broadcastAddress[4], broadcastAddress[5]);
        imprimeln(macStr);
      } else {
        imprime(F("Erro ao enviar no canal: "));
        imprimeln(scan_wifi_ch);
      }

      if(scan_wifi_ch > MAX_WIFI_CHANNELS){      
        scan_wifi_ch = 0;
      }
      else{
        scan_wifi_ch++;
      }     
      
      // wait 1secs
      vTaskDelay(1000 / portTICK_PERIOD_MS);

    } // FIM do while()
  #endif
     

} // FIM do Constructor



//////////////////////////////////////////
// Callback, ESP-NOW, when data is sent //
//////////////////////////////////////////
#if defined(ESP8266) 
  void callback_tx_esp_now(uint8_t *mac_addr, uint8_t status) { 
    /*char macStr[18];    
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    imprime("Destino: ");
    imprimeln(macStr);
    imprime("Canal: ");
    imprimeln(remote_wifi_channel);
    imprime("Status: ");
    imprimeln(status == 0 ? "Successo." : "Falha: ");  
    imprimeln("------------------");*/   
  }
#elif defined(ESP32) 
  void callback_tx_esp_now(const uint8_t *mac_addr, esp_now_send_status_t status) {
    /*char macStr[18];    
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    imprime("Destino: ");
    imprimeln(macStr);
    imprime("Canal: ");
    imprimeln(remote_wifi_channel);
    imprime("Status: ");
    imprimeln(status == 0 ? "Successo." : "Falha: ");
    imprimeln("------------------");
    */
  }  
#endif


//////////////////////////////////////////////
// Callback, ESP-NOW, when data is received //
//////////////////////////////////////////////
#if defined(ESP8266) 
  void callback_rx_esp_now(uint8_t * mac, uint8_t *incomingData, uint8_t len) {     
  
    Payload pld = {};   
    memcpy(&pld, incomingData, sizeof(pld));
    memcpy(pld.mac_servidor, mac, sizeof(pld.mac_servidor));
    ProcessarPayload(pld);   
  
  }// end callback_rx_esp_now(...)
#elif defined(ESP32) 
  void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len){ 
    
    Payload pld = {};   
    memcpy(&pld, incomingData, sizeof(pld));
    memcpy(pld.mac_servidor, mac, sizeof(pld.mac_servidor));
    ProcessarPayload(pld);  
    
  }// end callback_rx_esp_now(...)
#endif


///////////////////////////////////////////
// Processar mensagens esp-now recebidas //
///////////////////////////////////////////
void ProcessarPayload(Payload pld){

  // Guardar MAC do Servidor
  memcpy(SERVER_MAC, pld.mac_servidor, sizeof(SERVER_MAC));
  remote_wifi_channel = pld.canal_wifi;  
  
  // Procerssar mensagem de emparelhamento
  if(pld.tipo_msg == CONFIRM_PAIRING && (memcmp(localMac, pld.mac_cliente, sizeof(localMac)) == 0) ){ 
  //if(pld.tipo_msg == CONFIRM_PAIRING && (remote_wifi_channel != local_wifi_channel) ){   
    
    #if defined(ESP8266)      
      // ALterar Canal WiFi          
      wifi_promiscuous_enable(true);     
      wifi_set_channel(remote_wifi_channel);
      wifi_promiscuous_enable(false);  
    #elif defined(ESP32) 
      esp_wifi_set_channel(remote_wifi_channel, WIFI_SECOND_CHAN_NONE);      
    #endif

   
    device_paired = true;
    ctrl_time_ping_request = millis();
  }
  else if(pld.tipo_msg == CMD_SET /*&& strcmp(pld.nome_dispositivo, DEVICE_NAME) == 0*/){
    get_pin_state = pld.estado_pin;    
    set_device_pin = true;   
  }
  else if(pld.tipo_msg == PING_RESPONSE){
    ping_couter = 0;     
  }

  /*char macStr[18];
  imprime("Servidor: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
    pld.mac_servidor[0], pld.mac_servidor[1], pld.mac_servidor[2], pld.mac_servidor[3], pld.mac_servidor[4], pld.mac_servidor[5]);
  imprimeln(macStr);
  char macStr1[18];
  imprime("Cliente: ");
  snprintf(macStr1, sizeof(macStr1), "%02x:%02x:%02x:%02x:%02x:%02x",
    pld.mac_cliente[0], pld.mac_cliente[1], pld.mac_cliente[2], pld.mac_cliente[3], pld.mac_cliente[4], pld.mac_cliente[5]);
  imprimeln(macStr1);
  imprime("Comando: ");
  imprimeln(pld.tipo_msg);    
  imprime("Canal: ");
  imprimeln(remote_wifi_channel);
  imprimeln("-------------------");
  imprimeln(WiFi.macAddress());*/
  
} // FIM de ProcessarPayload(...)


///////////////////////////////////////
// Obter canal, esp-now, do Servidor //
///////////////////////////////////////
void ReEmparelhar(){

  imprimeln(F("Servidor Indisponível!"));

  Payload pld = {};
  pld.tipo_msg = ASK_PAIRING;
  memcpy(pld.nome_dispositivo, DEVICE_NAME, sizeof(DEVICE_NAME));
  memcpy(pld.classe_dispositivo, DEVICE_CLASS, sizeof(DEVICE_CLASS)); 
     
  // Enviar Broad cast
  #if defined(ESP8266) 
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);    
    for (int channel = 0; channel <= MAX_WIFI_CHANNELS && (device_paired == false); channel++) {
      wifi_promiscuous_enable(true);     
      wifi_set_channel(channel);
      wifi_promiscuous_enable(false);

      // Envia a mensagem de broadcast
      uint8_t result = esp_now_send(broadcastAddress, (uint8_t *)&pld, sizeof(pld));
      if (result == 0) {
        imprime(F("Mensagem enviada no canal: "));
        imprimeln(channel);
        imprime(F("MAC Destino: "));
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
    for (int channel = 0; channel <= MAX_WIFI_CHANNELS  && (device_paired == false); channel++) {

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
        imprime(F("MAC Destino: "));
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

  ctrl_time_ping_request = millis();

}// FIM de ReEmparelhar()
