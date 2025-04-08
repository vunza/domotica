#ifndef ESPNOW_H
#define ESPNOW_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Generica.h>

#if defined(ESP8266) 
  #include <ESP8266WiFi.h> 
  #include <espnow.h>  
#elif defined(ESP32) 
  #include <WiFi.h>
  #include <esp_wifi.h>
  #include <esp_now.h>  
#endif

#define DEVICE_NAME_SIZE 16
#define DEVICE_CLASS_SIZE 64

extern uint8_t pin_state;
extern char CLIENT_MAC[18];
extern char DEVICE_NAME[DEVICE_NAME_SIZE];
extern char DEVICE_CLASS[DEVICE_CLASS_SIZE];
extern boolean send_auto_discovery;


enum TipoMensagem {ASK_PAIRING = 0, CONFIRM_PAIRING, DATA, CMD_SET, PING_REQUEST, PING_RESPONSE};

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
    EspNow(const uint8_t *mac);       
       
  private:   
     
  protected:
};


////////////////////////////
// CallBack rx/tx ESP-NOW //
////////////////////////////
#if defined(ESP8266) 
  void callback_tx_esp_now(uint8_t *mac_addr, uint8_t status); 
  void callback_rx_esp_now(uint8_t * mac, uint8_t *incomingData, uint8_t len);
  void EmparelharDispositivos(uint8_t * mac, uint8_t* incomingData);
  void ProcessarPayload(uint8_t * mac, uint8_t* incomingData);
#elif defined(ESP32) 
  void callback_tx_esp_now(const uint8_t *mac_addr, esp_now_send_status_t status);
  void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len);
  void EmparelharDispositivos(const uint8_t * mac, const uint8_t* incomingData);
  void ProcessarPayload(const uint8_t * mac, const uint8_t* incomingData);
#endif

////////////////////////////////////////////
// Converter MAC (string para Byte array) //
////////////////////////////////////////////
uint8_t* ConverteMacString2Byte(const char* cz_mac);


#endif // ESPNOW_H



