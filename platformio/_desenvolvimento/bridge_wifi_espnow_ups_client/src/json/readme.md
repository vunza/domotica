# Documentação do Módulo `json`

Este documento descreve o módulo **JsonBuilder**, responsável por construir objetos JSON de forma dinâmica e eficiente, utilizando apenas `String` e funções nativas do Arduino, sem dependências externas.

---

## 📁 Arquivo

### **src/json/json_builder.h**

```cpp
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
        json.reserve(128);  // otimiza memória
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

        char buffer[20];
        dtostrf(value, 1, decimals, buffer);
        json += "\"" + key + "\":" + buffer;
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
```

---

## 🧩 Objetivo do Módulo

O módulo `json_builder` foi criado para permitir a montagem dinâmica de objetos JSON sem depender de bibliotecas pesadas como ArduinoJson. Ele é ideal para microcontroladores com pouca memória, como ESP8266 ou AVR.

A classe **JsonBuilder** fornece:

* Criação incremental de objetos JSON.
* Suporte para strings, inteiros e floats.
* Controle automático de vírgulas entre campos.
* Função `reset()` para reutilizar o objeto sem realocar memória.

---

## ⚙️ Funcionamento Interno

### **1. Construtor**

Reserva 128 bytes (otimizado) e inicia o JSON com `{`.

### **2. add(key, value)** — *String*

Permite adicionar um par chave–valor com ou sem aspas.

```cpp
add("nome", "sensor01");
add("ativo", "true", false);
```

### **3. add(key, value)** — *float*

Usa `dtostrf()` para evitar ambiguidades na conversão.

```cpp
add("temperatura", 22.567, 2);
```

### **4. add(key, value)** — *int*

Insere números inteiros diretamente.

```cpp
add("contador", 42);
```

### **5. build()**

Fecha o objeto JSON e devolve o `String` final.

### **6. reset()**

Limpa o JSON e permite reuso do mesmo objeto.

---

## 🧪 Exemplo de Uso

```cpp
#include "json/json_builder.h"

JsonBuilder json;

void setup() {
    Serial.begin(115200);

    json.add("corrente_mA", 123.45, 2);
    json.add("tensao_V", 12.78, 2);
    json.add("estado", "OK");

    String payload = json.build();
    Serial.println(payload);
}
```

**Saída:**

```json
{"corrente_mA":123.45,"tensao_V":12.78,"estado":"OK"}
```

---

## ✔️ Benefícios do Módulo

* Zero dependências externas.
* Baixo consumo de memória.
* Montagem rápida e segura de JSON.
* Evita custo alto de parsing e memória do ArduinoJson.
* Ideal para integração com APIs HTTP, MQTT e WebSockets.

---

## 📝 Resumo

O módulo **json_builder** fornece uma forma simples, leve e eficiente de criar objetos JSON em microcontroladores, atendendo a projetos onde simplicidade e economia de memória são prioridade.
