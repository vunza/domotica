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
