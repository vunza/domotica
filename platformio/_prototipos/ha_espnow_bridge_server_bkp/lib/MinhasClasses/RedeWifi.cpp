
#include "RedeWifi.h"

#define DEBUG 1
#define QTD_BOARDS_ESPNOW 7


#if DEBUG == 1
    #define imprime(x) Serial.print(x)
    #define imprimeln(x) Serial.println(x)
#else
    #define imprime(x)
    #define imprimeln(x)
#endif



// Construtor
//modo: WIFI_AP_STA | WIFI_STA| WIFI_AP
RedeWifi::RedeWifi(const char *ssid, const char *pwd, const char* modo, boolean change_mac, uint8_t* mac){
    RedeWifi::_ssid = ssid;
    RedeWifi::_pwd = pwd;
    
    if( strcmp(modo, "WIFI_AP_STA") == 0 ){
      WiFi.mode(WIFI_AP_STA);  
      // SSID oculto, apenas para o fucionameto do ESP-NOW
      WiFi.softAP("ESPNOW", "", 1, 1);  
      if(change_mac == true) AlterarMacSTA(mac);    
    }
    else if( strcmp(modo, "WIFI_STA") == 0 ){
      WiFi.mode(WIFI_STA);
      if(change_mac == true) AlterarMacSTA(mac);
    }
    else if( strcmp(modo, "WIFI_AP") == 0 ){
      WiFi.mode(WIFI_AP);  
      if(change_mac == true) AlterarMacAP(mac);      
    }  
    WiFi.disconnect();  
}

// Alterar MAC no Modo STA
void RedeWifi::AlterarMacSTA(uint8_t* newMAC){
 #if defined(ESP8266)       
    wifi_set_macaddr(0, &newMAC[0]);
  #elif defined(ESP32)        
    // ESP32 Board add-on after version > 1.0.5
    esp_wifi_set_mac(WIFI_IF_STA, &newMAC[0]);
    // ESP32 Board add-on before version < 1.0.5
    //esp_wifi_set_mac(ESP_IF_WIFI_AP, &macSTA[0]);
  #endif
} 

// Alterar MAC no Modo AP
void RedeWifi::AlterarMacAP(uint8_t* newMAC){
  #if defined(ESP8266)      
    wifi_set_macaddr(SOFTAP_IF, &newMAC[0]);   
  #elif defined(ESP32)        
    // ESP32 Board add-on after version > 1.0.5
    esp_wifi_set_mac(WIFI_IF_AP, &newMAC[0]);
    // ESP32 Board add-on before version < 1.0.5
    //esp_wifi_set_mac(ESP_IF_WIFI_AP, &macSTA[0]);
  #endif
}

// STA_IP_MODE = [DHCP | STATIC]
void RedeWifi::ConectaRedeWifi(const char* STA_IP_MODE){
    WiFi.begin(RedeWifi::_ssid , RedeWifi::_pwd);

    // Esperar a conexão com o Wi-Fi
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        imprime(".");
    }

    // Converter, MAC STA original, de string para byte array
    memcpy(_orgMAC, converteMacString2Byte(WiFi.macAddress().c_str()), sizeof(_orgMAC));

    imprimeln(F("\nWi-Fi conectado!"));
    imprimeln(F("Endereço IP: "));
    imprimeln(WiFi.localIP());
    imprimeln(WiFi.macAddress());
} 

const uint8_t* RedeWifi::GetMacAddress(){
  return _orgMAC;
}

// AP_IP_MODE = [DEFAULT | CUSTOM]
void RedeWifi::CriaRedeWifi(const char* AP_IP_MODE){

    // AP PADRAO
    WiFi.softAP(RedeWifi::_ssid , RedeWifi::_pwd); 

    // AP CUSTOMIZADA
    /*DadosRede dados_rede = {
      IPAddress(192, 168, 10, 1),   // IP
      IPAddress(192, 168, 10, 1),   // GateWay
      IPAddress(255, 255, 255, 0),  // Máscara de sub-rede
      false,                        // HIDE_SSID
      6,                            // CHANNEL
      QTD_BOARDS_ESPNOW,            // MAX_CONNECTIONS
      false                         // true -> rede_default (192.168.4.1/24); false -> Rede definida
    };

    WiFi.softAPConfig(dados_rede.ip, dados_rede.gateway, dados_rede.subnet);
    WiFi.softAP(_ssid, _pwd, dados_rede.CHANNEL, dados_rede.HIDE_SSID, dados_rede.MAX_CONNECTIONS);   
    */

    // Converter, MAC AP original, de string para byte array
    memcpy(_orgMAC, converteMacString2Byte(WiFi.softAPmacAddress().c_str()), sizeof(_orgMAC));

    char cz_MACADDR[18];
    strcpy(cz_MACADDR, WiFi.softAPmacAddress().c_str());
    imprimeln("SSID: " + WiFi.softAPSSID() + "\nIP: " + WiFi.softAPIP().toString());
    imprime("MAC: ");
    imprimeln(cz_MACADDR);
}  


////////////////////////////////////////////
// Converter MAC (string para Byte array) //
////////////////////////////////////////////
uint8_t* converteMacString2Byte(const char* cz_mac) {

  uint8_t* MAC = (uint8_t*)calloc(6, sizeof(uint8_t));
  char* ptr;

  MAC[0] = strtol(cz_mac, &ptr, HEX );
  for ( uint8_t i = 1; i < 6; i++ ) {
    MAC[i] = strtol(ptr + 1, &ptr, HEX );
  }

  return MAC;
}

