#ifndef REDEWIFI_H
#define REDEWIFI_H

#include <Arduino.h>

#if defined(ESP8266) 
  #include <ESP8266WiFi.h> 
#elif defined(ESP32) 
  #include <WiFi.h>
  #include <esp_wifi.h>
#endif



///////////////////////////////
// Gestão das connexões WiFi //
///////////////////////////////
typedef struct DadosRede{
  IPAddress ip;
  IPAddress gateway;
  IPAddress subnet;
  boolean HIDE_SSID;
  uint8_t CHANNEL;
  uint8_t MAX_CONNECTIONS;
  boolean rede_default;
}DadosRede;

 
class RedeWifi {
  public:
    RedeWifi(const char *ssid, const char *pwd, const char* modo, boolean change_mac, uint8_t* mac); //modo: WIFI_AP_STA | WIFI_STA| WIFI_AP  
    void AlterarMacSTA(uint8_t *newMAC);
    void AlterarMacAP(uint8_t* newMAC);
    void ConectaRedeWifi(const char* STA_IP_MODE);  // STA_IP_MODE = [DHCP | STATIC]
    void CriaRedeWifi(const char* AP_IP_MODE);      // AP_IP_MODE = [DEFAULT | CUSTOM]
    const uint8_t* GetMacAddress();

  private:
    const char *_ssid;
    const char *_pwd;  
    uint8_t _orgMAC[6];
     
    
  protected:
};

uint8_t* converteMacString2Byte(const char* cz_mac);


#endif // REDEWIFI_H
