#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "ctrl_debug.h"

/**
 * @class LCDDisplay
 * @brief Classe de abstração para displays LCD I2C
 *
 * Esta classe encapsula a inicialização e o uso de um display
 * LCD com interface I2C (baseado no controlador HD44780),
 * facilitando a apresentação de leituras como corrente e tensão.
 *
 * Compatível com ESP8266, ESP32 e outras plataformas Arduino.
 */
class LCDDisplay {
public:

    /**
     * @brief Construtor da classe LCDDisplay
     *
     * @param i2c_addr Endereço I2C do display (ex: 0x27, 0x3F)
     * @param cols Número de colunas do display
     * @param rows Número de linhas do display
     */
    LCDDisplay(uint8_t i2c_addr, uint8_t cols, uint8_t rows);

    /**
     * @brief Inicializa o display LCD
     *
     * Deve ser chamado uma única vez no `setup()`.
     * Responsável por:
     *  - Inicializar a comunicação I2C
     *  - Inicializar o LCD
     *  - Ligar a retroiluminação
     */
    void begin();

    /**
     * @brief Exibe leituras de corrente e tensão no display
     *
     * Atualiza o conteúdo do LCD apresentando os valores
     * de corrente (em miliampères) e tensão (em volts),
     * formatados de forma legível.
     *
     * @param current_mA Corrente medida em miliampères (mA)
     * @param voltage_V Tensão medida em volts (V)
     */
    void showReadings(float current_mA, float voltage_V);

private:

    /**
     * @brief Instância do controlador do display LCD I2C
     */
    LiquidCrystal_I2C lcd;

    /**
     * @brief Número de colunas do display
     */
    uint8_t _cols;

    /**
     * @brief Número de linhas do display
     */
    uint8_t _rows;
};

#endif
