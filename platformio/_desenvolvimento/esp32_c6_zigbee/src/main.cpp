#include <Arduino.h>
#include <Preferences.h>

#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

#define ZIGBEE_SWITCH_ENDPOINT 1

uint8_t led = 15;     // LED Integrado da XIAO ESP32-C6 (Ativo em LOW)
uint8_t button = 9;   // Botão BOOT (IO9)

ZigbeeLight zbLight = ZigbeeLight(ZIGBEE_SWITCH_ENDPOINT);
Preferences prefs;

/********************* Função de Controlo do LED **************************/
void setLED(bool value) {
  digitalWrite(led, value ? LOW : HIGH);
  
  if (prefs.getBool("state", false) != value) {
    prefs.putBool("state", value);
  }
  
  Serial.printf("Estado do LED alterado: %s\n", value ? "LIGADO" : "DESLIGADO");
}

/********************* Setup **************************/
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(led, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  prefs.begin("light", false);
  bool lastState = prefs.getBool("state", false);
  digitalWrite(led, lastState ? LOW : HIGH);

  // Use identidades limpas ou da Espressif para evitar regras estritas do Z2M
  zbLight.setManufacturerAndModel("CustomESP32", "C6_Zigbee_Switch");
  
  zbLight.onLightChange(setLED);

  Serial.println("A adicionar endpoint Zigbee...");
  Zigbee.addEndpoint(&zbLight);

  if (!Zigbee.begin()) {
    Serial.println("Falha ao iniciar o Zigbee! A reiniciar...");
    delay(1000);
    ESP.restart();
  }

  // CRUCIAL: Aguarda o dispositivo se conectar/parear de fato com a rede Zigbee
  Serial.println("A aguardar conexão com a rede Zigbee (Zigbee2MQTT)...");
  unsigned long startAttemptTime = millis();
  while (!Zigbee.connected()) {
    Serial.print(".");
    delay(500);
    // Se demorar mais de 45 segundos, reinicia para tentar novamente
    if (millis() - startAttemptTime > 45000) {
      Serial.println("\nTimeout de conexão. A reiniciar...");
      ESP.restart();
    }
  }
  Serial.println("\nConectado à rede Zigbee com sucesso!");

  zbLight.setLight(lastState);
  Serial.printf("Zigbee iniciado! Estado restaurado para: %s\n", lastState ? "LIGADO" : "DESLIGADO");
}



/********************* Loop **************************/
void loop() {
  if (digitalRead(button) == LOW) {  
    delay(50); // Debounce
    if (digitalRead(button) == LOW) {
      unsigned long startTime = millis();
      bool isLongPress = false;

      while (digitalRead(button) == LOW) {
        delay(50);
        if ((millis() - startTime) > 3000) {
          isLongPress = true;
          Serial.println("Reset de fábrica acionado! A apagar NVS...");
          delay(500);
          prefs.clear();
          Zigbee.factoryReset();
          break;
        }
      }

      if (!isLongPress) {
        // Alterna o estado lógico invertendo o estado atual do Zigbee
        bool newState = !zbLight.getLightState();
        
        // ATENÇÃO: Chamar setLight() aqui altera o atributo Zigbee, 
        // aciona a função onLightChange (que muda o LED e salva na NVS) 
        // e dispara o reporte automático para o Zigbee2MQTT!
        zbLight.setLight(newState);
      }

      while (digitalRead(button) == LOW) delay(10);
    }
  }
  delay(20);
}