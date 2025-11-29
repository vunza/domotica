#ifndef ASYNC_ELEGANT_OTA_H
#define ASYNC_ELEGANT_OTA_H

#include <Arduino.h>
#include <AsyncElegantOTA.h>
#include <ESPAsyncWebServer.h>
#include "ctrl_debug.h"

class ElengantOTA {
public:
    static void begin(AsyncWebServer* server);
};

#endif