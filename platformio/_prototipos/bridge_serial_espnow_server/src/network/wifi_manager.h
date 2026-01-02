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
    void criar_ap(const char* ssid, const char* password);

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
