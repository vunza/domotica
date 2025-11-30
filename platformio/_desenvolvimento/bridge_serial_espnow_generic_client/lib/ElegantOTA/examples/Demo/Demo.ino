#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "TPLINK";
const char* password = "gregorio@2012";

void setup() {
  Serial.begin(115200);

  // Conectar-se à rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conectado à rede WiFi");

  // Inicializar o servidor OTA
  ArduinoOTA.onStart([]() {
    Serial.println("Iniciando atualização OTA");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nAtualização OTA concluída com sucesso");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progresso: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Erro[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Falha na autenticação");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Falha no início da atualização");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Falha na conexão");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Falha na recepção");
    else if (error == OTA_END_ERROR) Serial.println("Falha no final da atualização");
  });

  ArduinoOTA.setPassword("gregorio@2012");

  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  // Seu código principal continua aqui
}
