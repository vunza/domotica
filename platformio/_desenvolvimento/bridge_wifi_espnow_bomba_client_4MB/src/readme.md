# Documentação Resumida do Projeto

Este documento apresenta uma visão geral e resumida de todos os módulos do projeto, incluindo depuração, OTA, sensores, display, WiFi, servidor web e lógica principal.

---

## 🔧 **1. Módulo de Depuração — `ctrl_debug.h`**

Ativa ou desativa mensagens via Serial conforme a macro `ACTIVAR_SERIAL_DEBUG` definida no `platformio.ini`.

* `imprime(x)` → `Serial.print(x)`
* `imprimeln(x)` → `Serial.println(x)`
* Sem debug: ambas funções ficam vazias.

---

## 📦 **2. Cabeçalho Global — `headers.h`**

Agrupa todos os módulos do projeto:

* `device`, `wifi_manager`, `async_elegant_ota`, `web_server`
* Sensores: INA226, DHT11
* Display LCD
* Sistema de debug

Permite incluir tudo com:

```cpp
#include <headers.h>
```

---

## 🔄 **3. OTA Elegante — `async_elegant_ota`**

Simplifica a ativação do **AsyncElegantOTA**:

* `ElengantOTA::begin(server);` inicia o serviço OTA.
* Exibe mensagem “OTA Inicializada!” se debug estiver ativo.

---

## ⚙️ **4. Núcleo do Dispositivo — `core/device`**

Gerencia inicialização e execução do firmware.

* `initialize()` → configurações específicas ESP32/ESP8266 + verificação de hardware.
* `run()` → lógica principal (vazia para implementação futura).

---

## 🖥️ **5. Display LCD — `lcd_display`**

Interface para displays I2C usando LiquidCrystal_I2C.

* `begin()` → inicia o LCD e o barramento I2C.
* `showReadings(corrente, tensao)` → exibe valores formatados no display.

---

## 📄 **6. Builder JSON — `json_builder`**

Construção leve e dinâmica de objetos JSON.

* Suporta: `String`, `float`, `int`.
* Não depende de ArduinoJSON.
* `build()` → retorna JSON final.

Exemplo:

```json
{"voltage":12.45,"current":123.4}
```

---

## 📡 **7. Gerenciamento de WiFi — `wifi_manager`**

Permite conectar à rede ou criar Access Point.

* `connect(ssid, password)` → conecta e configura reconexão automática.
* `criar_ap(ssid, password)` → cria AP.
* `checkConnection()` → monitora conexão a cada 10s.

---

## 🌡️ **8. Sensor de Temperatura/Umidade — `DHT11Sensor`**

Abstração do DHT11 usando biblioteca Adafruit.

* `begin()`
* `getTemperature()`
* `getHumidity()`

---

## ⚡ **9. Sensor INA226 — `INA226Sensor`**

Leituras de corrente e tensão via I2C.

* `begin()` → inicializa sensor.
* `update()` → atualiza valores internos.
* `readCurrent()` / `readVoltage()`

---

## 🌐 **10. Servidor Web — `web_server`**

Usa ESPAsyncWebServer + LittleFS.
Funcionalidades:

* Serve: `painel.html`, `ota.html`, `style.css`, `script.js`.
* API JSON `/api/sensores` retornando:

  ```json
  { "voltage": V, "current": A, "temperature": C, "humidity": % }
  ```
* Mensagens de debug durante inicialização.

---

## 🧠 **11. Arquivo Principal — `main.cpp`**

Integra todos os módulos.

### Fluxo geral:

1. Inicializa Serial
2. `device.initialize()`
3. Conecta WiFi
4. Inicia servidor Web
5. Inicia OTA
6. Inicia sensores (INA226, DHT11)
7. Inicia LCD
8. Loop principal:

   * Verifica WiFi
   * Atualiza sensores a cada 3 segundos
   * Exibe no LCD
   * Alimenta o WDT

### Variáveis globais:

* `g_voltage`, `g_current`, `g_temperature`, `g_humidity`
  → Atualizadas no loop e consumidas pelo servidor Web.

---

## 📝 **Resumo Final**

Este firmware implementa um sistema completo para ESP8266/ESP32 com:

* Monitoramento elétrico (INA226)
* Temperatura e umidade (DHT11)
* Interface LCD
* API REST em JSON
* Painel Web usando LittleFS
* OTA elegante assíncrono
* Sistema de debug configurável
* Arquitetura modular e organizada

Ideal para projetos IoT, automação, telemetria e monitoramento em tempo real.
