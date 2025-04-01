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

#define DEVICE_NAME_SIZE 16
#define DEVICE_CLASS_SIZE 64

extern uint8_t SERVER_MAC[6];
extern uint8_t localMac[6]; 
extern uint8_t broadcastAddress[6];
extern char DEVICE_NAME[DEVICE_NAME_SIZE];
extern char DEVICE_CLASS[DEVICE_CLASS_SIZE];
extern unsigned long ctrl_time_send_status;
extern boolean device_paired; 
extern uint8_t remote_wifi_channel;
extern uint8_t get_pin_state;
extern boolean set_device_pin; 

enum TipoMensagem {
  ASK_PAIRING = 0, CONFIRM_PAIRING, DATA, CMD_SET
};

typedef struct Payload{
  uint8_t tipo_msg;;          // ASK_PAIRING, CONFIRM_PAIRING, DATA, CMD_SET
  uint8_t canal_wifi;
  uint8_t mac_servidor[6];
  uint8_t mac_cliente[6];
  char nome_dispositivo[DEVICE_NAME_SIZE];
  char classe_dispositivo[DEVICE_CLASS_SIZE];
  uint8_t estado_pin;
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
uint8_t* ConverteMacString2Byte(const char* cz_mac);


#endif // ESPNOW_H
