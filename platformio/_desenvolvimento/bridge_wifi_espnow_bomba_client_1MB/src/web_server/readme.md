# Documentação do Módulo `web_server`

Este documento descreve o módulo **WebServer**, responsável por servir páginas HTML, CSS e JavaScript através do LittleFS, além de disponibilizar uma API REST que fornece dados dos sensores em formato JSON.

---

## 📁 Arquivos do Módulo

### **src/web_server/web_server.h**

```cpp
#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include "ctrl_debug.h"
#include "json/json_builder.h"

extern float g_voltage;
extern float g_current;
extern float g_temperature;
extern float g_humidity;

class WebServer {
public:
    WebServer(AsyncWebServer* server);
    void begin();
private:
    AsyncWebServer* server;
};

#endif
```

### **src/web_server/web_server.cpp**

```cpp
#include "web_server.h"

WebServer::WebServer(AsyncWebServer* s) {
    server = s;
}

void WebServer::begin() {

    if (!LittleFS.begin()) {
        imprimeln(F("Erro a montar SPIFFS"));
        return;
    }
    else{
        imprimeln(F("SPIFFS montado com sucesso"));
    }

    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/painel.html", "text/html"); });

    server->on("/ota", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/ota.html", "text/html"); });

    server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/style.css", "text/css"); });

    server->on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/script.js", "text/javascript"); });

    server->on("/api/sensores", HTTP_GET, [](AsyncWebServerRequest *request){            
        JsonBuilder json;
        json.add("voltage", g_voltage, 3);
        json.add("current", g_current, 3);
        json.add("temperature", g_temperature, 2);
        json.add("humidity", g_humidity, 2);

        request->send(200, "application/json", json.build());     
    });          

    server->begin();

    imprimeln(F("Servidor HTTP iniciado"));
}
```

---

## 🧩 Objetivo do Módulo

O módulo `web_server` tem como função:

* Inicializar o sistema de arquivos **LittleFS**.
* Servir arquivos estáticos (HTML, CSS, JS) diretamente do LittleFS.
* Criar uma API REST para disponibilizar valores dos sensores (INA226 e DHT11).
* Integrar o sistema de debug com mensagens de estado.

É um dos módulos centrais da interface web do dispositivo.

---

## ⚙️ Funcionamento Interno

### **1. Montagem do LittleFS**

O servidor tenta montar o sistema de arquivos interno:

* Em caso de falha → imprime mensagem de erro.
* Em caso de sucesso → confirma montagem.

### **2. Servir arquivos estáticos**

O módulo disponibiliza pelo menos 4 arquivos:

* `/` → **painel.html**
* `/ota` → **ota.html**
* `/style.css`
* `/script.js`

Esses arquivos permitem criar toda a interface Web.

### **3. API REST: `/api/sensores`**

Retorna um JSON com:

* `voltage` (V)
* `current` (mA)
* `temperature` (°C)
* `humidity` (%)

Os valores são lidos de variáveis globais:

```cpp
g_voltage
g_current
g_temperature
g_humidity
```

Essas variáveis devem ser atualizadas em outro módulo (ex.: sensores).

### **4. Inicialização do Servidor Web**

Após registrar rotas e montar o FS:

```cpp
server->begin();
```

É exibida a mensagem:

```
Servidor HTTP iniciado
```

---

## 🧪 Exemplo de Uso

```cpp
#include <ESPAsyncWebServer.h>
#include "web_server/web_server.h"

AsyncWebServer server(80);
WebServer web(&server);

void setup() {
    Serial.begin(115200);
    web.begin();
}

void loop() {
    // O servidor funciona de modo assíncrono
}
```

---

## ✔️ Benefícios do Módulo

* Arquitetura limpa, separando interface web da lógica principal.
* Suporte total a arquivos estáticos no LittleFS.
* API REST otimizada, usando `JsonBuilder` (leve e eficiente).
* Integra-se com OTA, sensores e display.
* Baseado no **ESPAsyncWebServer**, permitindo alta performance sem blocos.

---

## 📝 Resumo

O módulo **web_server** implementa o servidor HTTP responsável por servir a interface web e a API de sensores. Ele monta o sistema de arquivos LittleFS, registra rotas, disponibiliza dados em JSON e integra-se ao restante do projeto de forma modular, eficiente e expansível.
