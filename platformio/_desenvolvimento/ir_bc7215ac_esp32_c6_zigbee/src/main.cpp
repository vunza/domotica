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
#include <bc7215ac.h>
#include "ctrl_debug.h"
#include <eeprom_manager_pro.h>
#include <ArduinoJson.h>
#include "Zigbee.h"
#include "ep/ZigbeeDimmableLight.h"
#include "ep/ZigbeeSwitch.h"

#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#define ZIGBEE_TEMPERATURE_ENDPOINT 1
#define ZIGBEE_POWER_ENDPOINT 2
#define JSON_SIZE 128

const uint8_t MOD_PIN = 1;
const uint8_t BUSY_PIN = 21;
const uint8_t TX_PIN = 16;
const uint8_t RX_PIN = 17;
const uint8_t BUTTON_PIN = 9;

#if defined(ESP32)
HardwareSerial bc7215Serial(1);
#elif defined(ESP8266)
#define bc7215Serial Serial1
#endif

BC7215 bc7215Board(bc7215Serial, MOD_PIN, BUSY_PIN);
BC7215AC ac(bc7215Board);

const char *MODES[] = {"Auto", "Cool", "Heat", "Dry", "Fan", "Keep", "N/A"};
const char *FANSPEED[] = {"Auto", "Low", "Med", "High", "Keep", "N/A"};
const char *PWR_STATUS[] = {"OFF", "ON", "TOGGLE", "N/A"};

// Protótipos de função
void emparelharAc();
void descodificarSinalAc();
void cb_onTemperatureChange(bool state, uint8_t level);
void cb_onPowerChange(bool state);
void enviaDadosAc(int temp, int mode_index, int fan_index, int key);
void verificarBotaoFactoryReset();

ZigbeeDimmableLight zbTemperatureControl = ZigbeeDimmableLight(ZIGBEE_TEMPERATURE_ENDPOINT);
ZigbeeSwitch zbPowerControl = ZigbeeSwitch(ZIGBEE_POWER_ENDPOINT);

Preferences prefs;

/////////////
// setup() //
/////////////
void setup(){
  Serial.begin(115200);
  delay(1000);

#if defined(ESP32)
  bc7215Serial.begin(19200, SERIAL_8N2, RX_PIN, TX_PIN);
#elif defined(ESP8266)
  bc7215Serial.begin(19200, SERIAL_8N2);
#endif

  delay(100);
  bc7215Board.setRx();
  delay(50);

  lerDadosEEPROM(esp_cfg_data);
  ac.startCapture();

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  prefs.begin("light", false);

  // Configuração dos dispositivos Zigbee
  zbTemperatureControl.setManufacturerAndModel("CustomESP32", "C6_Multi_Sensor");  
  zbTemperatureControl.onLightChange(cb_onTemperatureChange);

  zbPowerControl.setManufacturerAndModel("CustomESP32", "C6_Multi_Sensor");
  zbPowerControl.onLightStateChange(cb_onPowerChange); // <--- Método correto corrigido aqui

  // ADICIONA ENDPOINTS
  Zigbee.addEndpoint(&zbTemperatureControl); 
  Zigbee.addEndpoint(&zbPowerControl);
 
  if (!Zigbee.begin()){
    imprimeln(F("Falha ao iniciar o Zigbee! A reiniciar..."));
    delay(1000);
    ESP.restart();
  }

  imprimeln(F("A aguardar conexão com a rede Zigbee..."));

  while (!Zigbee.connected()){
    delay(500);
    imprime(".");
  }

  imprimeln("\nConectado com sucesso!");
  
  if (!paired && !esp_cfg_data.configurado) {
    imprimeln(F("\nPAREAMENTO DO PROTOCOLO DO AC"));
    imprimeln(F("Ligue o controlo remoto no Modo COOL a 25°C e pressione FAN.\n"));
  }
}

////////////////////////////////
// Loop principal do programa //
////////////////////////////////
void loop() {
  verificarBotaoFactoryReset();

  if (!paired && !esp_cfg_data.configurado) {
    emparelharAc();
    return;
  }
  else if (!paired && esp_cfg_data.configurado) {
    if (ac.init(esp_cfg_data.dataPkt, esp_cfg_data.formatPkt)){
      paired = true;
      return;
    }
    else {
      esp_cfg_data.configurado = false;
      emparelharAc();
    }
  }
  else if (paired && esp_cfg_data.configurado) {
    descodificarSinalAc();
  }

  delay(10);
}

void verificarBotaoFactoryReset() {
  static unsigned long pressTime = 0;
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (pressTime == 0) {
      pressTime = millis();
    } else if (millis() - pressTime > 5000) {
      imprimeln(F("\n[RESET] A apagar configurações e reiniciar em modo de fábrica..."));
      esp_cfg_data.configurado = false;
      salvarDadosEEPROM(esp_cfg_data);
      prefs.clear();
      Zigbee.factoryReset();
      delay(1000);
      ESP.restart();
    }
  } else {
    pressTime = 0;
  }
}

void emparelharAc(){
  if (ac.signalCaptured()) {
    ac.stopCapture();
    if (ac.init()){
      paired = true;
      esp_cfg_data.configurado = true;
      esp_cfg_data.formatPkt = *ac.getFormatPkt();
      esp_cfg_data.dataPkt = *((bc7215DataMaxPkt_t *)ac.getDataPkt());
      salvarDadosEEPROM(esp_cfg_data);
      ac.startCapture();
    }
    else{
      ac.startCapture();
    }
  }
}

void descodificarSinalAc(){
  if (ac.signalCaptured()) {
    ac.stopCapture();
    int temp = -1, mode = -1, fan = -1, power = -1;

    if (ac.parse(temp, mode, fan, power)) {
      if (temp >= 16 && temp <= 30){
        uint8_t percent = map(temp, 16, 30, 0, 255);
        zbTemperatureControl.setLight(true, percent);
      }
    }
    ac.startCapture();
  }
}

void enviaDadosAc(int temp, int mode_index, int fan_index, int key){
  unsigned long startTime = millis();
  ac.init(esp_cfg_data.dataPkt, esp_cfg_data.formatPkt);
  ac.startCapture();
  delay(50); 

  bc7215Board.setTx();
  delay(50);
  ac.setTo(temp, mode_index, fan_index, -1);
  
  while (ac.isBusy() && (millis() - startTime < 3000)){
    delay(10);
  }
  bc7215Board.setRx();
}

void cb_onTemperatureChange(bool state, uint8_t level) {
  int novaTemp = 16 + round((float)level / 255.0 * 14.0);
  if (novaTemp < 16) novaTemp = 16;
  if (novaTemp > 30) novaTemp = 30;
  enviaDadosAc(novaTemp, 1, 0, 1);
}

void cb_onPowerChange(bool state) {
  int extraValue = state ? 1 : 0;
  imprimef("[ZIGBEE] Endpoint Extra alterado para: %d\n", extraValue);
}