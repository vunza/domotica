
#include "device.h"

void Device::initialize() {   

    #ifdef ESP32
        // Inicializações específicas do ESP32
    #elif defined(ESP8266)
        // Inicializações específicas do ESP8266
    #endif
    
    if(!checkHardware()) {
        imprimeln(F("Falha na verificação do hardware!"));
    }
}

bool Device::checkHardware() {
    // Verificações comuns de hardware
    return true;
}

void Device::run() {
    // Lógica principal do dispositivo
}