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

// Credeciais de acesso a rede WiFi
const char* ssid = "TPLINK";
const char* password = "gregorio@2012";

//EspNowManager espnow;
//EspNowData dados;
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

    // Rede WiFi
    wifiManager.connect(ssid, password);

    // Iniciar servidor web
    //webServer.begin();

    // Inicializar OTA elegante
    ElengantOTA::begin(&servidorHTTP); 


    // Iniciar ESP-NOW
    /*espnow.begin(1);

    // broadcast (tudo que enviar vai para todos)
    espnow.addPeer(broadcastMac);

    espnow.onReceive([](const uint8_t *mac, const uint8_t *data, int len){                  
        
        memcpy(&dados, data, sizeof(EspNowData));

        if( strcmp(dados.msg_type, "DATA") == 0 ) {   

            // Simula dados dos sensores, envia para HA via Serial
            
            float g_voltage = 0.0;
            float g_current = 0.0;
            float g_temperature = 0.0;
            float g_humidity = 0.0;

            if(g_voltage > 14.0) {
            g_voltage = 0.0;
            g_current = 0.0;
            }
            g_voltage += 1.0;
            g_current = g_voltage * 0.5;            

            if(g_temperature > 50.0) {
            g_temperature = 0.0;
            g_humidity = 0.0;
            }
            g_temperature += 1.0;
            g_humidity += 1.0;
            
            JsonBuilder json;
            json.add("ups1_current", g_current, 2);
            json.add("ups1_voltage", g_voltage, 2);
            json.add("ups1_temperature", g_temperature, 2);
            json.add("ups1_humidity", g_humidity, 2);
            json.add("device", "UPS1");

            String dados = json.build();
            Serial.println(dados); 
        }
        
    });


    espnow.onSend([](const uint8_t *mac, bool ok){
        
    });*/    
    
    
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
        
        
        if (recebido == "STATUS?") {
            
        }
        else if (recebido == "MODE_OTA") {
            
        }

    }

   
    // Envia um "ping" a cada 5 segundos
    static unsigned long lastPing = 0;
    if (millis() - lastPing > 5000) {   
        lastPing = millis();   

        
    } 

}


