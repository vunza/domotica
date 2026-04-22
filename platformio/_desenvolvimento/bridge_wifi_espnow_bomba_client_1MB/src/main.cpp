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


//Device device;
WiFiManager wifiManager;

// Servidor Web
AsyncWebServer servidorHTTP(80);
WebServer webServer(&servidorHTTP);


const uint8_t LED_PIN = 13; // GPIO2 - LED embutido (LOW = ligado)
const uint8_t PIN_BUTTON = 0; // GPIO0 - Botão embutido (LOW = pressionado)
const uint8_t PIN_RELAY = 12; // GPIO12 - Relé (HIGH = ligado)


volatile bool newDataReady = false;
EspNowManager espnow;
EspNowData dados_espnow;
uint8_t broadcastMac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

// EEPROM
EEPROMManager eeprom;

// Variáveis globais para cache dos sensores
DeviceData dados_dispositivo;

unsigned long lastUpdate = 0;


// ICACHE_RAM_ATTR IRAM_ATTR
void IRAM_ATTR attInterruptChangePIN();
void IRAM_ATTR attInterruptFallPIN();
void IRAM_ATTR attInterruptRisePIN();


//////////////////////////
// Configuração inicial //
//////////////////////////
void setup() {
    // Iniciar comunicação serial
    Serial.begin(115200);
    delay(1000);
    imprimeln(); 
    
    // TODO: Melhorar este trecho
    pinMode(LED_PIN, OUTPUT);
    pinMode(PIN_BUTTON, INPUT);
    pinMode(PIN_RELAY, OUTPUT);

    digitalWrite(PIN_RELAY, LOW);
    digitalWrite(LED_PIN, digitalRead(PIN_RELAY)); // O inverso do PIN_RELAY


    // Set pin as interrupt, assign interrupt function and set CHANGE/RISING mode
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptRisePIN, RISING); // Interrupção na borda de subida (RISING) do sinal do botão (GPIO0)

    
    // Obtém os ultimos 3 Bytes do endereço MAC, soma os mesmos, converte em decimal, 
    // para ter tempo de espera diferentes em cada ESP.
    
    uint8_t mac[6];
    WiFi.macAddress(mac);  
    uint16_t sum = mac[3] + mac[4] + mac[5];
  
    if( sum/60 <= 10){
        sum *= 5;
    }

   
    // Pausa necessaris para o emparelhamento com Master Esp-Now
    uint8_t counter = 0;
    while(counter < sum/60){
        counter++;
        delay(1000);        
        imprime(F("Iniciando... "));
        imprimeln(counter);
    }

    
    #ifdef ESP32
       Wire.begin(21, 22);  // SDA=21, SCL=22 no ESP32
    #elif defined(ESP8266)
        Wire.begin(4, 5);      // SDA=4, SCL=5 no D1 mini  
    #endif  
    

    // Inicializa a EEPROM com 512 bytes
    eeprom.begin(EEPROM_SIZE);   
    
    // Obtem Nome do Dispositivo e o guarda no array "DeviceData"
    eeprom.EEPROM_readStruct(EEPROM_START_ADDR, dados_dispositivo);

    imprime(F("Nome do Dispositivo: "));
    imprimeln(dados_dispositivo.device_name);
   
    
    Wire.setClock(100000); // 100 kHz → máximo de estabilidade com 2 I2C

    // Inicializar dispositivo
    //device.initialize(); 

    // Scanea canal Esp-Now
    espnow.emparelharDispositivo(broadcastMac, 1500, false);
    

    /////////////////////////////////////////////
    // Callback para recepção de dados esp-now //
    /////////////////////////////////////////////
    espnow.onReceive([](const uint8_t *mac, const uint8_t *data, int len){       

        // Verifica se o pacote tem o tamanho esperado
        if (len == sizeof(EspNowData)) {
             // Passa dados recebidos para a estructura              
            memcpy((void*)&dados_espnow, data, sizeof(EspNowData)); 
            newDataReady = true;  // sinaliza para o loop
        }
        else{
            return; // Ignora pacotes de tamanho inesperado
        }
        
        
        // Obtem MAC do Dispositivo
        char my_mac_addr[18];            
        WiFi.macAddress().toCharArray(my_mac_addr, 18);         

        // Checa e processa Mensagens Recebidas
        if( strcmp(dados_espnow.msg_type, "CHANNEL_RSP") == 0 ) {           
            
            // Veriifca a quem destina-se o comando TODO: rever o codigo
            if ( strcmp(my_mac_addr, dados_espnow.mac_client) == 0){
                Serial.print("CANAL: ");
                Serial.println(dados_espnow.state);
            } 
            else if( strcmp(my_mac_addr, dados_espnow.mac_client) != 0 && strcmp(dados_espnow.mac_server, "RTx") != 0) {  
                // Retrasmite os dados (uma vez), para alcançar dispositivos fora do alcance do servidor             
                strcpy(dados_espnow.msg_type, "CHANNEL_RSP");
                strcpy(dados_espnow.mac_server, "RTx");
                espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
                strcpy(dados_espnow.mac_server, "");
            }  

        } else if( strcmp(dados_espnow.msg_type, "REPAIR_MSG") == 0 ) { // Forçar reemparelhamento 
            
            if(strcmp(dados_espnow.mac_server, "RTx") != 0) {  
                // Retrasmite os dados (uma vez), para alcançar dispositivos fora do alcance do servidor             
                strcpy(dados_espnow.msg_type, "REPAIR_MSG");
                strcpy(dados_espnow.mac_server, "RTx");
                espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
                strcpy(dados_espnow.mac_server, "");
                
                // Scanea canal Esp-Now
                espnow.emparelharDispositivo(broadcastMac, 1500, false);
            }  
            else{
                // Scanea canal Esp-Now
                espnow.emparelharDispositivo(broadcastMac, 1500, false);
            }

        } else if( strcmp(dados_espnow.msg_type, "ALIVE_MSG") == 0 ) {                        

            if(strcmp(dados_espnow.mac_server, "RTx") != 0) { 
                
                espnow.server_alive_counter = 0;
                
                // Retrasmite os dados (uma vez), para alcançar dispositivos fora do alcance do servidor             
                strcpy(dados_espnow.msg_type, "ALIVE_MSG");
                strcpy(dados_espnow.mac_server, "RTx");
                espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
                strcpy(dados_espnow.mac_server, "");
            }  
            else{
                espnow.server_alive_counter = 0;
            }

        } else if( strcmp(dados_espnow.msg_type, "SET_STATE") == 0 ) {  
                                
            // Veriifca a quem destina-se o comando
            if ( strcmp(my_mac_addr, dados_espnow.mac_client) == 0){                
                
            #if defined(ESP32)
                // Ligar/Desligar PIN/LED (Logica directa)
                if (strcmp(dados_espnow.state, "OFF") == 0) {      
                    digitalWrite(PIN_RELAY, HIGH);
                    digitalWrite(LED_PIN, digitalRead(PIN_RELAY)); // O inverso do PIN_RELAY
                } else if (strcmp(dados_espnow.state, "ON") == 0) {      
                    digitalWrite(PIN_RELAY, LOW);
                    digitalWrite(LED_PIN, digitalRead(PIN_RELAY)); // O inverso do PIN_RELAY
                }
            #elif defined(ESP8266)
                // Ligar/Desligar PIN/LED (Logica invertida)
                if (strcmp(dados_espnow.state, "ON") == 0) {      
                    digitalWrite(PIN_RELAY, HIGH);
                    digitalWrite(LED_PIN, digitalRead(PIN_RELAY)); // O inverso do PIN_RELAY
                } else if (strcmp(dados_espnow.state, "OFF") == 0) {      
                    digitalWrite(PIN_RELAY, LOW);
                    digitalWrite(LED_PIN, digitalRead(PIN_RELAY)); // O inverso do PIN_RELAY
                }
            #endif


                // Publicar Estado do PIN/LED                
                strcpy(dados_espnow.msg_type, "STATE_SETED");                
                strcpy(dados_espnow.mac_client, WiFi.macAddress().c_str());
                esp_now_send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
            } 
            else if( strcmp(my_mac_addr, dados_espnow.mac_client) != 0 && strcmp(dados_espnow.mac_server, "RTx") != 0) {  
                // Retrasmite os dados (uma vez), para alcançar dispositivos fora do alcance do servidor             
                strcpy(dados_espnow.msg_type, "SET_STATE");
                strcpy(dados_espnow.mac_server, "RTx");
                espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
                strcpy(dados_espnow.mac_server, "");
            }           
        }
        else if( strcmp(dados_espnow.msg_type, "NORMAL_MODE") == 0 ) { 
            // Veriifca a quem destina-se o comando
            if ( strcmp(my_mac_addr, dados_espnow.mac_client) == 0) {
                ESP.restart(); // Rinicia o ESP para iniciar o modo e operação normal
            } 
            else if ( strcmp(my_mac_addr, dados_espnow.mac_client) != 0 && strcmp(dados_espnow.mac_server, "RTx") != 0){ // Retrasmite os dados, para alcançar dispositivos fora do alcance do servidor
                strcpy(dados_espnow.msg_type, "NORMAL_MODE");
                strcpy(dados_espnow.mac_server, "RTx");
                espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
                strcpy(dados_espnow.mac_server, "");
            }                         
        }
        else if( strcmp(dados_espnow.msg_type, "OTA_MODE") == 0 ) {             
          
            // Veriifca a quem destina-se o comando
            if ( strcmp(my_mac_addr, dados_espnow.mac_client) == 0) {

            #if defined(ESP32)
                const char* host_name = WiFi.getHostname();
            #elif defined(ESP8266)  
                String hostNameStr = WiFi.hostname();
                const char* host_name = hostNameStr.c_str();
            #endif           

                // Credeciais de acesso a rede WiFi
                const char* ssid = host_name;
                const char* password = "123456789";                

                // Rede WiFi            
                //wifiManager.connect(ssid, password);  
                wifiManager.criar_ap(ssid, password);              

                // Iniciar servidor web
                webServer.begin();

                // Inicializar OTA elegante
                ElengantOTA::begin(&servidorHTTP); 
            } 
            else if ( strcmp(my_mac_addr, dados_espnow.mac_client) != 0 && strcmp(dados_espnow.mac_server, "RTx") != 0){ // Retrasmite os dados, para alcançar dispositivos fora do alcance do servidor
                strcpy(dados_espnow.msg_type, "OTA_MODE");
                strcpy(dados_espnow.mac_server, "RTx");
                espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
                strcpy(dados_espnow.mac_server, "");
            }                   
        }      

    }); // FIM de espnow.onReceive(...)
    


    // Callback para envio de dados esp-now
    espnow.onSend([](const uint8_t *mac, bool ok){
        
    }); // FIM de espnow.onSend(...)  
    
}


////////////////////
// loop principal //
////////////////////
void loop() {
    // NOTA: AsyncElegantOTA não precisa de nada no loop

    
    // Verificar conexão WiFi
    /*#ifdef ESP32
        if (WiFi.getMode() == WIFI_MODE_STA || WiFi.getMode() == WIFI_MODE_APSTA){
            wifiManager.checkConnection();
        }
    #elif defined(ESP8266)
        if (WiFi.getMode() == WIFI_STA || WiFi.getMode() == WIFI_AP_STA){
            wifiManager.checkConnection();
        }        
    #endif*/


    // Controla reconexao com o Master Esp-Now
    static unsigned long lastPingCounter = 0;
    if (millis() - lastPingCounter > 1000) { 

        if( ++espnow.server_alive_counter >= 25){
            espnow.is_server_alive = false;                      
        } else {
            espnow.is_server_alive = true;
        }

        lastPingCounter = millis();

    }// FIM if(...)

    static unsigned long lastPing = 0;
    if (millis() - lastPing > 30000) {          
        
        if( espnow.is_server_alive == false){     
            // Scanea canal Esp-Now
            imprimeln("Servidor ESP-NOW desligado!");
            espnow.emparelharDispositivo(broadcastMac, 1500, false);               
        }

        lastPing = millis(); 

    } // FIM if(...)
 
    
    // Envia Dados cada X segundos
    if (millis() - lastUpdate > 5000) {
        lastUpdate = millis();      
    }
    
} 



/////////////////////////////////////////
// Atecnção à interrupção do PIN GPIO0 //
/////////////////////////////////////////
// ICACHE_RAM_ATTR IRAM_ATTR
void IRAM_ATTR attInterruptRisePIN(){  

    noInterrupts();
    
    digitalWrite(PIN_RELAY, !digitalRead(PIN_RELAY));
    digitalWrite(LED_PIN, digitalRead(PIN_RELAY)); // O inverso do PIN_RELAY

    // Publicar Estado do PIN_RELAY
    if(digitalRead(PIN_RELAY) == HIGH){
        strcpy(dados_espnow.state, "ON");
    } else {
        strcpy(dados_espnow.state, "OFF");
    }  

    strcpy(dados_espnow.msg_type, "STATE_SETED");
    strcpy(dados_espnow.mac_server, "");                
    strcpy(dados_espnow.mac_client, WiFi.macAddress().c_str());
    esp_now_send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));

    interrupts(); 

}




