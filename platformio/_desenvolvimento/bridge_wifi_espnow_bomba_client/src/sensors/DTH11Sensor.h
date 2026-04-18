// DHT11 temperature and humidity sensor
#ifndef DHT11SENSOR_H
#define DHT11SENSOR_H

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "ctrl_debug.h"

/**
 * @file dht11sensor.h
 * @brief Classe para leitura do sensor DHT11 (temperatura e umidade)
 *
 * Esta classe fornece uma interface simples para inicializar
 * e ler valores de temperatura e umidade do sensor DHT11,
 * encapsulando a biblioteca Adafruit DHT.
 *
 * Compatível com ESP8266, ESP32 e outras placas Arduino.
 */

/**
 * @class DHT11Sensor
 * @brief Classe de abstração do sensor DHT11
 *
 * Permite inicializar o sensor, ler temperatura e umidade
 * de forma fácil e segura, sem precisar acessar diretamente
 * a biblioteca DHT.
 */
class DHT11Sensor {
public:

    /**
     * @brief Construtor da classe
     *
     * Define o pino de dados e o tipo do sensor.
     *
     * @param pin Pino digital conectado ao DHT11
     * @param type Tipo do sensor (DHT11 ou DHT22)
     */
    DHT11Sensor(uint8_t pin, uint8_t type);

    /**
     * @brief Inicializa o sensor
     *
     * Deve ser chamado no `setup()`. Inicializa
     * a biblioteca DHT e prepara o sensor para leituras.
     */
    void begin();

    /**
     * @brief Obtém a temperatura atual
     *
     * @return Temperatura em graus Celsius
     */
    float getTemperature();

    /**
     * @brief Obtém a umidade relativa do ar
     *
     * @return Umidade relativa em porcentagem (%)
     */
    float getHumidity();

private:
    /** Instância da biblioteca DHT para comunicação com o sensor */
    DHT dht;
};

#endif
