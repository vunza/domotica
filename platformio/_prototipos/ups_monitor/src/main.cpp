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


Device device;
WiFiManager wifiManager;

// Servidor Web
AsyncWebServer servidorHTTP(80);
WebServer webServer(&servidorHTTP);

// Sensor INA226
#define INA226_I2C_ADDRESS 0x40 
INA226Sensor sensorINA226(INA226_I2C_ADDRESS);


// Define seu LCD (endereço e tamanho)
LCDDisplay displayLCD(0x27, 16, 2);


// DHT11 temperature and humidity sensor
#ifdef ESP32
  #define DHTPIN 4   
#elif defined(ESP8266)
  #define DHTPIN 12      
#endif    
   
#define DHTTYPE DHT11 
DHT11Sensor sensorDHT11(DHTPIN, DHTTYPE);


// Variáveis globais para cache dos sensores
float g_voltage = 0.0;
float g_current = 0.0;
float g_temperature = 0.0;
float g_humidity = 0.0;

unsigned long lastUpdate = 0;
 

// Credeciais de acesso a rede WiFi
const char* ssid = "TPLINK";
const char* password = "gregorio@2012";




//////////////////////////
// Configuração inicial //
//////////////////////////
void setup() {
    // Iniciar comunicação serial
    Serial.begin(115200);
    delay(1000);
    imprimeln();
    imprimeln(F("Iniciando..."));

    #ifdef ESP32
       Wire.begin(21, 22);  // SDA=21, SCL=22 no ESP32
    #elif defined(ESP8266)
        Wire.begin(4, 5);      // SDA=4, SCL=5 no D1 mini  
    #endif  
    
    Wire.setClock(100000); // 100 kHz → máximo de estabilidade com 2 I2C

    // Inicializar dispositivo
    device.initialize();
    
    // Conectar WiFi
    wifiManager.connect(ssid, password); 
    
    // Criar ponto de acesso WiFi
    //wifiManager.criar_ap("ESP8266", "123456789");

    // Iniciar servidor web
    webServer.begin();

    // Inicializar OTA elegante
    ElengantOTA::begin(&servidorHTTP); 
    
    // Inicializar sensor INA226
    sensorINA226.begin(); 
    
    // Inicializa display LCD
    displayLCD.begin();
    
}


////////////////////
// loop principal //
////////////////////
void loop() {
    // NOTA: AsyncElegantOTA não precisa de nada no loop

    // Verificar conexão WiFi
    #ifdef ESP32
        if (WiFi.getMode() == WIFI_MODE_STA || WiFi.getMode() == WIFI_MODE_APSTA){
            wifiManager.checkConnection();
        }
    #elif defined(ESP8266)
        if (WiFi.getMode() == WIFI_STA || WiFi.getMode() == WIFI_AP_STA){
            wifiManager.checkConnection();
        }        
    #endif  
 
    
    
    if (millis() - lastUpdate > 3000) {
        lastUpdate = millis();

        sensorINA226.update();
        yield(); // importante

        g_voltage = sensorINA226.readVoltage();
        g_current = sensorINA226.readCurrent();
        yield();

        g_temperature = sensorDHT11.getTemperature();
        g_humidity = sensorDHT11.getHumidity();
        yield();

        displayLCD.showReadings(g_current, g_voltage);
        yield();


        // Visualiza dadod no terminal
        // Serial.print("\nCorrente (mA): ");
        // Serial.print(corrente);
        // Serial.print(" -- Tensão (V): ");
        // Serial.print(tensao);


        // // Lê dados do sensor DHT11
        // float temperatura = sensorDHT11.getTemperature();
        // float humidade    = sensorDHT11.getHumidity();

        // if (isnan(temperatura) || isnan(humidade)) {
        //     imprimeln(F("Falha ao ler o DHT11!"));    
        // } else {
        //     imprime(F(" -- Temperatura (C): "));
        //     imprime(temperatura);
        //     imprime(F(" -- Humidade (%): "));
        //     imprime(humidade);
        // }
    }
    
}

