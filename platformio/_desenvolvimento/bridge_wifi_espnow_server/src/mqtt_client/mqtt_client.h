#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include "ctrl_debug.h"

/**
 * @class MQTTClient
 * @brief Cliente MQTT simples para ESP8266 e ESP32
 *
 * Encapsula conexão, reconexão automática e publicação.
 * Compatível com PlatformIO (Arduino framework).
 */
class MQTTClient {
  public:
    /**
     * @brief Construtor
     * @param client Cliente WiFi (WiFiClient)
     * @param host Endereço do broker MQTT
     * @param port Porta do broker MQTT
     */
    MQTTClient(WiFiClient& client, const char* host, uint16_t port = 1883);

    /**
     * @brief Define ID do cliente MQTT
     * @param clientId Identificador único
     */
    void setClientId(const char* clientId);

    /**
     * @brief Define função callback para mensagens recebidas
     * @param callback Função MQTT callback
     */
    void setCallback(MQTT_CALLBACK_SIGNATURE);

    /**
     * @brief Conecta ao broker MQTT
     * @return true se conectado
     */
    bool connect();

    /**
     * @brief Loop MQTT (deve ser chamado no loop principal)
     */
    void loop();

    /**
     * @brief Publica mensagem MQTT
     * @param topic Tópico
     * @param payload Mensagem
     * @param retain Retain flag
     * @return true se publicado
     */
    bool publish(const char* topic, const char* payload, bool retain = false);

    /**
     * @brief Subscreve a um tópico
     * @param topic Tópico MQTT
     */
    void subscribe(const char* topic);

    /**
     * @brief Define tamanho do buffer MQTT
     * @param size Tamanho em bytes (ex: 1024)
     */
    void setBufferSize(uint16_t size);


    /**
     * @brief Verifica se está conectado
     */
    bool connected();

  private:
    PubSubClient _mqtt;
    const char* _host;
    uint16_t _port;
    const char* _clientId;
};

#endif
