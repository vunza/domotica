#include <Arduino.h>


#define RXD2 16  // Pino RX do ESP32 para receber dados do CC2531 (P0.2)
#define TXD2 17  // Pino TX do ESP32 para enviar dados para o CC2531 (P0.3)

#include <HardwareSerial.h>

HardwareSerial Zigbee(2);  // Utilize Serial2 no ESP32



//////////////////////////
// Configuração inicial //
//////////////////////////
void setup() {
    Serial.begin(115200);    // Monitor Serial
    Zigbee.begin(9600, SERIAL_8N1, RXD2, TXD2); // Configuração UART
}


////////////////////
// loop principal //
////////////////////
void loop() {
    if (Zigbee.available()) {
        String data = Zigbee.readString();
        Serial.println(data);  // Exibe os dados brutos recebidos
    }
}