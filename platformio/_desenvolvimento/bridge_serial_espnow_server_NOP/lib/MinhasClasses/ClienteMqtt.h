#ifndef CLIENTEMQTT_H
#define CLIENTEMQTT_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <EspNow.h>

#if defined(ESP8266) 
  #include <WiFiClient.h>
  #include <ESP8266WiFi.h> 
#elif defined(ESP32) 
   #include <WiFi.h>
#endif

#define TOPICS_SIZE 64

extern WiFiClient espClient;
extern PubSubClient clientMqtt;
extern char SAVE_SERVER_MAC[18];
extern boolean set_server_pin;
extern uint8_t server_pin;


///////////////////////////////
// Gestão das connexões WiFi //
///////////////////////////////
typedef struct DadosMqtt{
  
}DadosMqtt;

 
class ClienteMqtt {
  public:
    ClienteMqtt(const char* mqtt_server, uint16_t mqtt_port, const char* mqtt_user, const char* mqtt_pwd);
    void ReconnectarMqtt(const char* mqtt_user, const char* mqtt_pwd);

  private:
    char _mqtt_server[64];
    uint16_t _mqtt_port;
    char _mqtt_user[64];
    char _mqtt_pwd[64];   
    
  protected:
};


void callback(char* topic, byte* payload, unsigned int length);


#endif // CLIENTEMQTT_H
