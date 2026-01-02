#ifndef ASYNC_ELEGANT_OTA_H
#define ASYNC_ELEGANT_OTA_H

#include <Arduino.h>
#include <AsyncElegantOTA.h>
#include <ESPAsyncWebServer.h>
#include "ctrl_debug.h"

/**
 * @class ElengantOTA
 * @brief Inicializa e integra o AsyncElegantOTA com um servidor web assíncrono
 *
 * Esta classe fornece uma interface simples e estática para ativar
 * atualizações OTA (Over-The-Air) em dispositivos ESP8266 e ESP32
 * usando a biblioteca AsyncElegantOTA.
 *
 * O OTA é exposto através do AsyncWebServer, permitindo:
 *  - Upload de firmware via navegador
 *  - Atualização segura sem cabo USB
 *  - Integração com sistemas web existentes
 *
 * A classe foi desenhada para ser:
 *  - Leve
 *  - Fácil de reutilizar
 *  - Compatível com PlatformIO
 */
class ElengantOTA {
public:

    /**
     * @brief Inicializa o serviço OTA no servidor web fornecido
     *
     * Este método deve ser chamado após a criação do AsyncWebServer,
     * antes de iniciar o servidor (`server->begin()`).
     *
     * Internamente, este método:
     *  - Regista as rotas OTA no servidor
     *  - Ativa o endpoint padrão do AsyncElegantOTA (`/update`)
     *
     * @param server Ponteiro para a instância do AsyncWebServer
     *
     * @note O servidor deve permanecer válido durante toda a execução
     * @note O OTA estará disponível em: http://<ip_do_dispositivo>/update
     */
    static void begin(AsyncWebServer* server);
};

#endif
