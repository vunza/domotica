
#include "Generica.h"

#define DEBUG 1


#if DEBUG == 1
    #define imprime(x) Serial.print(x)
    #define imprimeln(x) Serial.println(x)
#else
    #define imprime(x)
    #define imprimeln(x)
#endif



/////////////////
// Construtor //
////////////////
Generica::Generica(){

  #if defined(ESP8266)     
  #elif defined(ESP32) 
  #endif

}


///////////////////////////
// Remover delimitadores //
///////////////////////////
void Generica::RemoveMacDelimiters(char delimiter_char, const char* mac_address, char* buffer){

    uint8_t j = 0;
    char AUX_DEVICE_MAC[18];
  
    for (uint8_t i = 0; mac_address[i] != '\0'; i++) {
      if (mac_address[i] != delimiter_char) {     
        AUX_DEVICE_MAC[j] = mac_address[i];
        j++;
      }
    }

    AUX_DEVICE_MAC[j] = '\0'; 

    memcpy(buffer, AUX_DEVICE_MAC, sizeof(AUX_DEVICE_MAC));    
}


////////////////////////////////////////////
// Converter MAC (string para Byte array) //
////////////////////////////////////////////
uint8_t* Generica::ConverteMacString2Byte(const char* cz_mac) {

  //static uint8_t MAC[6];
  uint8_t* MAC = (uint8_t*)calloc(6, sizeof(uint8_t));
  char* ptr;

  MAC[0] = strtol(cz_mac, &ptr, HEX );
  for ( uint8_t i = 1; i < 6; i++ ) {
    MAC[i] = strtol(ptr + 1, &ptr, HEX );
  }

  return MAC;
}// converteMacString2Byte(const char* cz_mac)

   

