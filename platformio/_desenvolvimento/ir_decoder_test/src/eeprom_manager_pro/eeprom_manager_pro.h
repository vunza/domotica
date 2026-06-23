#ifndef EEPROM_MANAGER_PRO_H
#define EEPROM_MANAGER_PRO_H

#include <Arduino.h>
#include <bc7215.h>
#include <bc7215ac.h>
#include "ctrl_debug.h"


#if defined(ESP32)
  #include <WiFi.h>     
  #include <Preferences.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <EEPROM.h>
#endif


// Definição da estrutura com empacotamento de bytes rígido
typedef struct {
    char wifiSSID[25]; 
    char wifiPass[25];
    char mqtt_server_ip[16];
    char mqtt_user[25];
    char mqtt_password[25];   
    char device_name[25];     
    bc7215FormatPkt_t formatPkt;    
    bc7215DataMaxPkt_t dataPkt;  
    bool configurado;             
} __attribute__((packed)) ConfigDados;

#if defined(ESP8266)
  #define EEPROM_TAMANHO sizeof(ConfigDados) 
#endif


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

#endif //EEPROM_MANAGER_PRO_H