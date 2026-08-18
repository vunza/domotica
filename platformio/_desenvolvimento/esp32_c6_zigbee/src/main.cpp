#include <Arduino.h>
#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

// Endpoint 1 (Padrão para relés/interruptores Zigbee)
#define ZIGBEE_SWITCH_ENDPOINT 1

uint8_t led = 15;     // LED Integrado da XIAO ESP32-C6 (Ativo em LOW)
uint8_t button = 9;   // Botão BOOT (IO9)

ZigbeeLight zbLight = ZigbeeLight(ZIGBEE_SWITCH_ENDPOINT);

/********************* Função de Controlo do LED **************************/
void setLED(bool value) {
  digitalWrite(led, value ? LOW : HIGH);
  Serial.printf("Estado do LED alterado: %s\n", value ? "LIGADO" : "DESLIGADO");
}

/********************* Setup **************************/
void setup() {
  Serial.begin(115200);

  pinMode(led, OUTPUT);
  setLED(false); // Inicia desligado

  pinMode(button, INPUT_PULLUP);

  // EMULAÇÃO SONOFF BASICZBR3: O Zigbee2MQTT ativa automaticamente
  // o Binding + Reporting para atualizações do botão físico!
  zbLight.setManufacturerAndModel("SONOFF", "BASICZBR3");
  
  zbLight.onLightChange(setLED);

  Serial.println("A adicionar endpoint Zigbee...");
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
    delay(50); // Debounce
    if (digitalRead(button) == LOW) {
      unsigned long startTime = millis();
      bool isLongPress = false;

      while (digitalRead(button) == LOW) {
        delay(50);
        // Reset de fábrica se pressionado por mais de 3s
        if ((millis() - startTime) > 3000) {
          isLongPress = true;
          Serial.println("Reset de fábrica acionado! A apagar NVS...");
          delay(500);
          Zigbee.factoryReset();
          break;
        }
      }

      // Clique curto: Alterna o estado local e envia o pacote de relatório para a rede
      if (!isLongPress) {
        bool newState = !zbLight.getLightState();
        
        // 1. Atualiza a pilha Zigbee (dispara o Report Attribute para o Home Assistant)
        zbLight.setLight(newState);
        
        // 2. Atualiza o pino físico do LED
        setLED(newState);
      }

      while (digitalRead(button) == LOW) delay(10);
    }
  }
  delay(20);
}