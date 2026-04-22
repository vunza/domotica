#ifndef OLED_DISPLAY_128x64_H
#define OLED_DISPLAY_128x64_H

/**
 * @file oled_display.h
 * @brief Módulo para controlo de display OLED SSD1306 (128x64)
 *
 * Compatível com ESP8266 e ESP32.
 * Encapsula a inicialização, escrita de texto, valores numéricos
 * e exibição gráfica de percentagem de bateria.
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* =========================================================
   CONFIGURAÇÃO DO DISPLAY OLED
   ========================================================= */

/**
 * @def OLED_WIDTH
 * @brief Largura do display OLED em pixels
 */
#define OLED_WIDTH   128

/**
 * @def OLED_HEIGHT
 * @brief Altura do display OLED em pixels
 */
#define OLED_HEIGHT  64

/**
 * @def OLED_ADDR
 * @brief Endereço I2C do display OLED
 * 
 * Valores comuns:
 *  - 0x3C (mais comum)
 *  - 0x3D
 */
#define OLED_ADDR    0x3C


/**
 * @class OLEDDisplay
 * @brief Classe de abstração para displays OLED SSD1306
 *
 * Esta classe simplifica o uso do display OLED,
 * escondendo detalhes de inicialização, buffer e renderização.
 */
class OLEDDisplay {
public:
    /**
     * @brief Construtor da classe OLEDDisplay
     *
     * Inicializa internamente o objeto Adafruit_SSD1306,
     * mas não inicia o hardware (isso é feito em begin()).
     */
    OLEDDisplay();

    /**
     * @brief Inicializa o display OLED
     *
     * - Inicializa o barramento I2C
     * - Inicializa o controlador SSD1306
     * - Limpa o display
     *
     * @return true se o display foi inicializado com sucesso
     * @return false se houve falha na inicialização
     */
    bool begin();

    /**
     * @brief Limpa o buffer do display
     *
     * Nota: é necessário chamar update() para refletir no ecrã.
     */
    void clear();

    /**
     * @brief Imprime texto simples no display
     *
     * @param x Posição X (em pixels)
     * @param y Posição Y (em pixels)
     * @param text Texto a ser exibido
     * @param size Tamanho da fonte (1 = padrão)
     */
    void printText(uint8_t x, uint8_t y, const char* text, uint8_t size = 1);

    /**
     * @brief Imprime um valor numérico com rótulo
     *
     * Exemplo:
     *   "Temp: 25 C"
     *
     * @param x Posição X
     * @param y Posição Y
     * @param label Texto descritivo
     * @param value Valor numérico
     * @param suffix Sufixo opcional (ex: "°C", "%", "V")
     * @param size Tamanho da fonte
     */
    void printValue(uint8_t x, uint8_t y,
                    const char* label,
                    int value,
                    const char* suffix = "",
                    uint8_t size = 1);

    /**
     * @brief Desenha um indicador gráfico de bateria
     *
     * @param percent Percentagem da bateria (0–100)
     */
    void showBattery(uint8_t percent);

    /**
     * @brief Atualiza o display físico
     *
     * Envia o buffer interno para o ecrã.
     * Deve ser chamado após qualquer alteração.
     */
    void update();

private:
    /**
     * @brief Objeto da biblioteca Adafruit SSD1306
     *
     * Responsável pela comunicação direta com o display.
     */
    Adafruit_SSD1306 display;
};

#endif // OLED_DISPLAY_128x64_H
