// Cria um objeto JSON de forma dinâmica
#ifndef JSON_BUILDER_H
#define JSON_BUILDER_H

#include <Arduino.h>

/**
 * @file json_builder.h
 * @brief Classe simples para construção dinâmica de objetos JSON
 *
 * Este módulo permite criar strings JSON de forma incremental,
 * sem dependência de bibliotecas externas como ArduinoJson,
 * sendo ideal para microcontroladores com recursos limitados.
 */

/**
 * @class JsonBuilder
 * @brief Construtor simples de objetos JSON em formato string
 *
 * A classe permite adicionar pares chave/valor de diferentes
 * tipos (string, inteiro e float) e gerar um JSON válido
 * no final do processo.
 *
 * Exemplo de uso:
 * @code
 * JsonBuilder json;
 * json.add("device", "esp8266");
 * json.add("voltage", 220.5);
 * json.add("state", "ON");
 * String payload = json.build();
 * @endcode
 */
class JsonBuilder {
private:
    /** String interna que armazena o JSON em construção */
    String json;

    /** Indica se o próximo campo será o primeiro (controle de vírgulas) */
    bool firstField = true;

public:
    /**
     * @brief Construtor padrão
     *
     * Inicializa o objeto JSON e reserva memória
     * para reduzir fragmentação do heap.
     */
    JsonBuilder() {
        json.reserve(128);  // Otimiza uso de memória
        json = "{";
    }

    /**
     * @brief Adiciona um campo do tipo string ou valor bruto
     *
     * @param key Nome da chave JSON
     * @param value Valor associado à chave
     * @param quote Define se o valor deve ser envolvido por aspas
     *              - true  → valor tratado como string
     *              - false → valor tratado como número ou literal
     */
    void add(const String &key, const String &value, bool quote = true) {
        if (!firstField) json += ",";
        firstField = false;

        json += "\"" + key + "\":";
        if (quote) json += "\"" + value + "\"";
        else json += value;
    }

    /**
     * @brief Adiciona um campo numérico do tipo float
     *
     * O valor é convertido para string utilizando `dtostrf`,
     * garantindo controle do número de casas decimais.
     *
     * @param key Nome da chave JSON
     * @param value Valor numérico
     * @param decimals Número de casas decimais (padrão = 2)
     */
    void add(const String &key, float value, uint8_t decimals = 2) {
        if (!firstField) json += ",";
        firstField = false;

        char buffer[20];
        dtostrf(value, 1, decimals, buffer);
        json += "\"" + key + "\":" + buffer;
    }

    /**
     * @brief Adiciona um campo numérico do tipo inteiro
     *
     * @param key Nome da chave JSON
     * @param value Valor inteiro
     */
    void add(const String &key, int value) {
        if (!firstField) json += ",";
        firstField = false;

        json += "\"" + key + "\":" + String(value);
    }

    /**
     * @brief Finaliza e retorna o JSON construído
     *
     * @return String contendo o JSON válido
     */
    String build() {
        return json + "}";
    }

    /**
     * @brief Reinicia o construtor para reutilização
     *
     * Limpa o JSON interno e permite adicionar
     * novos campos a partir do zero.
     */
    void reset() {
        json = "{";
        firstField = true;
    }
};

#endif
