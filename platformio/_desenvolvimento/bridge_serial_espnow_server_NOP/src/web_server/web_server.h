#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include "ctrl_debug.h"
#include "json/json_builder.h"

extern float g_voltage;
extern float g_current;
extern float g_temperature;
extern float g_humidity;

class WebServer {
public:
    WebServer(AsyncWebServer* server);
    void begin();
private:
    AsyncWebServer* server;
};

#endif
