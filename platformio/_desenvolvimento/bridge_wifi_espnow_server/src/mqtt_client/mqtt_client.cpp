#include "mqtt_client.h"

MQTTClient::MQTTClient(WiFiClient& client, const char* host, uint16_t port)
  : _mqtt(client), _host(host), _port(port), _clientId("esp-client") {
  _mqtt.setServer(_host, _port);
}

void MQTTClient::setClientId(const char* clientId) {
  _clientId = clientId;
}

void MQTTClient::setCallback(MQTT_CALLBACK_SIGNATURE) {
  _mqtt.setCallback(callback);
}

bool MQTTClient::connect() {
  if (_mqtt.connected()) {
    return true;
  }
  return _mqtt.connect(_clientId);
}

void MQTTClient::loop() {
  if (!_mqtt.connected()) {
    connect();
  }
  _mqtt.loop();
}

bool MQTTClient::publish(const char* topic, const char* payload, bool retain) {
  return _mqtt.publish(topic, payload, retain);
}

void MQTTClient::setBufferSize(uint16_t size) {
  _mqtt.setBufferSize(size);
}

void MQTTClient::subscribe(const char* topic) {
  _mqtt.subscribe(topic);
}

bool MQTTClient::connected() {
  return _mqtt.connected();
}


bool MQTTClient::parseOtaMessageOptimized(const char* message, char* command, char* mac) {
    // Verifica primeiro e último caractere
    if (message[0] != '<' || message[strlen(message)-1] != '>') {
        return false;
    }
    
    // Encontra o separador |
    const char* pipe = strchr(message, '|');
    if (!pipe) return false;
    
    // Extrai comando (entre < e |)
    int cmdLen = pipe - (message + 1);
    if (cmdLen <= 0 || cmdLen >= 32) return false;
    strncpy(command, message + 1, cmdLen);
    command[cmdLen] = '\0';
    
    // Extrai MAC (entre | e >)
    int macLen = strlen(message) - 2 - cmdLen - 1;  // -2 para <>, -1 para |
    if (macLen != 17) return false;  // Formato MAC fixo
    strncpy(mac, pipe + 1, macLen);
    mac[macLen] = '\0';
    
    // Valida formato do MAC
    for (int i = 0; i < 17; i++) {
        if (i % 3 == 2) {
            if (mac[i] != ':') return false;
        } else {
            if (!isxdigit(mac[i])) return false;
        }
    }
    
    return true;
}


void MQTTClient::publishDiscoveryEntity(
    MQTTClient& mqttClient,
    const char* role,
    const char* component,
    const char* node_id,
    const char* entity_name,
    const char* base_topic,
    const char* unique_id,
    const char* mac_colon,
    const char* device_name,
    const char* manufacturer,
    const char* model,
    const char* extra_config
) {
    // homeassistant/<component>/<node_id>/config
    String discoveryTopic = "homeassistant/";
    discoveryTopic += component;
    discoveryTopic += "/";
    discoveryTopic += node_id;  
    discoveryTopic += "/config";
   

    JsonBuilder json;

    //json.add("name", entity_name);
    json.add("name", model);
    //json.add("name",  String(entity_id_prefix) + String(node_id));
    json.add("unique_id", unique_id);

    // State topic (sempre existe)
    char state_topic[64];
    snprintf(state_topic, sizeof(state_topic), "%s%s/state", base_topic, node_id);
    json.add("state_topic", state_topic);

    // Command topic (não usado em sensor/binary_sensor)
    char command_topic[64];
    if (strcmp(component, "sensor") != 0 && strcmp(component, "binary_sensor") != 0) {          
        snprintf(command_topic, sizeof(command_topic), "%s%s/set", base_topic, node_id);
        json.add("command_topic", command_topic);
    }   

    // availability topic
    char availability_topic[64];  
    snprintf(availability_topic, sizeof(availability_topic), "%s%s/availability", base_topic, node_id);
    json.add("availability_topic", availability_topic);

    // attributes topic
    char attributes_topic[64]; 
    snprintf(attributes_topic, sizeof(attributes_topic), "%s%s/attributes", base_topic, node_id);
    json.add("json_attributes_topic", attributes_topic);

    // Defaults para switch / light
    if (strcmp(component, "switch") == 0 || strcmp(component, "light") == 0 ) {
        json.add("payload_on", "ON");
        json.add("payload_off", "OFF");
        json.add("state_on", "ON");
        json.add("state_off", "OFF");
        json.add("retain", "true", false);
        json.add("qos", 1);
    }


    // Device block (partilhado por todas as entidades)
    JsonBuilder device;
    if(strcmp(role, "device") == 0){
      String device_id = "esp_" + String(component) + "_" + String(node_id);
      device.add("identifiers", "[\"" + device_id + "\"]", false);    
      device.add("connections", "[[\"mac\",\"" + String(mac_colon) + "\"]]", false);
      device.add("name", String(entity_id_prefix) + String(device_name));
      device.add("manufacturer", manufacturer);
      device.add("model", model);
    }
    else if(strcmp(role, "entity") == 0){
      String device_id = "esp_" + String(component) + "_" + String(main_device_node_id);
      device.add("identifiers", "[\"" + device_id + "\"]", false);                  
    }    

    json.add("device", device.build(), false);

    // Configuração extra específica do componente
    if (extra_config && strlen(extra_config)) {
      json.add("_extra", extra_config, false);
    }
    
    
    // Ajuste final para remover placeholder
    String payload = json.build();
    payload.replace("\"_extra\":", "");

    // Publica Disponibilidade
    mqttClient.publish(availability_topic, "online", true);

    // Publica atributo
    JsonBuilder attrjson;
    attrjson.add("mac_address", mac_colon);
    attrjson.add("basic_topic", MQTT_BASE_TOPIC);
    attrjson.add("identificador", mac_colon);
    String attributes_topic_payload = attrjson.build();    
    mqttClient.publish(attributes_topic, attributes_topic_payload.c_str(), true);

   
    // Subscrever ao tópico de comandos
    mqttClient.subscribe(command_topic);

    // Publicação MQTT (retain = true)
    boolean result = mqttClient.publish(discoveryTopic.c_str(), payload.c_str(), true);
    
    if(result){
      imprime(F("MQTT Discovery publicado: "));
      imprimeln(discoveryTopic);
      imprimeln(payload);     
    } 
    else{     
      imprime(F("Erro do MQTT Discovery!"));      
    } 
    
}
