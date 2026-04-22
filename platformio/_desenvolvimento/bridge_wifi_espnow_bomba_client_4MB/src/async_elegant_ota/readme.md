# Documentação do Módulo `async_elegant_ota`

Este documento descreve de forma clara e direta o funcionamento do módulo `async_elegant_ota`, responsável por integrar atualizações OTA assíncronas usando a biblioteca **AsyncElegantOTA**.

---

## 📁 Arquivos do Módulo

### **src/async_elegant_ota/async_elegant_ota.h**

```cpp
#ifndef ASYNC_ELEGANT_OTA_H
#define ASYNC_ELEGANT_OTA_H

#include <Arduino.h>
#include <AsyncElegantOTA.h>
#include <ESPAsyncWebServer.h>
#include "ctrl_debug.h"

class ElengantOTA {
public:
    static void begin(AsyncWebServer* server);
};

#endif
```

### **src/async_elegant_ota/async_elegant_ota.cpp**

```cpp
#include "async_elegant_ota.h"

void ElengantOTA::begin(AsyncWebServer* server) {
    AsyncElegantOTA.begin(server);
    imprimeln(F("OTA Inicializada!"));
}
```

---

## 🧩 Objetivo do Módulo

Este módulo fornece uma interface simples e modular para habilitar **OTA (Over‑the‑Air)** usando a biblioteca **AsyncElegantOTA** em conjunto com um servidor web baseado em **ESPAsyncWebServer**.

A função principal `ElengantOTA::begin()` encapsula:

* A inicialização do serviço de OTA.
* A integração ao servidor web existente.
* A emissão de mensagens de debug usando `imprimeln()`.

---

## ⚙️ Como Usar no Projeto

### **1. Inclua o módulo**

```cpp
#include "async_elegant_ota.h"
```

### **2. Crie o servidor Web**

```cpp
AsyncWebServer server(80);
```

### **3. Inicialize o OTA no `setup()`**

```cpp
void setup() {
    Serial.begin(115200);

    server.begin();

    ElengantOTA::begin(&server);

    imprimeln("OTA pronto e servidor iniciado.");
}
```

---

## 🔍 Comportamento Interno

### **ElengantOTA::begin()**

* Registra o serviço OTA no objeto `AsyncWebServer` fornecido.
* Habilita interface OTA acessível em:

```
http://<IP_DO_DISPOSITIVO>/update
```

* Emite a mensagem:

```
OTA Inicializada!
```

caso `ACTIVAR_SERIAL_DEBUG` esteja definido no `platformio.ini`.

---

## ✔️ Benefícios do Módulo

* Encapsula o OTA em um único ponto.
* Facilita reutilização do código em vários projetos.
* Evita repetição do código padrão de inicialização do AsyncElegantOTA.
* Integra automaticamente com o sistema de debug do projeto.

---

## 📝 Resumo

O módulo `async_elegant_ota` simplifica a inicialização e uso da biblioteca AsyncElegantOTA, proporcionando uma forma organizada, prática e extensível de habilitar atualizações OTA em dispositivos ESP8266/ESP32 usando `ESPAsyncWebServer`.
