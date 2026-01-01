#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H

#include <Arduino.h>
#include "constantes.h"

#if defined(ESP32)
  #include <EEPROM.h>
#elif defined(ESP8266)
  #include <EEPROM.h>
#else
  #error "Placa não suportada!"
#endif


////////////////////////////////////////////////
// Estrutura para guardar/ler dados na EEPROM //
////////////////////////////////////////////////
typedef struct {
    char device_name[DEVICE_NAME_SIZE];        
} DeviceData;



class EEPROMManager {
public:
    EEPROMManager() {}
    
    /** Inicializa a EEPROM */
    void begin(uint16_t eeprom_size) {
        EEPROM.begin(eeprom_size);
    }

    /** Escreve dados na EEPROM **/
    template <typename T>
    void EEPROM_writeStruct(int address, const T &data) {
        const byte *ptr = (const byte*) &data;
        for (unsigned int i = 0; i < sizeof(T); i++) {
            EEPROM.write(address + i, ptr[i]);
        }
        EEPROM.commit();
    }

    /** Le dados da EEPROM **/
    template <typename T>
    void EEPROM_readStruct(int address, T &data) {
        byte *ptr = (byte*) &data;
        for (unsigned int i = 0; i < sizeof(T); i++) {
            ptr[i] = EEPROM.read(address + i);
        }
    }
    
};

#endif
