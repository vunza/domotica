#include <Arduino.h>
#include <headers.h>


/*
          PIN_RELAY   PIN_BUTTON    PIN_LED   MCU
--------------------------------------------------------  
SLAMPHER  GPIO12      GPIO0         GPIO13    ESP8285 1M
BASIC     GPIO12      GPIO0         GPIO13    ESP8266 1M
ESP12E    GPIO12      GPIO0         GPIO2     ESP8266 4M
T4EU1C    GPIO12      GPIO0         GPIO13    ESP8285 1M 
T0EU1C    GPIO12      GPIO0         GPIO13    ESP8285 1M 
SHELLY1L  GPIO5       GPIO4         GPIO0     ESP8285 1M 
M51C      GPIO23      GPIO0         GPIO19    ESP32   4M

*/


float g_voltage = 0.0;
float g_current = 0.0;
float g_temperature = 0.0;
float g_humidity = 0.0;


WiFiManager wifiManager;

// Servidor Web
AsyncWebServer servidorHTTP(80);
WebServer webServer(&servidorHTTP);

unsigned long lastUpdate = 0;

EspNowManager espnow;
EspNowData dados;
uint8_t broadcastMac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};



//////////////////////////
// Configuração inicial //
//////////////////////////
void setup() {
    // Iniciar comunicação serial
    Serial.begin(115200);    

    while (!Serial) {
        ; // Aguarda porta serial iniciar (normal no ESP32 USB)
    }

    imprimeln();
    imprimeln(F("Iniciando..."));   

    
    // Iniciar ESP-NOW    
    espnow.begin(); // canal = 1 e modo = WIFI_STA por defeito

    // broadcast (tudo que enviar vai para todos)
    espnow.addPeer(broadcastMac); // canal = 1 por defeito
    

    // Callback de recepção ESP-NOW
    espnow.onReceive([](const uint8_t *mac, const uint8_t *data, int len){
        
        // Passa dados recebidos para a estructura
        memcpy(&dados, data, sizeof(EspNowData));

        // Checa e processa o tipo de mensagens
        if( strcmp(dados.msg_type, "DATA") == 0 ) {   

            // Converte em json os dados recebidos dos sensores           
            JsonBuilder json;
            json.add("ups1_current", dados.u1_current, 2);
            json.add("ups1_voltage", dados.u1_voltage, 2);
            json.add("ups1_temperature", dados.u1_temperature, 2);
            json.add("ups1_humidity", dados.u1_humidity);   
            String dados = json.build();

            // Evia pela serial os dados processados
            Serial.println(dados); 
        }
    });


    // Callback de envio ESP-NOW
    espnow.onSend([](const uint8_t *mac, bool ok){
        
    });

    
}


////////////////////
// loop principal //
////////////////////
void loop() {
    // NOTA: AsyncElegantOTA não precisa de nada no loop     

    // Se recebeu dados pela Serial
    if (Serial.available()) {

        String recebido = Serial.readStringUntil('\n');
        recebido.trim();        
        
        if (recebido == "NORMAL_MODE") {
            ESP.restart(); // Rinicia o ESP para iniciar o modo e operação normal
        }
        else if (recebido == "OTA_MODE") {

            // Credeciais de acesso a rede WiFi
            const char* ssid = "TPLINK";
            const char* password = "gregorio@2012";

            // Rede WiFi
            //wifiManager.criar_ap(ssid, password);
            wifiManager.connect(ssid, password);

            // Iniciar servidor web
            webServer.begin();

            // Inicializar OTA elegante
            ElengantOTA::begin(&servidorHTTP);       
        }

    }
   
    // Envia um "ping" a cada 5 segundos
    static unsigned long lastPing = 0;
    if (millis() - lastPing > 10000) {   
        lastPing = millis();  
       
    }

}

