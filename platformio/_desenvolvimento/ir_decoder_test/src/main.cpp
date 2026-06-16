/*
 * AC_Universal_HA_EEPROM.ino
 * Versão Final Simplificada - Compatível com ESP32 e ESP8266
 * Funcionalidades: 
 * - Auto-restauração / Salvamento do emparelhamento na EEPROM
 * - Controle do AC via Monitor Serial (Comando de envio)
 * - Escuta em tempo real dos códigos HEX do controle físico para atualização do Home Assistant
 */

 /*
    ESP8266:
    ========
    GPIO5 --> BC7215A TX
    GPIO16 -> BC7215A RX
    GPIO14 -> BC7215A MOD
    GPIO4 --> BC7215A BUSY
    3.3V ---> BC7215A VCC


    ESP32:
    ======
    GPIO25 -> BC7215A TX
    GPIO33 -> BC7215A RX
    GPIO27 -> BC7215A MOD
    GPIO26 -> BC7215A BUSY
    3.3V ---> BC7215A VCC
 */

#include <Arduino.h>
#include <EEPROM.h>
#include <bc7215.h>
#include <bc7215ac.h>
#include "ctrl_debug.h"
#include <PubSubClient.h>

#if defined(ESP32)
  #include <WiFi.h>     
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif



// Definições de Pinos
const int MOD_PIN = 27;         
const int BUSY_PIN = 26; 

// --- CONFIGURAÇÕES DE REDE ---
const char* ssid     = "TPLINK";
const char* password = "gregorio@2012";
const char* mqtt_server = "192.168.0.5"; 
const char* mqtt_user   = "";      
const char* mqtt_pass   = "";    

// --- TÓPICOS MQTT (Home Assistant Climate) --- (DEVEM SER PASSADO PARA O MOULO ac-card DO HA, no arquivo my_cards/js/globals.js)
#define MQTT_TOPIC_POWER_STATE "homeassistant/escritorio/ac/power/state"
#define MQTT_TOPIC_MODE_STATE  "homeassistant/escritorio/ac/mode/state"
#define MQTT_TOPIC_TEMP_STATE  "homeassistant/escritorio/ac/temp/state"
#define MQTT_TOPIC_FAN_STATE   "homeassistant/escritorio/ac/fan/state"

WiFiClient     espClient;
PubSubClient   mqttClient(espClient);

// Compatibilidade de Hardware Serial para ESP32 e ESP8266
#if defined(ESP32)
  HardwareSerial bc7215Serial(1);        
#elif defined(ESP8266)
  // No ESP8266 a Serial1 é usada por padrão. Ajuste os pinos no seu circuito físico se necessário.
  #define bc7215Serial Serial1 
#endif

BC7215         bc7215Board(bc7215Serial, MOD_PIN, BUSY_PIN);
BC7215AC       ac(bc7215Board);        

bool paired = false;
String lastPowerState = "OFF"; // Estado inicial do AC, usado para evitar publicações duplicadas no MQTT

// Estruturas de dados globais para gerenciamento da EEPROM
bc7215FormatPkt_t  irFormat;
bc7215DataMaxPkt_t irData;
const uint32_t     EEPROM_MAGIC_NUMBER = 0xBC7215A0; // Identificador de dados válidos salvos


// 1. FUNÇÃO DE CALLBACK (Onde as mensagens chegam)
void callback_mqtt_rx(char* topic, byte* payload, unsigned int length) {
  
  // Converte o payload (array de bytes) em uma String amigável
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  } 
 
  uint8_t temp = 25; // Valor padrão para temperatura, pode ser atualizado com base no comando recebido
  String power = "OFF"; // Valor padrão para power, pode ser atualizado com base no comando recebido

  if (String(topic) == MQTT_TOPIC_MODE_STATE) {    
    imprimeln("Comando MQTT recebido para MODE: " + messageTemp);
  }
  else if (String(topic) == MQTT_TOPIC_TEMP_STATE && messageTemp.toInt() >= 16 && messageTemp.toInt() <= 30 && lastPowerState == "ON") {
    unsigned long startTime = millis();
    temp = messageTemp.toInt();
    bc7215Board.setTx();
    delay(50);   
    ac.setTo(temp, 5, 4, 4); // Actualiza a temperatura do AC e mantem modo e fan como "Keep" (5 e 4 são os códigos para manter o estado atual)
    while (ac.isBusy() && (millis() - startTime < 3000)){
        delay(10);
    }
    bc7215Board.setRx();
  }
  else if (String(topic) == MQTT_TOPIC_FAN_STATE) {
    imprimeln("Comando MQTT recebido para FAN: " + messageTemp);
  } 
  else if (String(topic) == MQTT_TOPIC_POWER_STATE) {
    unsigned long startTime = millis();
    lastPowerState = messageTemp; // Atualiza o estado do power com base no comando recebido
    if (messageTemp == "ON") {
        bc7215Board.setTx();
        delay(50);        
        ac.setTo(temp, 5, 4, 4); // Liga o AC com a ultima temeratura conhecida e mantem modo e fan como "Keep" (5 e 4 são os códigos para manter o estado atual)
        while (ac.isBusy() && (millis() - startTime < 3000)){
            delay(10);
        }
        bc7215Board.setRx();
    }
    else if (messageTemp == "OFF") {
        bc7215Board.setTx();
        delay(50);
        ac.off();
        while (ac.isBusy() && (millis() - startTime < 3000)){
            delay(10);
        }
        bc7215Board.setRx();
    }
  }
  
} // Fim da função de callback MQTT



// Funções de Inicialização de Rede
void setup_wifi() {
    delay(10);
    imprimeln();
    imprime(F("Conectando em ")); 
    imprimeln(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        imprime(F("."));
    }

    imprimeln(F("\n[WiFi] Conectado com sucesso!"));
    imprime(F("[WiFi] IP obtido: ")); 
    imprimeln(WiFi.localIP());
}

// Função para reconectar ao MQTT Broker
void reconnectMQTT() {
    while (!mqttClient.connected()) {
        imprime(F("[MQTT] Tentando conectar ao Broker..."));
        // Cria um ID único para o cliente MQTT baseado no endereço MAC
        String clientID = "ESP32-TCL-AC-" + String(random(0xffff), HEX);
        
        if (mqttClient.connect(clientID.c_str(), mqtt_user, mqtt_pass)) {
            imprimeln(F("Conectado!"));
            // Se você quiser receber comandos do HA para o ESP, adicione o subscribe aqui:            
            mqttClient.subscribe(MQTT_TOPIC_POWER_STATE);
            mqttClient.subscribe(MQTT_TOPIC_MODE_STATE);
            mqttClient.subscribe(MQTT_TOPIC_TEMP_STATE);
            mqttClient.subscribe(MQTT_TOPIC_FAN_STATE);
        } else {
            imprime(F("Falha, rc=")); 
            imprime(mqttClient.state());
            imprimeln(F(" Tentando novamente em 5 segundos."));
            delay(5000);
        }
    }
}


// Função para gravar os dados atuais de emparelhamento na EEPROM
void salvarDadosEEPROM() {
    int endereco = 0;
    
    EEPROM.put(endereco, EEPROM_MAGIC_NUMBER);
    endereco += sizeof(EEPROM_MAGIC_NUMBER);
    
    EEPROM.put(endereco, *ac.getFormatPkt());
    endereco += sizeof(bc7215FormatPkt_t);
    
    EEPROM.put(endereco, *((bc7215DataMaxPkt_t*)ac.getDataPkt()));
    
    EEPROM.commit(); 
    imprimeln(F(">>> Dados de emparelhamento salvos com sucesso na EEPROM!"));
}

// Função para ler e inicializar o AC usando dados gravados na EEPROM
bool carregarDadosEEPROM() {
    int endereco = 0;
    uint32_t magicCheck = 0;
    
    EEPROM.get(endereco, magicCheck);
    if (magicCheck != EEPROM_MAGIC_NUMBER) {
        return false; // Memória limpa ou inválida
    }
    
    endereco += sizeof(EEPROM_MAGIC_NUMBER);
    EEPROM.get(endereco, irFormat);
    
    endereco += sizeof(bc7215FormatPkt_t);
    EEPROM.get(endereco, irData);
    
    // Tenta inicializar os parâmetros do AC recuperados
    if (ac.init(irData, irFormat)) {
        return true;
    }
    
    return false;
}


struct ACState {
    bool power;
    uint8_t temp;
    String mode;    
};

ACState decodificarTCL(uint8_t* bytes) {
    
    ACState estado;

    // 1. Aplica a fórmula matemática que você descobriu para a temperatura
    estado.temp = 31 - bytes[7]; 

    // 2. Decodifica o estado do power e do modo com base nos bytes capturados
    uint8_t bytePower = irData.data[5];    
    switch (bytePower) {        
        case 0x20: estado.power = false; break;   
        case 0x24: estado.power = true; break;               
        default:   estado.power = false; break;
    }
    
    // 3. Decodifica o modo de operação com base no byte específico
    uint8_t byteModo = irData.data[6];
    switch (byteModo) {
        case 0x01: estado.mode = "HEATING"; break;
        case 0x02: estado.mode = "DRY"; break;
        case 0x03: estado.mode = "COOLING"; break;         
        case 0x07: estado.mode = "FAN"; break;
        case 0x08: estado.mode = "AUTO"; break;  
        case 0x20: estado.power = false; break;   
        case 0x24: estado.power = true; break;               
        default:   estado.mode = "UNKNOWN"; break;
    }
    
    return estado;
}



void setup() {
    Serial.begin(115200);                                 
    
    // Calcula o tamanho dinâmico e inicializa a EEPROM
    int tamanhoEEPROM = sizeof(EEPROM_MAGIC_NUMBER) + sizeof(bc7215FormatPkt_t) + sizeof(bc7215DataMaxPkt_t);
    EEPROM.begin(tamanhoEEPROM);

    setup_wifi();
    mqttClient.setServer(mqtt_server, 1883);
    mqttClient.setCallback(callback_mqtt_rx); // Define a função de callback para mensagens MQTT recebidas

#if defined(ESP32)
    bc7215Serial.begin(19200, SERIAL_8N2, 25, 33); // RX=GPIO25, TX=GPIO33
#elif defined(ESP8266)
    bc7215Serial.begin(19200, SERIAL_8N2); 
#endif
    
    // Configura o BC7215 para modo de recepção e inicializa o AC com a configuração padrão (Celsius)   
    bc7215Board.setRx(); 
    delay(50);
    ac.setCelsius(); 
    
    imprimeln(F("\n--- Verificando Memória EEPROM ---"));
    if (carregarDadosEEPROM()) {
        imprimeln(F(">>> Sucesso: Configuração restaurada! Pronto para operar."));
        imprimeln(F("Aguardando comandos ou sinais do controle físico..."));
        paired = true;
    } else {
        imprimeln(F("Nenhum dado prévio encontrado. Iniciando modo de emparelhamento obrigatório..."));
        imprimeln(F("Configure o controle remoto do AC para: <Modo Refrigeração (Cool), 25°C>"));
        imprimeln(F("Aponte para o receptor e pressione o botão <Controle de Fan (Velocidade)>..."));
        ac.startCapture();
    }
}

void loop() {

    // Garante estabilidade das conexões MQTT e Wi-Fi
    if (WiFi.status() != WL_CONNECTED) {
        setup_wifi();
    }
    if (!mqttClient.connected()) {
        reconnectMQTT();
    }

    mqttClient.loop(); // Processa a pilha MQTT ativa

    // 1. Passo: Aguardar o emparelhamento inicial se não houver dados na EEPROM
    if (!paired) {
        if (ac.signalCaptured()) {
            ac.stopCapture();

            if (ac.init()) {
                imprimeln(F("\n*** EMPARELHAMENTO COM SUCESSO! ***"));
                
                // Grava na memória para não precisar re-emparelhar no próximo boot
                salvarDadosEEPROM();

                imprimeln(F("Pronto para uso! Formato de comandos: temperatura,modo,ventilação,tecla (Ex: 24,1,2,0)\n"));
                paired = true;
            } else {
                imprimeln(F("Falha ao inicializar com o sinal recebido. Tentando novamente..."));
                ac.startCapture();
            }
        }
    } 
    // 2. Passo: Sistema operacional e pronto para uso
    else {
        
        // --- ESCUTA DO CONTROLE REMOTO FÍSICO (Para enviar ao Home Assistant) ---
        // Usamos dataReady() diretamente na placa base para capturar dados brutos (RAW hex)
        if (bc7215Board.dataReady()) {
            word len = bc7215Board.dpketSize();
            
            if (len <= sizeof(irData)) {
                
                bc7215Board.getData(irData); // Extrai os dados puros recebidos
                
                imprimeln(F("\n===== Sinal Físico Capturado (Para Home Assistant) ====="));
                Serial.println("Tamanho: " + String(irData.bitLen) + " bits");
                imprimeln(F("Payload RAW (HEX): "));
                
                String payloadHex = "";
                for (int i = 0; i < len - 2; i++) {
                    if (irData.data[i] < 0x10) Serial.print("0"); // Alinhamento estético do zero à esquerda
                    Serial.print(irData.data[i], HEX);
                    imprime(' ');                    
                    payloadHex += (irData.data[i] < 0x10 ? "0" : "") + String(irData.data[i], HEX);
                }
                imprimeln();                
                imprimeln(F("========================================================\n"));                
               
                ACState estado = decodificarTCL(irData.data); 
                if(estado.mode != "UNKNOWN" && (estado.temp >= 16 && estado.temp <= 30) ) {
                   Serial.printf("POWER: %s\n", estado.power ? "ON" : "OFF");
                   Serial.printf("TEMPERATURA: %d°C\n", estado.temp);
                   Serial.printf("MODO: %s\n", estado.mode.c_str());

                    lastPowerState = estado.power ? "ON" : "OFF"; // Atualiza o estado do power com base no sinal físico recebido
                    mqttClient.publish(MQTT_TOPIC_POWER_STATE, estado.power ? "ON" : "OFF", true);
                    mqttClient.publish(MQTT_TOPIC_TEMP_STATE, String(estado.temp).c_str(), true);
                }  
                                
            }
        }

        // --- ENVIO DE COMANDOS VIA MONITOR SERIAL ---
        /*if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();

            unsigned long startTime = millis();

            if (input.length() > 0) {

               if(input == "-"){
                    bc7215Board.setTx();
                    delay(50);
                    ac.setTo(24, 1, 2, 0);
                    while (ac.isBusy() && (millis() - startTime < 3000)) {
                        delay(10);
                    }
                    bc7215Board.setRx();
                }
                                                                
                
            }// if (input.length() > 0)
        }*/
    }
    delay(50);
}