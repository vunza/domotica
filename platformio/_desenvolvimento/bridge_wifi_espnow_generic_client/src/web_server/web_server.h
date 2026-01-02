#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include "ctrl_debug.h"
#include "json/json_builder.h"
#include "eeprom_manager/eeprom_manager.h"

/**
 * @file web_server.h
 * @brief Classe de gerenciamento de servidor web assíncrono
 *
 * Este módulo fornece abstração para um servidor web baseado em
 * ESPAsyncWebServer, permitindo servir arquivos do sistema de
 * arquivos LittleFS e expor dados do dispositivo em JSON.
 *
 * Integra com:
 *  - Sensores (corrente, tensão, temperatura, umidade)
 *  - EEPROM para dados persistentes do dispositivo
 *  - JsonBuilder para respostas dinâmicas
 */

/** Variáveis globais compartilhadas com o servidor web */
extern float g_voltage;      /**< Tensão medida pelo dispositivo (V) */
extern float g_current;      /**< Corrente medida pelo dispositivo (mA) */
extern float g_temperature;  /**< Temperatura medida pelo sensor (°C) */
extern float g_humidity;     /**< Umidade relativa medida pelo sensor (%) */

/** Instância global do gerenciador de EEPROM */
extern EEPROMManager eeprom;

/** Estrutura global de dados do dispositivo */
extern DeviceData dados_dispositivo;

/**
 * @class WebServer
 * @brief Classe de abstração para servidor web assíncrono
 *
 * Permite inicializar e configurar rotas do servidor web
 * utilizando ESPAsyncWebServer, expondo dados em JSON
 * e servindo arquivos estáticos do LittleFS.
 */
class WebServer {
public:
    /**
     * @brief Construtor da classe
     *
     * Recebe uma instância de AsyncWebServer previamente criada
     * e armazena internamente para configurar rotas e callbacks.
     *
     * @param server Ponteiro para a instância de AsyncWebServer
     */
    WebServer(AsyncWebServer* server);

    /**
     * @brief Inicializa o servidor web
     *
     * Configura rotas HTTP, endpoints JSON e arquivos estáticos.
     * Deve ser chamado no `setup()` após inicialização do LittleFS
     * e configuração dos sensores.
     */
    void begin();

private:
    /** Ponteiro para a instância de servidor web assíncrono */
    AsyncWebServer* server;
};

#endif
