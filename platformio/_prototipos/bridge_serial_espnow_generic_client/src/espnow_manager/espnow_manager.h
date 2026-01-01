#ifndef ESPNOW_SIMPLE_H
#define ESPNOW_SIMPLE_H

#include <Arduino.h>

#if defined(ESP32)
  #include <WiFi.h>
  #include <esp_now.h>
  #include <esp_wifi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <espnow.h>
#endif



typedef struct {
    char msg_type[12];  // REGIST | DATA | COMMAND | RESPONSE
    char mac_server[18];  
    char mac_client[18]; 
    char state[4]; // ON | OFF
    char u1_voltage[15];   
    char u1_current[15];
    char u1_temperature[15];
    char u1_humidity[15];   
} EspNowData;



class EspNowManager {
public:
    typedef std::function<void(const uint8_t *mac, const uint8_t *data, int len)> RecvCallback;
    typedef std::function<void(const uint8_t *mac, bool success)> SendCallback;

    EspNowManager();

    bool begin(uint8_t channel = 1, bool useAP = false);
    bool addPeer(const uint8_t mac[6], uint8_t channel = 1);

    bool send(const uint8_t mac[6], const uint8_t *data, int len);

    void onReceive(RecvCallback cb);
    void onSend(SendCallback cb);

private:
    RecvCallback _recvCB;
    SendCallback _sendCB;

#if defined(ESP32)
    static void _onRecv(const uint8_t *mac, const uint8_t *data, int len);
    static void _onSent(const uint8_t *mac, esp_now_send_status_t status);
#elif defined(ESP8266)
    static void _onRecv(uint8_t *mac, uint8_t *data, uint8_t len);
    static void _onSent(uint8_t *mac, uint8_t status);
#endif

    static EspNowManager *instance;
};

#endif
