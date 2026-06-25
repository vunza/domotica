#include "eeprom_manager_pro.h"
#include "ctrl_debug.h"

ConfigDados esp_cfg_data;
bool paired = false;

void salvarDadosEEPROM(const ConfigDados &dados) {
#if defined(ESP32)
    Preferences prefs;
    prefs.begin("esp32cfg_mem", false);
    prefs.putBytes("esp32cfg", &dados, sizeof(ConfigDados));
    prefs.end();   
    imprimeln(F("Dados Gravados via NVS Preferences (ESP32)."));
#elif defined(ESP8266)
    EEPROM.begin(EEPROM_TAMANHO);
    EEPROM.put(0, dados);
    EEPROM.commit();
    EEPROM.end();
    imprimeln(F("Dados Gravados via Flash EEPROM (ESP8266)."));
#endif
}


void lerDadosEEPROM(ConfigDados &dadosDestino) { 
    memset(&dadosDestino, 0, sizeof(ConfigDados));      
#if defined(ESP32)
    Preferences prefs;
    prefs.begin("esp32cfg_mem", true);
    if (prefs.isKey("esp32cfg")) {
        prefs.getBytes("esp32cfg", &dadosDestino, sizeof(ConfigDados));
    }
    prefs.end();
#elif defined(ESP8266)
    EEPROM.begin(EEPROM_TAMANHO);
    EEPROM.get(0, dadosDestino);
    EEPROM.end();
#endif
}