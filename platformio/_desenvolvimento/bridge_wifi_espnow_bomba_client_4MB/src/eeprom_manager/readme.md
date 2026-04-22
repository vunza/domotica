# Documentação do Módulo `eeprom_manager`

Este documento descreve o módulo **EEPROMManager**, responsável por armazenar e recuperar dados persistentes na EEPROM do ESP8266 e ESP32. Ele permite gravar e ler estruturas completas (`structs`) de forma simples, segura e compatível entre placas, eliminando a complexidade do uso direto da API da EEPROM.

---

## 📁 Arquivo do Módulo

### **src/eeprom_manager/eeprom_manager.h**

```cpp
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

// Estrutura usada como exemplo de armazenamento
typedef struct {
    char device_name[DEVICE_NAME_SIZE];
} DeviceData;

class EEPROMManager {
public:
    EEPROMManager() {}

    void begin(uint16_t eeprom_size) {
        EEPROM.begin(eeprom_size);
    }

    template <typename T>
    void EEPROM_writeStruct(int address, const T &data) {
        const byte *ptr = (const byte*)&data;
        for (unsigned int i = 0; i < sizeof(T); i++) {
            EEPROM.write(address + i, ptr[i]);
        }
        EEPROM.commit();
    }

    template <typename T>
    void EEPROM_readStruct(int address, T &data) {
        byte *ptr = (byte*)&data;
        for (unsigned int i = 0; i < sizeof(T); i++) {
            ptr[i] = EEPROM.read(address + i);
        }
    }
};

#endif
```

## Exemplo de Uso
``` cpp
#include "eeprom_manager.h"

EEPROMManager eeprom;
DeviceData dados;

void setup() {
    Serial.begin(115200);

    eeprom.begin(512);  // inicializa EEPROM

    // Ler dados armazenados
    eeprom.EEPROM_readStruct(0, dados);
    Serial.println(dados.device_name);

    // Atualizar nome do dispositivo
    strcpy(dados.device_name, "ESP32-Device");
    eeprom.EEPROM_writeStruct(0, dados);

    // Confirmar leitura do novo dado
    DeviceData temp;
    eeprom.EEPROM_readStruct(0, temp);
    Serial.println(temp.device_name);
}

void loop() {}
```

