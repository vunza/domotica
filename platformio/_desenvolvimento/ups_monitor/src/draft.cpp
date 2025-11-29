// crie um arquivo markdown com a explicacao do codigo abaixo:

// include/ctrl_debug.h
// #ifndef CTRL_DEBUG_H
// #define CTRL_DEBUG_H

// #include <Arduino.h>

// // Ativar mensagens pela serial para depuração
// #ifdef ACTIVAR_SERIAL_DEBUG
// #define imprime(x) Serial.print(x)
// #define imprimeln(x) Serial.println(x)
// #else
// #define imprime(x)
// #define imprimeln(x)
// #endif


// #endif 

// include/headers.h
// #ifndef HEADERS_H
// #define HEADERS_H

// #include "core/device.h"
// #include "network/wifi_manager.h"
// #include "async_elegant_ota/async_elegant_ota.h"
// #include "web_server/web_server.h"
// #include "ctrl_debug.h"
// #include "sensors/INA226Sensor.h"
// #include "displays/lcd_display.h"
// #include "sensors/DTH11Sensor.h"

// #endif 


// src/async_elegant_ota/async_elegant_ota.h
// #ifndef ASYNC_ELEGANT_OTA_H
// #define ASYNC_ELEGANT_OTA_H

// #include <Arduino.h>
// #include <AsyncElegantOTA.h>
// #include <ESPAsyncWebServer.h>
// #include "ctrl_debug.h"

// class ElengantOTA {
// public:
//     static void begin(AsyncWebServer* server);
// };

// #endif

// src/async_elegant_ota/async_elegant_ota.cpp
// #include "async_elegant_ota.h"

// void ElengantOTA::begin(AsyncWebServer* server) {
//     AsyncElegantOTA.begin(server);
//     imprimeln(F("OTA Inicializada!"));
// }


// src/core/device.h
// #pragma once

// #include "ctrl_debug.h"

// class Device {
// public:
//     void initialize();
//     void run();
// private:
//     bool checkHardware();
// };

// src/core/device.cpp

// #include "device.h"

// void Device::initialize() {   

//     #ifdef ESP32
//         // Inicializações específicas do ESP32
//     #elif defined(ESP8266)
//         // Inicializações específicas do ESP8266
//     #endif
    
//     if(!checkHardware()) {
//         imprimeln(F("Falha na verificação do hardware!"));
//     }
// }

// bool Device::checkHardware() {
//     // Verificações comuns de hardware
//     return true;
// }

// void Device::run() {
//     // Lógica principal do dispositivo
// }

// src/displays/lcd_display.h
// #ifndef LCD_DISPLAY_H
// #define LCD_DISPLAY_H

// #include <Arduino.h>
// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>
// #include "ctrl_debug.h"

// class LCDDisplay {
// public:
//     LCDDisplay(uint8_t i2c_addr, uint8_t cols, uint8_t rows);

//     void begin();
//     void showReadings(float current_mA, float voltage_V);

// private:
//     LiquidCrystal_I2C lcd;
//     uint8_t _cols;
//     uint8_t _rows;
// };

// #endif

// src/displays/lcd_display.cpp
// #include "lcd_display.h"

// LCDDisplay::LCDDisplay(uint8_t i2c_addr, uint8_t cols, uint8_t rows)
//     : lcd(i2c_addr, cols, rows), _cols(cols), _rows(rows) {}


// void LCDDisplay::begin() {
//     lcd.init();
//     lcd.backlight();
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     Wire.begin();
// }


// void LCDDisplay::showReadings(float current_mA, float voltage_V) {
//     lcd.clear();
//     lcd.setCursor(0, 0);  // Coluna 0, linha 0
//     lcd.print("I = ");
//     lcd.print(current_mA);
//     lcd.print(" mA");  
//     lcd.setCursor(0, 1); // Coluna 0, linha 1
//     lcd.print("V = ");
//     lcd.print(voltage_V);
//     lcd.print(" V");    
// }


// src/json/json_builder.h
// // Cria um objeto JSON de forma  dinamica
// #ifndef JSON_BUILDER_H
// #define JSON_BUILDER_H

// #include <Arduino.h>

// class JsonBuilder {
// private:
//     String json;
//     bool firstField = true;

// public:
//     JsonBuilder() {
//         json.reserve(128);  // otimiza memória
//         json = "{";
//     }

//     void add(const String &key, const String &value, bool quote = true) {
//         if (!firstField) json += ",";
//         firstField = false;

//         json += "\"" + key + "\":";
//         if (quote) json += "\"" + value + "\"";
//         else json += value;
//     }

//     void add(const String &key, float value, uint8_t decimals = 2) {
//         if (!firstField) json += ",";
//         firstField = false;

//         char buffer[20];
//         dtostrf(value, 1, decimals, buffer);  // SEM AMBIGUIDADE
//         json += "\"" + key + "\":" + buffer;
//     }

//     void add(const String &key, int value) {
//         if (!firstField) json += ",";
//         firstField = false;

//         json += "\"" + key + "\":" + String(value);
//     }

//     String build() {
//         return json + "}";
//     }

//     void reset() {
//         json = "{";
//         firstField = true;
//     }
// };


// #endif


// src/network/wifi_manager.h
// // wifi_manager.h
// #pragma once

// #ifdef ESP32
// #include <WiFi.h>
// #elif defined(ESP8266)
// #include <ESP8266WiFi.h>
// #endif

// #include "ctrl_debug.h"

// class WiFiManager {
// public:
//     void connect(const char* ssid, const char* password);
//     void criar_ap(const char* ssid, const char* password);
//     void checkConnection();
// private:
//     unsigned long lastCheck = 0;
// };

// src/network/wifi_manager.cpp
// // src/network/wifi_manager.cpp
// #include "wifi_manager.h"

// // Conectar ao ponto de acesso WiFi
// void WiFiManager::connect(const char* ssid, const char* password) {
//     WiFi.begin(ssid, password);
    
//     while (WiFi.status() != WL_CONNECTED) {
//         delay(1000);
//         imprimeln(F("Conectando ao WiFi..."));
//     }

//     // Configurar reconexão automática e persistência
//     WiFi.setAutoReconnect(true);
//     WiFi.persistent(true);
    
//     imprimeln(F("\nConectado ao WiFi!"));    
//     imprimeln(WiFi.localIP());
// }


// // Criar ponto de acesso WiFi
// void WiFiManager::criar_ap(const char* ssid, const char* password) {
//     WiFi.softAP(ssid, password);
//     imprimeln(F("Ponto de acesso criado"));
//     imprimeln(WiFi.softAPIP());
// }


// // Checar conexão WiFi periodicamente
// void WiFiManager::checkConnection() {     
//     if (millis() - lastCheck >= 10000) {
//         if (WiFi.status() != WL_CONNECTED) {
//             imprimeln(F("Conexão WiFi perdida!"));
//             //TODO Implementar reconexão
//         }

//         lastCheck = millis();
//     }
// }


// src/sensors/DTH11Sensor.h
// // DHT11 temperature and humidity sensor
// #ifndef DHT11SENSOR_H
// #define DHT11SENSOR_H

// #include <Arduino.h>
// #include <Adafruit_Sensor.h>  
// #include <DHT.h>
// #include <DHT_U.h>
// #include "ctrl_debug.h"

// class DHT11Sensor {
//     public:
//         DHT11Sensor(uint8_t pin, uint8_t type);
//         void begin();
//         float getTemperature();
//         float getHumidity();

//     private:
//         DHT dht;
// };

// #endif

// src/sensors/DTH11Sensor.cpp
// // DHT11 temperature and humidity sensor
// #include "sensors/DTH11Sensor.h"

// DHT11Sensor::DHT11Sensor(uint8_t pin, uint8_t type) : dht(pin, type) {}

// void DHT11Sensor::begin() {
//     dht.begin();
// }

// float DHT11Sensor::getTemperature() {
//     return dht.readTemperature();
// }

// float DHT11Sensor::getHumidity() {
//     return dht.readHumidity();
// }


// src/sensors/INA226Sensor.h
// // INA226 current and voltage sensor 
// #ifndef INA226_SENSOR_H
// #define INA226_SENSOR_H

// #include <Arduino.h>
// #include <Wire.h>
// #include <INA226_WE.h>
// #include "ctrl_debug.h"

// class INA226Sensor {

// public:
//     INA226Sensor(uint8_t address = 0x40);

//     bool begin();                 
//     float readCurrent();          
//     float readVoltage();          
//     void update();                

// private:
//     INA226_WE ina;
//     float current_mA;
//     float voltage_V;
// };

// #endif

// src/sensors/INA226Sensor.cpp
// // INA226 current and voltage sensor
// #include "INA226Sensor.h"

// INA226Sensor::INA226Sensor(uint8_t address): ina(address), current_mA(0), voltage_V(0) {}

// bool INA226Sensor::begin() {
//     Wire.begin();

//     if(!ina.init()) {
//         imprimeln(F("Erro ao inicializar INA226!"));
//         return false;
//     }

//     // Ajuste opcional: média, tempo de conversão etc.
//     // ina.setAverage(INA226_AVG_16);
//     // ina.setConversionTime(INA226_CONV_TIME_1100);

//     return true;
// }

// void INA226Sensor::update() {
//     ina.readAndClearFlags();
//     current_mA = ina.getCurrent_mA();
//     voltage_V = ina.getBusVoltage_V();
// }

// float INA226Sensor::readCurrent() {
//     return current_mA;
// }

// float INA226Sensor::readVoltage() {
//     return voltage_V;
// }



// src/web_server/web_server.h
// #ifndef WEB_SERVER_H
// #define WEB_SERVER_H

// #include <Arduino.h>
// #include <LittleFS.h>
// #include <ESPAsyncWebServer.h>
// #include "ctrl_debug.h"
// #include "json/json_builder.h"

// extern float g_voltage;
// extern float g_current;
// extern float g_temperature;
// extern float g_humidity;

// class WebServer {
// public:
//     WebServer(AsyncWebServer* server);
//     void begin();
// private:
//     AsyncWebServer* server;
// };

// #endif


// src/web_server/web_server.cpp
// #include "web_server.h"


// // Constructor
// WebServer::WebServer(AsyncWebServer* s) {
//     server = s;
// }



// void WebServer::begin() {

//     // ---- INICIAR SPIFFS ----
//     if (!LittleFS.begin()) {
//         imprimeln(F("Erro a montar SPIFFS"));
//         return;
//     }
//     else{
//         imprimeln(F("SPIFFS montado com sucesso"));
//     }

//     // ---- SERVE ARQUIVOS DO SPIFFS ----
//     // Route for root / web page
//     server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
//               { request->send(LittleFS, "/painel.html", "text/html"); });

//     // Route to load ota.html file          
//     server->on("/ota", HTTP_GET, [](AsyncWebServerRequest *request)
//               { request->send(LittleFS, "/ota.html", "text/html"); });

//     // Route to load style.css file
//     server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
//               { request->send(LittleFS, "/style.css", "text/css"); });

//     // Route to load script.js file
//     server->on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
//               { request->send(LittleFS, "/script.js", "text/javascript"); });

//     // Route serve sensor INA226 data as JSON
//     server->on("/api/sensores", HTTP_GET, [](AsyncWebServerRequest *request){            

//         JsonBuilder json;
//         json.add("voltage", g_voltage, 3);
//         json.add("current", g_current, 3);
//         json.add("temperature", g_temperature, 2);
//         json.add("humidity", g_humidity, 2);

//         request->send(200, "application/json", json.build());     
//     });          

//     // Iniciar o servidor web          
//     server->begin();

//     imprimeln(F("Servidor HTTP iniciado"));
// }


// src/main.cpp
// #include <Arduino.h>
// #include <headers.h>


// /*
//           PIN_RELAY   PIN_BUTTON    PIN_LED   MCU
// --------------------------------------------------------  
// SLAMPHER  GPIO12      GPIO0         GPIO13    ESP8285 1M
// BASIC     GPIO12      GPIO0         GPIO13    ESP8266 1M
// ESP12E    GPIO12      GPIO0         GPIO2     ESP8266 4M
// T4EU1C    GPIO12      GPIO0         GPIO13    ESP8285 1M 
// T0EU1C    GPIO12      GPIO0         GPIO13    ESP8285 1M 
// SHELLY1L  GPIO5       GPIO4         GPIO0     ESP8285 1M 
// M51C      GPIO23      GPIO0         GPIO19    ESP32   4M

// */


// Device device;
// WiFiManager wifiManager;

// // Servidor Web
// AsyncWebServer servidorHTTP(80);
// WebServer webServer(&servidorHTTP);

// // Sensor INA226
// #define INA226_I2C_ADDRESS 0x40 
// INA226Sensor sensorINA226(INA226_I2C_ADDRESS);


// // Define seu LCD (endereço e tamanho)
// LCDDisplay displayLCD(0x27, 16, 2);


// // DHT11 temperature and humidity sensor
// #ifdef ESP32
//   #define DHTPIN 4   
// #elif defined(ESP8266)
//   #define DHTPIN 12      
// #endif    
   
// #define DHTTYPE DHT11 
// DHT11Sensor sensorDHT11(DHTPIN, DHTTYPE);


// // Variáveis globais para cache dos sensores
// float g_voltage = 0.0;
// float g_current = 0.0;
// float g_temperature = 0.0;
// float g_humidity = 0.0;

// unsigned long lastUpdate = 0;
 

// // Credeciais de acesso a rede WiFi
// const char* ssid = "TPLINK";
// const char* password = "gregorio@2012";




// //////////////
// // setup() //
// /////////////
// void setup() {
//     // Iniciar comunicação serial
//     Serial.begin(115200);
//     delay(1000);
//     imprimeln();
//     imprimeln(F("Iniciando..."));

//     // Inicializar dispositivo
//     device.initialize();
    
//     // Conectar WiFi
//     wifiManager.connect(ssid, password); 
    
//     // Criar ponto de acesso WiFi
//     //wifiManager.criar_ap("ESP8266", "123456789");

//     // Iniciar servidor web
//     webServer.begin();

//     // Inicializar OTA elegante
//     ElengantOTA::begin(&servidorHTTP); 
    
//     // Inicializar sensor INA226
//     sensorINA226.begin(); 
    
//     // Inicializa display LCD
//     displayLCD.begin();
    
// }


// ////////////
// // loop() //
// ////////////
// void loop() {
//     // NOTA: AsyncElegantOTA não precisa de nada no loop

//     // Verificar conexão WiFi
//     #ifdef ESP32
//         if (WiFi.getMode() == WIFI_MODE_STA || WiFi.getMode() == WIFI_MODE_APSTA){
//             wifiManager.checkConnection();
//         }
//     #elif defined(ESP8266)
//         if (WiFi.getMode() == WIFI_STA || WiFi.getMode() == WIFI_AP_STA){
//             wifiManager.checkConnection();
//         }        
//     #endif    
    
    
//     // Atualiza sensores a cada 2 segundos
//     if (millis() - lastUpdate > 3000) {
//       lastUpdate = millis();

//       sensorINA226.update();
//       g_voltage = sensorINA226.readVoltage();
//       g_current = sensorINA226.readCurrent();

//       g_temperature = sensorDHT11.getTemperature();
//       g_humidity = sensorDHT11.getHumidity();

//       // Vizualiza dados no visor LCD  
//       displayLCD.showReadings(g_current, g_voltage);


//       // Visualiza dadod no terminal
//       /*Serial.print("\nCorrente (mA): ");
//       Serial.print(corrente);
//       Serial.print(" -- Tensão (V): ");
//       Serial.print(tensao);


//       // Lê dados do sensor DHT11
//       float temperatura = sensorDHT11.getTemperature();
//       float humidade    = sensorDHT11.getHumidity();

//       if (isnan(temperatura) || isnan(humidade)) {
//         imprimeln(F("Falha ao ler o DHT11!"));    
//       } else {
//           imprime(F(" -- Temperatura (C): "));
//           imprime(temperatura);
//           imprime(F(" -- Humidade (%): "));
//           imprime(humidade);
//       }*/

//     }  


//     // Impede que o watchdog timer (WDT) reinicie o microcontrolador.   
//     #ifdef ESP32
//         vTaskDelay(1);
//     #elif defined(ESP8266)
//         yield();     
//     #endif     
// }

