/*
    ESP8266:
    ========
    GPIO5 --> BC7215A TX 
    GPIO16 -> BC7215A RX
    GPIO14 -> BC7215A MOD
    GPIO4 --> BC7215A BUSY
    3.3V ---> BC7215A VCC

    ESP32 WROOM:
    ============
    GPIO25 -> BC7215A TX 
    GPIO33 -> BC7215A RX
    GPIO27 -> BC7215A MOD 
    GPIO26 -> BC7215A BUSY
    3.3V ---> BC7215A VCC

    SEED STUDIO ESP32 C6:
    =====================
    GPIO17 -> BC7215A TX 
    GPIO16 -> BC7215A RX
    GPIO[1|2] -> BC7215A MOD 
    GPIO21 -> BC7215A BUSY
    3.3V ---> BC7215A VCC
*/



#include <Arduino.h>
#include <Preferences.h>
//#include <bc7215.h>
#include <bc7215ac.h>
#include "ctrl_debug.h"
#include <eeprom_manager_pro.h>
#include <ArduinoJson.h>
#include "Zigbee.h"
#include "ep/ZigbeeTempSensor.h" 

#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#define ZIGBEE_LIGHT_ENDPOINT 1
#define ZIGBEE_TEMP_ENDPOINT  2 
#define JSON_SIZE 128

const uint8_t MOD_PIN  = 1;
const uint8_t BUSY_PIN = 21;
const uint8_t TX_PIN  = 16;
const uint8_t RX_PIN = 17;
const uint8_t LED_PIN = 15;     
const uint8_t BUTTON_PIN = 9;   

#if defined(ESP32)
  HardwareSerial bc7215Serial(1);        
#elif defined(ESP8266)
  #define bc7215Serial Serial1 
#endif

BC7215 bc7215Board(bc7215Serial, MOD_PIN, BUSY_PIN);
BC7215AC ac(bc7215Board);

const char* MODES[]      = {"Auto", "Cool", "Heat", "Dry", "Fan", "Keep", "N/A"};
const char* FANSPEED[]   = {"Auto", "Low", "Med", "High", "Keep", "N/A"};
const char* PWR_STATUS[] = {"OFF", "ON", "TOGGLE", "N/A"};

// Protótipos de função
void emparelharAc();
void descodificarSinalAc();
//void callback_mqtt_rx(char* topic, byte* payload, unsigned int length);
//void setup_wifi();
//void reconnectMQTT();
void enviaDadosAc(int temp, int mode_index, int fan_index, int power_index);


ZigbeeLight zbLight = ZigbeeLight(ZIGBEE_LIGHT_ENDPOINT);
ZigbeeTempSensor zbTemperature = ZigbeeTempSensor(ZIGBEE_TEMP_ENDPOINT);

Preferences prefs;

void setLED(bool value) {
  digitalWrite(LED_PIN, value ? LOW : HIGH);
  if (prefs.getBool("state", false) != value) {
    prefs.putBool("state", value);
  }
}


/////////////
// setup() //
/////////////
void setup() {
  Serial.begin(115200);
  delay(1000);

#if defined(ESP32)
  bc7215Serial.begin(19200, SERIAL_8N2, RX_PIN, TX_PIN); // RX, TX
#elif defined(ESP8266)
  bc7215Serial.begin(19200, SERIAL_8N2);
#endif

  delay(100);
  bc7215Board.setRx();
  delay(50);

  // Carrega dados evitando cópias desnecessárias na Stack
  lerDadosEEPROM(esp_cfg_data); 
  ac.startCapture();

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  prefs.begin("light", false);
  bool lastState = prefs.getBool("state", false);
  digitalWrite(LED_PIN, lastState ? LOW : HIGH);

  zbLight.setManufacturerAndModel("CustomESP32", "C6_Multi_Sensor");
  zbLight.onLightChange(setLED);

  // 1. Adiciona a luz ao Endpoint 1
  Zigbee.addEndpoint(&zbLight);

  // 2. Configura e adiciona o Sensor de Temperatura ao Endpoint 2
  zbTemperature.setMinMaxValue(16.0, 30.0); // Opcional: Define limites
  zbTemperature.setTemperature(25.0);       // Temperatura inicial de exemplo
  Zigbee.addEndpoint(&zbTemperature);

  if (!Zigbee.begin()) {
    imprimeln("Falha ao iniciar o Zigbee! A reiniciar...");
    delay(1000);
    ESP.restart();
  }

  imprimeln("A aguardar conexão com a rede Zigbee...");

  while (!Zigbee.connected()) {
    delay(500);
    imprime(".");
  }

  imprimeln("\nConectado com sucesso!");

  zbLight.setLight(lastState);

}// Fim do setup()



////////////////////////////////
// Loop principal do programa //
////////////////////////////////
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
    
    imprimef("A atualizar temperatura no Zigbee: %.1f °C\n", leituraAtual);
    
    // Apenas atualiza o valor do atributo na pilha Zigbee (SEM chamar addEndpoint de novo)
    zbTemperature.setTemperature(leituraAtual); 
  }

  // Lógica do botão
  if (digitalRead(BUTTON_PIN) == LOW) {  
    delay(50); 
    if (digitalRead(BUTTON_PIN) == LOW) {
      bool newState = !zbLight.getLightState();
      zbLight.setLight(newState);
      while (digitalRead(BUTTON_PIN) == LOW) delay(10);
    }
  }
   
    
       
  if (!paired && !esp_cfg_data.configurado) {  
    emparelharAc();
    return;
  }
  else if (!paired && esp_cfg_data.configurado) {
    if (ac.init(esp_cfg_data.dataPkt, esp_cfg_data.formatPkt)) {
      paired = true;
      imprimeln(F(">>> Sucesso: Configuração restaurada! Pronto para operar."));
      return;
    } else {
      imprimeln(F("Falha ao inicializar com dados salvos. Forçando novo pareamento..."));
      esp_cfg_data.configurado = false;
      emparelharAc();
    }
  }
  else if (paired && esp_cfg_data.configurado) {
    descodificarSinalAc();
  }  

  delay(10); // Pequena pausa para evitar sobrecarga do loop

}// Fim do loop()




////////////////////////////////////////////////////////////
// Função para emparelhar o protocolo do ar-condicionado //
///////////////////////////////////////////////////////////
void emparelharAc() {   

    if (ac.signalCaptured()) {
        ac.stopCapture();
        imprimeln(F("Sinal recebido."));

        if (ac.init()) {
            paired = true;
            imprimeln(F("Pareamento OK!"));

            esp_cfg_data.configurado = true;                       
            esp_cfg_data.formatPkt = *ac.getFormatPkt(); 
            esp_cfg_data.dataPkt = *((bc7215DataMaxPkt_t*)ac.getDataPkt());
            
            salvarDadosEEPROM(esp_cfg_data);
            digitalWrite(LED_PIN, LOW);             
            ac.startCapture();
        } else {
            imprimeln(F("Falha no pareamento. Tente novamente."));
            ac.startCapture();
        }
    } else {
        static unsigned long ultimoPisca = 0;
        if (millis() - ultimoPisca > 250) {
            ultimoPisca = millis();
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));            
        }
    }

}// Fim de emparelharAc()


///////////////////////////////////////////////////////////////////////////
// Função para decodificar o sinal do controle remoto do ar-condicionado //
///////////////////////////////////////////////////////////////////////////
void descodificarSinalAc() {
    if (ac.signalCaptured()) {
        ac.stopCapture();

        int temp = -1, mode = -1, fan = -1, power = -1;

        if (ac.parse(temp, mode, fan, power)) {
            StaticJsonDocument<JSON_SIZE> doc;           

            if (power >= 0 && power <= 2) {                 
                doc["power"] = power;
                //mqttClient.publish(MQTT_TOPIC_POWER_STATE, PWR_STATUS[power], true);
            }            
            
            if (temp >= 16 && temp <= 30) {
                doc["temp"] = temp;
                //mqttClient.publish(MQTT_TOPIC_TEMP_STATE, String(temp).c_str(), true);
            }

            if (mode >= 0 && mode <= 4) {
                doc["mode"] = mode;
                //mqttClient.publish(MQTT_TOPIC_MODE_STATE, MODES[mode], true);
            }
            
            if (fan >= 0 && fan <= 3) {
                doc["fan"] = fan;
                //mqttClient.publish(MQTT_TOPIC_FAN_STATE, FANSPEED[fan], true); 
            }

            doc["for_tx"] = false;

            char payload[JSON_SIZE];
            serializeJson(doc, payload);
            
            //mqttClient.publish(MQTT_TOPIC_AC_STATES, payload, true); 
            
            imprimef("[COMANDO] >> POWER: %s TEP: %d MODE: %s FAN: %s\n", PWR_STATUS[power],temp,MODES[mode],FANSPEED[fan]);

        } else {
            imprimeln(F("Falha ao decodificar sinal."));
        }
        ac.startCapture();
    }

}// Fim de descodificarSinalAc()