#ifndef COMUM_H
#define COMUM_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <Time.h>
#include <DNSServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <assert.h>
#include <IRrecv.h>
#include <IRac.h>
#include <IRtext.h>
#include <IRutils.h>
#include <AsyncElegantOTA.h>
#include <TelnetStream.h>


#if defined(ESP8266) 
  #include <ESP8266WiFi.h>
  #include <FS.h>  
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <Hash.h> 
  #include <ESP8266mDNS.h>  
  #include <espnow.h>  
  #include <WiFiClient.h>
#elif defined(ESP32) 
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
  #include <esp_now.h>  
  #include <esp_wifi.h>
  #include <ESPmDNS.h> 
  #include <HTTPClient.h>
#endif


#endif // COMUM_H