#include <Arduino.h>
#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

#define ZIGBEE_LIGHT_ENDPOINT 10
uint8_t led = 15;     // LED Integrado da XIAO ESP32-C6 (Ativo em LOW)
uint8_t button = 9;   // Botão BOOT (IO9)

ZigbeeLight zbLight = ZigbeeLight(ZIGBEE_LIGHT_ENDPOINT);

/********************* Função de Controlo do LED **************************/
void setLED(bool value) {
  // Inverte o sinal (LOW = Acendido, HIGH = Apagado)
  digitalWrite(led, value ? LOW : HIGH);
  Serial.printf("Estado da luz alterado: %s\n", value ? "LIGADO" : "DESLIGADO");
}

/********************* Setup **************************/
void setup() {
  Serial.begin(115200);

  pinMode(led, OUTPUT);
  setLED(false); // Inicia desligado

  pinMode(button, INPUT_PULLUP);

  // EMULAÇÃO: Utiliza modelo nativamente suportado pelo Zigbee2MQTT
  zbLight.setManufacturerAndModel("IKEA of Sweden", "TRADFRI bulb E27 W opal 1000lm");
  //zbLight.setManufacturerAndModel("Espressif", "ZBLightBulb");

  zbLight.onLightChange(setLED);

  Serial.println("A adicionar endpoint ZigbeeLight...");
  Zigbee.addEndpoint(&zbLight);

  if (!Zigbee.begin()) {
    Serial.println("Falha ao iniciar o Zigbee! A reiniciar...");
    delay(1000);
    ESP.restart();
  }

  Serial.println("Zigbee iniciado! A aguardar ligação...");
}

/********************* Loop **************************/
void loop() {
  // Leitura do botão físico
  if (digitalRead(button) == LOW) {  
    delay(100); // Debounce
    if (digitalRead(button) == LOW) {
      int startTime = millis();
      bool isLongPress = false;

      while (digitalRead(button) == LOW) {
        delay(50);
        // Reset de fábrica se pressionado por mais de 3s
        if ((millis() - startTime) > 3000) {
          isLongPress = true;
          Serial.println("Reset de fábrica acionado! A apagar NVS...");
          delay(1000);
          Zigbee.factoryReset();
          break;
        }
      }

      // Clique curto: Alterna o estado local e notifica a rede
      if (!isLongPress) {
        bool newState = !zbLight.getLightState();
        
        // Atualiza a biblioteca Zigbee e envia o pacote para o Home Assistant
        zbLight.setLight(newState);
        
        // Atualiza o pino do LED localmente
        setLED(newState);
      }

      while (digitalRead(button) == LOW) delay(10);
    }
  }
  delay(100);
}