#include <Arduino.h>
#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

/* Configurações da XIAO ESP32-C6 */
#define ZIGBEE_LIGHT_ENDPOINT 10
const uint8_t led = 15;     // LED Integrado (Ativo em LOW)
const uint8_t button = 9;   // Botão BOOT / User (IO9)

ZigbeeLight zbLight = ZigbeeLight(ZIGBEE_LIGHT_ENDPOINT);
bool initialSyncDone = false;

/********************* Callback do LED **************************/
void aoReceberComandoLuz(bool estado) {
  Serial.printf("Comando Zigbee/Estado alterado! Novo estado: %s\n", estado ? "LIGADO" : "DESLIGADO");
  digitalWrite(led, estado ? LOW : HIGH); // Acende em LOW
}

/********************* Setup **************************/
void setup() {
  Serial.begin(115200);

  pinMode(led, OUTPUT);
  aoReceberComandoLuz(false); // Inicia apagado

  pinMode(button, INPUT_PULLUP);

  // Usar nome/modelo padrão Espressif para compatibilidade universal
  zbLight.setManufacturerAndModel("Espressif", "ZBLightBulb");
  zbLight.onLightChange(aoReceberComandoLuz);

  Serial.println("A adicionar endpoint Zigbee...");
  Zigbee.addEndpoint(&zbLight);

  // Inicializa o stack Zigbee
  if (!Zigbee.begin()) {
    Serial.println("Falha ao iniciar o Zigbee! A reiniciar...");
    delay(1000);
    ESP.restart();
  }

  Serial.println("Zigbee iniciado! A procurar rede para pareamento...");
}

/********************* Loop **************************/
void loop() {
  // 1. Sincroniza estado inicial ao conectar à rede
  if (Zigbee.connected() && !initialSyncDone) {
    bool savedState = zbLight.getLightState();
    aoReceberComandoLuz(savedState);
    Serial.printf("Conectado à rede Zigbee com sucesso! Estado: %s\n", savedState ? "LIGADO" : "DESLIGADO");
    initialSyncDone = true;
  }

  // 2. Leitura do Botão
  if (digitalRead(button) == LOW) {
    delay(50); // Debounce
    if (digitalRead(button) == LOW) {
      unsigned long startTime = millis();
      bool isLongPress = false;

      while (digitalRead(button) == LOW) {
        delay(50);
        // Pressionar por mais de 3s força o Reset de Fábrica e reabre o pareamento
        if ((millis() - startTime) > 3000) {
          isLongPress = true;
          Serial.println("Reset de fábrica acionado! A apagar NVS e reiniciar pareamento...");
          delay(500);
          Zigbee.factoryReset();
          break;
        }
      }

      // Clique curto: Alterna estado local e notifica a rede
      if (!isLongPress) {
        bool newState = !zbLight.getLightState();
        
        // Atualiza a biblioteca Zigbee e envia o pacote para o Home Assistant
        zbLight.setLight(newState);
        
        // Atualiza o pino do LED localmente
        aoReceberComandoLuz(newState);
      }

      while (digitalRead(button) == LOW) {
        delay(10);
      }
    }
  }

  delay(20);
}