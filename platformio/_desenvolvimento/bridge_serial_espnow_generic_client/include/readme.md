# Documentação dos arquivos cabeçlhos

Este documento descreve de forma clara e objetiva o uso dos arquivos `ctrl_debug.h` e `headers.h`, incluindo a explicação funcional e um exemplo completo de utilização.

---

## Arquivo: `include/ctrl_debug.h`

### **Definição automática via `platformio.ini`**

A macro `ACTIVAR_SERIAL_DEBUG` pode ser ativada diretamente no arquivo `platformio.ini`, evitando modificações no código-fonte. Exemplo:

```
build_flags =
    -DACTIVAR_SERIAL_DEBUG
```

Quando definida dessa forma, todos os módulos incluirão automaticamente suporte a mensagens de debug.

Este arquivo permite habilitar ou desabilitar mensagens de depuração (debug) via Serial sem alterar o restante do código da aplicação.

### **Funcionamento**

* Se a macro `ACTIVAR_SERIAL_DEBUG` estiver definida **antes** de incluir esse arquivo, as funções `imprime()` e `imprimeln()` irão chamar `Serial.print()` e `Serial.println()`.
* Se **não** estiver definida, as funções existirão mas serão "vazias" (não fazem nada), permitindo compilar o código sem remover prints manualmente.

### **Exemplo de ativação da depuração**

```cpp
#define ACTIVAR_SERIAL_DEBUG
#include "ctrl_debug.h"

void setup() {
    Serial.begin(115200);
    imprimeln("Sistema iniciado em modo debug!");
}
```

### **Exemplo sem depuração**

```cpp
#include "ctrl_debug.h"

void setup() {
    Serial.begin(115200);
    imprimeln("Isto não será impresso.");
}
```

---

## Arquivo: `include/headers.h`

Este arquivo funciona como um **agregador central** de todos os headers necessários para o projeto. Ele permite incluir apenas um arquivo no seu código principal.

### **Componentes incluídos**

* `core/device.h`: fornece funcionalidades centrais do dispositivo.
* `network/wifi_manager.h`: faz a gestão da conexão WiFi.
* `async_elegant_ota.h`: permite atualização OTA assíncrona.
* `web_server/web_server.h`: inicializa e gerencia o servidor web.
* `ctrl_debug.h`: ativa/desativa mensagens de debug.
* `sensors/INA226Sensor.h`: leitura de corrente/tensão INA226.
* `displays/lcd_display.h`: gestão do display LCD.
* `sensors/DTH11Sensor.h`: leitura de temperatura/umidade DHT11.

### **Uso típico**

Você pode incluir todos os módulos do sistema de forma simples:

```cpp
#include "headers.h"

Device device;
INA226Sensor powerSensor;
DTH11Sensor dht;
LCDDisplay lcd;

void setup() {
    Serial.begin(115200);

    // Inicializa módulos principais
    WiFiManager::connect("MinhaRede", "Senha123");
    AsyncElegantOTA.begin();
    WebServerManager::start();

    lcd.init();
    dht.begin();
    powerSensor.begin();

    imprimeln("Sistema inicializado com sucesso!");
}

void loop() {
    float temp = dht.getTemperature();
    float hum = dht.getHumidity();
    float current = powerSensor.getCurrent();

    lcd.print(String("T:") + temp + "C H:" + hum + "%");

    imprimeln(String("Corrente atual: ") + current + " mA");
}
```

---

## Resumo

* **`ctrl_debug.h`** simplifica a ativação/desativação de mensagens de depuração.
* **`headers.h`** centraliza todas as dependências do projeto e facilita a inclusão.
* O exemplo demonstra como inicializar sensores, OTA, WiFi e exibir dados.

Este documento pode ser ampliado caso você inclua novos módulos no projeto.
