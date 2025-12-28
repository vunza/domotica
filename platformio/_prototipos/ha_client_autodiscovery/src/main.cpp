#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ========== CONFIGURAÇÕES ==========
const char* WIFI_SSID = "TPLINK";
const char* WIFI_PASSWORD = "gregorio@2012";

const char* MQTT_BROKER = "192.168.0.5";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "wnr";
const char* MQTT_PASSWORD = "123";

// ========== VARIÁVEIS GLOBAIS ==========
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Variáveis para armazenar MAC address formatado
String deviceMac;           // Ex: "A0B1C2D3E4F5"
String macWithColons;       // Ex: "A0:B1:C2:D3:E4:F5"
String deviceBaseTopic;     // Ex: "wemos/A0B1C2D3E4F5/"
String discoveryTopic;      // Ex: "homeassistant/switch/A0B1C2D3E4F5/config"

bool ledState = false;
const int LED_PIN = 2; // GPIO2 - LED embutido (LOW = ligado)

// ========== FUNÇÕES AUXILIARES ==========
String getMacAddress(bool withColons = false) {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  
  if (withColons) {
    char macStr[18]; // 17 chars + null terminator
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", 
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(macStr);
  } else {
    char macStr[13]; // 12 hex chars + null terminator
    snprintf(macStr, sizeof(macStr), "%02X%02X%02X%02X%02X%02X", 
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(macStr);
  }
}

void setupWiFi() {  
  
  // Primeiro conecta ao WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Depois de conectado, obtém o MAC address
  deviceMac = getMacAddress(false);        // Sem dois pontos
  macWithColons = getMacAddress(true);     // Com dois pontos
  deviceBaseTopic = "wemos/" + deviceMac + "/";
  discoveryTopic = "homeassistant/switch/" + deviceMac + "/config";
  
  Serial.println("\nWiFi OK");
  Serial.print("MAC (sem colons): ");
  Serial.println(deviceMac);
  Serial.print("MAC (com colons): ");
  Serial.println(macWithColons);
}

void publishDiscovery() {
  // Construir mensagem JSON dinamicamente com MAC
  String discoveryMsg = "{";
  discoveryMsg += "\"name\":\"LED Wemos\","; // Nome visível da entidade no Home Assistant
  discoveryMsg += "\"unique_id\":\"" + deviceMac + "_switch\","; // Identificador único e permanente da entidade
  discoveryMsg += "\"state_topic\":\"" + deviceBaseTopic + "state\","; // Tópico onde o estado será publicado
  discoveryMsg += "\"command_topic\":\"" + deviceBaseTopic + "set\","; // Tópico onde comandos serão recebidos
  discoveryMsg += "\"availability_topic\":\"" + deviceBaseTopic + "availability\",";  // Indica se o dispositivo está online/offline
  discoveryMsg += "\"payload_on\":\"ON\","; // Payload para ligar a entidade
  discoveryMsg += "\"payload_off\":\"OFF\","; // Payload para desligar a entidade
  discoveryMsg += "\"state_on\":\"ON\","; // Representação do estado ligado
  discoveryMsg += "\"state_off\":\"OFF\","; // Representação do estado desligado
  discoveryMsg += "\"retain\":true,"; // Mensagem retida para persistência do estado
  discoveryMsg += "\"qos\":1,"; // Qualidade de serviço MQTT
  discoveryMsg += "\"device\":{";
  discoveryMsg += "\"identifiers\":[\"ESP_" + deviceMac + "\"],"; // ID interno do dispositivo (Deve ser único e fixo)
  discoveryMsg += "\"connections\":[[\"mac\",\"" + macWithColons + "\"]],"; // Informação adicional de identificação
  discoveryMsg += "\"name\":\"0x1000" + deviceMac + "\",";  // Nome do dispositivo (não da entidade)
  discoveryMsg += "\"manufacturer\":\"Espressif\",";  // Fabricante (informativo)
  discoveryMsg += "\"model\":\"Wemos D1 Mini\"";      // Modelo do hardware (informativo)
  discoveryMsg += "}";
  discoveryMsg += "}";
  
  Serial.print("Publicando Discovery no tópico: ");
  Serial.println(discoveryTopic);
  Serial.print("Mensagem: ");
  Serial.println(discoveryMsg);
  
  mqttClient.publish(discoveryTopic.c_str(), discoveryMsg.c_str(), true);
  Serial.println("Config MQTT publicado");
}

void publishState() {
  const char* state = ledState ? "ON" : "OFF";
  String stateTopic = deviceBaseTopic + "state";
  
  mqttClient.publish(stateTopic.c_str(), state, true);
  digitalWrite(LED_PIN, ledState ? LOW : HIGH);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Converter payload para string C
  char message[20];
  unsigned int i;
  for (i = 0; i < length && i < 19; i++) {
    message[i] = (char)payload[i];
  }
  message[i] = '\0';
  
  // Criar o tópico de comando esperado
  String commandTopic = deviceBaseTopic + "set";
  
  // Verificar se é comando
  if (strcmp(topic, commandTopic.c_str()) == 0) {
    if (strcmp(message, "ON") == 0) {
      ledState = true;
    } else if (strcmp(message, "OFF") == 0) {
      ledState = false;
    }
    publishState();
  }
}

void connectMQTT() {
  // Usar MAC como parte do clientId
  String clientId = "wemos_" + deviceMac;
  
  // Tópico de disponibilidade
  String availabilityTopic = deviceBaseTopic + "availability";
  
  Serial.print("Conectando MQTT como: ");
  Serial.println(clientId);
  
  if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD, 
                        availabilityTopic.c_str(), 1, true, "offline")) {
    Serial.println("MQTT conectado");
    
    // Publicar disponibilidade
    mqttClient.publish(availabilityTopic.c_str(), "online", true);
    
    // Subscrever ao tópico de comandos
    String commandTopic = deviceBaseTopic + "set";
    mqttClient.subscribe(commandTopic.c_str(), 1);
    Serial.print("Inscrito em: ");
    Serial.println(commandTopic);
    
    // Publicar configuração
    publishDiscovery();
    
    // Publicar estado inicial
    publishState();
  } else {
    Serial.print("Falha MQTT, rc=");
    Serial.println(mqttClient.state());
  }
}

// ========== SETUP E LOOP ==========
void setup() {

  Serial.begin(115200); 

  while (!Serial) {
        ; // Aguarda porta serial iniciar (normal no ESP32 USB)
  }
  
 
  // Desactiva Debug
  #if defined(ESP32)
      esp_log_level_set("*", ESP_LOG_NONE);
  #elif defined(ESP8266)
      Serial.setDebugOutput(false);
  #endif

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // LED desligado inicialmente
  
  setupWiFi();
  
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  mqttClient.setBufferSize(512);
  
  connectMQTT();
}

void loop() {
  if (!mqttClient.connected()) {
    delay(5000);
    connectMQTT();
  }
  mqttClient.loop();
  delay(10);
}
