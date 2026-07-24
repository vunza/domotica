
/*#include "ZigbeeCore.h"
#include "ep/ZigbeeLight.h"
#include <Arduino.h>

#define LED_PIN 15    // LED Azul Integrado
#define BUTTON_PIN 9  // Botão de Boot Integrado
#define ZIGBEE_LIGHT_ENDPOINT 10

ZigbeeLight zbLight = ZigbeeLight(ZIGBEE_LIGHT_ENDPOINT);

bool ledLigado = false;       // Armazena o estado lógico atual do LED
int estadoBotaoAnterior = HIGH; // Armazena o último estado lido do botão

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Garante estabilidade no sinal do botão

  // Como o LED é de lógica invertida, iniciamos desligando ele (enviando HIGH)
  digitalWrite(LED_PIN, HIGH); 
}

void loop() {
  // Lê o estado atual do botão (Solto = HIGH, Pressionado = LOW)
  int estadoBotaoAtual = digitalRead(BUTTON_PIN);

  // Detecta a BORDA DE DESCIDA: o botão estava solto (HIGH) e acabou de ser pressionado (LOW)
  if (estadoBotaoAtual == LOW && estadoBotaoAnterior == HIGH) {
    
    // Inverte a variável de estado do LED
    ledLigado = !ledLigado;

    // Aplica a lógica ao pino físico do LED
    // Se ledLigado for verdadeiro, envia LOW (acende). Se falso, envia HIGH (apaga).
    if (ledLigado) {
      digitalWrite(LED_PIN, LOW);   // Acende o LED
    } else {
      digitalWrite(LED_PIN, HIGH);  // Apaga o LED
    }

    // Pequeno atraso (debounce) para evitar que o ruído mecânico do botão 
    // faça o LED inverter múltiplas vezes em um único clique
    delay(200); 
  }

  // Atualiza o estado anterior para a próxima leitura do loop
  estadoBotaoAnterior = estadoBotaoAtual;
}*/





#include <Arduino.h>
#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

/* Zigbee light bulb configuration */
#define ZIGBEE_LIGHT_ENDPOINT 10
uint8_t led = 15;  // LED Azul Integrado
uint8_t button = 9;  // Botão de Boot Integrado

ZigbeeLight zbLight = ZigbeeLight(ZIGBEE_LIGHT_ENDPOINT);

/********************* RGB LED functions **************************/
void setLED(bool value) {
  digitalWrite(led, value);
}

/********************* Arduino functions **************************/
void setup() {
  Serial.begin(115200);

  // Init LED and turn it OFF (if LED_PIN == RGB_BUILTIN, the rgbLedWrite() will be used under the hood)
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  // Init button for factory reset
  pinMode(button, INPUT_PULLUP);

  //Optional: set Zigbee device name and model
  zbLight.setManufacturerAndModel("Espressif", "ZBLightBulb");

  // Set callback function for light change
  zbLight.onLightChange(setLED);

  //Add endpoint to Zigbee Core
  Serial.println("Adding ZigbeeLight endpoint to Zigbee Core");
  Zigbee.addEndpoint(&zbLight);

  // When all EPs are registered, start Zigbee. By default acts as ZIGBEE_END_DEVICE
  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  }
  Serial.println("Connecting to network");
  while (!Zigbee.connected()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
}

void loop() {
  // Checking button for factory reset
  if (digitalRead(button) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(button) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        // If key pressed for more than 3secs, factory reset Zigbee and reboot
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
    // Toggle light by pressing the button
    zbLight.setLight(!zbLight.getLightState());
  }
  delay(100);
}

