#include <headers.h>


/*
          PIN_RELAY   PIN_BUTTON    PIN_LED   MCU
--------------------------------------------------------  
SLAMPHER  GPIO12      GPIO0         GPIO13    ESP8285 1M
BASIC     GPIO12      GPIO0         GPIO13    ESP8266 1M
ESP12E    GPIO12      GPIO0         GPIO2     ESP8266 4M
T4EU1C    GPIO12      GPIO0         GPIO13    ESP8285 1M 
T0EU1C    GPIO12      GPIO0         GPIO13    ESP8285 1M 
SHELLY1L  GPIO5       GPIO4         GPIO0     ESP8285 1M 

*/

// Configuração do pin a controlar
// 12 -- GPIO12 do SLAMPHER -- //GPIO15 do ESP12E
// 2 --- wemos_d1_R2
// 1 --- ESP8266

//  2449233 -- Brdge Tasmota (Sonoff Pow Elite)
//  8591111 -- Brdge Tasmota (Sonoff Zigbee Bridge Pro)


// Configurações Wi-Fi
const char* ssid = "TPLINK";
const char* password = "gregorio@2012";

// Configurações MQTT
const char* mqtt_server = "192.168.0.5";
const int mqtt_port = 1883;
const char* mqtt_user = "wnr"; // Opcional
const char* mqtt_password = "123"; // Opcional
const char* mqtt_topic = "esp8266/sensor"; // Tópico MQTT

WiFiClient espClient;
PubSubClient client(espClient);
uint16_t cont = 0;

// Função de Conexão Wi-Fi
void setupWiFi() {
  delay(10);
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Conectado!");
}

// Função de Conexão MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setupWiFi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publica dados no MQTT  
  char msg[50];
  snprintf(msg, 50, "%d", cont++);
  client.publish(mqtt_topic, msg);
  if(cont >= 1000) cont = 0;

  delay(2000);
}





