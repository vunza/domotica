#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H

#include <Arduino.h>
#include "constantes.h"

#if defined(ESP32)
  #include <EEPROM.h>
#elif defined(ESP8266)
  #include <EEPROM.h>
#else
  #error "Placa não suportada!"
#endif

/**
 * @file eeprom_manager.h
 * @brief Gerenciador genérico para leitura e escrita de dados na EEPROM
 *
 * Este módulo fornece uma abstração simples para armazenar e recuperar
 * estruturas de dados na EEPROM de forma segura e reutilizável,
 * compatível com ESP8266 e ESP32.
 */

/**
 * @struct DeviceData
 * @brief Estrutura de dados persistentes do dispositivo
 *
 * Estrutura utilizada para armazenar informações permanentes
 * do dispositivo na EEPROM, como o nome do dispositivo.
 */
typedef struct {
    /** Nome do dispositivo (string C terminada em '\0') */
    char device_name[DEVICE_NAME_SIZE];
} DeviceData;

/**
 * @class EEPROMManager
 * @brief Classe responsável por gerenciar o acesso à EEPROM
 *
 * Permite inicializar a EEPROM e realizar operações de leitura
 * e escrita de estruturas completas utilizando templates,
 * evitando código repetitivo e facilitando a manutenção.
 */
class EEPROMManager {
public:

    /**
     * @brief Construtor padrão
     */
    EEPROMManager() {}

    /**
     * @brief Inicializa a EEPROM
     *
     * Deve ser chamado uma única vez no `setup()`.
     * No ESP8266 e ESP32 é necessário definir o tamanho total
     * da EEPROM a ser utilizada.
     *
     * @param eeprom_size Tamanho da EEPROM em bytes
     */
    void begin(uint16_t eeprom_size) {
        EEPROM.begin(eeprom_size);
    }

    /**
     * @brief Escreve uma estrutura de dados na EEPROM
     *
     * Serializa a estrutura byte a byte e grava na EEPROM
     * a partir do endereço informado.
     *
     * ⚠️ Atenção:
     * - O tamanho da estrutura não pode ultrapassar o espaço disponível
     * - O endereço deve estar alinhado corretamente
     *
     * @tparam T Tipo da estrutura de dados
     * @param address Endereço inicial na EEPROM
     * @param data Estrutura contendo os dados a serem gravados
     */
    template <typename T>
    void EEPROM_writeStruct(int address, const T &data) {
        const byte *ptr = (const byte*) &data;
        for (unsigned int i = 0; i < sizeof(T); i++) {
            EEPROM.write(address + i, ptr[i]);
        }
        EEPROM.commit();
    }

    /**
     * @brief Lê uma estrutura de dados da EEPROM
     *
     * Reconstrói a estrutura de dados a partir dos bytes
     * armazenados na EEPROM.
     *
     * ⚠️ Atenção:
     * - O tipo de leitura deve ser o mesmo usado na escrita
     * - O endereço deve corresponder ao endereço de escrita
     *
     * @tparam T Tipo da estrutura de dados
     * @param address Endereço inicial na EEPROM
     * @param data Estrutura onde os dados lidos serão armazenados
     */
    template <typename T>
    void EEPROM_readStruct(int address, T &data) {
        byte *ptr = (byte*) &data;
        for (unsigned int i = 0; i < sizeof(T); i++) {
            ptr[i] = EEPROM.read(address + i);
        }
    }
};

#endif
