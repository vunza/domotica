#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include "constantes.h"
#include "ctrl_debug.h"
#include "json/json_builder.h"
#include "eeprom_manager/eeprom_manager.h"

extern float g_voltage;
extern float g_current;
extern float g_temperature;
extern float g_humidity;
extern EEPROMManager eeprom;
extern char device_name[DEVICE_NAME_SIZE];

class WebServer {
public:
    WebServer(AsyncWebServer* server);
    void begin();
private:
    AsyncWebServer* server;
};

#endif
