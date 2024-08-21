

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


// Reemplaza con tus credenciales de red Wi-Fi
const char* ssid = "TPLINK";
const char* password = "gregorio@2012";

// Crear una instancia del servidor web asincrónico
AsyncWebServer server(8090);


void setup(){
  // Iniciar la conexión serial
  Serial.begin(115200);

  // Conectar a la red Wi-Fi
  WiFi.begin(ssid, password);

  // Esperar hasta que se conecte a la red Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  // Imprimir la dirección IP en la consola serial
  Serial.println("");
  Serial.println("Conectado a Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize SPIFFS
  if (!LittleFS.begin()) {
    Serial.println(F("Erro de montagem do SPIFFS!"));
    return;
  }
  else{
    Serial.println(F("\nSPIFFS OK.\n"));   
  }  


  // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {    
      request->send(LittleFS, "/index.html", "text/html");
    });
    
    // Route to load style.css file
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(LittleFS, "/style.css", "text/css");
    });

    // Route to load script.js file
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(LittleFS, "/script.js", "text/javascript");
    });

  // Iniciar el servidor
  server.begin();
}// Fim de setup()



void loop(){

}// Fim do loo()