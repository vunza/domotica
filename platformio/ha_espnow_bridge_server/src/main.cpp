
#include <headers.h>

/*
          PIN_RELAY   PIN_BUTTON    PIN_LED   MCU
--------------------------------------------------------  
SLAMPHER  GPIO12      GPIO0         GPIO13    ESP8285 1M
BASIC     GPIO12      GPIO0         GPIO13    ESP8266 1M
ESP12E    GPIO12      GPIO0         GPIO2     ESP8266 4M
T4EU1C    GPIO12      GPIO0         GPIO13    ESP8285 1M 
T0EU1C    GPIO12      GPIO0         GPIO13    ESP8285 1M 
SHELLY1L  GPIO5       GPIO4         GPIO0     ESP8285 1M 
M51C      GPIO23      GPIO0         GPIO19    ESP32   4M

*/

#define DEBUG 1

#if DEBUG == 1
#define imprime(x) Serial.print(x)
#define imprimeln(x) Serial.println(x)
#else
#define imprime(x)
#define imprimeln(x)
#endif
  
uint8_t macSTA[6] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};
uint8_t macAP[6]  = {0x02, 0x00, 0x00, 0x00, 0x00, 0x02};   
const char* ssid = "TPLINK";  
const char* password = "gregorio@2012"; 
 

/////////////
// setup() //
/////////////
void setup() {

  #if DEBUG == 1
    // Inicializar Serial
    Serial.begin(115200);
    
    // Aguardar pela inicializacao da Serual
    while (!Serial){    
    } 
  #endif 

  // Criar AP 
  //RedeWifi ap_obj("ESPNOW", "123456789", "WIFI_AP");
  //ap_obj.AlterarMacAP(macAP);
  //ap_obj.CriaRedeWifi("");
  

  // Conectar-se a WiFi
#if defined(ESP8266) 
  RedeWifi sta_obj(ssid, password, "WIFI_STA");     //modo: WIFI_AP_STA | WIFI_STA | WIFI_AP
  sta_obj.AlterarMacSTA(macSTA);                    // Altera MC no modo STA
  sta_obj.ConectaRedeWifi("");                      // STA_IP_MODE = [DHCP | STATIC]
#elif defined(ESP32) 
  RedeWifi sta_obj(ssid, password, "WIFI_AP_STA");  //modo: WIFI_AP_STA | WIFI_STA | WIFI_AP
  sta_obj.AlterarMacSTA(macSTA);                    // Altera MC no modo STA
  sta_obj.ConectaRedeWifi("");                      // STA_IP_MODE = [DHCP | STATIC]
#endif 
  
  // Configurar inicializar esp-now
  EspNow objespnow(&macSTA[0]);

}// setup()



////////////
// loop() //
////////////
void loop() {
  

}// loop()
