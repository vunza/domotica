
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
uint8_t macaddr[6];  
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
  //RedeWifi wifi_obj("ESPNOW", "123456789", "WIFI_AP", true, macAP);  
  //wifi_obj.CriaRedeWifi("");
  

  // Conectar-se a WiFi
#if defined(ESP8266) 
  RedeWifi wifi_obj(ssid, password, "WIFI_STA", true, macSTA);     //modo: WIFI_AP_STA | WIFI_STA | WIFI_AP
  wifi_obj.ConectaRedeWifi("");                                   // STA_IP_MODE = [DHCP | STATIC]
#elif defined(ESP32) 
  RedeWifi wifi_obj(ssid, password, "WIFI_AP_STA", true, macSTA);   //modo: WIFI_AP_STA | WIFI_STA | WIFI_AP
  wifi_obj.ConectaRedeWifi("");                                     // STA_IP_MODE = [DHCP | STATIC]
#endif 


  // Obter MAC   
  memcpy(macaddr, wifi_obj.GetMacAddress(), sizeof(macaddr));

  // Iniciar ESP-NOW  
  EspNow objespnow(&macaddr[0]);   
  

}// setup()



////////////
// loop() //
////////////
uint8_t mac_addr[6] = {0x58,0xbf,0x25,0x4d,0x30,0x1f};
Payload pld = {};
      
void loop() {

  /*strcpy(pld.comando, "PAIRING_TEST");
  esp_now_send(mac_addr, (uint8_t *)&pld, sizeof(pld)); 
  delay(2000);*/
 
}// loop()
