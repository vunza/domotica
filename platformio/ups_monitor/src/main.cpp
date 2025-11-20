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
#define DHTPIN 2      // GPIO2 = D4 no ESP8266
#define DHTTYPE DHT11 // Pode usar DHT22 se quiser
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




//////////////
// setup() //
/////////////
void setup() {
    // Iniciar comunicação serial
    Serial.begin(115200);
    delay(1000);
    imprimeln();
    imprimeln(F("Iniciando..."));

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


////////////
// loop() //
////////////
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
    
    
    // Atualiza sensores a cada 2 segundos
    if (millis() - lastUpdate > 3000) {
      lastUpdate = millis();

      sensorINA226.update();
      g_voltage = sensorINA226.readVoltage();
      g_current = sensorINA226.readCurrent();

      g_temperature = sensorDHT11.getTemperature();
      g_humidity = sensorDHT11.getHumidity();
      
      // Vizualiza dados no visor LCD  
      displayLCD.showReadings(g_current, g_voltage);
    }


    
   
    
    // Visualiza dadod no terminal
    /*Serial.print("\nCorrente (mA): ");
    Serial.print(corrente);
    Serial.print(" -- Tensão (V): ");
    Serial.print(tensao);


    // Lê dados do sensor DHT11
    float temperatura = sensorDHT11.getTemperature();
    float humidade    = sensorDHT11.getHumidity();

    if (isnan(temperatura) || isnan(humidade)) {
      imprimeln(F("Falha ao ler o DHT11!"));    
    } else {
        imprime(F(" -- Temperatura (C): "));
        imprime(temperatura);
        imprime(F(" -- Humidade (%): "));
        imprime(humidade);
    }*/

    yield();
}




// #include <Arduino.h>
// #include <Adafruit_Sensor.h>  
// #include <DHT.h>
// #include <DHT_U.h>

// #define DHTPIN 2      // GPIO2 = D4 no ESP8266
// #define DHTTYPE DHT11 // Pode usar DHT22 se quiser

// DHT11Sensor dht(DHTPIN, DHTTYPE);


// void setup() {
//   Serial.begin(115200);
//   dht.begin();
//   Serial.println("Lendo DHT11...");
// }

// void loop() {
//   float temperatura = dht.getTemperature();
//   float humidade    = dht.getHumidity();

//   // Se falhar a leitura
//   if (isnan(temperatura) || isnan(humidade)) {
//     Serial.println("Falha ao ler o DHT11!");
//     delay(2000);
//     return;
//   }

//   Serial.print("Temperatura: ");
//   Serial.print(temperatura);
//   Serial.println(" °C");

//   Serial.print("Humidade: ");
//   Serial.print(humidade);
//   Serial.println(" %");

//   delay(3000);
// }
