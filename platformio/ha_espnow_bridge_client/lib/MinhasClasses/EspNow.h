#ifndef ESPNOW_H
#define ESPNOW_H

#include <Arduino.h>

#if defined(ESP8266) 
  #include <ESP8266WiFi.h>   
  extern "C" {
    #include <espnow.h>
  }
#elif defined(ESP32) 
  #include <WiFi.h>
  #include <esp_wifi.h>
  #include <esp_now.h>  
#endif


extern uint8_t SERVER_MAC[6];
extern uint8_t broadcastAddress[6];
extern unsigned long ctrl_server_alive;
extern boolean device_paired; 

enum TipoMensagem {ASK_PAIRING = 0, CONFIRM_PAIRING, DATA};

typedef struct Payload{
  uint8_t tipo_msg;;          // PAIRING, DATA
  uint8_t canal_wifi;
  uint8_t mac_servidor[6];
  uint8_t mac_cliente[6];
}Payload;


 
class EspNow {
  public:
    EspNow();     
       
  private:
     
  protected:
};


////////////////////////////
// CallBack rx/tx ESP-NOW //
////////////////////////////
#if defined(ESP8266) 
  void callback_tx_esp_now(uint8_t *mac_addr, uint8_t status); 
  void callback_rx_esp_now(uint8_t * mac, uint8_t *incomingData, uint8_t len);
#elif defined(ESP32) 
  void callback_tx_esp_now(const uint8_t *mac_addr, esp_now_send_status_t status);
  void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len);
#endif

void ProcessarPayload(Payload pld);
void ReEmparelhar();


#endif // ESPNOW_H
