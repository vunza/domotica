/*#include "eeprom_manager_pro.h"
#include "ctrl_debug.h"

#if defined(ESP32)
  #include <Preferences.h>
#elif defined(ESP8266)
  #include <EEPROM.h>
  #define EEPROM_TAMANHO sizeof(ConfigDados)
#endif

// Gravação estável na memória estável
void salvarDadosEEPROM(const ConfigDados &dados) {
    #if defined(ESP32)
        Preferences prefs; // Instanciado localmente para evitar bugs de link de memória
        prefs.begin("esp32cfg_mem", false);
        prefs.putBytes("esp32cfg", (const uint8_t*)&dados, sizeof(ConfigDados));
        prefs.end();                
        imprimeln(F("Dados do Emparelhamento Gravados na EEPROM via Preferences (ESP32)."));
    #elif defined(ESP8266)
        EEPROM.begin(EEPROM_TAMANHO);
        EEPROM.put(0, dados);
        EEPROM.commit();
        EEPROM.end();
        imprimeln(F("Dados do Emparelhamento Gravados na EEPROM via EEPROM (ESP8266)."));
    #endif
}

// Leitura limpa da memória estável
ConfigDados lerDadosEEPROM() { 
    ConfigDados dadosTemporarios;  
    memset(&dadosTemporarios, 0, sizeof(ConfigDados)); // Preenche com zeros para evitar lixo de memória
    
    #if defined(ESP32)
        Preferences prefs;
        prefs.begin("esp32cfg_mem", true);
        if(prefs.isKey("esp32cfg")) {
            prefs.getBytes("esp32cfg", (uint8_t*)&dadosTemporarios, sizeof(ConfigDados));
        }
        prefs.end();
    #elif defined(ESP8266)
        EEPROM.begin(EEPROM_TAMANHO);
        EEPROM.get(0, dadosTemporarios);
        EEPROM.end();
    #endif
    
    return dadosTemporarios;
}

*/

/*#include "eeprom_manager_pro.h"
#include "ctrl_debug.h"

#if defined(ESP32)
  Preferences prefs; 
#endif

// Gravação usando ponteiro para estabilidade absoluta
void salvarDadosEEPROM(ConfigDados *dados) {
    #if defined(ESP32)        
        prefs.begin("esp32cfg_mem", false);
        prefs.putBytes("esp32cfg", (const uint8_t*)dados, sizeof(ConfigDados));
        prefs.end();                
        Serial.println(F("Dados Gravados via Ponteiro (ESP32)."));
    #elif defined(ESP8266)
        EEPROM.begin(EEPROM_TAMANHO);
        EEPROM.put(0, *dados);
        EEPROM.commit();
        EEPROM.end();
        Serial.println(F("Dados Gravados via Ponteiro (ESP8266)."));
    #endif
}

// ⚠️ MUDANÇA CRUCIAL: A função não retorna nada, ela recebe o endereço do main e grava DIRETO nele
void lerDadosEEPROM(ConfigDados *dadosDestino) { 
    // Zera a memória original do main para garantir limpeza
    memset(dadosDestino, 0, sizeof(ConfigDados)); 
    
    #if defined(ESP32)
        Preferences prefs;
        prefs.begin("esp32cfg_mem", true);
        if(prefs.isKey("esp32cfg")) {
            // Injeta os bytes da flash direto no endereço de memória do main.cpp
            prefs.getBytes("esp32cfg", (uint8_t*)dadosDestino, sizeof(ConfigDados));
        }
        prefs.end();
    #elif defined(ESP8266)
        EEPROM.begin(EEPROM_TAMANHO);
        EEPROM.get(0, *dadosDestino);
        EEPROM.end();
    #endif
}*/

