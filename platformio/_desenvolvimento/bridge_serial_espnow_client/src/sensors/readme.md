# Documentação do Módulo `sensors`

Este documento descreve os módulos de sensores utilizados no projeto, incluindo o sensor de temperatura/umidade **DHT11** e o sensor de corrente/tensão **INA226**. Ambos os módulos fornecem interfaces simples, encapsuladas e preparadas para uso com outros componentes do firmware.

---

# 🌡️ Módulo `DTH11Sensor`

Sensor de temperatura e umidade baseado no **DHT11** utilizando a biblioteca oficial da Adafruit.

## 📁 Arquivos

### **src/sensors/DTH11Sensor.h**

```cpp
#ifndef DHT11SENSOR_H
#define DHT11SENSOR_H

#include <Arduino.h>
#include <Adafruit_Sensor.h>  
#include <DHT.h>
#include <DHT_U.h>
#include "ctrl_debug.h"

class DHT11Sensor {
    public:
        DHT11Sensor(uint8_t pin, uint8_t type);
        void begin();
        float getTemperature();
        float getHumidity();

    private:
        DHT dht;
};

#endif
```

### **src/sensors/DTH11Sensor.cpp**

```cpp
#include "sensors/DTH11Sensor.h"

DHT11Sensor::DHT11Sensor(uint8_t pin, uint8_t type) : dht(pin, type) {}

void DHT11Sensor::begin() {
    dht.begin();
}

float DHT11Sensor::getTemperature() {
    return dht.readTemperature();
}

float DHT11Sensor::getHumidity() {
    return dht.readHumidity();
}
```

---

## 🧩 Objetivo do Módulo DHT11

Este módulo encapsula completamente a lógica necessária para:

* Inicializar o sensor.
* Ler temperatura em °C.
* Ler umidade relativa (%).

Ele abstrai a biblioteca da Adafruit e entrega funções simples para uso no firmware.

---

## ⚙️ Funcionamento Interno

### **Construtor**

Recebe:

* `pin` → pino de dados do sensor.
* `type` → normalmente `DHT11`, mas também pode ser `DHT22` se desejado.

### **begin()**

Inicializa a comunicação com o sensor.

### **getTemperature()**

Retorna a temperatura atual em graus Celsius.

### **getHumidity()**

Retorna a umidade relativa do ar em porcentagem.

---

## 🧪 Exemplo de Uso

```cpp
#include "sensors/DTH11Sensor.h"

DHT11Sensor dht(5, DHT11);

void setup() {
    Serial.begin(115200);
    dht.begin();
}

void loop() {
    Serial.println(dht.getTemperature());
    Serial.println(dht.getHumidity());
    delay(1000);
}
```

---

# ⚡ Módulo `INA226Sensor`

Sensor de tensão e corrente baseado no **INA226**, utilizando a biblioteca **INA226_WE**.

## 📁 Arquivos

### **src/sensors/INA226Sensor.h**

```cpp
#ifndef INA226_SENSOR_H
#define INA226_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <INA226_WE.h>
#include "ctrl_debug.h"

class INA226Sensor {
public:
    INA226Sensor(uint8_t address = 0x40);

    bool begin();                 
    float readCurrent();          
    float readVoltage();          
    void update();                

private:
    INA226_WE ina;
    float current_mA;
    float voltage_V;
};

#endif
```

### **src/sensors/INA226Sensor.cpp**

```cpp
#include "INA226Sensor.h"

INA226Sensor::INA226Sensor(uint8_t address): ina(address), current_mA(0), voltage_V(0) {}

bool INA226Sensor::begin() {
    Wire.begin();

    if(!ina.init()) {
        imprimeln(F("Erro ao inicializar INA226!"));
        return false;
    }

    return true;
}

void INA226Sensor::update() {
    ina.readAndClearFlags();
    current_mA = ina.getCurrent_mA();
    voltage_V = ina.getBusVoltage_V();
}

float INA226Sensor::readCurrent() {
    return current_mA;
}

float INA226Sensor::readVoltage() {
    return voltage_V;
}
```

---

## 🧩 Objetivo do Módulo INA226

Este módulo oferece uma interface simples para:

* Inicializar o sensor INA226.
* Ler corrente elétrica (mA).
* Ler tensão do barramento (V).
* Atualizar as medições com baixa latência.

Ele simplifica o uso da biblioteca `INA226_WE`, encapsulando toda a lógica necessária.

---

## ⚙️ Funcionamento Interno

### **Construtor**

Recebe o endereço I2C do sensor (default `0x40`).

### **begin()**

* Inicializa o barramento I2C.
* Tenta iniciar o sensor com `ina.init()`.
* Em caso de erro, imprime mensagem via Serial (se debug estiver ativo).

### **update()**

* Lê os valores mais recentes de corrente e tensão.
* Os dados são armazenados internamente para leitura rápida.

### **readCurrent()**

Retorna o valor da corrente em **mA**.

### **readVoltage()**

Retorna o valor da tensão em **volts**.

---

## 🧪 Exemplo de Uso

```cpp
#include "sensors/INA226Sensor.h"

INA226Sensor ina;

void setup() {
    Serial.begin(115200);

    if(!ina.begin()) {
        Serial.println("Falha na inicialização do INA226");
        while(true);
    }
}

void loop() {
    ina.update();

    Serial.println(ina.readCurrent());
    Serial.println(ina.readVoltage());
    delay(500);
}
```

---

# ✔️ Benefícios dos Módulos de Sensores

* APIs simples e intuitivas.
* Baixo acoplamento com o restante do firmware.
* Prontos para uso com displays, JSON e WebServer.
* Melhor organização e manutenção do código.

---

# 📝 Resumo

O módulo **sensors** fornece implementações completas para leitura de temperatura/umidade (DHT11) e corrente/tensão (INA226). Eles encapsulam toda a complexidade das bibliotecas externas e deixam a integração extremamente simples para o restante do projeto.
