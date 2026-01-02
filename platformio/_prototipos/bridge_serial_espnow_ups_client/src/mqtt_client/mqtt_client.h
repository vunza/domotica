#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include "json/json_builder.h"
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
     * @brief Prefixo fixo usado para identificação do dispositivo no Home Assistant.
     *
     * Este prefixo é utilizado para compor o nome lógico do dispositivo
     * (não da entidade) exposto ao Home Assistant, garantindo:
     *  - Padronização visual dos dispositivos
     *  - Evitar colisões de nomes entre dispositivos diferentes
     *  - Facilidade de identificação em ambientes com múltiplos nós
     *
     * Exemplo de uso:
     * @code
     * const char* entity_id_prefix = "0x1000";
     * char device_name[32];
     * snprintf(device_name, sizeof(device_name),
     *          "%s%s", entity_id_prefix, node_id);
     * @endcode
     *
     * Resultado esperado:
     *  "0x1000c8c9a338d2e7"
     *
     * @note
     * - O prefixo não afeta o `entity_id` interno do Home Assistant,
     *   apenas o nome exibido do dispositivo.
     * - Pode ser alterado livremente sem quebrar entidades existentes,
     *   desde que o `unique_id` permaneça inalterado.
     */
    const char* entity_id_prefix = "0x1000";


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

   /**
 * @brief Publica uma entidade via MQTT Discovery para o Home Assistant.
 *
 * Esta função constrói dinamicamente a mensagem JSON de auto-descoberta
 * (MQTT Discovery) e publica no tópico correto do Home Assistant, permitindo
 * que diferentes tipos de entidades (switch, sensor, light, binary_sensor, etc.)
 * sejam criadas automaticamente e associadas ao mesmo dispositivo físico.
 *
 * A função é genérica e reutilizável, suportando configuração extra por
 * componente através de um bloco JSON adicional.
 *
 * @param mqttClient   Referência para o cliente MQTT já conectado.
 *
 * @param component    Tipo de entidade do Home Assistant.
 *                     Exemplos:
 *                     - "switch"
 *                     - "sensor"
 *                     - "light"
 *                     - "binary_sensor"
 *
 * @param node_id      Identificador do nó/dispositivo.
 *                     Normalmente o MAC sem separadores (ex: C8C9A338D2C3).
 *                     Usado como namespace do dispositivo no HA.
 *
 * @param entity_name  Nome visível da entidade no Home Assistant.
 *                     Exemplo: "LED Wemos", "Tensão", "Corrente".
 *
 * @param base_topic   Tópico base MQTT do dispositivo.
 *                     Exemplo: "wemos/C8C9A338D2C3/"
 *
 * @param unique_id    Identificador único e permanente da entidade.
 *                     Deve ser estável entre reinicializações.
 *                     Exemplo: "C8C9A338D2C3_switch_led"
 *
 * @param mac_colon    Endereço MAC formatado com ":".
 *                     Exemplo: "C8:C9:A3:38:D2:C3"
 *
 * @param device_name  Nome do dispositivo (não da entidade) no Home Assistant.
 *                     Exemplo: "Wemos D1 Mini Sala"
 *
 * @param manufacturer Nome do fabricante do hardware.
 *                     Exemplo: "Espressif"
 *
 * @param model        Modelo do dispositivo.
 *                     Exemplo: "Wemos D1 Mini"
 *
 * @param extra_config (Opcional) String JSON com campos adicionais
 *                     específicos do componente.
 *                     Exemplos:
 *                     - "\"unit_of_measurement\":\"V\""
 *                     - "\"device_class\":\"voltage\""
 *                     - "\"state_class\":\"measurement\""
 *
 *                     ⚠️ Deve ser um JSON válido SEM chaves externas `{}`.
 *
 * @note
 * - A função publica a configuração como mensagem retida (`retain = true`),
 *   garantindo persistência após reinício do broker ou do Home Assistant.
 * - Todas as entidades criadas com o mesmo `node_id` aparecem agrupadas
 *   sob um único dispositivo no Home Assistant.
 *
 * @warning
 * - O `unique_id` deve ser absolutamente único no ambiente do Home Assistant.
 * - Os ponteiros `const char*` passados devem permanecer válidos durante
 *   a execução da função.
 *
 * @example
 * publishDiscoveryEntity(
 *     mqttClient,
 *     "sensor",
 *     "C8C9A338D2C3",
 *     "Tensão",
 *     "wemos/C8C9A338D2C3/",
 *     "C8C9A338D2C3_voltage",
 *     "C8:C9:A3:38:D2:C3",
 *     "Wemos D1 Mini Energia",
 *     "Espressif",
 *     "Wemos D1 Mini",
 *     "\"unit_of_measurement\":\"V\",\"device_class\":\"voltage\""
 * );
 */
void publishDiscoveryEntity(
    MQTTClient& mqttClient,
    const char* component,
    const char* node_id,
    const char* entity_name,
    const char* base_topic,
    const char* unique_id,
    const char* mac_colon,
    const char* device_name,
    const char* manufacturer,
    const char* model,
    const char* extra_config = nullptr
);



  private:
    PubSubClient _mqtt;
    const char* _host;
    uint16_t _port;
    const char* _clientId;
};

#endif
