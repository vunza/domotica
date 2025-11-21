# Web Server Module - README

## 📌 Objetivo

Este módulo **web_server** foi criado para facilitar a implementação de um servidor web assíncrono no ESP8266/ESP32 utilizando **ESPAsyncWebServer**. Ele oferece uma forma simples, modular e escalável de expor APIs, servir páginas HTML, interagir com sensores e integrar outras partes do firmware.

---

## 🚀 Funcionalidades Principais

* Inicialização fácil do servidor web.
* Registro simplificado de rotas HTTP (GET/POST).
* Retorno de dados no formato JSON.
* Compatível com ESP8266 e ESP32.
* Pode ser integrado com módulos de sensores (INA226, DHT11, etc.).
* Permite servir páginas locais (SPIFFS/LittleFS).

---

## 📦 Arquitetura do Módulo

o módulo normalmente contém:

```
web_server/
├── web_server.h
└── web_server.cpp
```

### `web_server.h`

Define a classe WebServerModule e sua interface pública.

### `web_server.cpp`

Implementa:

* inicialização do servidor
* rotas
* handlers de API
* integração com sensores externos

O `main.cpp` apenas instancia e inicializa o módulo.

---

## 🧩 Como Usar

### 1. **Incluir o módulo no seu projeto**

No `main.cpp`:

```cpp
#include "web_server.h"
#include "sensorINA226.h"
#include "sensorDHT11.h"

WebServerModule serverModule;
SensorINA226 sensorINA226;
SensorDHT11 sensorDHT11;

void setup() {
    Serial.begin(115200);
    sensorINA226.begin();
    sensorDHT11.begin();
    serverModule.begin(&sensorINA226, &sensorDHT11);
}

void loop() {
    // Nada necessário — AsyncWebServer não usa loop()
}
```

---

## 🌐 Rotas Disponíveis

### **GET /api/sensores**

Retorna JSON com valores dos sensores:

```json
{
  "voltage": 12.34,
  "current": 125.6,
  "temperature": 24.5,
  "humidity": 60
}
```

### **GET /**

Serve a página HTML principal (opcional via SPIFFS/LittleFS).

---

## 📁 Servindo Páginas Estáticas

Se usar SPIFFS/LittleFS:

**Upload da pasta `data/` → Sistema de Arquivos**

```cpp
server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
```

---

## 🔌 Requisitos

* **ESPAsyncWebServer**
* **AsyncTCP** (ESP32)
* **ESPAsyncTCP** (ESP8266)
* Sensor modules opcionais (INA226, DHT11, etc.)
* LittleFS ou SPIFFS para servir páginas

---

## 📚 Exemplo de Fetch no Front-end

```javascript
fetch('/api/sensores')
    .then(res => res.json())
    .then(data => {
        console.log(data.voltage, data.current);
    });
```

---

## 🛠️ Personalização

Você pode adicionar novas rotas facilmente em `web_server.cpp`:

```cpp
server.on("/api/custom", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(200, "application/json", "{\"ok\":true}");
});
```

---

## 📄 Licença

Uso livre para projetos pessoais e comerciais.

---

Se quiser, posso gerar:

* exemplo completo do módulo
* versão com OTA integrada
* versão com autenticação
* diagrama da arquitetura
