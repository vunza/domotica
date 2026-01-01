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
EspNowData dados_espnow;
uint8_t broadcastMac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};


// EEPROM
EEPROMManager eeprom;

// Variáveis globais para cache dos sensores
float g_voltage = 0.0;
float g_current = 0.0;
float g_temperature = 0.0;
float g_humidity = 0.0;
DeviceData dados_dispositivo;


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

    // Inicializa a EEPROM com 512 bytes
    eeprom.begin(EEPROM_SIZE);   
    
    // Obtem Nome do Dispositivo e o guarda no array "DeviceData"
    eeprom.EEPROM_readStruct(EEPROM_START_ADDR, dados_dispositivo);

    imprime(F("Nome do Dispositivo: "));
    imprimeln(dados_dispositivo.device_name);

    // Iniciar ESP-NOW
    espnow.begin(); // canal = 1 e modo = WIFI_STA por defeito

    // broadcast (tudo que enviar vai para todos)
    espnow.addPeer(broadcastMac); // canal = 1 por defeito

 
    // Callback para recepção de dados esp-now
    espnow.onReceive([](const uint8_t *mac, const uint8_t *data, int len){
        // Passa dados recebidos para a estructura
        memcpy(&dados_espnow, data, sizeof(EspNowData));
        
        char my_mac_addr[18];            
        WiFi.macAddress().toCharArray(my_mac_addr, 18);       

        // Checa e processa o tipo de mensagens
        if( strcmp(dados_espnow.msg_type, "DATA") == 0 ) {   
            // Veriifca a quem destina-se o comando
            if ( strcmp(my_mac_addr, dados_espnow.mac_client) == 0){
                
            } 
            else if(strcmp(dados_espnow.state, "RTx") != 0) {  
                // Retrasmite os dados (uma vez), para alcançar dispositivos fora do alcance do servidor             
                strcpy(dados_espnow.msg_type, "DATA");
                strcpy(dados_espnow.state, "RTx");
                espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
            }           
        }
        else if( strcmp(dados_espnow.msg_type, "NORMAL_MODE") == 0 ) { 
            // Veriifca a quem destina-se o comando
            if ( strcmp(my_mac_addr, dados_espnow.mac_client) == 0) {
                ESP.restart(); // Rinicia o ESP para iniciar o modo e operação normal
            } 
            else{ // Retrasmite os dados, para alcançar dispositivos fora do alcance do servidor
                strcpy(dados_espnow.msg_type, "DATA");
                espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
            }                         
        }
        else if( strcmp(dados_espnow.msg_type, "OTA_MODE") == 0 ) {   
            // TODO: Atribuir nome sugestivo.    
            // Veriifca a quem destina-se o comando
            if ( strcmp(my_mac_addr, dados_espnow.mac_client) == 0) {
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
            else{ // Retrasmite os dados, para alcançar dispositivos fora do alcance do servidor
                strcpy(dados_espnow.msg_type, "OTA_MODE");
                espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
            }                   
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

       // Eliminar corrente negativa
        if(g_current < 0) g_current = 0.00;
        
        // Converte os dados a Char Array.
        String(g_voltage, 2).toCharArray(dados_espnow.u1_voltage, sizeof(dados_espnow.u1_voltage));
        String(g_current, 2).toCharArray(dados_espnow.u1_current, sizeof(dados_espnow.u1_current));
        String(g_temperature, 2).toCharArray(dados_espnow.u1_temperature, sizeof(dados_espnow.u1_temperature));
        String(g_humidity).toCharArray(dados_espnow.u1_humidity, sizeof(dados_espnow.u1_humidity));
               
        // Ponto parachecar/ver o tamanho do Array
        //sizeof(dados);

        // Envia dados, apenas, se forem diferentes de "nan"
        if (!isnan(g_voltage) && !isnan(g_current) && !isnan(g_temperature)  && !isnan(g_humidity)){      
            strcpy(dados_espnow.mac_client, mac);
            strcpy(dados_espnow.msg_type, "DATA");
            espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));  
        }      
    } 

}

