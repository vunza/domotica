#ifndef EEPROM_MANAGER_PRO_H
#define EEPROM_MANAGER_PRO_H

#include <Arduino.h>
#include <bc7215.h>
#include <bc7215ac.h>
#include "ctrl_debug.h"
#include "constantes.h"


#if defined(ESP32)   
  #include <Preferences.h>
#elif defined(ESP8266)  
  #include <EEPROM.h>
#endif


// Definição da estrutura com empacotamento de bytes rígido
typedef struct {
    char wifiSSID[25]; // 24 Caracteres + '\0'
    char wifiPass[25]; // 24 Caracteres + '\0'
    char mqtt_server_ip[17]; // 16 + '\0'
    char mqtt_user[25]; // 24 Caracteres + '\0'
    char mqtt_password[25]; // 24 Caracteres + '\0' 
    char device_name[DEVICE_NAME_SIZE + 1]; // 24 Caracteres + '\0'  
    bc7215FormatPkt_t formatPkt;    
    bc7215DataMaxPkt_t dataPkt;  
    bool configurado;             
} __attribute__((packed)) ConfigDados;

// Instância global de configuração
extern ConfigDados esp_cfg_data;
//extern boolean pairing_state;
extern bool paired;

#if defined(ESP8266)
  #define EEPROM_TAMANHO sizeof(ConfigDados) 
#endif

void salvarDadosEEPROM(const ConfigDados &dados);
void lerDadosEEPROM(ConfigDados &dadosDestino);

#endif //EEPROM_MANAGER_PRO_H