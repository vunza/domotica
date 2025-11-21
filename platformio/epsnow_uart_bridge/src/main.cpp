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




void setup() {
  Serial.begin(115200);   // Mesma velocidade no Armbian
  delay(1000);
  Serial.println("ESP8266 pronto");
}

void loop() {
  // Enviar mensagem periódica
  Serial.println("Mensagem do ESP8266");
  delay(2000);

  // Verificar se recebeu algo do Armbian
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    Serial.print("Recebido do Armbian: ");
    Serial.println(msg);
  }
}