# Documentação do Módulo `core`

Este documento descreve a estrutura, funcionamento e uso do módulo **core**, responsável pela inicialização e execução lógica principal do dispositivo.

---

## 📁 Arquivos do Módulo

### **src/core/device.h**

```cpp
#pragma once

#include "ctrl_debug.h"

class Device {
public:
    void initialize();
    void run();
private:
    bool checkHardware();
};
```

### **src/core/device.cpp**

```cpp
#include "device.h"

void Device::initialize() {   

    #ifdef ESP32
        // Inicializações específicas do ESP32
    #elif defined(ESP8266)
        // Inicializações específicas do ESP8266
    #endif
    
    if(!checkHardware()) {
        imprimeln(F("Falha na verificação do hardware!"));
    }
}

bool Device::checkHardware() {
    // Verificações comuns de hardware
    return true;
}

void Device::run() {
    // Lógica principal do dispositivo
}
```

---

## 🧩 Objetivo do Módulo

O módulo **core** fornece uma classe central chamada **Device**, responsável por:

* Executar rotinas de inicialização do hardware.
* Validar o estado básico do dispositivo.
* Executar a lógica principal da aplicação.

Esse módulo funciona como o "coração" do firmware, concentrando os passos essenciais de inicialização e operação.

---

## ⚙️ Funcionamento Interno

### **1. Device::initialize()**

Executa as etapas iniciais antes da execução normal do firmware.

Inclui:

* Rotinas específicas para ESP32 ou ESP8266 (usando diretivas de pré‑processador).
* Chamada à função `checkHardware()` para validar o estado básico.
* Emissão de mensagem de depuração caso o hardware falhe:

  ```cpp
  imprimeln(F("Falha na verificação do hardware!"));
  ```

  (Somente exibida se `ACTIVAR_SERIAL_DEBUG` estiver habilitado.)

### **2. Device::checkHardware()**

Responsável por verificar condições básicas do hardware.

* No momento, retorna sempre `true`.
* Projetado para ser expandido com verificações reais (sensores, alimentação, periféricos, etc.).

### **3. Device::run()**

Contém a lógica principal que deve ser executada continuamente.

* Pode ser chamada dentro do `loop()` do Arduino.
* Deve futuramente incluir o processamento central do firmware.

---

## 🧪 Exemplo de Uso do Módulo `core`

```cpp
#include "core/device.h"

Device device;

void setup() {
    Serial.begin(115200);
    device.initialize();
}

void loop() {
    device.run();
}
```

---

## ✔️ Benefícios do Módulo

* Organiza o código em uma estrutura clara e modular.
* Permite expandir facilmente verificações de hardware.
* Mantém a lógica principal separada da inicialização.
* Centraliza a lógica do dispositivo de forma reutilizável.

---

## 📝 Resumo

O módulo **core** implementa a classe `Device`, que gerencia a inicialização e execução contínua do firmware. Ele serve como um ponto central de organização e evolução da lógica do dispositivo, mantendo o projeto estruturado, limpo e fácil de expandir.
