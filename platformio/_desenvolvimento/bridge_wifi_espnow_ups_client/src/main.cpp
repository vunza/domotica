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
INA226Sensor sensorINA226(INA226_I2C_ADDRESS);


// Define seu LCD (endereço e tamanho)
LCDDisplay displayLCD(0x27, 16, 2);

const uint8_t LED_PIN = 2; // GPIO2 - LED embutido (LOW = ligado)


// DHT11 temperature and humidity sensor
#ifdef ESP32
  #define DHTPIN 4   
#elif defined(ESP8266)
  #define DHTPIN 12      
#endif    
   
#define DHTTYPE DHT11 
DHT11Sensor sensorDHT11(DHTPIN, DHTTYPE);


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

    // Pausa necessaris para o emparelhamento com Master Esp-Now
    uint8_t counter = 0;
    while(counter < 14){
        counter++;
        delay(1000);        
        imprime(F("Iniciando..."));
        imprimeln(counter);
    }


    #ifdef ESP32
       Wire.begin(21, 22);  // SDA=21, SCL=22 no ESP32
    #elif defined(ESP8266)
        Wire.begin(4, 5);      // SDA=4, SCL=5 no D1 mini  
    #endif  


    // TODO: Melhorar este trecho
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH); // LED desligado inicialmente


    // Inicializa a EEPROM com 512 bytes
    eeprom.begin(EEPROM_SIZE);   
    
    // Obtem Nome do Dispositivo e o guarda no array "DeviceData"
    eeprom.EEPROM_readStruct(EEPROM_START_ADDR, dados_dispositivo);

    imprime(F("Nome do Dispositivo: "));
    imprimeln(dados_dispositivo.device_name);
   
    
    Wire.setClock(100000); // 100 kHz → máximo de estabilidade com 2 I2C

    // Inicializar dispositivo
    device.initialize(); 

    // Scanea canal Esp-Now
    espnow.emparelharDispositivo(broadcastMac, 1500, false);
   

    // Callback para recepção de dados esp-now
    espnow.onReceive([](const uint8_t *mac, const uint8_t *data, int len){
        // Passa dados recebidos para a estructura
        memcpy(&dados_espnow, data, sizeof(EspNowData));        
        
        // Obtem MAC do Dispositivo
        char my_mac_addr[18];            
        WiFi.macAddress().toCharArray(my_mac_addr, 18);       

        // Checa e processa Mensagens Recebidas
        if( strcmp(dados_espnow.msg_type, "CHANNEL_RSP") == 0 ) {        
            
            // Veriifca a quem destina-se o comando
            if ( strcmp(my_mac_addr, dados_espnow.mac_client) == 0){
                Serial.print("CANAL: ");
                Serial.println(dados_espnow.state);
            } 
            else if(strcmp(dados_espnow.state, "RTx") != 0) {  
                // Retrasmite os dados (uma vez), para alcançar dispositivos fora do alcance do servidor             
                strcpy(dados_espnow.msg_type, "DATA");
                strcpy(dados_espnow.state, "RTx");
                espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
            }  

        } else if( strcmp(dados_espnow.msg_type, "REPAIR_MSG") == 0 ) { // Forçar reemparelhamento
            
             // Scanea canal Esp-Now
            espnow.emparelharDispositivo(broadcastMac, 1500, false);

            if(strcmp(dados_espnow.state, "RTx") != 0) {  
                // Retrasmite os dados (uma vez), para alcançar dispositivos fora do alcance do servidor             
                strcpy(dados_espnow.msg_type, "REPAIR_MSG");
                strcpy(dados_espnow.state, "RTx");
                espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
            }  

        } else if( strcmp(dados_espnow.msg_type, "ALIVE_MSG") == 0 ) {            
            
            espnow.server_alive_counter = 0;            

            /*if ( atoi(dados_espnow.state) != WiFi.channel()){                
                espnow.is_server_alive = false;                
            }*/
            

            if(strcmp(dados_espnow.state, "RTx") != 0) {  
                // Retrasmite os dados (uma vez), para alcançar dispositivos fora do alcance do servidor             
                strcpy(dados_espnow.msg_type, "ALIVE_MSG");
                strcpy(dados_espnow.state, "RTx");
                espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
            }  

        } else if( strcmp(dados_espnow.msg_type, "SET_STATE") == 0 ) {             
            // Veriifca a quem destina-se o comando
            if ( strcmp(my_mac_addr, dados_espnow.mac_client) == 0){
                
                // Ligar/Desligar PIN/LED (Logica invertida)
                if (strcmp(dados_espnow.state, "ON") == 0) {      
                    digitalWrite(LED_PIN, LOW);
                } else if (strcmp(dados_espnow.state, "OFF") == 0) {      
                    digitalWrite(LED_PIN, HIGH);
                }

                // Publicar Estado do PIN/LED                
                strcpy(dados_espnow.msg_type, "STATE_SETED");                
                strcpy(dados_espnow.mac_client, WiFi.macAddress().c_str());
                esp_now_send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
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

    
    // Conectar WiFi
    /*wifiManager.connect(ssid, password); 

    // Criar ponto de acesso WiFi
    //wifiManager.criar_ap("ESP8266", "123456789");
    // Iniciar servidor web
    webServer.begin();

    // Inicializar OTA elegante
    ElengantOTA::begin(&servidorHTTP); 

    
    // Inicializar sensor INA226
    sensorINA226.begin();   
    
    // Inicializa display LCD
    displayLCD.begin();*/
    
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
            espnow.emparelharDispositivo(broadcastMac, 1500, false);               
        }

        lastPing = millis(); 

    } // FIM if(...)
 
    
    // Envia Dados cada X segundos
    if (millis() - lastUpdate > 5000) {
        lastUpdate = millis();

        sensorINA226.update();
        yield(); // importante

        g_voltage = sensorINA226.readVoltage() * (0.986874487285); // Com factor de ceorrecção
        g_current = sensorINA226.readCurrent()/1000.0;
        yield();

        g_temperature = sensorDHT11.getTemperature();
        g_humidity = sensorDHT11.getHumidity();
        yield();

        displayLCD.showReadings(g_current, g_voltage);
        yield();


        char mac[18]; 
        String macStr = WiFi.macAddress();
        macStr.toCharArray(mac, 18);

        // Eliminar corrente negativa
        if(g_current < 0 || g_current <= 0.09 ) g_current = 0.00;
        
        // Converte os dados a Char Array.
        String(g_voltage, 2).toCharArray(dados_espnow.u1_voltage, sizeof(dados_espnow.u1_voltage));
        String(g_current, 2).toCharArray(dados_espnow.u1_current, sizeof(dados_espnow.u1_current));
        String(g_temperature, 2).toCharArray(dados_espnow.u1_temperature, sizeof(dados_espnow.u1_temperature));
        String(g_humidity).toCharArray(dados_espnow.u1_humidity, sizeof(dados_espnow.u1_humidity));
               
        // Ponto parachecar/ver o tamanho do Array
        //sizeof(dados);

        // Envia dados, apenas, se forem diferentes de "nan"
        if (!isnan(g_voltage) && !isnan(g_current) && !isnan(g_temperature)  && !isnan(g_humidity)){  
            strcpy(dados_espnow.state, "ON"); // Assuming the device is on
            strcpy(dados_espnow.mac_client, mac);
            strcpy(dados_espnow.msg_type, "DATA");
            espnow.send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));  
        }      
    }
    
}





