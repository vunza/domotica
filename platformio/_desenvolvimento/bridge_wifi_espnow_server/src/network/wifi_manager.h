// wifi_manager.h
#pragma once

#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include "ctrl_debug.h"

/**
 * @file wifi_manager.h
 * @brief Gerenciador simples de Wi-Fi para ESP8266 e ESP32
 *
 * Este módulo encapsula operações básicas de conexão Wi-Fi,
 * criação de Access Point (AP) e verificação do estado da conexão,
 * abstraindo diferenças entre ESP8266 e ESP32.
 */

/**
 * @class WiFiManager
 * @brief Classe responsável pela gestão da conectividade Wi-Fi
 *
 * Permite conectar o dispositivo a uma rede Wi-Fi existente
 * (modo Station), criar um ponto de acesso próprio (modo AP)
 * e monitorar o estado da conexão ao longo do tempo.
 */
class WiFiManager {
public:
    /**
     * @brief Conecta o dispositivo a uma rede Wi-Fi
     *
     * Coloca o módulo em modo Station (STA) e tenta conectar
     * ao SSID fornecido.
     *
     * @param ssid Nome da rede Wi-Fi
     * @param password Senha da rede Wi-Fi
     */
    void connect(const char* ssid, const char* password);

    /**
     * @brief Cria um Access Point (AP)
     *
     * Configura o dispositivo para operar como ponto de acesso,
     * permitindo que outros dispositivos se conectem diretamente.
     *
     * @param ssid Nome do Access Point
     * @param password Senha do Access Point (mínimo 8 caracteres)
     */
    void criar_ap(const char* ssid, const char* password, uint8_t channel = 1);

    /**
     * @brief Obtém o endereço MAC do dispositivo Wi-Fi.
     *
     * Esta função lê o endereço MAC da interface Wi-Fi e o grava
     * no buffer fornecido pelo chamador, com ou sem separadores
     * de dois pontos (`:`).
     *
     * É uma alternativa segura ao uso de `String`, evitando
     * problemas de ponteiros inválidos e fragmentação de memória,
     * especialmente em ESP8266 e ESP32.
     *
     * @param buffer      Buffer onde o endereço MAC será armazenado.
     *                    - Deve ter tamanho mínimo de:
     *                      • 18 bytes para formato com ":" (XX:XX:XX:XX:XX:XX)
     *                      • 13 bytes para formato sem ":" (XXXXXXXXXXXX)
     *
     * @param withColons  Define o formato de saída:
     *                    - true  → MAC com separadores ":" (ex: C8:C9:A3:38:D2:C3)
     *                    - false → MAC contínuo, sem separadores (ex: C8C9A338D2C3)
     *
     * @note
     * - O buffer será sempre finalizado com `\\0`.
     * - Os caracteres hexadecimais são gerados em minúsculas (`%02x`).
     *   Caso seja necessário letras maiúsculas, utilize `%02X`.
     *
     * @warning
     * - O chamador é responsável por garantir que o buffer tenha
     *   tamanho suficiente para o formato escolhido.
     *
     * @example
     * char mac_no_colon[13];
     * char mac_with_colon[18];
     *
     * getMacAddress(mac_no_colon, false);
     * getMacAddress(mac_with_colon, true);
     *
     * // Resultados possíveis:
     * // mac_no_colon   → "c8c9a338d2c3"
     * // mac_with_colon → "c8:c9:a3:38:d2:c3"
     */
    void getMacAddress(char* buffer, bool withColons);


    /**
     * @brief Verifica o estado da conexão Wi-Fi
     *
     * Deve ser chamado periodicamente no `loop()`.
     * Permite detectar desconexões e executar ações corretivas,
     * como reconexão automática.
     */
    void checkConnection();


    /**
     * @brief Extrai o identificador de dispositivo de um tópico MQTT formatado.
     * 
     * Esta função membro da classe WiFiManager processa tópicos MQTT no formato
     * "prefixo/<ID>/sufixo" e extrai o <ID> localizado entre a primeira e segunda
     * ocorrência do caractere delimitador '/'. É projetada especificamente para
     * extrair identificadores únicos de dispositivos (como endereços MAC normalizados)
     * de tópicos utilizados em sistemas de automação residencial com Home Assistant.
     * 
     * @param topico Referência constante a um objeto String contendo o tópico MQTT
     *               completo. Exemplo: "casa/c8c9a338d2e7/set"
     *               Formato esperado: "prefixo/identificador/sufixo"
     * 
     * @return String Objeto String contendo:
     *                - O identificador extraído (ex: "c8c9a338d2e7")
     *                - String vazia ("") se:
     *                  * O tópico não contiver pelo menos duas barras '/'
     *                  * O identificador entre barras estiver vazio
     *                  * O tópico for uma String vazia
     * 
     * @exception-safe Esta função não lança exceções. Todas as situações de erro
     *                 são tratadas internamente, retornando String vazia.
     * 
     * @note Performance: Utiliza métodos indexOf() e substring() da classe String,
     *       que criam cópias dos dados. Para processamento em tempo crítico,
     *       considere implementação com ponteiros e buffers estáticos.
     * 
     * @warning O delimitador é fixo como '/'. Tópicos com formato diferente
     *          (ex: "casa.c8c9a338d2e7.set") retornarão String vazia.
     * 
     * @see WiFiManager::configurarTopico(), WiFiManager::processarMensagemMQTT()
     * @ingroup mqtt_utilities
     * 
     * @example Exemplo básico de uso:
     * @code
     * WiFiManager manager;
     * String tpc = "casa/c8c9a338d2e7/set";
     * String id = manager.extrairIdDoTopico(tpc);
     * // id agora contém "c8c9a338d2e7"
     * @endcode
     * 
     * @example Com validação do resultado:
     * @code
     * String id = wifiManager.extrairIdDoTopico(topicoRecebido);
     * if (!id.isEmpty()) {
     *     Serial.printf("ID extraído: %s\n", id.c_str());
     *     // Processar ID válido
     * } else {
     *     Serial.println("Falha: tópico com formato inválido");
     * }
     * @endcode
     */
    String extrairIdDoTopico(const String& topico);


    /**
     * @brief Converte uma string contendo endereço MAC sem separadores para o formato padrão com letras maiúsculas.
     * 
     * Esta função recebe uma representação hexadecimal contínua de um endereço MAC (ex: "5abf254d301f")
     * e a converte para o formato canônico com separadores, padronizando todos os caracteres alfabéticos
     * para maiúsculas (ex: "5A:BF:25:4D:30:1F").
     * 
     * O formato de saída segue o padrão IEEE 802, apresentando 6 grupos de dois caracteres hexadecimais
     * separados por dois-pontos (:), amplamente utilizado em sistemas de rede, interfaces de usuário
     * e protocolos como MQTT para identificação de dispositivos.
     * 
     * @param macSemSeparador Referência constante a um objeto String contendo o endereço MAC
     *                        em formato contínuo (12 caracteres hexadecimais).
     *                        - Deve conter exatamente 12 caracteres (0-9, a-f, A-F)
     *                        - Não deve conter separadores ou espaços
     *                        - Exemplos válidos: "5abf254d301f", "A1B2C3D4E5F6"
     * 
     * @return String Objeto String contendo:
     *                - Endereço MAC formatado com separadores ':' e letras maiúsculas
     *                - String vazia ("") se:
     *                  * O parâmetro não contiver exatamente 12 caracteres
     *                  * O parâmetro contiver caracteres não hexadecimais
     *                  * O parâmetro for uma String vazia
     * 
     * @exception-safe Esta função não lança exceções. Todos os erros são tratados
     *                 internamente retornando String vazia.
     * 
     * @note Características da formatação:
     *       - Saída sempre em maiúsculas (A-F)
     *       - Usa ':' como separador (padrão IEEE)
     *       - Preserva zeros à esquerda (ex: "01" → "01")
     *       - Não modifica a string de entrada
     * 
     * @warning A função realiza validação hexadecimal. Strings com caracteres
     *          inválidos (g-z, G-Z, símbolos) resultam em retorno vazio.
     * 
     * @see extrairIdDoTopico(), converterParaMac(), normalizarMacAddress()
     * @ingroup formatting_functions
     * 
     * @example Exemplo básico de uso:
     * @code
     * String resultado = formatarMac("5abf254d301f");
     * // resultado = "5A:BF:25:4D:30:1F"
     * 
     * String resultado2 = formatarMac("a1b2c3d4e5f6");
     * // resultado2 = "A1:B2:C3:D4:E5:F6"
     * @endcode
     * 
     * @example Com verificação de erro:
     * @code
     * String mac = "5abf254d301f";
     * String formatado = formatarMac(mac);
     * 
     * if (!formatado.isEmpty()) {
     *     Serial.printf("MAC formatado: %s\n", formatado.c_str());
     *     // Usar MAC formatado em MQTT discovery
     *     configurarDispositivoMQTT(formatado);
     * } else {
     *     Serial.println("Erro: MAC inválido");
     *     // Lógica de tratamento de erro
     * }
     * @endcode
     */
    String formatarMac(const String& macSemSeparador);


private:
    /**
     * @brief Armazena o tempo da última verificação
     *
     * Utilizado para evitar verificações contínuas
     * e controlar o intervalo entre checagens.
     */
    unsigned long lastCheck = 0;
};
