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

#define PIN_LED 2

void setup_wifi() {
  delay(10);
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Função de callback para mensagens recebidas
  Serial.print("Mensagem recebida em: ");
  Serial.println(topic);
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Conectando ao MQTT...");
    if (client.connect("ESP8266", mqtt_user, mqtt_password)) {
      Serial.println("Conectado!");
      
      // Publica o dispositivo para o autodiscovery
      client.publish("homeassistant/switch/esp8266_switch/config",
                     "{\"name\": \"teste_switch\", \"command_topic\": \"homeassistant/switch/esp8266_switch/set\", \"state_topic\": \"homeassistant/switch/esp8266_switch/state\"}",
                     true);
    } else {
      Serial.print("Falha ao conectar. Erro: ");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Simulação de estado
  digitalWrite(PIN_LED, !digitalRead(PIN_LED));
  uint8_t state = !digitalRead(PIN_LED); 
  client.publish("homeassistant/switch/esp8266_switch/state", state ? "ON" : "OFF");
  delay(5000);
}
