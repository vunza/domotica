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

    // Callback para recepção de dados esp-now
    espnow.onReceive([](const uint8_t *mac, const uint8_t *data, int len){
        // Passa dados recebidos para a estructura
        memcpy(&dados, data, sizeof(EspNowData));

        // Checa e processa o tipo de mensagens
        if( strcmp(dados.msg_type, "DATA") == 0 ) {   

            
        }
        else if( strcmp(dados.msg_type, "NORMAL_MODE") == 0 ) {   
            ESP.restart(); // Rinicia o ESP para iniciar o modo e operação normal            
        }
        else if( strcmp(dados.msg_type, "OTA_MODE") == 0 ) {   
            // TODO: Atribuir nome sugestivo.
            // TODO: Checar se a mensagem destina-se ao dispositivo.
            // Credeciais de acesso a rede WiFi
            const char* ssid = "FIRMEWARE";
            const char* password = "123456789";

            // Rede WiFi            
            //wifiManager.connect(ssid, password);
            wifiManager.criar_ap(ssid, password);

            // Iniciar servidor web
            webServer.begin();

            // Inicializar OTA elegante
            ElengantOTA::begin(&servidorHTTP);       
        }

    });



    // Callback para envio de dados esp-now
    espnow.onSend([](const uint8_t *mac, bool ok){
        
    });

    
}


////////////////////
// loop principal //
////////////////////
void loop() {
    
    // Envia um "ping" a cada 5 segundos
    static unsigned long lastPing = 0;
    if (millis() - lastPing > 5000) {
        lastPing = millis();  
        
        char mac[18]; // static para persistir após a função
        String macStr = WiFi.macAddress();
        macStr.toCharArray(mac, 18);

        // Simular dados dos sensores
        static float volt = 0.0;
        static float curr = 0.0;
        static float temp = 0.0;
        static float humi = 0.0;

        if(volt >= 14 ) volt = 0.0;
        if(curr >= 3 ) curr = 0.0;
        if(temp >= 50 ) temp = 0.0;
        if(humi >= 100 ) humi = 0.0;

        String(++volt, 2).toCharArray(dados.u1_voltage, sizeof(dados.u1_voltage));
        String(++volt/5, 2).toCharArray(dados.u1_current, sizeof(dados.u1_current));
        String(++temp, 2).toCharArray(dados.u1_temperature, sizeof(dados.u1_temperature));
        String(++temp*2).toCharArray(dados.u1_humidity, sizeof(dados.u1_humidity));  
        
        //sizeof(dados);
      
        strcpy(dados.mac_source, mac);
        strcpy(dados.msg_type, "DATA");
        espnow.send(broadcastMac, (uint8_t*)&dados, sizeof(EspNowData));
    } 

}

