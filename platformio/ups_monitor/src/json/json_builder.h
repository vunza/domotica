// Cria um objeto JSON de forma  dinamica
#ifndef JSON_BUILDER_H
#define JSON_BUILDER_H

#include <Arduino.h>

class JsonBuilder {
private:
    String json;
    bool firstField = true;

public:
    JsonBuilder() {
        json.reserve(128);  // otimizar memória
        json = "{";
    }

    void add(const String &key, const String &value, bool quote = true) {
        if (!firstField) json += ",";
        firstField = false;

        json += "\"" + key + "\":";
        if (quote) json += "\"" + value + "\"";
        else json += value;
    }

    void add(const String &key, float value, uint8_t decimals = 2) {
        if (!firstField) json += ",";
        firstField = false;

        json += "\"" + key + "\":" + String(value, decimals);
    }

    void add(const String &key, int value) {
        if (!firstField) json += ",";
        firstField = false;

        json += "\"" + key + "\":" + String(value);
    }

    String build() {
        return json + "}";
    }

    void reset() {
        json = "{";
        firstField = true;
    }
};

#endif
