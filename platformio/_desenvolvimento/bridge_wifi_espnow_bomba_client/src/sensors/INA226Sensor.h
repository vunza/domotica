// INA226 current and voltage sensor 
#ifndef INA226_SENSOR_H
#define INA226_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <INA226_WE.h>
#include "ctrl_debug.h"

/**
 * @file ina226_sensor.h
 * @brief Classe para leitura de corrente e tensão usando o sensor INA226
 *
 * Esta classe encapsula a biblioteca INA226_WE para fornecer
 * uma interface simples de inicialização, atualização e leitura
 * de corrente e tensão.
 *
 * Compatível com ESP8266, ESP32 e outras placas Arduino.
 */

/**
 * @class INA226Sensor
 * @brief Abstração do sensor INA226 para corrente e tensão
 *
 * Permite inicializar o sensor, ler valores de corrente (mA)
 * e tensão (V), e atualizar as leituras de forma eficiente.
 */
class INA226Sensor {

public:
    /**
     * @brief Construtor da classe
     *
     * Define o endereço I2C do sensor.
     *
     * @param address Endereço I2C do sensor (padrão 0x40)
     */
    INA226Sensor(uint8_t address = 0x40);

    /**
     * @brief Inicializa o sensor INA226
     *
     * Deve ser chamado no `setup()`. Inicializa a comunicação I2C
     * e verifica se o sensor está respondendo.
     *
     * @return true se a inicialização for bem-sucedida
     * @return false se houver falha na comunicação
     */
    bool begin();                 

    /**
     * @brief Lê a corrente medida pelo sensor
     *
     * @return Corrente em miliampères (mA)
     */
    float readCurrent();          

    /**
     * @brief Lê a tensão medida pelo sensor
     *
     * @return Tensão em volts (V)
     */
    float readVoltage();          

    /**
     * @brief Atualiza as leituras internas de corrente e tensão
     *
     * Chame periodicamente para manter valores atualizados
     * antes de chamar `readCurrent()` ou `readVoltage()`.
     */
    void update();                

private:
    /** Objeto da biblioteca INA226_WE */
    INA226_WE ina;

    /** Última leitura de corrente em mA */
    float current_mA;

    /** Última leitura de tensão em V */
    float voltage_V;
};

#endif
