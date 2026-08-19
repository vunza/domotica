#include <Arduino.h>
#include <Preferences.h> // 1. Biblioteca para salvar estados na memória flash NVS

#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

#define ZIGBEE_SWITCH_ENDPOINT 1

uint8_t led = 15;     // LED Integrado da XIAO ESP32-C6 (Ativo em LOW)
uint8_t button = 9;   // Botão BOOT (IO9)

ZigbeeLight zbLight = ZigbeeLight(ZIGBEE_SWITCH_ENDPOINT);
Preferences prefs;    // Instância para gerenciar a memória NVS

/********************* Função de Controlo do LED **************************/
void setLED(bool value) {
  digitalWrite(led, value ? LOW : HIGH);
  
  // Salva na NVS apenas se o estado mudar (evita desgaste desnecessário da flash)
  if (prefs.getBool("state", false) != value) {
    prefs.putBool("state", value);
  }
  
  Serial.printf("Estado do LED alterado: %s\n", value ? "LIGADO" : "DESLIGADO");
}

/********************* Setup **************************/
void setup() {
  Serial.begin(115200);

  pinMode(led, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  // 2. Abre o espaço de memória NVS "light"
  prefs.begin("light", false);
  
  // 3. Lê o último estado gravado (padrão 'false' se for a primeira vez)
  bool lastState = prefs.getBool("state", false);
  
  // Aplica o estado restaurado diretamente ao pino físico
  digitalWrite(led, lastState ? LOW : HIGH);

  zbLight.setManufacturerAndModel("SONOFF", "BASICZBR3");
  zbLight.onLightChange(setLED);

  Serial.println("A adicionar endpoint Zigbee...");
  Zigbee.addEndpoint(&zbLight);

  if (!Zigbee.begin()) {
    Serial.println("Falha ao iniciar o Zigbee! A reiniciar...");
    delay(1000);
    ESP.restart();
  }

  // 4. Sincroniza o estado restaurado com o atributo da pilha Zigbee
  zbLight.setLight(lastState);

  Serial.printf("Zigbee iniciado! Estado restaurado para: %s\n", lastState ? "LIGADO" : "DESLIGADO");
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
          prefs.clear(); // Limpa o estado salvo na memória NVS
          Zigbee.factoryReset();
          break;
        }
      }

      // Clique curto: Alterna o estado local e envia o pacote de relatório para a rede
      if (!isLongPress) {
        bool newState = !zbLight.getLightState();
        
        // 1. Atualiza a pilha Zigbee
        zbLight.setLight(newState);
        
        // 2. Atualiza o pino físico e grava na NVS
        setLED(newState);
      }

      while (digitalRead(button) == LOW) delay(10);
    }
  }
  delay(20);
}