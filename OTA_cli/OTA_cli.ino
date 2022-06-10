

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include "creds.h"

void setup()  {

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // Inicializa Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(staticIP, gateway, subnet);
  WiFi.softAP(ap_ssid, ap_password, CHANNEL, HIDE_SSID, MAX_CONNECTIONS);

  ArduinoOTA.setHostname(OTA_Host_Name);
  ArduinoOTA.begin();
}



void loop() {
  ArduinoOTA.handle();

  digitalWrite(PIN_LED, !digitalRead(PIN_LED));
  delay(250);
}
