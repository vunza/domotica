# 📊 Monitor de Sensores – Interface Web

Este projeto consiste em uma **interface web responsiva** para monitoramento em tempo real de sensores conectados a um microcontrolador (ESP8266/ESP32).  
Os dados são obtidos via API e representados através de **gauges gráficos**, além de indicadores de status e métricas derivadas (potência, uptime, etc).

---

## 🚀 **Funcionalidades**

- Exibição de **4 gauges semicirculares**:
  - 🔴 **Tensão (V)**
  - 🟢 **Corrente (mA)**
  - 🟠 **Temperatura (°C)**
  - 🔵 **Umidade (%)**

- Indicadores de:
  - **Status Normal / Atenção / Crítico**
  - **Potência instantânea (W)**
  - **Energia consumida (Wh)**
  - **Tempo de operação (uptime)**
  - **Última atualização**

- Design moderno, responsivo e leve (HTML + CSS + Canvas + JS)
- Atualização automática dos dados via `fetch()` a cada 2 segundos
- Botões de:
  - 🔄 **Atualização manual**
  - ⚙️ **Configurações / OTA**

---

## 📡 **Fonte dos Dados**

O frontend consome dados de uma API REST exposta pelo microcontrolador:

