# Documentação do Módulo `displays`

Este documento descreve o módulo **LCD Display**, responsável por exibir informações como corrente e tensão em um display LCD I2C baseado na biblioteca **LiquidCrystal_I2C**.

---

## 📁 Arquivos do Módulo

### **src/displays/lcd_display.h**

```cpp
#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "ctrl_debug.h"

class LCDDisplay {
public:
    LCDDisplay(uint8_t i2c_addr, uint8_t cols, uint8_t rows);

    void begin();
    void showReadings(float current_mA, float voltage_V);

private:
    LiquidCrystal_I2C lcd;
    uint8_t _cols;
    uint8_t _rows;
};

#endif
```

### **src/displays/lcd_display.cpp**

```cpp
#include "lcd_display.h"

LCDDisplay::LCDDisplay(uint8_t i2c_addr, uint8_t cols, uint8_t rows)
    : lcd(i2c_addr, cols, rows), _cols(cols), _rows(rows) {}


void LCDDisplay::begin() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    Wire.begin();
}


void LCDDisplay::showReadings(float current_mA, float voltage_V) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("I = ");
    lcd.print(current_mA);
    lcd.print(" mA");  

    lcd.setCursor(0, 1);
    lcd.print("V = ");
    lcd.print(voltage_V);
    lcd.print(" V");    
}
```

---

## 🧩 Objetivo do Módulo

O módulo **LCD Display** fornece uma interface simples e modular para exibir valores no display LCD via protocolo I2C.

Permite:

* Inicializar automaticamente o barramento I2C e o display.
* Exibir leituras de sensores como corrente e tensão.
* Manter o código organizado e sem repetições em outros módulos.

---

## ⚙️ Funcionamento Interno

### **1. Construtor**

```cpp
LCDDisplay(uint8_t i2c_addr, uint8_t cols, uint8_t rows)
```

Inicializa o objeto `LiquidCrystal_I2C` com:

* Endereço I2C do display.
* Número de colunas.
* Número de linhas.

Armazena as dimensões internamente.

### **2. begin()**

Prepara o display para uso:

* `lcd.init()` — inicializa o controlador LCD.
* `lcd.backlight()` — liga a luz de fundo.
* `lcd.clear()` — apaga o conteúdo da tela.
* `lcd.setCursor(0, 0)` — posiciona cursor inicial.
* `Wire.begin()` — inicializa I2C.

### **3. showReadings()**

Exibe os valores de corrente e tensão em duas linhas:

* Linha 1 → `I = <corrente> mA`
* Linha 2 → `V = <tensão> V`

Essa função limpa o display e reescreve os valores sempre que chamada.

---

## 🧪 Exemplo de Uso

```cpp
#include "displays/lcd_display.h"

LCDDisplay lcd(0x27, 16, 2); // Endereço I2C comum

void setup() {
    Serial.begin(115200);
    lcd.begin();
}

void loop() {
    float corrente = 123.4;
    float tensao = 12.56;

    lcd.showReadings(corrente, tensao);
    delay(1000);
}
```

---

## ✔️ Benefícios do Módulo

* Encapsula toda lógica do LCD em uma classe limpa.
* Facilita a troca do tipo de display caso necessário.
* Mantém o firmware modular e organizado.
* Integra-se naturalmente com sensores e outros módulos.

---

## 📝 Resumo

O módulo `displays` implementa a classe **LCDDisplay**, responsável por inicializar o display LCD via I2C e exibir valores de sensores. Ele simplifica o uso do display e mantém o projeto modular, limpo e fácil de expandir.
