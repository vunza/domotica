#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include "json/json_builder.h"
#include "ctrl_debug.h"
#include "constantes.h"


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
     * Buffer estático que armazena o identificador único do dispositivo para MQTT Auto Discovery.
     * 
     * Este identificador é utilizado para agrupar múltiplas entidades (sensores, interruptores, 
     * etc.) sob um único dispositivo na interface do Home Assistant, permitindo organização
     * hierárquica e gerenciamento conjunto.
     * 
     * Formato: "esp32_[TIPO]_[ID_UNICO]"
     * Exemplo: "esp32_sensor_001", "esp32_relay_kitchen"
     */
    char main_device_node_id[13];

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
     * @brief Parseia uma mensagem de comando OTA otimizada.
     *
     * Esta função processa uma mensagem de comando OTA no formato `<COMANDO|MAC_ADDRESS>`.
     * Ela é otimizada para evitar alocações dinâmicas de memória, trabalhando diretamente com strings de estilo C.
     * A função valida o formato da mensagem, extrai o comando e o endereço MAC, e valida o formato do MAC.
     *
     * @param[in] message Ponteiro para a string da mensagem a ser parseada. Deve estar no formato `<COMANDO|MAC_ADDRESS>`.
     *                    Exemplo: "<OTA_MODE|C8:C9:A3:38:D2:E7>"
     * @param[out] command Ponteiro para o buffer de caracteres onde o comando extraído será armazenado.
     *                     O buffer deve ter tamanho suficiente (pelo menos 32 caracteres) para armazenar o comando.
     *                     O comando será copiado sem os delimitadores '<' e '|', e com terminador nulo.
     * @param[out] mac Ponteiro para o buffer de caracteres onde o endereço MAC extraído será armazenado.
     *                 O buffer deve ter tamanho suficiente (pelo menos 18 caracteres) para armazenar o MAC no formato "XX:XX:XX:XX:XX:XX".
     *                 O MAC será copiado sem os delimitadores '|' e '>', e com terminador nulo.
     *
     * @return true se a mensagem foi parseada com sucesso e o comando e MAC foram extraídos e validados.
     * @return false se a mensagem não estiver no formato correto, se o comando for muito longo, se o MAC não tiver o formato correto, ou se ocorrer qualquer erro de parsing.
     *
     * @note A função espera que a mensagem comece com '<' e termine com '>', e que tenha um separador '|' entre o comando e o MAC.
     *       O endereço MAC deve estar no formato "XX:XX:XX:XX:XX:XX", onde X é um dígito hexadecimal (0-9, A-F, a-f).
     *       A função não verifica se o comando é um comando OTA válido, apenas extrai a string.
     *       A função não aloca memória dinamicamente, portanto os buffers de saída devem ser fornecidos pelo chamador.
     *
     * @example
     *   char command[32];
     *   char mac[18];
     *   const char* message = "<OTA_MODE|C8:C9:A3:38:D2:E7>";
     *   if (parseOtaMessageOptimized(message, command, mac)) {
     *       // Usar command e mac
     *   } else {
     *       // Tratar erro
     *   }
     */
    bool parseOtaMessageOptimized(const char* message, char* command, char* mac);




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
   *  @param role        Tipo de dispositivo [device|entity].
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
   *     "[device|entity]"
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
      const char* extra_config = nullptr
  );


  /**
   * @brief Atualiza o estado de um dispositivo identificado por seu endereço MAC.
   * 
   * Esta função gerencia um array de estruturas que armazenam o estado de dispositivos.
   * Se o dispositivo (identificado por um endereço MAC sem dois pontos) já estiver registrado,
   * seu estado é atualizado. Caso contrário, um novo registro é criado, desde que haja espaço.
   * Cada registro inclui um timestamp que marca o momento da última atualização.
   * 
   * @param mac_without_colon Endereço MAC do dispositivo, sem dois pontos (ex: "A1B2C3D4E5F6").
   *                          Deve ser uma string terminada em null.
   * @param estado String contendo o estado atual do dispositivo (ex: "ON", "OFF").
   *               Deve ser uma string terminada em null.
   * 
   * @note A função utiliza um array estático de estruturas `estados`, com tamanho máximo definido por `MAX_TOPICOS`.
   *       Se o array estiver cheio e um novo dispositivo tentar ser adicionado, uma mensagem de erro será impressa
   *       e a função retornará sem fazer alterações.
   * 
   * @note O timestamp é obtido através da função `millis()` e representa o momento da atualização em milissegundos
   *       desde que o sistema começou a rodar.
   * 
   * @warning Esta função não é thread-safe. Se usada em um contexto de múltiplas threads ou tarefas,
   *          deve ser protegida por mecanismos de sincronização.
   * 
   * @warning As strings `mac_without_colon` e `estado` devem ter comprimento adequado para caber nos campos
   *          da estrutura `EstadoTopico`. Não há verificação de comprimento nesta função, portanto, o chamador
   *          deve garantir que não excedam os limites definidos na estrutura.
   * 
   * @see MAX_TOPICOS
   * @see EstadoTopico
   * @see millis()
   * @see imprimeln()
   * 
   * @example
   * // Exemplo de uso:
   * MQTTClient client;
   * client.atualizarEstado("A1B2C3D4E5F6", "ON");
   * client.atualizarEstado("A1B2C3D4E5F6", "OFF"); // Atualiza o mesmo dispositivo
   * client.atualizarEstado("B2C3D4E5F6A7", "ON");  // Adiciona um novo dispositivo
   * 
   * @return void
   */
  void atualizarEstado(const char* mac_without_colon, const char* estado);


  /**
   * @brief Busca e retorna o estado atual de um dispositivo na cache local
   * 
   * Esta função realiza uma busca linear no array de estados armazenados para encontrar
   * o estado correspondente a um endereço MAC específico. É utilizada para acessar
   * rapidamente o último estado conhecido de um dispositivo sem necessidade de consulta
   * ao broker MQTT ou aguardar novas mensagens.
   * 
   * @param mac_without_colon Endereço MAC do dispositivo no formato hexadecimal sem separadores
   *                         (12 caracteres para MAC-48, ex: "A1B2C3D4E5F6")
   *                         Deve corresponder exatamente ao formato armazenado.
   * 
   * @return const char* Ponteiro para string contendo o estado do dispositivo:
   *         - String não vazia contendo o estado (ex: "ON", "OFF", "25.5", "CONNECTED")
   *         - String vazia ("") se o dispositivo não for encontrado ou se o registro for inválido
   * 
   * @note O ponteiro retornado aponta para o buffer interno da estrutura EstadoTopico.
   *       Não modifique ou libere esta memória.
   * @note A função retorna string vazia em caso de falha, não NULL.
   * @note A busca é case-sensitive (MAC deve estar em maiúsculas conforme armazenado).
   * @note Complexidade temporal: O(n) onde n = totalEstados
   * 
   * @section algoritmo Algoritmo
   * 1. Percorre o array `estados` de 0 até `totalEstados - 1`
   * 2. Para cada elemento:
   *    a. Compara o MAC usando `strcmp()`
   *    b. Verifica se o registro está marcado como `valido == true`
   *    c. Se ambas condições forem verdadeiras, retorna `estados[i].state`
   * 3. Se nenhum registro atender aos critérios, retorna string vazia ("")
   * 
   * @warning A função não valida a idade do estado (timestamp). Estados muito antigos
   *          ainda serão retornados se `valido == true`. Use `estaEstadoRecente()` para
   *          verificar a validade temporal.
   * @warning Thread-unsafe: Acesso não sincronizado ao array compartilhado.
   * 
   * @code{.cpp}
   * // Exemplo de uso básico:
   * const char* estado = mqttClient.buscarEstado("A1B2C3D4E5F6");
   * if (estado[0] != '\0') {  // Verifica se não é vazio
   *     Serial.print("Estado do dispositivo: ");
   *     Serial.println(estado);
   * } else {
   *     Serial.println("Dispositivo não encontrado ou estado inválido");
   * }
   * 
   * // Exemplo com conversão para tipos específicos:
   * const char* estadoStr = mqttClient.buscarEstado("B2C3D4E5F6A7");
   * if (estadoStr[0] != '\0') {
   *     if (strcmp(estadoStr, "ON") == 0) {
   *         ligarDispositivo();
   *     } else if (strcmp(estadoStr, "OFF") == 0) {
   *         desligarDispositivo();
   *     } else {
   *         float valor = atof(estadoStr);  // Para valores numéricos
   *         processarValor(valor);
   *     }
   * }
   * @endcode
   * 
   * @see MQTTClient::atualizarEstado()
   * @see MQTTClient::estaEstadoRecente()
   * @see MQTTClient::removerEstado()
   */
  const char* buscarEstado(const char* mac_without_colon);


  private:
    PubSubClient _mqtt;
    const char* _host;
    uint16_t _port;
    const char* _clientId;

    struct GuardaEstadoTopico {
      char mac_without_colon[13];
      char state[4];    
      bool valido;
      //unsigned long timestamp;
    };

    GuardaEstadoTopico estados[MAX_TOPICOS];
    uint8_t totalEstados = 0;
};

#endif
