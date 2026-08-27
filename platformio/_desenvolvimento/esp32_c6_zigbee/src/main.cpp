#include <Arduino.h>
#include <Preferences.h>

#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"
#include "ep/ZigbeeTempSensor.h" 

#define ZIGBEE_LIGHT_ENDPOINT 1
#define ZIGBEE_TEMP_ENDPOINT  2   

uint8_t led = 15;     
uint8_t button = 9;   

ZigbeeLight zbLight = ZigbeeLight(ZIGBEE_LIGHT_ENDPOINT);
ZigbeeTempSensor zbTemperature = ZigbeeTempSensor(ZIGBEE_TEMP_ENDPOINT);

Preferences prefs;

void setLED(bool value) {
  digitalWrite(led, value ? LOW : HIGH);
  if (prefs.getBool("state", false) != value) {
    prefs.putBool("state", value);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(led, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  prefs.begin("light", false);
  bool lastState = prefs.getBool("state", false);
  digitalWrite(led, lastState ? LOW : HIGH);

  zbLight.setManufacturerAndModel("CustomESP32", "C6_Multi_Sensor");
  zbLight.onLightChange(setLED);

  // 1. Adiciona a luz ao Endpoint 1
  Zigbee.addEndpoint(&zbLight);

  // 2. Configura e adiciona o Sensor de Temperatura ao Endpoint 2
  zbTemperature.setMinMaxValue(16.0, 30.0); // Opcional: Define limites
  zbTemperature.setTemperature(25.0);       // Temperatura inicial de exemplo
  Zigbee.addEndpoint(&zbTemperature);

  if (!Zigbee.begin()) {
    Serial.println("Falha ao iniciar o Zigbee! A reiniciar...");
    delay(1000);
    ESP.restart();
  }

  Serial.println("A aguardar conexão com a rede Zigbee...");
  while (!Zigbee.connected()) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado com sucesso!");

  zbLight.setLight(lastState);
}



/********************* Loop **************************/
void loop() {
  // Exemplo: Atualiza a temperatura a cada 30 segundos
  static unsigned long lastTempUpdate = 0;
  static float leituraAtual = 16.0; // Valor inicial de temperatura
  
  if (millis() - lastTempUpdate > 30000) {
    lastTempUpdate = millis();
    
    if (leituraAtual < 29.0) {
      leituraAtual += 1.0;
    } else {
      leituraAtual = 16.0; // Reinicia o ciclo de simulação
    } 
    
    Serial.printf("A atualizar temperatura no Zigbee: %.1f °C\n", leituraAtual);
    
    // Apenas atualiza o valor do atributo na pilha Zigbee (SEM chamar addEndpoint de novo)
    zbTemperature.setTemperature(leituraAtual); 
  }

  // Lógica do botão
  if (digitalRead(button) == LOW) {  
    delay(50); 
    if (digitalRead(button) == LOW) {
      bool newState = !zbLight.getLightState();
      zbLight.setLight(newState);
      while (digitalRead(button) == LOW) delay(10);
    }
  }
  delay(20);
}