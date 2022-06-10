

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <FS.h>
#include "DHT.h"
#include "LittleFS.h"

#include "creds.h"

#define SPIFFS LittleFS

#define OTA_Host_Name "ESP32_UPS"

const uint8_t PIN_LED_R = 4;         // GPIO4 LED VERMELHO
const uint8_t PIN_LED_G = 5;         // GPIO5 LED VERDE
const uint8_t PIN_GND_AUX = 23;      // GPIO23 Usado com GND
const uint8_t PIN_DHT = 14;          // GPIO14 DHT11
const uint8_t PIN_SENSOR = 36;       // GPIO36-ADC0 battery voltage
float R1 = 30000.0;
float R2 = 7150.0;                   // 7500.0;



WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


// Para processamento da Temperatura a Humidade
#define DHTTYPE DHT11   // DHT 11
DHT dht(PIN_DHT, DHTTYPE);


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


// Timer variables
uint32_t contador = 0;
uint32_t contador_leds = 0;
uint32_t tempo_pisca_led = 1000;


float temperature;
float humidity;
float pressure;
float tensao;


String processor(const String& var) {

  //Serial.println("process variable " + var);

  if (var == "VOLT") {
    return String(tensao, 2);
  }
  else  if (var == "TEMP") {
    return String(temperature, 1);
  }
  else  if (var == "HUMI") {
    return String(humidity, 0);
  }
  else  if (var == "PRES") {
    return String(pressure, 0);
  }

  return String();
}




///////////////////////////////////// Função setup //////////////////////////////////
void setup() {

  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_GND_AUX, OUTPUT);
  digitalWrite(PIN_LED_R, LOW);
  digitalWrite(PIN_LED_G, LOW);
  digitalWrite(PIN_GND_AUX, LOW);

  dht.begin();

  Serial.begin(115200);
  delay(700);


  // Conexão à rede WiFi local
  conectarWiFiLocal(ssid, password);


  // Conectar ao servidor MQTT
  conectarServidorMQTT(mqtt_ip, mqtt_port);


  // Configuração e inicialição do OTA
  ArduinoOTA.setHostname(OTA_Host_Name);
  ArduinoOTA.begin();


  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }


  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LittleFS, "/style.css", "text/css");
  });

  // Route to load script.js file
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LittleFS, "/script.js", "text/javascript");
  });


  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
    String dta = F("data:image/x-icon;base64,AAAB AAAAA");
    request->send_P(200, "image/x-icon", dta.c_str() );
  });


  server.on("/TELEMETRIA", HTTP_GET, [](AsyncWebServerRequest * request) {    
    getSensorReadings();
    request->send_P(200, "text/plain", criarTextoJSON().c_str());
  });


  server.on("/IP", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getIP().c_str());
  });



  server.begin();

}



///////////////////////////////////// Função loop //////////////////////////////////
void loop() {

  // Tratar OTA
  ArduinoOTA.handle();

  // Publicar mensagens MQTT cada X segundos
  if ( (millis() - contador) >= 5000) {

    mqttClient.publish("dados/ESP12E_UPS/SENSORES", criarTextoJSON().c_str());
    // Serial.println( criarTextoJSON()); // Debug
    contador = millis();

  }


  // Controlar LED's cada X segundos
  if ( (millis() - contador_leds) >= tempo_pisca_led) {

    controlarLEDS();
    contador_leds = millis();

  }


  // Conectar à Rede WiFi Local, se necessário
  if (WiFi.status() != WL_CONNECTED) {

    //conectarWiFiLocal(ssid, password);

  }


  // Reconectar ao Servidor MQTT, se necessário
  if (!mqttClient.connected()) {
    conectarServidorMQTT(mqtt_ip, mqtt_port);
  }


}



///////////////////////////////////// Função medirTensao //////////////////////////////////
float medirTensao(uint8_t pin_sensor) {

  float Tvoltage = 0.0;
  float Vvalue = 0.0, Rvalue = 0.0;


  for (uint32_t i = 0; i < 500; i++) {
    Vvalue = Vvalue + analogRead(pin_sensor);     //Read analog Voltage
    delay(5);                                     //ADC stable
  }

  Vvalue = (float)Vvalue / 500.0;                 //Find average of 500 values

  Tvoltage = (Vvalue * (3.3 / 4096.0)) * ((R1 + R2) / R2);

  return Tvoltage;
}



////////////////////////////////// Obter medições do Sensor ///////////////////////////////
void getSensorReadings() {

  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  tensao = medirTensao(PIN_SENSOR);

  float F = dht.readTemperature(true);
  float HiF = dht.computeHeatIndex(F, humidity); //  read and compute heat index in Fahrenheit (the default)
  float DewPoint = (temperature - (100 - humidity) / 5);   //  dewpoint calculation using Celsius value
  float DP = (DewPoint * 9 / 5) + 32;     //  converts dewPoint calculation to fahrenheit

  pressure = DP;

}




/////////////////////////////////// Criação de Texto JSON ////////////////////////////////
String criarTextoJSON() {

  getSensorReadings();

  String sz_json;

  // Create object, texto com formato json, serialização
  DynamicJsonDocument doc(226);

  doc["Tensao"] = String(tensao, 2);            // Duas casas decimais
  doc["Temperatura"] = String(temperature, 1);  // Uma casa decimal
  doc["Humidade"] = String(humidity);
  doc["Pressao"] = String(pressure);

  serializeJson(doc, sz_json);

  return sz_json;
}




////////////////////////////////// Conectar à Rede WiFi local ///////////////////////////////
void conectarWiFiLocal(const char* ssid, const char* pwd) {

  uint16_t contador = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) {

    delay(1000);

    if (contador++ >  30) {
      /*WiFi.mode(WIFI_AP);
      //WiFi.softAPConfig(staticIP, gateway, subnet);
      //WiFi.softAP(ap_ssid, ap_password, CHANNEL, HIDE_SSID, MAX_CONNECTIONS);
      WiFi.softAP(ap_ssid, ap_password);*/
      break;
    }

  }

}



////////////////////////////////// Conectar ao Servidor MQTT ///////////////////////////////
void conectarServidorMQTT(const char* ip_srvr, uint16_t port_srvr) {

  if (WiFi.status() == WL_CONNECTED) {

    mqttClient.setServer (ip_srvr, port_srvr);
    mqttClient.connect ("ESP32_UPS", NULL, NULL);

  }

}



////////////////////////////////////// Controlar LED's ///////////////////////////////////
void controlarLEDS() {

  float volt = medirTensao(PIN_SENSOR);

  if (volt >= 13) {
    digitalWrite(PIN_LED_R, LOW);
    digitalWrite(PIN_LED_G, HIGH);
  }
  else if (volt >= 12 && volt < 13) {
    digitalWrite(PIN_LED_R, LOW);
    digitalWrite(PIN_LED_G, !digitalRead(PIN_LED_G));
    tempo_pisca_led = 1000;
  }
  else if (volt < 13 && volt >= 11) {
    digitalWrite(PIN_LED_R, !digitalRead(PIN_LED_R));
    digitalWrite(PIN_LED_G, LOW);
    tempo_pisca_led = 500;
  }
  else if (volt < 11) {
    digitalWrite(PIN_LED_R, !digitalRead(PIN_LED_R));
    digitalWrite(PIN_LED_G, !digitalRead(PIN_LED_G));
    tempo_pisca_led = 250;
  }

}



////////////////////////////////////// Tratar conexões/requisições ///////////////////////////////////
String getIP() {

  String sz_json = "";
  String ip = "";

  if (WiFi.status() == WL_CONNECTED) {
    ip = WiFi.localIP().toString().c_str();
  }
  else {
    ip = WiFi.softAPIP().toString().c_str();
  }

  // Create object, texto com formato json, serialização
  DynamicJsonDocument doc(64);

  doc["Ip"] = ip;

  serializeJson(doc, sz_json);

  return sz_json;

}





////////////////////////////////////// Tratar conexões/requisições ///////////////////////////////////
/*String getContentType(String filename)
  {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
  }

  bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/"))
  {
    path += "index.html";
  }

  String contentType = getContentType(path);
  if (SPIFFS.exists(path)){
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }

  Serial.println("\tFile Not Found");
  return false;
  }*/
