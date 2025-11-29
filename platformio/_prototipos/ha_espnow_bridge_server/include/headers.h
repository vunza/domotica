#ifndef COMUM_H
#define COMUM_H

#include <Arduino.h>
#include <LittleFS.h>
#include <FS.h> 
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <Time.h>
#include <DNSServer.h>
#include <AsyncElegantOTA.h>
#include <TelnetStream.h>
#include <ESPAsyncWebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Minhas classes
#include <RedeWifi.h>
#include <EspNow.h>
#include <ClienteMqtt.h>


#if defined(ESP8266) 
  #include <ESP8266WiFi.h> 
  #include <ESPAsyncTCP.h>
  #include <Hash.h> 
  #include <ESP8266mDNS.h>  
  #include <espnow.h>  
  #include <WiFiClient.h>
  #include <ESP8266HTTPClient.h>
#elif defined(ESP32) 
  //#include <esp_task_wdt.h>
  #include <esp_task_wdt.h>
  #include <soc/rtc_wdt.h>
  #include <WiFi.h>
  //#include <SPIFFS.h>
  #include <esp_now.h>  
  #include <esp_wifi.h>
  #include <ESPmDNS.h> 
  #include <HTTPClient.h>
  #include "AsyncTCP.h"
#endif

#define TOPICS_SIZE 64
#define FRIENDLY_NAME_SIZE 32
#define PAYLOAD_SIZE 254


#endif // COMUM_H