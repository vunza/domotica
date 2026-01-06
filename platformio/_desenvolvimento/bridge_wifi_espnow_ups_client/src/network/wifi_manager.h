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

private:
    /**
     * @brief Armazena o tempo da última verificação
     *
     * Utilizado para evitar verificações contínuas
     * e controlar o intervalo entre checagens.
     */
    unsigned long lastCheck = 0;
};
