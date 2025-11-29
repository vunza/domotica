// wifi_manager.h
#pragma once

#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include "ctrl_debug.h"

class WiFiManager {
public:
    void connect(const char* ssid, const char* password);
    void criar_ap(const char* ssid, const char* password);
    void checkConnection();
private:
    unsigned long lastCheck = 0;
};