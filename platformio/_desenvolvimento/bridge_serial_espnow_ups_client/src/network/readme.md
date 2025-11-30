# Documentação do Módulo `network`

Este documento descreve o módulo **WiFiManager**, responsável por gerenciar a conexão WiFi do dispositivo, incluindo conexão a redes existentes, criação de ponto de acesso (AP) e verificação periódica da conexão.

---

## 📁 Arquivos do Módulo

### **src/network/wifi_manager.h**

```cpp
#pragma once

#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include "ctrl_debug.h"

class WiFiManager {
public:
    void connect(const char* ssid, const char* password);
    void criar_ap(const char* ssid, const char* password);
    void checkConnection();
private:
    unsigned long lastCheck = 0;
};
```

### **src/network/wifi_manager.cpp**

```cpp
#include "wifi_manager.h"

void WiFiManager::connect(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        imprimeln(F("Conectando ao WiFi..."));
    }

    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    
    imprimeln(F("
Conectado ao WiFi!"));    
    imprimeln(WiFi.localIP());
}

void WiFiManager::criar_ap(const char* ssid, const char* password) {
    WiFi.softAP(ssid, password);
    imprimeln(F("Ponto de acesso criado"));
    imprimeln(WiFi.softAPIP());
}

void WiFiManager::checkConnection() {     
    if (millis() - lastCheck >= 10000) {
        if (WiFi.status() != WL_CONNECTED) {
            imprimeln(F("Conexão WiFi perdida!"));
            // TODO Implementar reconexão
        }
        lastCheck = millis();
    }
}
```

---

## 🧩 Objetivo do Módulo

O módulo `network` foi criado para fornecer uma interface única e simples para:

* Conectar o dispositivo a uma rede WiFi.
* Criar um ponto de acesso (modo AP).
* Monitorar periodicamente a conexão para detectar quedas.

Ele atua como a camada de transporte principal do projeto, garantindo que o dispositivo esteja sempre conectado ou acessível.

---

## ⚙️ Funcionamento Interno

### **1. connect(ssid, password)**

Conecta o dispositivo a uma rede WiFi existente.

* Usa `WiFi.begin()`.
* Aguarda a conexão dentro de um loop.
* Exibe mensagens de depuração se habilitado.
* Ativa reconexão automática (`setAutoReconnect`).
* Torna as configurações persistentes com `persistent(true)`.

### **2. criar_ap(ssid, password)**

Cria um ponto de acesso (Access Point).

* Usa `WiFi.softAP()`.
* Exibe endereço IP do AP (`WiFi.softAPIP()`).
* Útil para modo fallback ou configuração inicial do dispositivo.

### **3. checkConnection()**

Verifica a cada 10 segundos se o dispositivo continua conectado.

* Se não estiver, registra mensagem de perda de conexão.
* Ponto planejado para implementação futura de reconexão.

---

## 🧪 Exemplo de Uso

```cpp
#include "network/wifi_manager.h"

WiFiManager wifi;

void setup() {
    Serial.begin(115200);

    wifi.connect("MinhaRede", "Senha123");
    // ou
    // wifi.criar_ap("MeuAP", "12345678");
}

void loop() {
    wifi.checkConnection();
}
```

---

## ✔️ Benefícios do Módulo

* Compatível com ESP8266 e ESP32.
* Fornece API simples para conexão ou criação de AP.
* Mantém o firmware robusto com checagem periódica.
* Integrado ao sistema de depuração do projeto.
* Pode ser facilmente expandido para:

  * Reconexão automática personalizada.
  * Modo STA + AP simultâneo.
  * Configuração via Web Portal.

---

## 📝 Resumo

O módulo **WiFiManager** centraliza o gerenciamento de WiFi no projeto, oferecendo funções de conexão, criação de AP e monitoramento. Ele simplifica a lógica de rede do firmware e mantém o dispositivo sempre acessível ou conectado, permitindo futuras expansões como portais de configuração e reconexão automática inteligente.
