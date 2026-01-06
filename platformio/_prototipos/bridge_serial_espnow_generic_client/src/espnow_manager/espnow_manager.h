#ifndef ESPNOW_MANAGER_H
#define ESPNOW_MANAGER_H

#include <Arduino.h>
#include "ctrl_debug.h"
#include "eeprom_manager/eeprom_manager.h"

#if defined(ESP32)
  #include <WiFi.h>
  #include <esp_now.h>
  #include <esp_wifi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <espnow.h>
#endif

/**
 * @file espnow_simple.h
 * @brief Abstração simples para comunicação ESP-NOW em ESP8266 e ESP32
 *
 * Este módulo fornece uma interface unificada para inicializar,
 * enviar e receber dados via ESP-NOW, suportando callbacks
 * personalizados para envio e receção.
 */

/**
 * @struct EspNowData
 * @brief Estrutura de dados transmitidos via ESP-NOW
 *
 * Estrutura genérica para troca de mensagens entre dispositivos,
 * incluindo dados de estado, medições e identificação.
 *
 * Campos do tipo string são usados para facilitar interoperabilidade,
 * debug e serialização.
 */
typedef struct {
    /** Tipo da mensagem: REGIST | DATA | COMMAND | RESPONSE | CHANNEL_REQ | CHANNEL_RSP */
    char msg_type[12];

    /** Endereço MAC do servidor (formato string XX:XX:XX:XX:XX:XX) */
    char mac_server[18];

    /** Endereço MAC do cliente (formato string XX:XX:XX:XX:XX:XX) */
    char mac_client[18];

    /** Estado lógico: ON | OFF | RTx | canal WiFi*/
    char state[4];

    /** Tensão medida na unidade 1 */
    char u1_voltage[15];

    /** Corrente medida na unidade 1 */
    char u1_current[15];

    /** Temperatura medida na unidade 1 */
    char u1_temperature[15];

    /** Humidade medida na unidade 1 */
    char u1_humidity[15];
} EspNowData;

/**
 * @class EspNowManager
 * @brief Classe de gerenciamento da comunicação ESP-NOW
 *
 * Esta classe encapsula toda a lógica de inicialização,
 * gestão de peers e envio/receção de mensagens ESP-NOW,
 * funcionando de forma transparente tanto em ESP8266
 * como em ESP32.
 */
class EspNowManager {
public:

    /**
     * @typedef RecvCallback
     * @brief Callback chamado quando dados são recebidos
     *
     * @param mac Endereço MAC do remetente
     * @param data Ponteiro para os dados recebidos
     * @param len Tamanho dos dados recebidos
     */
    typedef std::function<void(const uint8_t *mac, const uint8_t *data, int len)> RecvCallback;

    /**
     * @typedef SendCallback
     * @brief Callback chamado após tentativa de envio
     *
     * @param mac Endereço MAC do destinatário
     * @param success Resultado do envio (true = sucesso)
     */
    typedef std::function<void(const uint8_t *mac, bool success)> SendCallback;

    /**
     * @brief Construtor padrão
     */
    EspNowManager();

    /**
     * @brief Inicializa o ESP-NOW
     *
     * Configura o Wi-Fi no canal especificado e inicializa
     * a stack ESP-NOW.
     *
     * @param channel Canal Wi-Fi a ser utilizado
     * @param useAP Define se o dispositivo deve operar em modo AP
     * @return true se a inicialização for bem-sucedida
     */
    bool begin(uint8_t channel, bool useAP = false);

    /**
     * @brief Adiciona um peer ESP-NOW
     *
     * Regista um dispositivo remoto (peer) para permitir
     * comunicação direta via ESP-NOW.
     *
     * @param mac Endereço MAC do peer
     * @param channel Canal Wi-Fi do peer
     * @return true se o peer for adicionado com sucesso
     */
    bool addPeer(const uint8_t mac[6], uint8_t channel);

    /**
     * @brief Obtém o canal Wi-Fi atualmente em uso pelo dispositivo.
     *
     * Esta função retorna o canal Wi-Fi no qual o rádio está operando no momento.
     * É compatível tanto com ESP8266 quanto com ESP32, abstraindo as diferenças
     * entre as APIs de cada plataforma.
     *
     * ⚠️ Importante:
     * - O valor retornado só é válido se o Wi-Fi já estiver inicializado.
     * - Em ESP-NOW, o canal retornado corresponde ao canal atualmente configurado
     *   para comunicação ESP-NOW.
     *
     * @return uint8_t
     *         Número do canal Wi-Fi (1–13).
     *         Retorna 0 se o Wi-Fi não estiver inicializado ou o canal for inválido.
     *
     * @note
     * - ESP8266 utiliza a função de baixo nível `wifi_get_channel()`.
     * - ESP32 utiliza a API de alto nível `WiFi.channel()`.
     *
     * @example
     * uint8_t channel = getCurrentWiFiChannel();
     * if (channel > 0) {
     *     Serial.print("Canal atual: ");
     *     Serial.println(channel);
     * }
     */
    uint8_t getCurrentWiFiChannel();



    /**
     * @brief Descobre automaticamente o canal Wi-Fi do master ESP-NOW via broadcast.
     *
     * Esta função varre os canais Wi-Fi (1 a 13) e, em cada canal, envia uma
     * mensagem de broadcast ESP-NOW solicitando o canal do dispositivo master.
     * O master, ao receber a requisição, responde informando o canal em uso.
     *
     * O processo termina assim que uma resposta válida é recebida ou quando
     * todos os canais forem testados.
     *
     * ✔ Compatível com ESP8266 e ESP32  
     * ✔ Não requer SSID ou conexão Wi-Fi  
     * ✔ Ideal para redes ESP-NOW puras  
     *
     * @param timeoutMs
     *        Tempo máximo (em milissegundos) que o cliente aguarda por uma resposta
     *        do master em cada canal testado.
     *        Valor padrão: 300 ms.
     *
     * @return uint8_t
     *         - Canal Wi-Fi descoberto (1–13) em caso de sucesso.
     *         - Retorna 0 se nenhum master responder durante a varredura.
     *
     * @note
     * - Esta função assume que o callback de receção ESP-NOW já esteja configurado.
     * - O master deve responder com uma mensagem do tipo `"CHANNEL_RSP"`.
     * - O canal retornado deve ser fixado imediatamente após a descoberta.
     *
     * @warning
     * - Durante a varredura, o dispositivo muda temporariamente de canal,
     *   interrompendo comunicações ESP-NOW ativas.
     * - Deve ser executada apenas na fase de inicialização.
     *
     * @example
     * uint8_t channel = discoverEspNowChannel();
     * if (channel == 0) {
     *     Serial.println("❌ Master ESP-NOW não encontrado");
     * } else {
     *     Serial.print("✅ Canal ESP-NOW descoberto: ");
     *     Serial.println(channel);
     *
     * #if defined(ESP32)
     *     esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
     * #elif defined(ESP8266)
     *     wifi_set_channel(channel);
     * #endif
     * }
     */
    uint8_t discoverEspNowChannel(uint32_t timeoutMs = 3000);



    /**
     * @brief Envia dados via ESP-NOW
     *
     * @param mac Endereço MAC do destinatário
     * @param data Ponteiro para os dados a enviar
     * @param len Tamanho dos dados
     * @return true se o envio for iniciado com sucesso
     */
    bool send(const uint8_t mac[6], const uint8_t *data, int len);

    /**
     * @brief Regista callback de receção de dados
     *
     * @param cb Função callback a ser chamada na receção
     */
    void onReceive(RecvCallback cb);

    /**
     * @brief Regista callback de status de envio
     *
     * @param cb Função callback a ser chamada após envio
     */
    void onSend(SendCallback cb);

private:
    /** Callback interno de receção */
    RecvCallback _recvCB;

    /** Callback interno de envio */
    SendCallback _sendCB;

    /** Controla o processo de obtencao do canal WiFi */
    bool channelFound;

    /** Guarda o processo de obtencao do canal WiFi */
    uint8_t discoveredChannel;

    // Callback estática que será registrada no ESP-NOW (para autodiscovery)
#ifdef ESP8266
    static void staticOnRecv(uint8_t* mac, uint8_t* incomingData, uint8_t len);
#elif defined(ESP32)
    static void staticOnRecv(const uint8_t* mac, const uint8_t* incomingData, int len);
#endif

    // Callback de instância (não estática) que processa os dados
    void onRecvInstance(const uint8_t* mac, const uint8_t* data, int len);


    // Callback Normal
#if defined(ESP32)
    /**
     * @brief Callback estático de receção (ESP32)
     */
    static void _onRecv(const uint8_t *mac, const uint8_t *data, int len);

    /**
     * @brief Callback estático de envio (ESP32)
     */
    static void _onSent(const uint8_t *mac, esp_now_send_status_t status);
#elif defined(ESP8266)
    /**
     * @brief Callback estático de receção (ESP8266)
     */
    static void _onRecv(uint8_t *mac, uint8_t *data, uint8_t len);

    /**
     * @brief Callback estático de envio (ESP8266)
     */
    static void _onSent(uint8_t *mac, uint8_t status);
#endif

    /**
     * @brief Ponteiro estático para instância ativa
     *
     * Necessário para ligar callbacks C-style à instância da classe.
     */
    static EspNowManager *instance;
};

#endif // ESPNOW_MANAGER_H
