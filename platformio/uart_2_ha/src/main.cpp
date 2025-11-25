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

// Variáveis globais para cache dos sensores
float g_voltage = 0.0;
float g_current = 0.0;
float g_temperature = 0.0;
float g_humidity = 0.0;
 
// Credeciais de acesso a rede WiFi
const char* ssid = "TPLINK";
const char* password = "gregorio@2012";



//////////////////////////
// Configuração inicial //
//////////////////////////
void setup() {
    // Iniciar comunicação serial
    Serial.begin(115200);    // Monitor Serial   

    while (!Serial) {
        ; // Aguarda porta serial iniciar (normal no ESP32 USB)
    }

    imprimeln();
    imprimeln(F("Iniciando..."));   
    
    // Criar ponto de acesso WiFi
    wifiManager.connect(ssid, password);

    // Iniciar servidor web
    webServer.begin();

    // Inicializar OTA elegante
    ElengantOTA::begin(&servidorHTTP); 
    
}


////////////////////
// loop principal //
////////////////////
void loop() {
    // NOTA: AsyncElegantOTA não precisa de nada no loop      

    // Se recebeu dados do Linux
    if (Serial.available()) {
        String recebido = Serial.readStringUntil('\n');
        recebido.trim();

        Serial.print("Recebido do Linux: ");
        Serial.println(recebido);

        // Resposta automática
        Serial.print("Eco: ");
        Serial.println(recebido);

        if(g_voltage > 14.0) {
           g_voltage = 0.0;
           g_current = 0.0;
        }
        g_voltage += 1.0;
        g_current = g_voltage * 0.5;
        yield();

        if(g_temperature > 50.0) {
           g_temperature = 0.0;
           g_humidity = 0.0;
        }
        g_temperature += 1.0;
        g_humidity += 1.0;
        yield();
    }

    // Envia um "ping" a cada 5 segundos
    static unsigned long lastPing = 0;
    if (millis() - lastPing > 5000) {
        lastPing = millis();
        Serial.println("Ping do ESP32");
    }    
}

