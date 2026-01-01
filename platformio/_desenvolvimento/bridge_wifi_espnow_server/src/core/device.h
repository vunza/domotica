#pragma once

#include "ctrl_debug.h"

/**
 * @class Device
 * @brief Classe base para inicialização e execução do dispositivo
 *
 * Esta classe encapsula a lógica principal do ciclo de vida do dispositivo,
 * incluindo:
 *  - Inicialização de hardware e serviços
 *  - Execução contínua (loop principal)
 *  - Verificação do estado do hardware
 *
 * Foi desenhada para manter o código organizado, legível
 * e facilmente extensível em projetos embarcados.
 */
class Device {
public:

    /**
     * @brief Inicializa o dispositivo
     *
     * Deve ser chamado uma única vez, normalmente no `setup()`.
     * Responsável por:
     *  - Inicializar hardware
     *  - Configurar periféricos
     *  - Validar condições mínimas de funcionamento
     *
     * @note Se a verificação de hardware falhar, a inicialização pode
     *       ser abortada ou tratada internamente.
     */
    void initialize();

    /**
     * @brief Executa o ciclo principal do dispositivo
     *
     * Deve ser chamado repetidamente, normalmente no `loop()`.
     * Responsável por:
     *  - Processar tarefas contínuas
     *  - Monitorizar estados
     *  - Executar lógica de aplicação
     */
    void run();

private:

    /**
     * @brief Verifica se o hardware está funcional
     *
     * Método interno usado durante a inicialização para validar
     * a presença e o estado correto do hardware necessário
     * (sensores, comunicação, memória, etc.).
     *
     * @return true se o hardware estiver operacional
     * @return false se houver falha na verificação
     */
    bool checkHardware();
};
