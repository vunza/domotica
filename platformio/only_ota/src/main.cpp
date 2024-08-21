/*#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

#define PIN_RELAY 5//12
#define PIN_BUTTON 4//0                    
#define PIN_LED 1//13//2 

void IRAM_ATTR attInterruptChangePIN();

volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 60; // Ajuste esse valor conforme necessário
uint32_t ctl_btn_pulso = 0;

void setup() {

  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLDOWN_16);
  pinMode(PIN_LED, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptChangePIN, RISING);
 
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("[Nome]", "123456789");
  ArduinoOTA.setHostname("[Nome]");
  ArduinoOTA.begin();    
 
}

void loop() {
  ArduinoOTA.handle();
}





void IRAM_ATTR  attInterruptChangePIN(){  
   
  unsigned long currentMillis = millis();  
  if (currentMillis - lastDebounceTime >= debounceDelay) {
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));    
    lastDebounceTime = currentMillis;
  }

}*/



/*
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
#endif


//#include <ElegantOTA.h>


// Import required libraries
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <AsyncElegantOTA.h>

// Replace with your network credentials
const char* ssid = "TPLINK";
const char* password = "gregorio@2012";

// Create AsyncWebServer object on port 80
AsyncWebServer server(8090);


void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  delay(1000);

  WiFi.softAP("[Nome]", "123456789");


  // Start ElegantOTA
  AsyncElegantOTA.begin(&server);
 //ElegantOTA.begin(&server);    // Start ElegantOTA
  // Start server
  server.begin();
}

void loop() {

}
*/


/*************************************************************
 * BASIC OTA sketch, adapted from Example sketch - BasicOTA
 * 
 * Neil Kenyon 
 * 1 December 2019
 * 
 *************************************************************
*/


#include <Arduino.h>
//#include <LittleFS.h>
//#include <FS.h> 
#include <ArduinoOTA.h>
//#include <ArduinoJson.h>
//#include <EEPROM.h>
//#include <Time.h>
//#include <DNSServer.h>
//#include <AsyncElegantOTA.h>
//#include <TelnetStream.h>
//#include <ESPAsyncWebServer.h>



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
  //#include <esp_task_wdt.h>
  //#include <soc/rtc_wdt.h>
  #include <WiFi.h>
  //#include <SPIFFS.h>
  //#include <esp_now.h>  
  //#include <esp_wifi.h>
  //#include <ESPmDNS.h> 
  //#include <HTTPClient.h>
  //#include "AsyncTCP.h"
#endif



void setup(){
  Serial.begin(115200);
  delay(500);

  
  WiFi.mode(WIFI_AP_STA); 
  WiFi.softAP("ESP_OTA", "");

  ArduinoOTA.onStart([]() {
   
  });

}


void loop(){
  ArduinoOTA.handle();
}