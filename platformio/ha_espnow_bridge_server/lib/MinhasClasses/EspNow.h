#ifndef ESPNOW_H
#define ESPNOW_H

#include <Arduino.h>

#if defined(ESP8266) 
  #include <ESP8266WiFi.h> 
  #include <espnow.h>  
#elif defined(ESP32) 
  #include <WiFi.h>
  #include <esp_wifi.h>
  #include <esp_now.h>  
#endif


 
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

void ResponderEmparelhamento();

#endif // ESPNOW_H
