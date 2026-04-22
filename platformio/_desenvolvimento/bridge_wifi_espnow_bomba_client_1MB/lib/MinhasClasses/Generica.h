#ifndef GENERICA_H
#define GENERICA_H

#include <Arduino.h>

#if defined(ESP8266)   
#elif defined(ESP32)   
#endif



 
class Generica {
  public:
    Generica();
    void RemoveMacDelimiters(char delimiter_char, const char* mac_address, char* buffer);
    uint8_t* ConverteMacString2Byte(const char* cz_mac);
    
  private:
       
  protected:
};



#endif // GENERICA_H
