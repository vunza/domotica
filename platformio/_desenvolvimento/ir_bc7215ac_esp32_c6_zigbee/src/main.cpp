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
#include "ep/ZigbeeTempSensor.h"
#include "ep/ZigbeeDimmableLight.h"

#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#define ZIGBEE_SLIDER_ENDPOINT 1
#define JSON_SIZE 128

const uint8_t MOD_PIN = 1;
const uint8_t BUSY_PIN = 21;
const uint8_t TX_PIN = 16;
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

const char *MODES[] = {"Auto", "Cool", "Heat", "Dry", "Fan", "Keep", "N/A"};
const char *FANSPEED[] = {"Auto", "Low", "Med", "High", "Keep", "N/A"};
const char *PWR_STATUS[] = {"OFF", "ON", "TOGGLE", "N/A"};

// Protótipos de função
void emparelharAc();
void descodificarSinalAc();
void cb_onTemperatureChange(bool state, uint8_t level);
void enviaDadosAc(int temp, int mode_index, int fan_index, int key);

ZigbeeDimmableLight zbSliderControl = ZigbeeDimmableLight(ZIGBEE_SLIDER_ENDPOINT);
int temperaturaAtualVirtual = 25; 
Preferences prefs;

/////////////
// setup() //
/////////////
void setup(){
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

  // Configuração do dispositivo Zigbee
  zbSliderControl.setManufacturerAndModel("CustomESP32", "C6_Multi_Sensor");  
  zbSliderControl.onLightChange(cb_onTemperatureChange);

  // ADICIONA ENDPOINTS (Apenas uma vez cada)
  Zigbee.addEndpoint(&zbSliderControl); 
 
  
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
    imprimeln(F("Esperando o sinal para o Pareamento..."));
  }
  else {
    imprimeln(F("\nConfiguração do AC já existente. A iniciar operação normal..."));
  }

} // Fim do setup()

////////////////////////////////
// Loop principal do programa //
////////////////////////////////
void loop() {

  if (!paired && !esp_cfg_data.configurado) {
    emparelharAc();
    return;
  }
  else if (!paired && esp_cfg_data.configurado) {
    if (ac.init(esp_cfg_data.dataPkt, esp_cfg_data.formatPkt)){
      paired = true;
      imprimeln(F(">>> Sucesso: Configuração restaurada! Pronto para operar."));
      return;
    }
    else {
      imprimeln(F("Falha ao inicializar com dados salvos. Forçando novo pareamento..."));
      esp_cfg_data.configurado = false;
      emparelharAc();
    }
  }
  else if (paired && esp_cfg_data.configurado) {
    descodificarSinalAc();
  }

  delay(10); // Pequena pausa para evitar sobrecarga do loop

} // Fim do loop()

////////////////////////////////////////////////////////////
// Função para emparelhar o protocolo do ar-condicionado //
///////////////////////////////////////////////////////////
void emparelharAc(){

  if (ac.signalCaptured()) {
    ac.stopCapture();
    imprimeln(F("Sinal recebido."));

    if (ac.init()){
      paired = true;
      imprimeln(F("Pareamento OK!"));

      esp_cfg_data.configurado = true;
      esp_cfg_data.formatPkt = *ac.getFormatPkt();
      esp_cfg_data.dataPkt = *((bc7215DataMaxPkt_t *)ac.getDataPkt());

      salvarDadosEEPROM(esp_cfg_data);
      digitalWrite(LED_PIN, LOW);
      ac.startCapture();
    }
    else{
      imprimeln(F("Falha no pareamento. Tente novamente."));
      ac.startCapture();
    }
  }
  else{
    static unsigned long ultimoPisca = 0;
    if (millis() - ultimoPisca > 250){
      ultimoPisca = millis();
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
  }

} // Fim de emparelharAc()

///////////////////////////////////////////////////////////////////////////
// Função para decodificar o sinal do controle remoto do ar-condicionado //
///////////////////////////////////////////////////////////////////////////
void descodificarSinalAc(){
  if (ac.signalCaptured()) {
    ac.stopCapture();

    int temp = -1, mode = -1, fan = -1, power = -1;

    if (ac.parse(temp, mode, fan, power)) {
      StaticJsonDocument<JSON_SIZE> doc;

      if (power >= 0 && power <= 2){
        doc["power"] = power;
      }

      if (temp >= 16 && temp <= 30){
        doc["temp"] = temp;
        temperaturaAtualVirtual = temp; // Sincroniza a variável local
      }

      if (mode >= 0 && mode <= 4){
        doc["mode"] = mode;
      }

      if (fan >= 0 && fan <= 3){
        doc["fan"] = fan;
      }

      doc["for_tx"] = false;

      char payload[JSON_SIZE];
      serializeJson(doc, payload);

      imprimef("[COMANDO] >> POWER: %s TEP: %d MODE: %s FAN: %s\n", PWR_STATUS[power], temp, MODES[mode], FANSPEED[fan]);
    }
    else{
      imprimeln(F("Falha ao decodificar sinal."));
    }
    ac.startCapture();
  }

} // Fim de descodificarSinalAc()

////////////////////////////////////////////////////////////////
// Função para enviar os dados do ar-condicionado via BC7215 //
///////////////////////////////////////////////////////////////
void enviaDadosAc(int temp, int mode_index, int fan_index, int key){
  unsigned long startTime = millis();
  
  ac.init(esp_cfg_data.dataPkt, esp_cfg_data.formatPkt);
  ac.startCapture();
  delay(50); 

  bc7215Board.setTx();
  delay(50);
  ac.setTo(temp, mode_index, fan_index, -1);
  imprimef("[SOFTWARE] >> POWER: %s TEP: %d MODE: %s FAN: %s FOR_TX: %d\n", PWR_STATUS[1], temp, MODES[mode_index], FANSPEED[fan_index], 1);
  
  while (ac.isBusy() && (millis() - startTime < 3000)){
    delay(10);
  }
  bc7215Board.setRx();

} // Fim de enviaDadosAc()

//////////////////////////////////////////////////////////////////
// call back function to handle temperature changes from Zigbee //
//////////////////////////////////////////////////////////////////
void cb_onTemperatureChange(bool state, uint8_t level) {

  imprimef("[ZIGBEE] Callback acionado! State: %s, Level: %d\n", state ? "ON" : "OFF", level);

  // Mapeia os 255 níveis proporcionalmente para os 14 intervalos (16°C a 30°C)
  int novaTemp = 16 + round((float)level / 255.0 * 14.0);
  
  if (novaTemp < 16) novaTemp = 16;
  if (novaTemp > 30) novaTemp = 30;

  if (novaTemp != temperaturaAtualVirtual) {
    temperaturaAtualVirtual = novaTemp;
    imprimef("[ZIGBEE] Temperatura alterada -> Nível ZCL: %d -> Temperatura: %d°C\n", level, temperaturaAtualVirtual);
    
    enviaDadosAc(temperaturaAtualVirtual, 1, 0, 1);
  }

}// Fim de cb_onTemperatureChange()