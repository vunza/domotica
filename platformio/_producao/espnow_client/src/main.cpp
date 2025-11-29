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

*/

// Configuração do pin a controlar
// 12 -- GPIO12 do SLAMPHER -- //GPIO15 do ESP12E
// 2 --- wemos_d1_R2
// 1 --- ESP8266

//  2449233 -- Brdge Tasmota (Sonoff Pow Elite)
//  8591111 -- Brdge Tasmota (Sonoff Zigbee Bridge Pro)

#define SHELLY1L 0
#define M5_1C_86 0
#define POWER320D 0
#define BISTABLE_RELAY 0
#define ESPNOW_TASMOTA_ZIGBEE_BRIDGE 0
#define ESPNOW_TASMOTA_BRIDGE 0
#define SWAP_UART 0
#define DEBUG 1
#define IS_PASSIVE_MASTER 0
#define INVERT_PIN 0
#define TELNET 0

#if SHELLY1L == 1
  #define PIN_RELAY   5
  #define PIN_BUTTON  4                
  #define PIN_LED     0  
#elif M5_1C_86 == 1
  #define PIN_RELAY   23
  #define PIN_BUTTON  0                
  #define PIN_LED     19      //LED 1 (RED)  
  #define PIN_STATUS_LED  5   // Status LED (BLUE)  
#elif POWER320D == 1 
  #define BISTABLE_ON 2   // (320D - bi-stable - On)
  #define BISTABLE_OFF 4  // (320D - bi-stable - Off)
  #define PIN_BUTTON  0                
  #define PIN_WIFI_LED  5     
  #define PIN_LED  18                               
#else
  #define PIN_RELAY   12
  #define PIN_BUTTON  0            
  #define PIN_LED     2//13//1
#endif


#if DEBUG == 1
#define imprime(x) Serial.print(x)
#define imprimeln(x) Serial.println(x)
#else
#define imprime(x)
#define imprimeln(x)
#endif


#define QTD_TMRS 16                         // Número máximo de Timers
#define MAC_BYTE_SIZE 6                     // Tamanho do buffer de MAC Address 
#define MAX_NAME_LENGTH 24                  // Comprimento do Nome dos dispositivos (64 - 1)
#define JSON_DOC_SIZE 249                   // 250 - Max para esp-now (249 + 1bye de fim de cadeia)
#define ZB_JSON_DOC_SIZE 512                // Tamnho do json recebido do tasmota  
#define ZB_SHORT_ID 7                       // Tamnho do short id dos dispositivos zigbee 
#define QTD_DEVS_ZIGBEE 20                  // Número máximo de Dispositivos ZigBee
#define DT_STR_SIZE 21                      // Tamanho da string contendo a data e hora do Device
#define SSID_STR_SIZE 31                    // Tamanho da string contendo o nome de SSID
#define PWD_STR_SIZE 31                     // Tamanho da string contendo a senha
#define MAX_MOD_SIZE 11   
#define MAX_WIFI_CH 15
#define QTD_BOARDS 10                      // Número máximo de Clientes ESP
#define MSG_LENGTH 15
#define YEAR_COMPARE 2023
#define HORA_MINUTOS 6
#define MAX_TIME_CONN 15                   // Tempo, em segundos, para conexão à rede WiFi  
#define TEMPO_ENVIAR_ALIVE 5               // Tempo em segundos
#define TMP_CTRL_SERVER_ALIVE 60           // Tempo em segundos
#define IP_MODE_SIZE 8                     // Tamanho do buff P/ modos de ip nos modos WIFI_STA e WIFI_AP  
#define EEPROM_START_ADDR 0
//#define NOME_OTA "CLNT_ESPNOW"
#define FILE_DATA_SPIFFS "/data.txt"
#define FILE_ZBTMRS_SPIFFS "/zbtimers.txt"
#define TYPE_SIZE 5                         // Tamanho do BUFF para o tipo de dispositivo
#define MAX_ID_DIGITS 7
#define PASSIVE_SLAVE_ID 4164129//0
#define ID_BROADCAST_ESPNOW 9999999         // ID para broadcast aos dispositivos ESP-NOW


//////////////////////////////////////////
// Estrutura e Array p/ controlar alive //
//////////////////////////////////////////
typedef struct Alive{
  uint32_t board_id;
  uint32_t temp_alive;
}Alive;


///////////////////////////////
// Gestão das connexões WiFi //
///////////////////////////////
typedef struct DadosRede{
  IPAddress ip;
  IPAddress gateway;
  IPAddress subnet;
  boolean HIDE_SSID;
  uint8_t CHANNEL;
  uint8_t MAX_CONNECTIONS;
  boolean rede_default;
}DadosRede;




////////////////////////////////////////////////////////////////////////
// Estrutura para guardar dados, frequentemente alterdados, na SPIFFS //
////////////////////////////////////////////////////////////////////////
typedef struct data2SPIFFS{
  uint8_t estado_pin;
  char DT[DT_STR_SIZE];
  //uint32_t BOARDS_ID[QTD_BOARDS];  // TODO: Adaptar para Clientes que processa Código iR
} data2SPIFFS;


/////////////////////////////////////////////
// Estrutura para programar Timers/Alarmes //
/////////////////////////////////////////////
typedef struct timers2EEPROM{
  uint8_t activar;
  uint8_t horas;
  uint8_t minutos;
  char weekDays[8];
  uint8_t output;
  uint8_t action;   
} timers2EEPROM;


////////////////////////////////////////////
// Estrutura para guardar dados na EEPROM //
////////////////////////////////////////////
typedef struct data2EEPROM{
  
  // DEVICES DATA
  char nome[MAX_NAME_LENGTH];   
  char Modo[MAX_MOD_SIZE];                            // OTA, OPRA, OPRS, OTA
  char SSID[SSID_STR_SIZE];
  char PWD[PWD_STR_SIZE];
  byte ip[4];
  byte gateway[4];
  byte subnet[4];
  byte dns1[4];
  byte dns2[4];
  //char SOCKSERVERADDR[50];
  //uint16_t SOCKSERVERPORT;  
  char CONFIGURADO[3];
  uint8_t TIPO; // lAMPADA = 1, HUB_IR = 2, E_BOMBA = 3 ...PASSIVO = 9

} data2EEPROM;



////////////////////////////////////////////////
// Estrutura para guardar dispositivos Zigbee //
////////////////////////////////////////////////
typedef struct ZigbeeDevices{
  char short_id[ZB_SHORT_ID];
  char name[MAX_NAME_LENGTH];
  uint8_t power;
  uint32_t zb_board_id = 0;
  boolean Reachable = false;
  //boolean setted = false;
} ZigbeeDevices;


/////////////////////////////////////////////
// Estrutura para programar Timers devs ZB //
/////////////////////////////////////////////
typedef struct zbtimers2SPIFFS{
  char short_id[ZB_SHORT_ID];
  uint8_t timer;
  uint8_t activar;
  char hora_minutos[HORA_MINUTOS];
  char weekDays[8];
  uint8_t output;
  uint8_t action;   
} zbtimers2SPIFFS;

#define EEPROM_SIZE ( 4 + sizeof(data2EEPROM) + (sizeof(timers2EEPROM) * QTD_TMRS))


//////////////////////////
// Tipo de Dispositivos //
//////////////////////////
enum DEV_TYPES{
  lAMPADA = 1, 
  HUB_IR, 
  E_BOMBA,
  PASSIVO = 9
};  



////////////////////////////////////////////////////
// Classe para Cpnfiguração da Rede WiFi (STA/AP) //
////////////////////////////////////////////////////
class RedeWifi {

  public:
      RedeWifi(const char *ssid, const char *pwd);    
      void ConectaRedeWifi(const char* STA_IP_MODE); // STA_IP_MODE = [DHCP | STATIC]
      void CriaRedeWifi(const char* AP_IP_MODE); // AP_IP_MODE = [DEFAULT | CUSTOM]

  private:
      const char *_ssid;
      const char *_pwd;
    
  protected:
};



unsigned long ctrl_flash_led = 0;  
uint32_t ctl_btn_pulso = 0;
uint64_t cont_alive = 0;
unsigned long cont_exec_timer = 0;
uint64_t cont_send_alive = 0;
// Create AsyncWebServer object on port 8090
AsyncWebServer servidorHTTP(8090);
const char* CMND = "cmnd";               // Comandos recebidos por via do Javascript
uint8_t save_channel;
//uint8_t broadcastAddress[MAC_BYTE_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xB0}; // Produção
//uint8_t broadcastAddress[MAC_BYTE_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC}; // Teste

uint8_t broadcastAddress[MAC_BYTE_SIZE] = {0x78, 0xE3, 0x6D, 0x12, 0x29, 0x30}; // Teste 

char pairingStatus[MSG_LENGTH] = "PRT";
unsigned long ctrl_send_alive = 0;
unsigned long ctrl_server_alive = 0;
uint8_t seve_server_mac[6];
uint32_t BOARD_ID;
// Autoemparelhamento
uint8_t channel = 1;
unsigned long previousMillis = 0;  
unsigned long currentMillis = millis(); 
char json_array[JSON_DOC_SIZE]; 
data2EEPROM data2eeprom = {0};
data2SPIFFS data2spiffs = {0};
// TODO: Tornar dinâmica as escolhas abaixo (xx_ip_mode)
const char* ap_ip_mode = "DEFAULT";     // IP_MODE = [DEFAULT | CUSTOM]
const char* sta_ip_mode = "DHCP";       // STA_IP_MODE = [DHCP | STATIC]
#define EEPROM_TIMERS_ADDR sizeof(data2EEPROM) + 2
StaticJsonDocument <JSON_DOC_SIZE> json_doc;
bool exec_toggle = false;
uint8_t relayState = 0; 

#if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1 

  #define SHORT_DEV_ID_SIZE 7

  boolean procesing_uart_data = false;
  ZigbeeDevices arr_zb_devs[QTD_DEVS_ZIGBEE];
  uint8_t cont_dev_zb = 0;
  uint8_t total_zb_devs = 0;
  uint8_t ctl_pings_zb_devs = 0;
  char get_short_id[SHORT_DEV_ID_SIZE];

  StaticJsonDocument <ZB_JSON_DOC_SIZE> doc;   
  //DynamicJsonDocument doc(ZB_JSON_DOC_SIZE);  

  zbtimers2SPIFFS timersZB[QTD_DEVS_ZIGBEE];

#endif


#if defined(ESP32) 
  esp_now_peer_info_t peerInfo;
#endif



// Mudar Modo de Operação
void TrocarModoOperacao(const char* cz_modo);

// Limpar EEPROM
void LimpaDadosDispositivo();

// Limpara ArquivoSPIFFS com pin_sttate, DT e Array Alive 
void ResetSPIFFS();

// Funçaão para ler spiffs 
void LerSPIFFS(data2SPIFFS *ddv, const char* nome_arquivo);

// Funçaão para escrever spiffs
void EscreverSPIFFS(data2SPIFFS *ddv, const char* nome_arquivo);

// Limpar configurações de Temporizadores
void ResetTimers();

// Aumentar a Potencia de Transmissao do WiFi
void aumentarPotenciaTxWiFi();

// CallBack dados Enviados ESP-NOW
#if defined(ESP8266) 
 void callback_tx_esp_now(uint8_t *mac_addr, uint8_t status); 
#elif defined(ESP32) 
  void callback_tx_esp_now(const uint8_t *mac_addr, esp_now_send_status_t status);
#endif
 

// CallBack dados Recebidos ESP-NOW
#if defined(ESP8266) 
 void callback_rx_esp_now(uint8_t * mac, uint8_t *incomingData, uint8_t len);
#elif defined(ESP32) 
  void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len);
#endif

// Autoemparelhar com o servidor
const char* autoPairing();


//Criar dados JSON
const char* CriarJSON(const char* MSG_TYPE, const char* CMD, const char* MAC, uint32_t ID, int8_t PIN_STT, uint8_t WIFI_CH);

// Executar acções do Timer 
void ExecutarTimer(const char* nome_arquivo);

// Convert Data/Hora actual a string
void ObterDTActual(char* cz_dt);


// Converter MAC (string para Byte array)
uint8_t* converteMacString2Byte(const char* cz_mac);


// troca o estado de PIN e guarda dados (estado, data e hora) na SPIFFS
void TogglePIN();

// Consulta de Timers 
void ConsultarTimer(const char* nome_arquivo, uint8_t index);

// Configuracão de timers 
void ConfigurarTimer(const char* cz_json, const char* nome_arquivo);


// Enviar comando para alterar estado do PIN do Dispositivo PASSIVO 
void mudarEstadoPINPassivo(uint8_t estado, uint32_t dev_id);

// Criar ID 
uint32_t CriarDEVICE_ID();

// troca o estado de PIN para Relay bistable
#if BISTABLE_RELAY == 1
  void ToggleBISTABLE(uint8_t pin_on, uint8_t pin_off);
#endif  

// Liga/Desliga Relay bistable 
#if BISTABLE_RELAY == 1
  void OnOffBISTABLE(uint8_t estado, uint8_t pin_on, uint8_t pin_off);
#endif


// Envia requisicao HTTP 
#if ESPNOW_TASMOTA_BRIDGE == 1 
  void SendCommandtasmota(const char* cmnd, uint8_t cmnd_size);
  void processarMSG_2(String dados);
  uint8_t save_pin_state = 0;
  uint8_t iz_wifi_state = 0;
#endif

// Consulta Estado de dispositivos Zigbee
#if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1 
  uint32_t ZbDEVICE_ID(String zbmac,  DEV_TYPES type);
  void processarMSG(String dados);
  int8_t ArrayZbHasElement(ZigbeeDevices arr[], const char* element);
  //int8_t ArrayZbTimerHasElement(zbtimers2SPIFFS arr[], const char* element);
  uint8_t obterDigitoInvertido(uint32_t numero, uint8_t posicao);
  void LerZbSPIFFS(zbtimers2SPIFFS *ddv, const char* nome_arquivo);
  void EscreverZbSPIFFS(zbtimers2SPIFFS *ddv, const char* nome_arquivo);
  void ExecutarTimerZb(const char* nome_arquivo); 
#endif


// Processar dados recebidos via Telne
#if TELNET == 1
  void LeerTelnet() ;
#endif


// ICACHE_RAM_ATTR IRAM_ATTR
void IRAM_ATTR attInterruptChangePIN();
void IRAM_ATTR attInterruptFallPIN();
void IRAM_ATTR attInterruptRisePIN();

/*#if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1
  #define RX D5
  #define TX D6
  #define BAUD 115200
  SwSerial mySerial(RX, TX);
#endif*/

/////////////
// setup() //
/////////////
void setup() {
  // Init Serial Monitor  
  #if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1
    Serial.begin(115200);      
  #else
      Serial.begin(115200);       
  #endif

  #if SWAP_UART == 1
    Serial.swap(); // Para alguns Wemos D1 Mini GPIO15(D8,TX) and GPIO13(D7,RX)          
  #endif
 
  while(!Serial);
  
  // Configurar PIN's 
  #if ESPNOW_TASMOTA_BRIDGE == 0
    pinMode(PIN_LED, OUTPUT);
  #endif
 

  #if POWER320D == 1  
    pinMode(BISTABLE_ON, OUTPUT); 
    pinMode(BISTABLE_OFF, OUTPUT); 
    pinMode(PIN_WIFI_LED, OUTPUT); 
    digitalWrite(PIN_WIFI_LED, LOW);  
  #else  
    pinMode(PIN_RELAY, OUTPUT);
  #endif
  
  #if M5_1C_86 == 1
    pinMode(PIN_STATUS_LED, OUTPUT); 
    digitalWrite(PIN_STATUS_LED, LOW); // Logica invertida
  #endif

 
 

  #if SHELLY1L == 1
    pinMode(PIN_BUTTON, INPUT_PULLDOWN_16);
  #else
    pinMode(PIN_BUTTON, INPUT_PULLUP);         
  #endif

  
 
  // Set pin as interrupt, assign interrupt function and set CHANGE/RISING mode
  #if SHELLY1L == 1
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptChangePIN, CHANGE);
  #else
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptFallPIN, FALLING);         
  #endif

 
  // Inicializa EEPROM
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(EEPROM_START_ADDR, data2eeprom);


  // Toma o ID do dev e adiciona ao mesmo o TIPO de dev correspondente
  BOARD_ID = CriarDEVICE_ID();

  char buf[16];
  char tipo[4];
  itoa(data2eeprom.TIPO, tipo, 10);
	itoa(BOARD_ID, buf, 10);
  strcat(buf, tipo);
  
  // Toma, apenas,os últimos "MAX_ID_DIGITS" digitos do ID
  uint64_t num = atoi(buf);
  uint8_t n = MAX_ID_DIGITS;
  BOARD_ID = num % (int) pow(10, n);

 
  // Checar se o BOARD_ID tem menos de 7 digitos;
  uint8_t digitCount = 0;
  uint64_t aux_id = BOARD_ID;
  while (aux_id > 0) {
    aux_id /= 10;
    digitCount++;
  }

  // adicionar digitos complementares se necessario
  if(digitCount < MAX_ID_DIGITS){
    for(uint8_t cont = 0; cont < (MAX_ID_DIGITS - digitCount); cont++){
      BOARD_ID = (BOARD_ID * 10) + atoi(tipo);
    }    
  }


  
  // Initialize SPIFFS
  if (!LittleFS.begin()) {
    imprimeln(F("Erro de montagem do SPIFFS!"));
    return;
  }
  else{
    imprimeln(F("\nSPIFFS OK.\n"));
    imprimeln(BOARD_ID);
  }  


  if (!LittleFS.exists(FILE_DATA_SPIFFS)) {
    imprime("ERRO: O arquivo: \"");
    imprime(FILE_DATA_SPIFFS);
    imprimeln("\" Não existe!!!");

    ResetSPIFFS();
    
  }
  else{
    File arquivo = LittleFS.open(FILE_DATA_SPIFFS, "r");
    arquivo.read((byte *)&data2spiffs, sizeof(data2spiffs));
    imprime("Tamano do atquivo SPIFFS: ");
    imprime(FILE_DATA_SPIFFS);
    imprime(" ");
    imprime((size_t)arquivo.size());
    imprimeln(" bytes.");
    arquivo.close();    
  }


   // Arquivo spiffs para timers dos dispositivos zigbee
  #if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1 
    if (!LittleFS.exists(FILE_ZBTMRS_SPIFFS)) {
      imprime("ERRO: O arquivo: \"");
      imprime(FILE_ZBTMRS_SPIFFS);
      imprimeln("\" Não existe!!!");
      ResetSPIFFS();    
    }
    else{
      File arquivo = LittleFS.open(FILE_ZBTMRS_SPIFFS, "r");      
      arquivo.read((byte *)&timersZB, sizeof(timersZB));
      imprime("Tamanho do arquivo SPIFFS: ");
      imprime(FILE_ZBTMRS_SPIFFS);
      imprime(" ");
      imprime((size_t)arquivo.size());
      imprimeln(" bytes.");
      arquivo.close();    
    }
  #endif
  



   // Se não estiver configurado
  if( strcmp(data2eeprom.CONFIGURADO, "OK") != 0 ){
    LimpaDadosDispositivo();
    ResetTimers();
    ResetSPIFFS();
    ESP.restart();
  }


#if DEBUG == 1
  char msg[64]; 
        
  #if defined(ESP8266) 
    FSInfo fs_info;
    LittleFS.info(fs_info);   
    sprintf(msg, "\nSPIFFS Disponível: %i bytes\nSPIFFS Usada: %i bytes\n\n",fs_info.totalBytes, fs_info.usedBytes);
  #elif defined(ESP32) 
    sprintf(msg, "\nSPIFFS Disponível: %i bytes\nSPIFFS Usada: %i bytes\n\n",LittleFS.totalBytes(), LittleFS.usedBytes());
  #endif

  imprimeln(msg);
#endif
 
   
  if( strcmp(data2eeprom.Modo, "OPR") == 0){

    // Set device as a Wi-Fi Station  
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();   

    
    // Init ESP-NOW
    if (esp_now_init() != 0) {
      imprimeln(F("Error initializing ESP-NOW"));
      return;
    }

    // ESP-NOW
    #if defined(ESP8266) 
      esp_now_set_self_role(ESP_NOW_ROLE_COMBO);          
    #endif

    esp_now_register_send_cb(callback_tx_esp_now);
    esp_now_register_recv_cb(callback_rx_esp_now);


    // Inicialização de variáveis globais.
    ctrl_send_alive = millis();
    cont_alive = millis();
    //contador = millis();

    // Restabelecer estado do PIN
    LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);
   
    #if POWER320D == 1
      OnOffBISTABLE(data2spiffs.estado_pin, BISTABLE_ON, BISTABLE_OFF);
    #else
      digitalWrite(PIN_RELAY, data2spiffs.estado_pin);             
    #endif        
       
    
    #if IS_PASSIVE_MASTER == 1
      if(data2eeprom.TIPO != PASSIVO){
        mudarEstadoPINPassivo(data2spiffs.estado_pin, PASSIVE_SLAVE_ID);
      }                  
    #endif  

    #if defined(ESP32) 
      #if POWER320D == 0
        digitalWrite(PIN_LED, !digitalRead(PIN_RELAY)); // O inverso do PIN_RELAY
      #endif      
    #else
      digitalWrite(PIN_LED, digitalRead(PIN_RELAY)); // O inverso do PIN_RELAY
    #endif  
   
    cont_exec_timer = millis();
    cont_send_alive = millis();
  } 
  else  if(strcmp(data2eeprom.Modo, "OTA") == 0){

    // Cria rede AP
    if(data2eeprom.TIPO == PASSIVO){
      uint8_t len = String(BOARD_ID).length();
      char cz_ssid[len + 1];  
      String(BOARD_ID).toCharArray(cz_ssid, sizeof(cz_ssid));
      RedeWifi redeAP(cz_ssid, "123456789");
      redeAP.CriaRedeWifi(ap_ip_mode); // AP_IP_MODE = [DEFAULT | CUSTOM]
    }
    else{
       RedeWifi redeAP(data2eeprom.nome, "123456789");
       redeAP.CriaRedeWifi(ap_ip_mode); // AP_IP_MODE = [DEFAULT | CUSTOM]
    }

   
  
    // Configuração e inicialição do OTA
    //ArduinoOTA.setHostname(NOME_OTA);
    //ArduinoOTA.begin();  

    // Inicializar o ElegantOTA
    AsyncElegantOTA.begin(&servidorHTTP);


    // Route for root / web page
    servidorHTTP.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(LittleFS, "/index.html", "text/html");
    });

    // Route to load style.css file
    servidorHTTP.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(LittleFS, "/style.css", "text/css");
    });

    // Route to load script.js file
    servidorHTTP.on("/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(LittleFS, "/script.js", "text/javascript");
    });

       
    servidorHTTP.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
      const char* dta = ("data:image/x-icon;base64,AAAB AAAAA");
      request->send(200, "image/x-icon", dta);        
    });

     servidorHTTP.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
      if( strcmp(data2eeprom.CONFIGURADO, "OK") != 0 ){
        LimpaDadosDispositivo();
        ResetTimers();
        ResetSPIFFS();
        ESP.restart();
      }
      else{
        strncpy(data2eeprom.CONFIGURADO, "NO", sizeof(data2eeprom.CONFIGURADO));
        EEPROM.put(EEPROM_START_ADDR, data2eeprom);
        EEPROM.commit();
        delay(10);
        ESP.restart(); 
      }
      
      /*LimpaDadosDispositivo();
      ResetTimers();
      ResetSPIFFS();
      ESP.restart();*/
    });


     servidorHTTP.on("/restart", HTTP_GET, [](AsyncWebServerRequest * request) {
      ESP.restart();
    });

     servidorHTTP.on("/chmod", HTTP_GET, [](AsyncWebServerRequest * request) {
      data2EEPROM data2eeprom = {0};
      EEPROM.get(EEPROM_START_ADDR, data2eeprom);
      if( strcmp(data2eeprom.Modo, "OTA") == 0 ){
        TrocarModoOperacao("OPRA"); // TODO: Voltar no modo anterior  
      } 
      else{
        TrocarModoOperacao("OTA");   
      }
    });


     servidorHTTP.on("/info", HTTP_GET, [](AsyncWebServerRequest * request) {
      
      File arquivo = LittleFS.open(FILE_DATA_SPIFFS, "r");
      uint32_t f_size = (size_t)arquivo.size();
      arquivo.close();

      //size_t fileSizeKB = f_size / 1024;
     #if defined(ESP8266) 
      //Memória Flash Total:
      uint32_t flashSize = ESP.getFlashChipRealSize();      
      flashSize /= 1024;

       //Memória RAM Total:
      uint32_t ramSize = ESP.getFreeContStack();     
      ramSize /= 1024;
      // Obter a quantidade de memória livre
      uint32_t freeHeap = ESP.getFreeHeap();
      // Calcular a quantidade de memória usada
      /*uint32_t usedMemory = ramSize - freeHeap;
      // Converter para kilobytes
      uint32_t usedMemoryKB = usedMemory / 1024;*/
      freeHeap /= 1024;

      FSInfo fs_info;
      LittleFS.info(fs_info);

      // Obter o tamanho total do sistema de arquivos LittleFS
      uint32_t totalSize = fs_info.totalBytes;

      // Obter o espaço livre disponível no sistema de arquivos LittleFS
      uint32_t freeSpace = fs_info.totalBytes - fs_info.usedBytes;
      uint32_t usedSPIFFSSpaceKB = fs_info.usedBytes / 1024;
    #elif defined(ESP32) 
      // Memória Flash Total:
      uint32_t flashSize = ESP.getFlashChipSize();
      flashSize /= 1024;
      // Memória RAM Total:
      uint32_t ramSize = ESP.getFlashChipSize() - ESP.getFreeHeap();
      // uint32_t ramSize = ESP.getFreeContStack();
      ramSize /= 1024;
      // Obter a quantidade de memória livre
      uint32_t freeHeap = ESP.getFreeHeap();
      freeHeap /= 1024;  

      // Obter o tamanho total do sistema de arquivos LittleFS
      uint32_t totalSize = LittleFS.totalBytes();

      // Obter o espaço livre disponível no sistema de arquivos LittleFS
      uint32_t freeSpace = LittleFS.totalBytes() - LittleFS.usedBytes();
      uint32_t usedSPIFFSSpaceKB = LittleFS.usedBytes() / 1024;
    #endif
      // Converter para kilobytes
      uint32_t totalSPIFFSSizeKB = totalSize / 1024;
      uint32_t freeSPIFFSSpaceKB = freeSpace / 1024;

      char msg[512];
      sprintf(
        msg,"\nID: %d\nMAC: %s\n\nFLASH Total: %d KB\nRAM Total: %d KB\nRAM Livre: %d KB\n\nSPIFFS Total: %d KB\nTamanho do arquivo SPIFFS (%s): %d Bytes\nSPIFFS Usado: %d KB\nSPIFFS Livre: %d KB\n\n",
      BOARD_ID, WiFi.macAddress().c_str(),flashSize, ramSize, freeHeap, totalSPIFFSSizeKB, FILE_DATA_SPIFFS, f_size, usedSPIFFSSpaceKB, freeSPIFFSSpaceKB);
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", msg);
      response->addHeader("Content-Type", "text/plain; charset=utf-8");
      request->send(response);
    });


    // Processa comandos recebidos através de requisições HTTP/GET 
      servidorHTTP.on("/cm", HTTP_GET, [](AsyncWebServerRequest * request) {

        //char cpy_cz_json[JSON_DOC_SIZE];
        char cz_json[JSON_DOC_SIZE];       
        json_doc.clear();

        if (request->hasParam(CMND)) {
          strncpy(cz_json, request->getParam(CMND)->value().c_str(), sizeof(cz_json));
        
          //StaticJsonDocument<JSON_DOC_SIZE> doc; 
          json_doc.clear();    
          deserializeJson(json_doc, cz_json);
          const char* cz_cmd = json_doc["CMD"];                         

          if (json_doc.isNull()) {
            return;
          }

          if ( strcmp(cz_cmd, "CANCEL_OTA") == 0 ) { // Cancelar Modo CFG
            TrocarModoOperacao("OPR");
            request->send(200, "text/plain", "");                        
            // Reinicialioza o Dispositivo
            ESP.restart();  
          }
          else if ( strcmp(cz_cmd, "RESET_DEVICE") == 0 ) { // Cancelar Modo CFG
            LimpaDadosDispositivo();
            TrocarModoOperacao("OPR");
            request->send(200, "text/plain", "");                        
            // Reinicialioza o Dispositivo
            ESP.restart();  
          }          
          else if( strcmp(cz_cmd, "RENAME_DEV") == 0 ){

            const char* cz_new_name = json_doc["Nome"]; 
            EEPROM.get(EEPROM_START_ADDR, data2eeprom);
            strncpy(data2eeprom.nome, cz_new_name, sizeof(data2eeprom.nome)); 
            EEPROM.put(EEPROM_START_ADDR, data2eeprom);
            EEPROM.commit();
            delay(10);

            request->send(200, "text/plain", "OK");   
          }
          else if(strcmp(cz_cmd, "GET_DEV_NAME") == 0){
            request->send(200, "text/plain", data2eeprom.nome); 
          }         
                   
        }       

      });   

      // Start server
      servidorHTTP.begin();  
      ctrl_flash_led = millis();     

    // Inicializa Telnet  
      #if TELNET == 1
        TelnetStream.begin();
      #endif
  }  
  
  // Aumenta Potencia de Tx do WiFi
  aumentarPotenciaTxWiFi(); 
  
}
 

////////////
// loop() //
////////////
void loop() {


  if( strcmp(data2eeprom.Modo, "OPR") == 0){

    if(exec_toggle == true){
      exec_toggle = false;  
      #if ESPNOW_TASMOTA_BRIDGE == 1                   
        SendCommandtasmota("Power TOGGLE", 13);
      #else    
        TogglePIN();      
      #endif  
    }

     #if ESPNOW_TASMOTA_BRIDGE == 1      

      while (Serial.available() > 0){

        static String data;
        char rx_byte = Serial.read();
        data += rx_byte;

        if (rx_byte == '\n') {
          processarMSG_2(data);
          data = "";
        }
      }

    #endif 


    #if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1      

      while (Serial.available() > 0){

        static String data;
        char rx_byte = Serial.read();
        data += rx_byte;

        if (rx_byte == '\n') {
          processarMSG(data);
          data = "";
        }
      }

    #endif 

  

    // Verificar se o servidor está ligado.
    if( (millis() - ctrl_server_alive)/1000 >= TMP_CTRL_SERVER_ALIVE ){
      // Entra em modo de emparelhamento.  
      strncpy(pairingStatus, "PRT", sizeof(pairingStatus));     
      ctrl_server_alive = millis();
    }

  
    // auto emarelhamento
    if ( strcmp(autoPairing(), "PAIR_PAIRED") == 0) { 

      // Envia msg alive cada X tempo.
      if( (millis() - cont_alive)/1000 >= TEMPO_ENVIAR_ALIVE){    
      
        // Actualiza estado do dispositivo sonoff POW 
        #if ESPNOW_TASMOTA_BRIDGE == 1          
          SendCommandtasmota("Power", 6);         
          delay(50);  
          SendCommandtasmota("Status 8", 9);        
        #endif
             
        cont_alive = millis();  

        // Correcção da Data
        if (year() < YEAR_COMPARE){
          char czjson[JSON_DOC_SIZE]; 
          strcpy(czjson, CriarJSON("", "GDT", WiFi.macAddress().c_str(), BOARD_ID, 2, WiFi.channel()));
          esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));
        }


        // Checar Timer cada X tempo
        if((data2eeprom.TIPO != PASSIVO) && (millis() - cont_exec_timer)/1000 > 1 ){
          cont_exec_timer = millis();
          ExecutarTimer(FILE_DATA_SPIFFS);  
          #if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1  
            ExecutarTimerZb(FILE_ZBTMRS_SPIFFS); 
          #endif       
        }
        
      }  
    }


    // Envia msg alive cada X tempo.
    if((data2eeprom.TIPO != PASSIVO) && (millis() - cont_send_alive)/1000 >= TEMPO_ENVIAR_ALIVE ){    
     
  

      char czjson[JSON_DOC_SIZE];     
     
      uint8_t estado = 0;
      #if POWER320D == 1
        estado = relayState;
      #else
        estado = digitalRead(PIN_RELAY); 
      #endif  

    #if INVERT_PIN == 1
      #if POWER320D == 1
        estado = !relayState;
      #else
        estado = !digitalRead(PIN_RELAY); 
      #endif  
    #endif

      LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);  
      EEPROM.get(EEPROM_START_ADDR, data2eeprom); 

      //TODO: Processar Timers a serem executados      
      timers2EEPROM tmrs[QTD_TMRS];
      EEPROM.get(EEPROM_TIMERS_ADDR, tmrs);
      char cz_dt_tmr[DT_STR_SIZE];     
      uint8_t hora_menor = 0;
      uint8_t minuto_menor = 0; 
      uint8_t guarda_indice = 0; 
      uint8_t accao_timer = 0;

      // Obtem a posicao do elemento no array das estructuras "zbtimers2SPIFFS"
      for (uint8_t indice = 0; indice < QTD_TMRS; indice++){
        if( tmrs[indice].weekDays[weekday() - 1] == '1' && tmrs[indice].activar == 1){
          if( ((tmrs[indice].horas*60) + tmrs[indice].minutos) > ((hour()*60) + minute()) ){
            hora_menor = tmrs[indice].horas;
            minuto_menor = tmrs[indice].minutos;
            guarda_indice = indice;  
            break;  
          }                    
        }       
      } 


      // Procurar hora menor
      for (uint8_t i = (guarda_indice + 1); i < QTD_TMRS; i++) { // Percorre o array a partir do segundo elemento
        if( tmrs[i].weekDays[weekday() - 1] == '1' && tmrs[i].activar == 1){          
          if ( ((tmrs[i].horas*60) + tmrs[i].minutos) < ((hora_menor)*60) + minuto_menor) { 
            hora_menor = tmrs[i].horas;
            minuto_menor = tmrs[i].minutos;
            guarda_indice = i;            
          }                
        }         
      }

      
     if(((hora_menor*60) + minuto_menor) > ((hour()*60) + minute())){
        sprintf(cz_dt_tmr, "%d:%02d:%02d, %d/%d/%d", hora_menor, minuto_menor, 0, day(), month(), year()); 
        accao_timer = tmrs[guarda_indice].action;
      }  
      else{
        sprintf(cz_dt_tmr, "%s:%s:%s, %d/%d/%d", "__", "__", "__", day(), month(), year()); 
        accao_timer = 2;
      }

      // Envia resposta para actualização de estado nas páginas web.
      //strcpy(czjson, CriarJSON(data2spiffs.DT, "SAC", data2eeprom.nome, BOARD_ID, estado, WiFi.channel()));
      strcpy(czjson, CriarJSON(cz_dt_tmr, "SAC", data2eeprom.nome, BOARD_ID, estado, accao_timer));
      //czjson[strlen(czjson)] = '\0';      
      esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));


      // Enviar alive ZicBee
      #if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1
        for (uint8_t cont = 0; (cont < QTD_DEVS_ZIGBEE); cont++){ // Start_for1

          if (arr_zb_devs[cont].zb_board_id > 0 &&  arr_zb_devs[cont].Reachable == true){
            strcpy(czjson, CriarJSON(data2spiffs.DT, "SAC", arr_zb_devs[cont].name, arr_zb_devs[cont].zb_board_id, arr_zb_devs[cont].power, WiFi.channel()));
            //czjson[strlen(czjson)] = '\0';
            esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));
            delay(100);             
          }
                
        } // End_for1        
      #endif   
      

      #if IS_PASSIVE_MASTER == 1
        delay(100);
        // Envia resposta para actualização de estado no Dispositivo PASSIVO associado.
        strcpy(czjson, CriarJSON(data2spiffs.DT, "SAP", "PASSIVO", PASSIVE_SLAVE_ID, estado, WiFi.channel()));
        //czjson[strlen(czjson)] = '\0';
        esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));
      #endif

      cont_send_alive = millis();       
    }    


  } 
  else  if(strcmp(data2eeprom.Modo, "OTA") == 0){
    
    // Tratar OTA
    //ArduinoOTA.handle();
    
    // Piscar o LED cada X tempo
    if( (millis() - ctrl_flash_led) >= 200){
      digitalWrite(PIN_LED, !digitalRead(PIN_LED));
      ctrl_flash_led = millis();
    } 

    #if TELNET == 1
      LeerTelnet();  
      delay(2);
    #endif

  } 

} // FIM de loop()




/////////////////////////////////////////
// Atecnção à interrupção do PIN GPIO0 //
/////////////////////////////////////////
// ICACHE_RAM_ATTR IRAM_ATTR
void IRAM_ATTR attInterruptFallPIN() {    
  #if SHELLY1L == 0
    noInterrupts();
    ctl_btn_pulso = millis();
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptRisePIN, RISING);  
    interrupts();
  #elif SHELLY1L == 1 
    #if TELNET == 1
      TelnetStream.print("FALL -- ");
      TelnetStream.println(digitalRead(PIN_BUTTON));
    #endif  
    noInterrupts();
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptRisePIN, RISING);  
    exec_toggle = true; 
    interrupts();
  #endif
}


///////////////////////////////////////////////////
// Atecnção à interrupção do PIN GPIO4 SHELLY 1L //
///////////////////////////////////////////////////
void IRAM_ATTR  attInterruptChangePIN(){  
  #if SHELLY1L == 1 
    #if TELNET == 1
      TelnetStream.print("CHANGE -- ");
      TelnetStream.println(digitalRead(PIN_BUTTON));
    #endif
    noInterrupts();    
    if(digitalRead(PIN_BUTTON)){
      attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptFallPIN, FALLING);  
      exec_toggle = true; 
    }else{
      attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptRisePIN, RISING);  
      exec_toggle = true; 
    }

    interrupts();
   #endif
}


/////////////////////////////////////////
// Atecnção à interrupção do PIN GPIO0 //
/////////////////////////////////////////
// ICACHE_RAM_ATTR IRAM_ATTR
void IRAM_ATTR attInterruptRisePIN(){
  #if SHELLY1L == 0  
    noInterrupts();
    uint32_t tmp = ((millis() - ctl_btn_pulso));    
  
    if( tmp >= 3000 ){          
      if( strcmp(data2eeprom.Modo, "OTA") == 0 ){
        TrocarModoOperacao("OPR"); // TODO: Voltar no modo anterior  
      } 
      else{
        TrocarModoOperacao("OTA");   
      }      
    } 
    else if( tmp < 3000 && tmp >= 100 ){
      exec_toggle = true;       
    }  

    ctl_btn_pulso = millis();
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptFallPIN, FALLING);  
  #elif SHELLY1L == 1 
    #if TELNET == 1
      TelnetStream.print("RAISE -- ");
      TelnetStream.println(digitalRead(PIN_BUTTON));
    #endif
    noInterrupts();
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptFallPIN, FALLING);  
    exec_toggle = true; 
    interrupts();
  #endif
}


///////////////////////////////
// Gestão das connexões WiFi //
///////////////////////////////
RedeWifi::RedeWifi(const char* ssid, const char* pwd) {
  _ssid = ssid;
  _pwd = pwd;
}



//////////////////////////////////
// Conecta à Rede Wifi STA_MODE //
//////////////////////////////////
void RedeWifi::ConectaRedeWifi(const char* STA_IP_MODE) { // STA_IP_MODE = [DHCP | STATIC]
  /*uint8_t novoMAC[MAC_BYTE_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xB0};
  WiFi.mode(WIFI_STA);  

#if defined(ESP8266)
  // For Soft Access Point (AP) Mode
  // wifi_set_macaddr(SOFTAP_IF, &novoMAC[0]);
  wifi_set_macaddr(0, &novoMAC[0]);
#elif defined(ESP32)
  // ESP32 Board add-on after version > 1.0.5
  esp_wifi_set_mac(WIFI_IF_STA, &novoMAC[0]);
  // ESP32 Board add-on before version < 1.0.5
  //esp_wifi_set_mac(ESP_IF_WIFI_STA, &novoMAC[0]);
#endif*/


  char aux[IP_MODE_SIZE];
  if( strcmp(STA_IP_MODE, "") == 0){
    strncpy(aux, "DHCP", IP_MODE_SIZE);
  }
  else{
    strncpy(aux, STA_IP_MODE, IP_MODE_SIZE);
  }

  if( strcmp(aux, "DHCP") == 0 ){
    WiFi.begin(_ssid, _pwd);
  }
  else if( strcmp(aux, "STATIC") == 0 ){   
    data2EEPROM data2eeprom = {0};    
    EEPROM.get(EEPROM_START_ADDR, data2eeprom);
   
    IPAddress ip(data2eeprom.ip);
    IPAddress gateway(data2eeprom.gateway);
    IPAddress dns1(data2eeprom.dns1);
    IPAddress dns2(data2eeprom.dns2);
    IPAddress subnet(data2eeprom.subnet);
    WiFi.config(ip, gateway, subnet, dns1, dns2);
    WiFi.begin(_ssid, _pwd);
  }

 
  
  uint8_t contador = 0;
  while (WiFi.status() != WL_CONNECTED && contador < MAX_TIME_CONN) {
    delay(1000);
    imprimeln("Conectando-se a: " + (String)_ssid + " ... " + String(contador++) + " Seg");
  }

  if (WiFi.status() == WL_CONNECTED) {
    imprimeln("");
    imprimeln(F("================="));
    imprimeln(F("Rede WiFi Ligada."));
    imprimeln(F("================="));
    imprimeln("SSID: " + WiFi.SSID() + "\nIP: " + WiFi.localIP().toString());
    imprimeln("MAC: " + WiFi.macAddress());   
  }
  else{
    imprimeln("");
    imprimeln(F("Rede WiFi Indisponível!"));
    TrocarModoOperacao("OPR"); 
  } 

}




/////////////////////////////
// Criar Rede Wifi AP_MODE //
/////////////////////////////
void RedeWifi::CriaRedeWifi(const char* AP_IP_MODE) {// AP_IP_MODE = [DEFAULT | CUSTOM]
/*uint8_t novoMAC[MAC_BYTE_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xB0};
  WiFi.mode(WIFI_AP_STA);
  
#if defined(ESP8266)
  // For Soft Access Point (AP) Mode
  //wifi_set_macaddr(SOFTAP_IF, &novoMAC[0]);
  wifi_set_macaddr(0, &novoMAC[0]);
#elif defined(ESP32)
  // ESP32 Board add-on after version > 1.0.5
  esp_wifi_set_mac(WIFI_IF_STA, &novoMAC[0]);
  // ESP32 Board add-on before version < 1.0.5
  //esp_wifi_set_mac(ESP_IF_WIFI_STA, &novoMAC[0]);
#endif*/

  char aux[IP_MODE_SIZE];
  if( strcmp(AP_IP_MODE, "") == 0){
    strncpy(aux, "DEFAULT", IP_MODE_SIZE);
    aux[strlen(aux)] = '\0';
  }
  else{
    strncpy(aux, AP_IP_MODE, IP_MODE_SIZE);
    aux[strlen(aux)] = '\0';
  }

  if( strcmp(aux, "DEFAULT") == 0 ){
    WiFi.softAP(_ssid, _pwd);
  }
  else if( strcmp(aux, "CUSTOM") == 0 ){
    
    DadosRede dados_rede = {
      IPAddress(192, 168, 10, 1),   // IP
      IPAddress(192, 168, 10, 1),   // GateWay
      IPAddress(255, 255, 255, 0),  // Máscara de sub-rede
      false,                        // HIDE_SSID
      6,                            // CHANNEL
      QTD_BOARDS,                   // MAX_CONNECTIONS
      false                         // true -> rede_default (192.168.4.1/24); false -> Rede definida
    };

    WiFi.softAPConfig(dados_rede.ip, dados_rede.gateway, dados_rede.subnet);
    WiFi.softAP(_ssid, _pwd, dados_rede.CHANNEL, dados_rede.HIDE_SSID, dados_rede.MAX_CONNECTIONS);   
    
  }

  imprimeln("");  
  imprimeln(F("======================="));
  imprimeln(F("Ponto de Acesso Criado."));
  imprimeln(F("======================="));
  imprimeln("SSID: " + WiFi.softAPSSID() + "\nIP: " + WiFi.softAPIP().toString());
  imprimeln("MAC: " + WiFi.macAddress());
   
}



////////////////////////////////////////////
// Limppa EEPROM, altera modo de operação //
////////////////////////////////////////////
void LimpaDadosDispositivo(){  

  data2EEPROM data2eeprom = {0};
  memset(&data2eeprom, 0, sizeof(data2eeprom));
  EEPROM.put(EEPROM_START_ADDR, data2eeprom);  
  EEPROM.commit();
  delay(10);

  EEPROM.get(EEPROM_START_ADDR, data2eeprom); 

  // Guardar informações padrão na EEPROM.
  strncpy(data2eeprom.Modo, "OTA", sizeof(data2eeprom.Modo));
  data2eeprom.Modo[strlen(data2eeprom.Modo)] = '\0';
  strncpy(data2eeprom.SSID, "", sizeof(data2eeprom.SSID)); 
  data2eeprom.SSID[strlen(data2eeprom.SSID)] = '\0';
  strncpy(data2eeprom.PWD, "", sizeof(data2eeprom.PWD));
  data2eeprom.PWD[strlen(data2eeprom.PWD)] = '\0';
  strncpy(data2eeprom.nome, "[Nome]", sizeof(data2eeprom.nome));  
  data2eeprom.nome[strlen(data2eeprom.nome)] = '\0'; 
  //strncpy(data2eeprom.AP_STA_IP_MODE, "", sizeof(data2eeprom.AP_STA_IP_MODE));
  strncpy(data2eeprom.CONFIGURADO, "OK", sizeof(data2eeprom.CONFIGURADO));
  data2eeprom.CONFIGURADO[strlen(data2eeprom.CONFIGURADO)] = '\0';
  memset(data2eeprom.ip, 0, sizeof(data2eeprom.ip));
  memset(data2eeprom.gateway, 0, sizeof(data2eeprom.gateway));
  memset(data2eeprom.subnet, 0, sizeof(data2eeprom.subnet));
  memset(data2eeprom.dns1, 0, sizeof(data2eeprom.dns1));
  memset(data2eeprom.dns2, 0, sizeof(data2eeprom.dns2)); 
  //strncpy(data2eeprom.SOCKSERVERADDR, "", sizeof(data2eeprom.SOCKSERVERADDR));
  //data2eeprom.SOCKSERVERPORT = 0;

  data2eeprom.TIPO = lAMPADA;//lAMPADA;// PASSIVO;//E_BOMBA;//HUB_IR; // Lâmpada por defeito
 
  EEPROM.put(EEPROM_START_ADDR, data2eeprom);  
  EEPROM.commit();
  delay(10); 

}



//////////////////////////////////////////////
// Mudar Modo de Operação (OTA, OPRA, OPRS) //
//////////////////////////////////////////////
void TrocarModoOperacao(const char* cz_modo){
  data2EEPROM data2eeprom = {0};
  EEPROM.get(EEPROM_START_ADDR, data2eeprom);

  //strncpy(device_data.save_Modo, device_data.Modo, sizeof(device_data.Modo));
  strncpy(data2eeprom.Modo, cz_modo, sizeof(data2eeprom.Modo));
    
  EEPROM.put(EEPROM_START_ADDR, data2eeprom);
  EEPROM.commit();
  delay(10);
 
  ESP.restart();
}



////////////////////////////////////////////////////////////
// Limpara ArquivoSPIFFS com pin_sttate, DT e Array Alive //
////////////////////////////////////////////////////////////
void ResetSPIFFS(){
  LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);

    strncpy(data2spiffs.DT, "00::00, 00/00/0000", sizeof(data2spiffs.DT));
    data2spiffs.estado_pin = 0;

    // TODO: Adaptar para Clientes que processa Código iR
    /*for (uint8_t index = 0; index < QTD_BOARDS; index++){
      data2spiffs.BOARDS_ID[index] = 255;
      array_alive[index].board_id =  255;
      array_alive[index].temp_alive =  0;
    }    
    data2spiffs.BOARDS_ID[0] = BOARD_ID;
    array_alive[0].board_id = BOARD_ID;
    array_alive[0].temp_alive = 0;*/

     // Limpar Timers Zigbee
    #if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1
    
      File arquivo = LittleFS.open(FILE_ZBTMRS_SPIFFS, "w"); 
  
      for (uint8_t index = 0; index < QTD_DEVS_ZIGBEE; index++){
        timersZB[index].action = 0;
        timersZB[index].activar = 0;
        timersZB[index].timer = 255;
        strcpy(timersZB[index].hora_minutos, "12:12");
        strncpy( timersZB[index].short_id, "", sizeof(timersZB[index].short_id) );
        strncpy( timersZB[index].weekDays, "0000000", sizeof(timersZB[index].weekDays) );
      }

      arquivo.write((byte *)&timersZB, sizeof(timersZB));
      arquivo.close();

    #endif    

    EscreverSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);
}


/////////////////////////////
// Funçaão para ler spiffs //
/////////////////////////////
void LerSPIFFS(data2SPIFFS *ddv, const char* nome_arquivo){
  File arquivo = LittleFS.open(nome_arquivo, "r");
  arquivo.read((byte *)ddv, sizeof(*ddv));
  arquivo.close();  
}



//////////////////////////////////
// Funçaão para escrever spiffs //
//////////////////////////////////
void EscreverSPIFFS(data2SPIFFS *ddv, const char* nome_arquivo){
  File arquivo = LittleFS.open(nome_arquivo, "w");
  arquivo.write((byte *)ddv, sizeof(*ddv));
  arquivo.close();  
}


////////////////////////////////////////////
// Limpar configurações de Temporizadores //
////////////////////////////////////////////
void ResetTimers(){
 
  timers2EEPROM timers[QTD_TMRS];

  for (uint8_t index = 0; index < QTD_TMRS; index++){
    timers[index].activar = 0;
    timers[index].horas = 12;
    timers[index].minutos = 12;
    strncpy(timers[index].weekDays, "0000000", 8);
    timers[index].weekDays[strlen(timers[index].weekDays)] = '\0'; 
    timers[index].output = 0;
    timers[index].action = 0;
  }

  EEPROM.put(EEPROM_TIMERS_ADDR, timers);  
  EEPROM.commit();
  delay(10);

}



//////////////////////////////////////////
// Callback, ESP-NOW, when data is sent //
//////////////////////////////////////////
#if defined(ESP8266) 
  void callback_tx_esp_now(uint8_t *mac_addr, uint8_t status) { 
    /*/ char macStr[18];
    imprime("Destino: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    imprime(macStr);
    imprime(" Status:\t");
  // imprime(status == ESP_NOW_SEND_SUCCESS ? "Successo." : "Falha: ");*/
}
#endif


//////////////////////////////////////////
// Callback, ESP-NOW, when data is sent //
//////////////////////////////////////////
#if defined(ESP32) 
  void callback_tx_esp_now(const uint8_t *mac_addr, esp_now_send_status_t status) {
    
    /*char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.println(macStr);*/

    /*char macStr[18];
    imprime("Destino: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    imprime(macStr);
    imprime(" Status:\t");    
    // imprime(status == ESP_NOW_SEND_SUCCESS ? "Successo." : "Falha: ");*/    
  }  
#endif



//////////////////////////////////////////////
// Callback, ESP-NOW, when data is received //
//////////////////////////////////////////////
#if defined(ESP8266) 
void callback_rx_esp_now(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  //Serial.println((const char*)incomingData);
  data2SPIFFS data2spiffs = {0};
  data2EEPROM data2eeprom = {0}; 
  char czjson[JSON_DOC_SIZE];
  //StaticJsonDocument <JSON_DOC_SIZE> dadosJson; 
  json_doc.clear();
  deserializeJson(json_doc, (const char*)incomingData);

  EEPROM.get(EEPROM_START_ADDR, data2eeprom);
  LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);

  if (json_doc.isNull()){
    imprimeln("JSON inválido/nulo!");
    return;
  }
  

  const char* cz_tipo_msg = json_doc["MSG"];
  const char* cz_cmd = json_doc["CMD"];
  const char* cz_mac = json_doc["MAC"];  
  uint32_t iz_id_dev = json_doc["ID"];
  uint8_t iz_pin_st = json_doc["PST"];
  uint8_t iz_canal_wifi = json_doc["WCH"];

  #if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1
    // Obter marca de devices zigbee, de tras para frente
    uint8_t quarto = obterDigitoInvertido(iz_id_dev, 1);
    uint8_t quinto = obterDigitoInvertido(iz_id_dev, 2); 
  #endif

  uint8_t* uz_mac = converteMacString2Byte(cz_mac);

  if( strcmp(cz_tipo_msg, "PNG") == 0){
    esp_now_add_peer(uz_mac, ESP_NOW_ROLE_COMBO, iz_canal_wifi, NULL, 0);
    strncpy(pairingStatus, "PAIR_PAIRED", sizeof(pairingStatus) );
    memcpy(seve_server_mac, uz_mac, 6);

    // Inicializa AP para a Ponte % ESP-NOW e TASMOTA
    #if ESPNOW_TASMOTA_BRIDGE == 1
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
    #else
      WiFi.mode(WIFI_STA);
    #endif

  }
  else if(strcmp(cz_tipo_msg, "PRT") == 0 ){
    bool exists = esp_now_is_peer_exist(uz_mac);
    if(!exists){
      esp_now_add_peer(uz_mac, ESP_NOW_ROLE_COMBO, iz_canal_wifi, NULL, 0);
      imprimeln("Emparelhado!");  
    }   
  }  
  else if( strncmp(cz_tipo_msg, "ALA", sizeof(pairingStatus)) == 0) {
    ctrl_server_alive = millis(); // Resetear conterolo do Servidor (ligado ou desligado)    
  }
  else if(strcmp(cz_tipo_msg, "DATA") == 0 ){
    
    // Processar Comando "TOGGLE", destinado ao device
    if(strcmp(cz_cmd, "TGL") == 0 && iz_id_dev == BOARD_ID && data2eeprom.TIPO != PASSIVO){      
      exec_toggle = true;       
    }
    # if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1
    else if(strcmp(cz_cmd, "TGL") == 0 && iz_id_dev != BOARD_ID  && quarto == 0 && quinto == 0 ){
      for (uint8_t cont = 0; (cont < QTD_DEVS_ZIGBEE); cont++){ // Start_for1

        if (arr_zb_devs[cont].zb_board_id == iz_id_dev){
          char data[52] = "ZbSend {\"Device\":\""; 
          strcat(data, arr_zb_devs[cont].short_id); 
          strcat(data, "\",\"Send\":{\"Power\":\"TOGGLE\"}}");           
          Serial.write(data, sizeof(data));        
          Serial.write('\n');          
          break;
        }
      }
    }
    #endif  
    # if ESPNOW_TASMOTA_BRIDGE == 1
      else if(strcmp(cz_cmd, "GWS") == 0 && iz_id_dev == BOARD_ID){ 
        SendCommandtasmota("WiFi", 5); // Solicita estado do WiFi Tasmota
        delay(100);
        SendCommandtasmota("Status 5", 9); // Solicita dados da conexao WiFi Tasmota
      }    
      else if(strcmp(cz_cmd, "TWN") == 0 && iz_id_dev == BOARD_ID){ 
        SendCommandtasmota("WiFi ON", 8); // Ligar WiFi Tasmota
      }    
      else if(strcmp(cz_cmd, "TWF") == 0 && iz_id_dev == BOARD_ID){ 
        SendCommandtasmota("WiFi OFF", 8); // Desligar WiFi Tasmota
      }   
    #endif  
    else if(strcmp(cz_cmd, "TGG") == 0 && iz_id_dev == BOARD_ID && data2eeprom.TIPO == PASSIVO){      
      
      if(iz_pin_st != digitalRead(PIN_RELAY)){
        digitalWrite(PIN_RELAY, iz_pin_st);
        LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);
        data2spiffs.estado_pin = iz_pin_st;
        EscreverSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);
      }     
    }
    else if(strcmp(cz_cmd, "GRLOTA") == 0 && data2eeprom.TIPO == PASSIVO){
      TrocarModoOperacao("OTA");
    }
  }
  else if(strcmp(cz_cmd, "UPT") == 0 ){   
    // Correcção da Data
    /*if (year() < YEAR_COMPARE){     
      setTime((uint32_t)dadosJson["MSG"]);      
    }*/   

    // Envia resposta para actualização de estado nas páginas web (código reaproveitado).    
    uint8_t pin_state = digitalRead(PIN_RELAY);   
#if INVERT_PIN == 1
    pin_state = !digitalRead(PIN_RELAY);
#endif

    strcpy(czjson, CriarJSON(data2spiffs.DT, "TGA", data2eeprom.nome, BOARD_ID, pin_state, WiFi.channel())); 
    //czjson[strlen(czjson)] = '\0';  
    esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));    
  }
  else if(strcmp(cz_cmd, "SDT") == 0 ){      
    setTime((uint32_t)json_doc["DT"]);    
  }
  else if( strcmp(cz_cmd, "GTM") == 0 && iz_id_dev == BOARD_ID){
   
    uint8_t iz_index = json_doc["Timer"];
    ConsultarTimer(FILE_DATA_SPIFFS, iz_index);
    // Enviar mensagem ao servidor, render.   
    esp_now_send(seve_server_mac, (uint8_t *)json_array, strlen(json_array)); 
    //imprimeln(json_array);
  }
  # if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1
  else if(strcmp(cz_cmd, "GTM") == 0 && iz_id_dev != BOARD_ID && quarto == 0 && quinto == 0){              
        
        char short_id[ZB_SHORT_ID];
        uint8_t indice = 0; 
        uint8_t iz_timer = json_doc["Timer"];
        
        // Obtem ID do dispositivos, naestructura "ZigbeeDevices"
        for (uint8_t index = 0; index < QTD_DEVS_ZIGBEE; index++){
          if(arr_zb_devs[index].zb_board_id == iz_id_dev){
            strcpy(short_id, arr_zb_devs[index].short_id); 
            //Serial.println(short_id);           
            break;
          }
        }

        // Obtem a posicao do elemento no array das estructuras "zbtimers2SPIFFS"
        for (uint8_t index = 0; index < QTD_DEVS_ZIGBEE; index++){
          if(strcmp(timersZB[index].short_id, short_id) == 0 && timersZB[index].timer == iz_timer){
            indice = index;  
            //Serial.printf("%d -- %d\n",timersZB[indice].timer, iz_timer);    
            break;
          }
          else{
            indice = 255;
          }
        }        

        //Serial.println(timersZB[indice].short_id);

        // Obter os dados do Timer correspondente
        LerZbSPIFFS(timersZB, FILE_ZBTMRS_SPIFFS);
        
        uint8_t enable = timersZB[indice].activar;
        uint8_t saida = timersZB[indice].output;
        uint8_t accao = timersZB[indice].action;
        char dias_semana[8];
        strncpy(dias_semana, timersZB[indice].weekDays, 8);

        char aux_hor[6];
        char aux_min[6];
        char hora[6]; 
        String horas = String(timersZB[indice].hora_minutos[0]) + String(timersZB[indice].hora_minutos[1]);
        String minutos = String(timersZB[indice].hora_minutos[3]) + String(timersZB[indice].hora_minutos[4]);
        
        uint8_t hours = horas.toInt();
        uint8_t minuts = minutos.toInt();

        if (hours <= 9) {
          snprintf(aux_hor, sizeof(aux_hor) - 1, "0%i", hours);
        }
        else {
          snprintf(aux_hor, sizeof(aux_hor) - 1, "%i", hours);
        }

        if (minuts <= 9) {
          snprintf(aux_min, sizeof(aux_min) - 1, "0%i", minuts);
        }
        else {
          snprintf(aux_min, sizeof(aux_min) - 1, "%i", minuts);
        }

        strcpy(hora, aux_hor);
        strcat(hora, ":");
        strcat(hora, aux_min);

        if( indice == 255){
          enable = 0;
          strcpy(hora, "12:12");
          strcpy(dias_semana, "0000000");
          saida = 1;
          accao = 0;
        }

        json_doc["CMD"] = "TCG";
        json_doc["Enable"] = enable;
        json_doc["Time"] = hora;
        json_doc["WeekDays"] = dias_semana;
        json_doc["Output"] = saida;
        json_doc["Action"] = accao;  
        serializeJson(json_doc, json_array);
        //json_array[strlen(json_array)] = '\0';
       
        esp_now_send(seve_server_mac, (uint8_t *)json_array, strlen(json_array));   
        json_doc.clear();      
  }  
  else if( strcmp(cz_cmd, "STM") == 0 && iz_id_dev != BOARD_ID && quarto == 0 && quinto == 0){      

    deserializeJson(json_doc, (const char*)incomingData); 
 
    // Actualiza regsidto indicado
    uint8_t offset = json_doc["Timer"];
    uint8_t iz_output = json_doc["Output"];
    uint8_t iz_action = json_doc["Action"];
    uint8_t iz_enable = json_doc["Enable"];
    uint32_t iz_id = json_doc["ID"];
    const char* hor_min = json_doc["Time"];
    const char* weedays = json_doc["WeekDays"];
    char short_id[ZB_SHORT_ID];
        
    // Obtem short_id do dispositivos
    for (uint8_t index = 0; index < QTD_DEVS_ZIGBEE; index++){
      if(arr_zb_devs[index].zb_board_id == iz_id){
        strcpy(short_id, arr_zb_devs[index].short_id);            
        break;
      }
    }

                     
    // Guarda/Actualiza Timers
    for (uint8_t index = 0; index < QTD_DEVS_ZIGBEE; index++){     
      // Carregar dados existentes no array "timersZB"
      LerZbSPIFFS(timersZB, FILE_ZBTMRS_SPIFFS);

      // Se o array esta vazio, preencher a primeira posicao  
      if(strcmp( timersZB[index].short_id, "") == 0 && index == 0){            
        strncpy(timersZB[index].short_id, short_id, sizeof(timersZB[index].short_id));
        timersZB[index].action = iz_action;
        timersZB[index].activar = iz_enable;
        timersZB[index].output = iz_output;
        timersZB[index].timer = offset;
        strcpy(timersZB[index].hora_minutos, hor_min);
        strncpy(timersZB[index].weekDays, weedays, sizeof(timersZB[index].weekDays));
        // Guardar Timers
        EscreverZbSPIFFS(timersZB, FILE_ZBTMRS_SPIFFS);
        break;
      }
      else if(strcmp( timersZB[index].short_id, short_id) == 0 && timersZB[index].timer == offset ){
        // Se o timer ja existe, actualiza
        strncpy(timersZB[index].short_id, short_id, sizeof(timersZB[index].short_id));
        timersZB[index].action = iz_action;
        timersZB[index].activar = iz_enable;
        timersZB[index].output = iz_output;            
        strcpy(timersZB[index].hora_minutos, hor_min);
        strncpy(timersZB[index].weekDays, weedays, sizeof(timersZB[index].weekDays));
        // Guardar Timers
        EscreverZbSPIFFS(timersZB, FILE_ZBTMRS_SPIFFS);
        break;           
      }
      else if(strcmp( timersZB[index].short_id, short_id) != 0 && strcmp( timersZB[index].short_id, "") == 0){
        // Se o timer nao existe, regista
        strncpy(timersZB[index].short_id, short_id, sizeof(timersZB[index].short_id));
        timersZB[index].action = iz_action;
        timersZB[index].activar = iz_enable;
        timersZB[index].output = iz_output;
        timersZB[index].timer = offset;
        strcpy(timersZB[index].hora_minutos, hor_min);
        strncpy(timersZB[index].weekDays, weedays, sizeof(timersZB[index].weekDays));
        // Guardar Timers
        EscreverZbSPIFFS(timersZB, FILE_ZBTMRS_SPIFFS);
        break;           
      }        
    } 
    //Serial.println(short_id); 
  }
  #endif
  else if( strcmp(cz_cmd, "STM") == 0 && (iz_id_dev == BOARD_ID || iz_id_dev == ID_BROADCAST_ESPNOW) ){      
    ConfigurarTimer(cz_tipo_msg, FILE_DATA_SPIFFS);         
  }
  else if( strcmp(cz_cmd, "RSD") == 0 && iz_id_dev == BOARD_ID){       
     strncpy(data2eeprom.CONFIGURADO, "NO", sizeof(data2eeprom.CONFIGURADO));
     EEPROM.put(EEPROM_START_ADDR, data2eeprom);
     EEPROM.commit();
     delay(10);
     ESP.restart(); 
  }
  else if( strcmp(cz_cmd, "RNM") == 0 && iz_id_dev != BOARD_ID){
    # if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1
    for (uint8_t cont = 0; (cont < QTD_DEVS_ZIGBEE); cont++){ // Start_for1

      if ( arr_zb_devs[cont].zb_board_id == iz_id_dev){
        String cmnd = "ZbName " + String(arr_zb_devs[cont].name) + "," + String(cz_mac);
        uint8_t var_len = cmnd.length() + 1;
        char data[var_len];
        cmnd.toCharArray(data, var_len);         
        Serial.write(data, sizeof(data));        
        Serial.write('\n');          
        break;
      }
    }
    #endif
  }
  else if( strcmp(cz_cmd, "ADS") == 0 && iz_id_dev == BOARD_ID){
    char cz_dt_tmr[DT_STR_SIZE];
    sprintf(cz_dt_tmr, "%s:%s:%s, %d/%d/%d", "__", "__", "__", day(), month(), year()); 
    uint8_t estado = digitalRead(PIN_RELAY);   
    strcpy(czjson, CriarJSON(cz_dt_tmr, "SAC", data2eeprom.nome, BOARD_ID, estado, 2));
    //czjson[strlen(czjson)] = '\0';      
    esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));
  }
  else if( strcmp(cz_cmd, "RNM") == 0 && iz_id_dev == BOARD_ID){    

    strncpy(data2eeprom.nome, cz_mac, sizeof(data2eeprom.nome));
    EEPROM.put(EEPROM_START_ADDR, data2eeprom);
    EEPROM.commit();
    delay(10);
    uint8_t pin_state = digitalRead(PIN_RELAY);
#if INVERT_PIN == 1
    pin_state = !digitalRead(PIN_RELAY);
#endif

    //StaticJsonDocument <JSON_DOC_SIZE> json_doc; 
    json_doc.clear();
    json_doc["MSG"] = data2spiffs.DT;  
    json_doc["CMD"] = "RAN";
    json_doc["ID"] = BOARD_ID;  
    json_doc["PST"] = pin_state;
    json_doc["WCH"] = 0;
    json_doc["MAC"] = cz_mac;
    serializeJson(json_doc, json_array);
    //json_array[strlen(json_array)] = '\0'; 
    esp_now_send(seve_server_mac, (uint8_t *)json_array, strlen(json_array)); 
  }
  else if( strcmp(cz_cmd, "ENABLE_OTA") == 0 && iz_id_dev == BOARD_ID){      
    TrocarModoOperacao("OTA");
  }
  # if ESPNOW_TASMOTA_BRIDGE == 1
    else if(strcmp(cz_cmd, "TCW") == 0 && iz_id_dev == BOARD_ID){      
      // Conectar Tasmota a Rede WiFi
      char cmnd[512];
      const char* cz_ssid = json_doc["MSG"];
      const char* cz_senha = json_doc["MAC"];

      sprintf(cmnd, "Backlog SSID1 %s; Password1 %s; Ipaddress1 0.0.0.0; restart 1", cz_ssid, cz_senha);
      Serial.write(cmnd, strlen(cmnd));      
      Serial.write('\n'); 
    }
  #endif
  

  free(uz_mac);
  
}// end callback_rx_esp_now(...)
#endif


//////////////////////////////////////////////
// Callback, ESP-NOW, when data is received //
//////////////////////////////////////////////
#if defined(ESP32) 
void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len){ 
  
  //imprimeln((const char*)incomingData);

  data2SPIFFS data2spiffs = {0};
  data2EEPROM data2eeprom = {0}; 
  char czjson[JSON_DOC_SIZE];
  //StaticJsonDocument <JSON_DOC_SIZE> dadosJson; 
  json_doc.clear();
  deserializeJson(json_doc, (const char*)incomingData);

  EEPROM.get(EEPROM_START_ADDR, data2eeprom);
  LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);

  if (json_doc.isNull()){
    imprimeln("JSON inválido/nulo!");
    return;
  }


  const char* cz_tipo_msg = json_doc["MSG"];
  const char* cz_cmd = json_doc["CMD"];
  const char* cz_mac = json_doc["MAC"];  
  uint32_t iz_id_dev = json_doc["ID"];
  uint8_t iz_pin_st = json_doc["PST"];
  uint8_t iz_canal_wifi = json_doc["WCH"];

  // Obter marca de devices zigbee, de tras para frente
  uint8_t quarto = obterDigitoInvertido(iz_id_dev, 1);
  uint8_t quinto = obterDigitoInvertido(iz_id_dev, 2); 

  uint8_t* uz_mac = converteMacString2Byte(cz_mac);

  if( strcmp(cz_tipo_msg, "PNG") == 0){
    #if defined(ESP32) 
      memcpy(peerInfo.peer_addr, uz_mac, 6);
      peerInfo.channel = iz_canal_wifi;
      peerInfo.encrypt = false;
      esp_now_add_peer(&peerInfo);
    #elif defined(ESP8266) 
      esp_now_add_peer(uz_mac, ESP_NOW_ROLE_COMBO, iz_canal_wifi, NULL, 0);
    #endif
    
    strncpy(pairingStatus, "PAIR_PAIRED", sizeof(pairingStatus) );
    memcpy(seve_server_mac, uz_mac, 6);
  }
  else if(strcmp(cz_tipo_msg, "PRT") == 0 ){
    bool exists = esp_now_is_peer_exist(uz_mac);
    if(!exists){
      #if defined(ESP32) 
      memcpy(peerInfo.peer_addr, uz_mac, 6);
      peerInfo.channel = iz_canal_wifi;
      peerInfo.encrypt = false;
      esp_now_add_peer(&peerInfo);
    #elif defined(ESP8266) 
      esp_now_add_peer(uz_mac, ESP_NOW_ROLE_COMBO, iz_canal_wifi, NULL, 0);
    #endif
      
      imprimeln(F("Emparelhado!"));  
    }   
  }  
  else if( strncmp(cz_tipo_msg, "ALA", sizeof(pairingStatus)) == 0) {
    ctrl_server_alive = millis(); // Resetear conterolo do Servidor (ligado ou desligado)    
  }
  else if(strcmp(cz_tipo_msg, "DATA") == 0 ){
    // Processar Comando "TOGGLE", destinado ao device
    if(strcmp(cz_cmd, "TGL") == 0 && iz_id_dev == BOARD_ID && data2eeprom.TIPO != PASSIVO){      
      exec_toggle = true;         
    }
    else if(strcmp(cz_cmd, "TGL") == 0 && iz_id_dev != BOARD_ID  && quarto == 0 && quinto == 0){
      for (uint8_t cont = 0; (cont < QTD_DEVS_ZIGBEE); cont++){ // Start_for1

        if (arr_zb_devs[cont].zb_board_id == iz_id_dev){
          char data[52] = "ZbSend {\"Device\":\""; 
          strcat(data, arr_zb_devs[cont].short_id); 
          strcat(data, "\",\"Send\":{\"Power\":\"TOGGLE\"}}");           
          Serial.write(data, sizeof(data));        
          Serial.write('\n');          
          break;
        }
      }
    }
    # if ESPNOW_TASMOTA_BRIDGE == 1
      else if(strcmp(cz_cmd, "GWS") == 0 && iz_id_dev == BOARD_ID){ 
        SendCommandtasmota("WiFi", 5); // Solicita estado do WiFi Tasmota
        delay(100);
        SendCommandtasmota("Status 5", 9); // Solicita dados da conexao WiFi Tasmota
      }    
    #endif  
    else if(strcmp(cz_cmd, "TGG") == 0 && iz_id_dev == BOARD_ID && data2eeprom.TIPO == PASSIVO){      
      uint8_t estado = 0;
      #if POWER320D == 1
        estado = relayState;
      #else  
        estado = digitalRead(PIN_RELAY);
      #endif
      
      if(iz_pin_st != estado){     

        #if POWER320D == 1
          OnOffBISTABLE(iz_pin_st, BISTABLE_ON, BISTABLE_OFF);
        #else  
          digitalWrite(PIN_RELAY, iz_pin_st);
        #endif

        LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);
        data2spiffs.estado_pin = iz_pin_st;        
        EscreverSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);
      }   
    }
    else if(strcmp(cz_cmd, "GRLOTA") == 0 && data2eeprom.TIPO == PASSIVO){
      TrocarModoOperacao("OTA");
    }
  }
  else if(strcmp(cz_cmd, "UPT") == 0 ){   
    // Envia resposta para actualização de estado nas páginas web (código reaproveitado).    
   
    uint8_t pin_state = 0;
    #if POWER320D == 1
      pin_state = relayState;
    #else  
      pin_state = digitalRead(PIN_RELAY);
    #endif

    #if INVERT_PIN == 1
      #if POWER320D == 1
        pin_state = !relayState;
      #else  
        pin_state = !digitalRead(PIN_RELAY);
      #endif
    #endif

    strcpy(czjson, CriarJSON(data2spiffs.DT, "TGA", data2eeprom.nome, BOARD_ID, pin_state, WiFi.channel())); 
    //czjson[strlen(czjson)] = '\0';  
    esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));    
  }
  else if(strcmp(cz_cmd, "SDT") == 0 ){      
    setTime((uint32_t)json_doc["DT"]);    
  }
  else if( strcmp(cz_cmd, "GTM") == 0 && iz_id_dev == BOARD_ID){
   
    uint8_t iz_index = json_doc["Timer"];
    ConsultarTimer(FILE_DATA_SPIFFS, iz_index);
    // Enviar mensagem ao servidor, render.   
    esp_now_send(seve_server_mac, (uint8_t *)json_array, strlen(json_array)); 
    //imprimeln(json_array);
  }
   else if(strcmp(cz_cmd, "GTM") == 0 && iz_id_dev != BOARD_ID && quarto == 0 && quinto == 0){              
        
        char short_id[ZB_SHORT_ID];
        uint8_t indice = 0; 
        uint8_t iz_timer = json_doc["Timer"];
        
        // Obtem ID do dispositivos, naestructura "ZigbeeDevices"
        for (uint8_t index = 0; index < QTD_DEVS_ZIGBEE; index++){
          if(arr_zb_devs[index].zb_board_id == iz_id_dev){
            strcpy(short_id, arr_zb_devs[index].short_id); 
            //Serial.println(short_id);           
            break;
          }
        }

        // Obtem a posicao do elemento no array das estructuras "zbtimers2SPIFFS"
        for (uint8_t index = 0; index < QTD_DEVS_ZIGBEE; index++){
          if(strcmp(timersZB[index].short_id, short_id) == 0 && timersZB[index].timer == iz_timer){
            indice = index;  
            //Serial.printf("%d -- %d\n",timersZB[indice].timer, iz_timer);    
            break;
          }
          else{
            indice = 255;
          }
        }        

        //Serial.println(timersZB[indice].short_id);

        // Obter os dados do Timer correspondente
        LerZbSPIFFS(timersZB, FILE_ZBTMRS_SPIFFS);
        
        uint8_t enable = timersZB[indice].activar;
        uint8_t saida = timersZB[indice].output;
        uint8_t accao = timersZB[indice].action;
        char dias_semana[8];
        strncpy(dias_semana, timersZB[indice].weekDays, 8);

        char aux_hor[6];
        char aux_min[6];
        char hora[6]; 
        String horas = String(timersZB[indice].hora_minutos[0]) + String(timersZB[indice].hora_minutos[1]);
        String minutos = String(timersZB[indice].hora_minutos[3]) + String(timersZB[indice].hora_minutos[4]);
        
        uint8_t hours = horas.toInt();
        uint8_t minuts = minutos.toInt();

        if (hours <= 9) {
          snprintf(aux_hor, sizeof(aux_hor) - 1, "0%i", hours);
        }
        else {
          snprintf(aux_hor, sizeof(aux_hor) - 1, "%i", hours);
        }

        if (minuts <= 9) {
          snprintf(aux_min, sizeof(aux_min) - 1, "0%i", minuts);
        }
        else {
          snprintf(aux_min, sizeof(aux_min) - 1, "%i", minuts);
        }

        strcpy(hora, aux_hor);
        strcat(hora, ":");
        strcat(hora, aux_min);

        if( indice == 255){
          enable = 0;
          strcpy(hora, "12:12");
          strcpy(dias_semana, "0000000");
          saida = 1;
          accao = 0;
        }

        json_doc["CMD"] = "TCG";
        json_doc["Enable"] = enable;
        json_doc["Time"] = hora;
        json_doc["WeekDays"] = dias_semana;
        json_doc["Output"] = saida;
        json_doc["Action"] = accao;  
        serializeJson(json_doc, json_array);
        //json_array[strlen(json_array)] = '\0';
       
        esp_now_send(seve_server_mac, (uint8_t *)json_array, strlen(json_array));   
        json_doc.clear();

        /*for (uint8_t index = 0; index < QTD_DEVS_ZIGBEE; index++){
          if( strcmp(timersZB[index].short_id, "") != 0){
            Serial.printf("==== [%d] ====\n", index);
            Serial.println(timersZB[index].short_id);
            Serial.println(timersZB[index].action);
            Serial.println(timersZB[index].activar);
            Serial.println(timersZB[index].output);
            Serial.println(timersZB[index].timer);
            Serial.println(timersZB[index].hora_minutos);
            Serial.println(timersZB[index].weekDays);
          }          
        }*/   
  }
  else if( strcmp(cz_cmd, "STM") == 0 && iz_id_dev != BOARD_ID && quarto == 0 && quinto == 0){      

    deserializeJson(json_doc, (const char*)incomingData); 
 
    // Actualiza regsidto indicado
    uint8_t offset = json_doc["Timer"];
    uint8_t iz_output = json_doc["Output"];
    uint8_t iz_action = json_doc["Action"];
    uint8_t iz_enable = json_doc["Enable"];
    uint32_t iz_id = json_doc["ID"];
    const char* hor_min = json_doc["Time"];
    const char* weedays = json_doc["WeekDays"];
    char short_id[ZB_SHORT_ID];
        
    // Obtem short_id do dispositivos
    for (uint8_t index = 0; index < QTD_DEVS_ZIGBEE; index++){
      if(arr_zb_devs[index].zb_board_id == iz_id){
        strcpy(short_id, arr_zb_devs[index].short_id);            
        break;
      }
    }

                     
    // Guarda/Actualiza Timers
    for (uint8_t index = 0; index < QTD_DEVS_ZIGBEE; index++){     
      // Carregar dados existentes no array "timersZB"
      LerZbSPIFFS(timersZB, FILE_ZBTMRS_SPIFFS);

      // Se o array esta vazio, preencher a primeira posicao  
      if(strcmp( timersZB[index].short_id, "") == 0 && index == 0){            
        strncpy(timersZB[index].short_id, short_id, sizeof(timersZB[index].short_id));
        timersZB[index].action = iz_action;
        timersZB[index].activar = iz_enable;
        timersZB[index].output = iz_output;
        timersZB[index].timer = offset;
        strcpy(timersZB[index].hora_minutos, hor_min);
        strncpy(timersZB[index].weekDays, weedays, sizeof(timersZB[index].weekDays));
        // Guardar Timers
        EscreverZbSPIFFS(timersZB, FILE_ZBTMRS_SPIFFS);
        break;
      }
      else if(strcmp( timersZB[index].short_id, short_id) == 0 && timersZB[index].timer == offset ){
        // Se o timer ja existe, actualiza
        strncpy(timersZB[index].short_id, short_id, sizeof(timersZB[index].short_id));
        timersZB[index].action = iz_action;
        timersZB[index].activar = iz_enable;
        timersZB[index].output = iz_output;            
        strcpy(timersZB[index].hora_minutos, hor_min);
        strncpy(timersZB[index].weekDays, weedays, sizeof(timersZB[index].weekDays));
        // Guardar Timers
        EscreverZbSPIFFS(timersZB, FILE_ZBTMRS_SPIFFS);
        break;           
      }
      else if(strcmp( timersZB[index].short_id, short_id) != 0 && strcmp( timersZB[index].short_id, "") == 0){
        // Se o timer nao existe, regista
        strncpy(timersZB[index].short_id, short_id, sizeof(timersZB[index].short_id));
        timersZB[index].action = iz_action;
        timersZB[index].activar = iz_enable;
        timersZB[index].output = iz_output;
        timersZB[index].timer = offset;
        strcpy(timersZB[index].hora_minutos, hor_min);
        strncpy(timersZB[index].weekDays, weedays, sizeof(timersZB[index].weekDays));
        // Guardar Timers
        EscreverZbSPIFFS(timersZB, FILE_ZBTMRS_SPIFFS);
        break;           
      }        
    } 
    //Serial.println(short_id); 

  }
  else if( strcmp(cz_cmd, "STM") == 0 && (iz_id_dev == BOARD_ID || iz_id_dev == ID_BROADCAST_ESPNOW) ){ 
    ConfigurarTimer(cz_tipo_msg, FILE_DATA_SPIFFS);      
  }
  else if( strcmp(cz_cmd, "RSD") == 0 && iz_id_dev == BOARD_ID){       
     strncpy(data2eeprom.CONFIGURADO, "NO", sizeof(data2eeprom.CONFIGURADO));
     EEPROM.put(EEPROM_START_ADDR, data2eeprom);
     EEPROM.commit();
     delay(10);
     ESP.restart(); 
  }
   else if( strcmp(cz_cmd, "RNM") == 0 && iz_id_dev != BOARD_ID){
    for (uint8_t cont = 0; (cont < QTD_DEVS_ZIGBEE); cont++){ // Start_for1

      if ( arr_zb_devs[cont].zb_board_id == iz_id_dev){
        String cmnd = "ZbName " + String(arr_zb_devs[cont].name) + "," + String(cz_mac);
        uint8_t var_len = cmnd.length() + 1;
        char data[var_len];
        cmnd.toCharArray(data, var_len);         
        Serial.write(data, sizeof(data));        
        Serial.write('\n');          
        break;
      }
    }
  }
  else if( strcmp(cz_cmd, "ADS") == 0 && iz_id_dev == BOARD_ID){
    char cz_dt_tmr[DT_STR_SIZE];
    sprintf(cz_dt_tmr, "%s:%s:%s, %d/%d/%d", "__", "__", "__", day(), month(), year()); 
    uint8_t estado = digitalRead(PIN_RELAY);   
    strcpy(czjson, CriarJSON(cz_dt_tmr, "SAC", data2eeprom.nome, BOARD_ID, estado, 2));
    //czjson[strlen(czjson)] = '\0';      
    esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));
  }
  else if( strcmp(cz_cmd, "RNM") == 0 && iz_id_dev == BOARD_ID){      
    //imprimeln((const char*)incomingData);
    strncpy(data2eeprom.nome, cz_mac, sizeof(data2eeprom.nome));
    EEPROM.put(EEPROM_START_ADDR, data2eeprom);
    EEPROM.commit();
    delay(10);


    uint8_t pin_state = 0;
    #if POWER320D == 1
      pin_state = relayState;
    #else  
      pin_state = digitalRead(PIN_RELAY);
    #endif

    #if INVERT_PIN == 1
      #if POWER320D == 1
        pin_state = !relayState;
      #else  
        pin_state = !digitalRead(PIN_RELAY);
      #endif
    #endif

    //StaticJsonDocument <JSON_DOC_SIZE> json_doc; 
    json_doc.clear();
    json_doc["MSG"] = data2spiffs.DT;  
    json_doc["CMD"] = "RAN";
    json_doc["ID"] = BOARD_ID;  
    json_doc["PST"] = pin_state;
    json_doc["WCH"] = 0;
    json_doc["MAC"] = cz_mac;
    serializeJson(json_doc, json_array);
    //json_array[strlen(json_array)] = '\0'; 
    esp_now_send(seve_server_mac, (uint8_t *)json_array, strlen(json_array)); 
  }
  else if( strcmp(cz_cmd, "ENABLE_OTA") == 0 && iz_id_dev == BOARD_ID){      
    TrocarModoOperacao("OTA");
  }
  #if ESPNOW_TASMOTA_BRIDGE == 1
    else if(strcmp(cz_cmd, "TCW") == 0 && iz_id_dev == BOARD_ID){      
      // Conectar Tasmota a Rede WiFi
      char cmnd[512];
      const char* cz_ssid = json_doc["MSG"];
      const char* cz_senha = json_doc["MAC"];

      sprintf(cmnd, "Backlog SSID1 %s; Password1 %s; Ipaddress1 0.0.0.0; restart 1", cz_ssid, cz_senha);
      cmnd[strlen(cmnd)] = '\0';
      Serial.write(cmnd, strlen(cmnd));      
      Serial.write('\n'); 
    }
  #endif
  

  free(uz_mac);
  
}// end callback_rx_esp_now(...)
#endif


///////////////////////////////////
// Autoemparelhar com o servidor //
///////////////////////////////////
const char* autoPairing(){

  char czjson[JSON_DOC_SIZE];
  DynamicJsonDocument dadosJson(JSON_DOC_SIZE);

  if( strncmp(pairingStatus, "PRT", sizeof(pairingStatus)) == 0) {
    imprime(F("Pairing request on channel: "));
    imprimeln(channel);
    imprimeln(WiFi.macAddress());
  
    // clean esp now
    esp_now_deinit();
       

    #if defined(ESP32) 
      //Force espnow to use a specific channel
      esp_wifi_set_promiscuous(true);
      esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
      esp_wifi_set_promiscuous(false);
    #elif defined(ESP8266) 
      wifi_promiscuous_enable(true);
      wifi_set_channel(channel);
      wifi_promiscuous_enable(false);
    #endif

    WiFi.disconnect();

    // Init ESP-NOW
    if (esp_now_init() != 0) {
      imprimeln(F("Error initializing ESP-NOW"));
    }

    // ESP-NOW
    #if defined(ESP8266) 
      esp_now_set_self_role(ESP_NOW_ROLE_COMBO);        
    #endif

    esp_now_register_send_cb(callback_tx_esp_now);
    esp_now_register_recv_cb(callback_rx_esp_now);

    #if defined(ESP32) 
      memcpy(peerInfo.peer_addr, broadcastAddress, 6);
      peerInfo.channel = 0;
      peerInfo.encrypt = false;
      if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        imprimeln(F("Failed to add peer"));        
      }        
    #endif
    
    
    previousMillis = millis();

    // add peer and send request
    strcpy(czjson, CriarJSON("PRT", "", WiFi.macAddress().c_str(), BOARD_ID, 2, channel));
    //czjson[strlen(czjson)] = '\0';
    esp_now_send(broadcastAddress, (uint8_t *)czjson, strlen(czjson));  
    strncpy(pairingStatus, "PAIR_REQUESTED", sizeof(pairingStatus));
  }
  else if( strncmp(pairingStatus, "PAIR_REQUESTED", sizeof(pairingStatus)) == 0) {
    // time out to allow receiving response from server
    currentMillis = millis();
    if(currentMillis - previousMillis > 1000) {
      previousMillis = currentMillis;
      // time out expired,  try next channel
      channel ++;
      if (channel > MAX_WIFI_CH) {
        channel = 0;
      }
      strncpy(pairingStatus, "PRT", sizeof(pairingStatus));
    }
  }
  else if( strncmp(pairingStatus, "PAIR_PAIRED", sizeof(pairingStatus)) == 0) {
    // Enviar msg alive
    if( (millis() - ctrl_send_alive)/1000 >= TEMPO_ENVIAR_ALIVE ){

      #if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1
        for (uint8_t cont = 0; (cont < QTD_DEVS_ZIGBEE); cont++){ // Start_for1

          if (arr_zb_devs[cont].zb_board_id > 0){
            strcpy(czjson, CriarJSON("PAL", "", WiFi.macAddress().c_str(), arr_zb_devs[cont].zb_board_id, 2, channel));
            //czjson[strlen(czjson)] = '\0';
            esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));  
            delay(100);
          }
              
        } // End_for1
      #endif
           
      strcpy(czjson, CriarJSON("PAL", "", WiFi.macAddress().c_str(), BOARD_ID, 2, channel));
      //czjson[strlen(czjson)] = '\0';
      esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));  
      //imprimeln(pairingStatus); 
      ctrl_send_alive = millis();
    }
    
  }
  
  pairingStatus[strlen(pairingStatus)] = '\0';
  return pairingStatus;

} 


/////////////////////
//Criar dados JSON //
/////////////////////
const char* CriarJSON(const char* MSG_TYPE, const char* CMD, const char* MAC, uint32_t ID, int8_t PIN_STT, uint8_t WIFI_CH){
  
  //DynamicJsonDocument json_doc(JSON_DOC_SIZE);  
  json_doc["MSG"] = MSG_TYPE;
  json_doc["CMD"] = CMD;
  json_doc["MAC"] = MAC;
  json_doc["ID"] = ID;
  json_doc["PST"] = PIN_STT;
  json_doc["WCH"] = WIFI_CH;   

  serializeJson(json_doc, json_array);
  //json_array[strlen(json_array)] = '\0';
  json_doc.clear();
  return json_array;
}



//////////////////////////////
// Executar acções do Timer //
//////////////////////////////
void ExecutarTimer(const char* nome_arquivo) {

  // Para tirar e guardar dados EEPROM e SPIFFS
  data2SPIFFS data2spiffs = {0};
  data2EEPROM data2eeprom = {0};
  EEPROM.get(EEPROM_START_ADDR, data2eeprom);

  // Tira Timers da EEPROM
  timers2EEPROM tmrs[QTD_TMRS];
  EEPROM.get(EEPROM_TIMERS_ADDR, tmrs);

  for (uint8_t index = 0; index < QTD_TMRS; index++) {

    // Verficar se o dia de semana, do momento, está programado, timer activado
    if ( tmrs[index].weekDays[weekday() - 1] == '1' && tmrs[index].activar == 1 && year() > YEAR_COMPARE) {
      if (hour() == tmrs[index].horas && minute() == tmrs[index].minutos && second() <= 5) {
        
      #if POWER320D == 1   
        OnOffBISTABLE(tmrs[index].action, BISTABLE_ON, BISTABLE_OFF);
      #elif ESPNOW_TASMOTA_BRIDGE == 1
        if(tmrs[index].action == 1){          
          SendCommandtasmota("Power ON", 9);
        }
        else if(tmrs[index].action == 0){
          SendCommandtasmota("Power OFF", 10);
        }      
      #else
        digitalWrite(PIN_RELAY, tmrs[index].action);
      #endif

      #if INVERT_PIN == 1
          digitalWrite(PIN_RELAY, !tmrs[index].action);
      #endif  

      #if IS_PASSIVE_MASTER == 1
          if(data2eeprom.TIPO != PASSIVO){
            mudarEstadoPINPassivo(tmrs[index].action, PASSIVE_SLAVE_ID);
          }                  
      #endif    

        #if POWER320D == 1
          digitalWrite(PIN_LED, relayState);
        #else   
          digitalWrite(PIN_LED, digitalRead(PIN_RELAY)); // O inverso do PIN_RELAY
        #endif
                
        char cz_dt[DT_STR_SIZE];
       
        ObterDTActual(cz_dt);  
        cz_dt[strlen(cz_dt)] = '\0';      
        LerSPIFFS(&data2spiffs, nome_arquivo);

        #if POWER320D == 1 
          data2spiffs.estado_pin = relayState;
        #else
          data2spiffs.estado_pin = digitalRead(PIN_RELAY);
        #endif

        strncpy(data2spiffs.DT, cz_dt, sizeof(cz_dt));
        char nome[MAX_NAME_LENGTH];
        strncpy(nome, data2eeprom.nome, sizeof(nome));
        nome[strlen(nome)] = '\0'; 
        EscreverSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);

        uint8_t pin_state = 0;

        #if POWER320D == 1 
          pin_state = relayState;  
        #else
          pin_state = digitalRead(PIN_RELAY);
        #endif

        #if INVERT_PIN == 1
          #if POWER320D == 1 
            pin_state = !relayState;  
          #else
            pin_state = !digitalRead(PIN_RELAY);
          #endif
        #endif

        // Envia resposta para actualização de estado nas páginas web.
        char czjson[JSON_DOC_SIZE]; 
        strcpy(czjson, CriarJSON(cz_dt, "TGA", data2eeprom.nome, BOARD_ID, pin_state, WiFi.channel()));
        //czjson[strlen(czjson)] = '\0'; 
        // imprimeln(czjson);
        esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));
      }
    }
  }

}



///////////////////////////////////////////////////////////
// Conforma a data e a hora numa stiring (H:mm:ss d m Y) //
///////////////////////////////////////////////////////////
void ObterDTActual(char* cz_dt){  
  //sprintf(cz_data_hora, "%02d:%02d:%02d %02d/%02d/%d", hour(), minute(), second(), day(), month(), year());
  sprintf(cz_dt, "%d:%02d:%02d, %d/%d/%d", hour(), minute(), second(), day(), month(), year()); 
}


////////////////////////////////////////////
// Converter MAC (string para Byte array) //
////////////////////////////////////////////
uint8_t* converteMacString2Byte(const char* cz_mac) {

  uint8_t* MAC = (uint8_t*)calloc(6, sizeof(uint8_t));
  char* ptr;

  MAC[0] = strtol(cz_mac, &ptr, HEX );
  for ( uint8_t i = 1; i < 6; i++ ) {
    MAC[i] = strtol(ptr + 1, &ptr, HEX );
  }

  return MAC;
}


//////////////////////////////////////////////////////////////////////////
// troca o estado de PIN e guarda dados (estado, data e hora) na SPIFFS //
//////////////////////////////////////////////////////////////////////////
void TogglePIN(){

  char czjson[JSON_DOC_SIZE];
  char cz_dt[DT_STR_SIZE];
  uint8_t pin_state = 0;

  data2EEPROM data2eeprom = {0};
  data2SPIFFS data2spiffs = {0};
  EEPROM.get(EEPROM_START_ADDR, data2eeprom);  
  LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);

  #if POWER320D == 1
    ToggleBISTABLE(BISTABLE_ON, BISTABLE_OFF);
  #else  
    digitalWrite(PIN_RELAY, !digitalRead(PIN_RELAY));
  #endif

  #if defined(ESP32) 
      #if POWER320D == 0
        digitalWrite(PIN_LED, !digitalRead(PIN_RELAY)); // O inverso do PIN_RELAY
      #endif  
    #else
      digitalWrite(PIN_LED, digitalRead(PIN_RELAY)); // O inverso do PIN_RELAY
    #endif


   #if POWER320D == 1
    data2spiffs.estado_pin = relayState;
    pin_state = relayState;
  #else 
    data2spiffs.estado_pin = digitalRead(PIN_RELAY);
    pin_state = digitalRead(PIN_RELAY);
  #endif
  
  

  ObterDTActual(cz_dt);
  cz_dt[strlen(cz_dt)] = '\0'; 
 
  
#if INVERT_PIN == 1
  pin_state = !pin_state;
#endif

  strncpy(data2spiffs.DT, cz_dt, sizeof(cz_dt));
  data2spiffs.DT[strlen(data2spiffs.DT)] = '\0'; 
  char nome[MAX_NAME_LENGTH];  
  strncpy(nome, data2eeprom.nome, sizeof(nome));
  nome[strlen(nome)] = '\0'; 

  EscreverSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);

  
  // Envia resposta para actualização de estado nas páginas web.
  strcpy(czjson, CriarJSON(cz_dt, "TGA", nome, BOARD_ID, pin_state, WiFi.channel()));
  //imprimeln(czjson);
  //czjson[strlen(czjson)] = '\0'; 
  esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));

  // Envia comando para muidanca do estado do PIN do Dispositivo passivo associado  
  #if IS_PASSIVE_MASTER == 1
      if(data2eeprom.TIPO != PASSIVO){
        mudarEstadoPINPassivo(digitalRead(PIN_RELAY), PASSIVE_SLAVE_ID);
      }                  
  #endif    
}



////////////////////////
// Consulta de Timers //
////////////////////////
void ConsultarTimer(const char* nome_arquivo, uint8_t index) {

  char aux_hor[6];
  char aux_min[6];
  char hora[6];
  timers2EEPROM tmrs[QTD_TMRS];
  EEPROM.get(EEPROM_TIMERS_ADDR, tmrs);

  uint8_t enable = tmrs[index].activar;
  uint8_t saida = tmrs[index].output;
  uint8_t accao = tmrs[index].action;
  char dias_semana[8];
  strncpy(dias_semana, tmrs[index].weekDays, 8);
  dias_semana[strlen(dias_semana)] = '\0';
  
  if (tmrs[index].horas <= 9) {
    snprintf(aux_hor, sizeof(aux_hor) - 1, "0%i", tmrs[index].horas);
  }
  else {
    snprintf(aux_hor, sizeof(aux_hor) - 1, "%i", tmrs[index].horas);
  }

  if (tmrs[index].minutos <= 9) {
    snprintf(aux_min, sizeof(aux_min) - 1, "0%i", tmrs[index].minutos);
  }
  else {
    snprintf(aux_min, sizeof(aux_min) - 1, "%i", tmrs[index].minutos);
  }

  strcpy(hora, aux_hor);
  strcat(hora, ":");
  strcat(hora, aux_min);

  //sprintf(buffer, "{\"Enable\":%i,\"Time\":\"%s\",\"Window\":0,\"WeekDays\":\"%s\",\"Repeat\":1,\"Output\":%i,\"Action\":%i}", enable, hora, dias_semana, saida, accao );
  //sprintf(buffer, "{\"Enable\":%d,\"Time\":\"%s\",\"WeekDays\":\"%s\",\"Output\":%d,\"Action\":%d}", enable, hora, dias_semana, saida, accao );

  //StaticJsonDocument <JSON_DOC_SIZE> json_doc;  
  json_doc["MSG"] = "";  
  json_doc["CMD"] = "TCA";
  json_doc["ID"] = 0;  
  json_doc["PST"] = 2;
  json_doc["WCH"] = 0;
  json_doc["MAC"] = "";
  json_doc["Enable"] = enable;
  json_doc["Time"] = hora;
  json_doc["WeekDays"] = dias_semana;
  json_doc["Output"] = saida;
  json_doc["Action"] = accao;  
  serializeJson(json_doc, json_array);
  //json_array[strlen(json_array)] = '\0';
  json_doc.clear();
}


////////////////////////////
// Configuracão de timers //
////////////////////////////
void ConfigurarTimer(const char* cz_json, const char* nome_arquivo) {
  
  //Serial.println(cz_json);

  //StaticJsonDocument<JSON_DOC_SIZE> doc_json; 
  json_doc.clear();
  deserializeJson(json_doc, cz_json);

  // Tira Timers da EEPROM
  timers2EEPROM timers[QTD_TMRS];
  EEPROM.get(EEPROM_TIMERS_ADDR, timers);
  

  // Actualiza regsidto indicado
  uint8_t offset = json_doc["Timer"];
  uint8_t iz_output = json_doc["Output"];
  uint8_t iz_action = json_doc["Action"];

  // Separar hora e minutos
  char hour_min[DT_STR_SIZE];  
  strcpy(hour_min, json_doc["Time"]);
  char *horas, *minutos;
  int iz_horas, iz_minutos;
  char delim[2] = ":";
  horas = strtok(hour_min, delim);  // primeira parte
  minutos = strtok(NULL, delim);    // Segunda parte
  sscanf(horas, "%d", &iz_horas);
  sscanf(minutos, "%d", &iz_minutos);
    
  timers[offset].activar = json_doc["Enable"];
  timers[offset].horas = iz_horas; 
  timers[offset].minutos = iz_minutos; 
  strcpy(timers[offset].weekDays, json_doc["WeekDays"]);
  timers[offset].output = iz_output;
  timers[offset].action = iz_action;

  EEPROM.put(EEPROM_TIMERS_ADDR, timers);
  EEPROM.commit();
  delay(10);

}


//////////////////////////////////////////////////////////////////////
// Enviar comando para alterar estado do PIN do Dispositivo PASSIVO //
//////////////////////////////////////////////////////////////////////
void mudarEstadoPINPassivo(uint8_t estado, uint32_t dev_id){
  char czjson[JSON_DOC_SIZE];      
  strcpy(czjson, CriarJSON("DATA", "TGG", "", dev_id, estado, WiFi.channel()));     
  //czjson[strlen(czjson)] = '\0'; 
  esp_now_send(0, (uint8_t *)czjson, strlen(czjson));  
}




/////////////////////////////////////////
// Processar dados recebidos via Telne //
/////////////////////////////////////////
#if TELNET == 1
  void LeerTelnet() {
    if (TelnetStream.available()) {
      char Letra = TelnetStream.read();
      switch (Letra) {
        case 'R':
          TelnetStream.stop();
          delay(100);
          ESP.restart();
          break;
        case 'C':
          TelnetStream.println("Adios, adios");
          TelnetStream.stop();
          break;
        case 'M':
        case 'm':
          TelnetStream.println("Dale Like al video :)");
          break;
      }
    }
  }
#endif


////////////////////////////////////////////////
// Aumentar a Potencia de Transmissao do WiFi //
////////////////////////////////////////////////
void aumentarPotenciaTxWiFi(){  
    #ifdef ESP32 
      WiFi.setTxPower(WIFI_POWER_19_5dBm); 
    #else     
      WiFi.setOutputPower(20.5);      
    #endif    
}


//////////////
// Criar ID //
//////////////
uint32_t CriarDEVICE_ID(){
  // Toma o ID do dev e adiciona ao mesmo o TIPO de dev correspondente
  
  #if defined(ESP8266) 
    uint32_t BOARD_ID = ESP.getChipId();
  #elif defined(ESP32) 
    uint32_t BOARD_ID = 0;    
    uint64_t macAddress = ESP.getEfuseMac();
    uint64_t macAddressTrunc = macAddress << 40;
    BOARD_ID = macAddressTrunc >> 40;
  #endif  

  char buf[16];
  char tipo[4];
  itoa(data2eeprom.TIPO, tipo, 10);
	itoa(BOARD_ID, buf, 10);
  strcat(buf, tipo);
  buf[strlen(buf)] = '\0';
  tipo[strlen(tipo)] = '\0';
  
 
  // Toma, apenas,os últimos "MAX_ID_DIGITS" digitos do ID
  uint64_t num = atoi(buf);
  uint8_t n = MAX_ID_DIGITS;
  BOARD_ID = num % (int) pow(10, n);

 
  // Checar se o BOARD_ID tem menos de 7 digitos;
  uint8_t digitCount = 0;
  uint64_t aux_id = BOARD_ID;
  while (aux_id > 0) {
    aux_id /= 10;
    digitCount++;
  }

  // adicionar digitos complementares se necessario
  if(digitCount < MAX_ID_DIGITS){
    for(uint8_t cont = 0; cont < (MAX_ID_DIGITS - digitCount); cont++){
      BOARD_ID = (BOARD_ID * 10) + atoi(tipo);
    }    
  }

  return BOARD_ID;

}



///////////////////////////////////////////////
// troca o estado de PIN para Relay bistable //
///////////////////////////////////////////////
#if BISTABLE_RELAY == 1
  void ToggleBISTABLE(uint8_t pin_on, uint8_t pin_off){
    if (relayState == 0) {
      digitalWrite(pin_on, HIGH); // Liga o relé
      delay(10); 
      digitalWrite(pin_on, LOW);      
      relayState = 1;
      digitalWrite(PIN_LED, HIGH);
    } else {
      digitalWrite(pin_off, HIGH); // Desliga o relé
      delay(10); 
      digitalWrite(pin_off, LOW);     
      relayState = 0;
      digitalWrite(PIN_LED, LOW);
    }   
  }
#endif


/////////////////////////////////
// Liga/Desliga Relay bistable //
/////////////////////////////////
#if BISTABLE_RELAY == 1
  void OnOffBISTABLE(uint8_t estado, uint8_t pin_on, uint8_t pin_off){
    if (estado == 1) {
      digitalWrite(pin_on, HIGH); // Liga o relé
      delay(10); 
      digitalWrite(pin_on, LOW);      
      relayState = 1;
      digitalWrite(PIN_LED, HIGH);
    } 
    else if (estado == 0){
      digitalWrite(pin_off, HIGH); // Desliga o relé
      delay(10); 
      digitalWrite(pin_off, LOW);     
      relayState = 0;
      digitalWrite(PIN_LED, LOW);
    }   
  }
#endif





#if ESPNOW_TASMOTA_BRIDGE == 1
 
  ///////////////////////////////////////
  // Processa dados obtidos da consola //
  ///////////////////////////////////////
  void processarMSG_2(String data){      

    // Processar res[posta do comando "Power"
    if( data.indexOf("RESULT = {\"POWER\":\"") != -1){  
      uint8_t startIndex = data.indexOf("{");
      uint8_t endIndex = data.indexOf("}", startIndex);

      data = data.substring(startIndex, endIndex + 1);               
       
      StaticJsonDocument<JSON_DOC_SIZE> doc;
      deserializeJson(doc, data);

      const char* cz_pin_state = doc["POWER"];          

      if(strcmp(cz_pin_state, "ON") == 0){
        save_pin_state = 1;
      }
      else if(strcmp(cz_pin_state, "OFF") == 0){
        save_pin_state = 0;
      }

      // Guarda o estado do PIN, se necessario
      data2SPIFFS data2spiffs = {0};          
      LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);          

      if(data2spiffs.estado_pin != save_pin_state){
        char cz_dt[DT_STR_SIZE];
        data2spiffs.estado_pin = save_pin_state;
        digitalWrite(PIN_RELAY, save_pin_state);

        ObterDTActual(cz_dt);
        cz_dt[strlen(cz_dt)] = '\0'; 
        strncpy(data2spiffs.DT, cz_dt, sizeof(cz_dt));
        data2spiffs.DT[strlen(data2spiffs.DT)] = '\0'; 
        EscreverSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);
      }
         
    }
    // Processar res[posta do comando "Status 8"
    else if( data.indexOf("STATUS8 = {\"StatusSNS\":") != -1){ 
      uint8_t startIndex = data.indexOf("{");
      
      data = data.substring(startIndex);
          
      StaticJsonDocument<512> doc;
      deserializeJson(doc, data);
                      
      uint8_t voltage = doc["StatusSNS"]["ENERGY"]["Voltage"];
      float current = doc["StatusSNS"]["ENERGY"]["Current"];
      uint8_t power = doc["StatusSNS"]["ENERGY"]["Power"];
      float energy = doc["StatusSNS"]["ENERGY"]["Today"];     

      /*if(power > 0){
        save_pin_state = 1;
      }
      else if(power <= 0){
        save_pin_state = 0;
      }*/

      // Guarda o estado do PIN, se necessario
      data2SPIFFS data2spiffs = {0};          
      LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);          

      if(data2spiffs.estado_pin != save_pin_state){
        char cz_dt[DT_STR_SIZE];
        data2spiffs.estado_pin = save_pin_state;
        digitalWrite(PIN_RELAY, save_pin_state);

        ObterDTActual(cz_dt);
        cz_dt[strlen(cz_dt)] = '\0'; 
        strncpy(data2spiffs.DT, cz_dt, sizeof(cz_dt));
        data2spiffs.DT[strlen(data2spiffs.DT)] = '\0'; 
        EscreverSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);
      }

      char jsondata[48];    
      sprintf(jsondata,"{\"V\":%d,\"A\":\"%.3f\", \"W\":%d, \"Wh\":%.3f}", voltage, current, power, energy);
      
      json_doc.clear();
      json_doc["MSG"] = "SNS";  
      json_doc["CMD"] = "NRG";
      json_doc["ID"] = BOARD_ID;  
      json_doc["PST"] = save_pin_state;
      json_doc["WCH"] = 0;
      json_doc["MAC"] = jsondata;//cs_json; // {"V":220,"A":"0.000","W":0,"Wh":"0.000"}
      serializeJson(json_doc, json_array);    
      
      esp_now_send(seve_server_mac, (uint8_t *)json_array, strlen(json_array)); 
    }
    // Processar res[posta do comando "WiFi"
    else if( data.indexOf("RESULT = {\"Wifi\":\"") != -1){  
      uint8_t startIndex = data.indexOf("{");
      uint8_t endIndex = data.indexOf("}", startIndex);

      data = data.substring(startIndex, endIndex + 1);               
       
      StaticJsonDocument<JSON_DOC_SIZE> doc;
      deserializeJson(doc, data);

      const char* cz_wifi_state = doc["Wifi"];

      if(strcmp(cz_wifi_state, "ON") == 0){
        iz_wifi_state = 1;
      } 
      else  if(strcmp(cz_wifi_state, "OFF") == 0){
        iz_wifi_state = 0;
      }          
         
    }
    // Processar res[posta do comando "Status 5"
    else if( data.indexOf("STATUS5 = {\"StatusNET\":{\"") != -1){  
      uint8_t startIndex = data.indexOf("{");
      
      data = data.substring(startIndex);               
       
      StaticJsonDocument<JSON_DOC_SIZE> doc;
      deserializeJson(doc, data);

      const char* cz_ip = doc["StatusNET"]["IPAddress"];  

      char jsondata[48];
      sprintf(jsondata,"{\"WifiState\":%d,\"IPAddress\":\"%s\"}", iz_wifi_state, cz_ip);
      
      char czjson[JSON_DOC_SIZE]; 
      // MAC = cz_ip
      // PST = iz_wifi_state
      strcpy(czjson, CriarJSON("", "SWS", cz_ip, BOARD_ID, iz_wifi_state, WiFi.channel()));
      //czjson[strlen(czjson)] = '\0'; 
      esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));      
         
    }
   

  }// void processarMSG_2(String data)


  ///////////////////////////////
  // Enviar comando via serial //
  ///////////////////////////////
  void SendCommandtasmota(const char* cmnd, uint8_t cmnd_size){
    Serial.write(cmnd, strlen(cmnd)); 
    Serial.write('\n');
  }

  
#endif


  



 



#if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1

  //////////////////////////////////////////////////////////////
  // Checa se um array de timerszb contem o elemento indicado //
  //////////////////////////////////////////////////////////////
  /*int8_t ArrayZbTimerHasElement(zbtimers2SPIFFS arr[], const char* element){
    uint8_t cont = 0;

    for (; cont < QTD_DEVS_ZIGBEE; cont++){

      if(strcmp(arr[cont].short_id, "") == 0) break;

      //Se existe
      if (strcmp(arr[cont].short_id, element) == 0){
        return -1;
      }   
    }

    // Se nao existe
    return cont;
  }*/



  //////////////////////////////////////////////////////////////
  // Checa se um array de inteiros contem o elemento indicado //
  //////////////////////////////////////////////////////////////
  int8_t ArrayZbHasElement(ZigbeeDevices arr[], const char* element){

    uint8_t cont = 0;

    for (; cont < QTD_DEVS_ZIGBEE; cont++){

      if(strcmp(arr[cont].short_id, "") == 0) break;

      if (strcmp(arr[cont].short_id, element) == 0){
        return -1;
      }   
    }

    return cont;
  }

  ///////////////////////////////////////////
  // Cria BOARD_ID dos Dispositivos Zigbee //
  ///////////////////////////////////////////
  uint32_t ZbDEVICE_ID(String hexString, DEV_TYPES type){
 
    uint32_t BOARD_ID = 0;    
    uint8_t nextInt;
      
    for (uint8_t i = 0; i < hexString.length(); i++) {
        
      nextInt = int(hexString.charAt(i));
      if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
      if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
      if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
      nextInt = constrain(nextInt, 0, 15);
        
      BOARD_ID = (BOARD_ID * 16) + nextInt;
    }
       
    BOARD_ID = BOARD_ID/1000;      

    char buf[16];
    char tipo[4];
    itoa(type, tipo, 10);
    itoa(BOARD_ID, buf, 10);
    strcat(buf, tipo);
    buf[strlen(buf)] = '\0';
    tipo[strlen(tipo)] = '\0';
    
  
    // Toma, apenas,os últimos "MAX_ID_DIGITS" digitos do ID
    uint64_t num = atoi(buf);
    uint8_t n = MAX_ID_DIGITS;
    BOARD_ID = num % (int) pow(10, n);

  
    // Checar se o BOARD_ID tem menos de 7 digitos;
    uint8_t digitCount = 0;
    uint64_t aux_id = BOARD_ID;
    while (aux_id > 0) {
      aux_id /= 10;
      digitCount++;
    }

    // adicionar digitos complementares se necessario
    if(digitCount < MAX_ID_DIGITS){
      for(uint8_t cont = 0; cont < (MAX_ID_DIGITS - digitCount); cont++){
        BOARD_ID = (BOARD_ID * 10) + atoi(tipo);
      }    
    }

    // Colocar marca de dispositivo Zigbee 00, nas posicoes 4 e 5
    uint8_t len = String(BOARD_ID).length();
    char cz_id[len + 1];  
    String(BOARD_ID).toCharArray(cz_id, sizeof(cz_id));
    cz_id[4] = '0';
    cz_id[5] = '0';  
        
    BOARD_ID = atoi(cz_id);
    return BOARD_ID;
  }


  ///////////////////////////////////////
  // Processa dados obtidos da consola //
  ///////////////////////////////////////
  void processarMSG(String data){       
    
    if( data.indexOf("RESULT = {\"ZbStatus1\":[{") != -1){  
      uint8_t startIndex = data.indexOf("[");
      uint8_t endIndex = data.indexOf("]", startIndex);

      data = data.substring(startIndex, endIndex + 1);
      uint8_t iz_len = data.length() + 1;
      char cz_json[iz_len + 1];
      data.toCharArray(cz_json, iz_len);

      //Serial.println(cz_json);
              
      DeserializationError error = deserializeJson(doc, cz_json);
      if (error) {
        imprimeln(F("Falha no parsing do JSON: "));
        imprimeln(error.c_str());
        return;
      }

      // Verificar se o JSON é um array
      if (!doc.is<JsonArray>()) {
        imprimeln(F("JSON não é um array."));
        return;
      }

      JsonArray array = doc.as<JsonArray>();
      for (JsonVariant value : array) { // Start_for1
        // Verificar se o elemento do array é um objeto
        if (value.is<JsonObject>()) { // Start_if1
          // Obter o objeto
          JsonObject obj = value.as<JsonObject>();
                  
          // Obter os elementos do objeto
          const char* short_id = obj["Device"];
          const char* name = obj["Name"];

          int8_t index = ArrayZbHasElement(arr_zb_devs, short_id);  

          if (index != -1 && index != -2){      
            strcpy(arr_zb_devs[index].name, name);
            strcpy(arr_zb_devs[index].short_id, short_id); 
          }           
              
        }// End_if1

      } // End_for1

    }
    else if (data.indexOf("SENSOR = {\"ZbReceived\":{\"") != -1 && data.indexOf("\"Power\"") != -1 ){// Start_if1
      
      data = data.substring(data.indexOf("{\"Device\":", data.indexOf("ZbReceived\":{")), data.indexOf("}}}") + 1);
          
      uint8_t iz_len = data.length() + 1;
      char cz_json[iz_len + 1];
      data.toCharArray(cz_json, iz_len);

      //Serial.println(cz_json);

      DeserializationError error = deserializeJson(doc, cz_json);
      if (error){
        imprimeln(F("Falha no parsing do JSON: "));
        imprimeln(error.c_str());
        return;
      }
          
      const char *short_id = doc["Device"];
      strcpy(get_short_id, short_id);
      const char *name = doc["Name"];
      uint8_t power = doc["Power"];   
      boolean reachable = doc["Reachable"];  
      char czjson[JSON_DOC_SIZE];    
         
      for (uint8_t cont = 0; (cont < QTD_DEVS_ZIGBEE); cont++){// Star_for
       
        if (arr_zb_devs[cont].zb_board_id > 0 && strcmp(arr_zb_devs[cont].short_id, short_id) == 0 && strcmp(arr_zb_devs[cont].short_id, "") != 0){         
                  
          strcpy(czjson, CriarJSON(data2spiffs.DT, "PRZ", name, arr_zb_devs[cont].zb_board_id, power, WiFi.channel()));
          //czjson[strlen(czjson)] = '\0';
          //esp_now_send(novoMAC, (uint8_t *)czjson, sizeof(czjson));
          #if defined(ESP8266) 
            callback_rx_esp_now(0, (uint8_t *)czjson, strlen(czjson));
          #elif defined(ESP32) 
            callback_rx_esp_now(0, (uint8_t *)czjson, strlen(czjson));
          #endif    
          delay(10); 

          char cz_dt[DT_STR_SIZE];
          ObterDTActual(cz_dt);
          cz_dt[strlen(cz_dt)] = '\0';            
                   
          strcpy(czjson, CriarJSON(cz_dt, "SAC", arr_zb_devs[cont].name, arr_zb_devs[cont].zb_board_id, power, WiFi.channel()));
          //czjson[strlen(czjson)] = '\0';
          //esp_now_send(novoMAC, (uint8_t *)czjson, sizeof(czjson));
          #if defined(ESP8266) 
            callback_rx_esp_now(0, (uint8_t *)czjson, strlen(czjson));
          #elif defined(ESP32) 
            callback_rx_esp_now(0, (uint8_t *)czjson, strlen(czjson));
          #endif    
          delay(10);

          // Actualiza informacoes os devices
          strcpy(arr_zb_devs[cont].name, name);
          arr_zb_devs[cont].power = power;
          arr_zb_devs[cont].Reachable = reachable;
          //Serial.printf("[%i] = %s, %s, %i, %i, %i, %i\n", cont, name,short_id,arr_zb_devs[cont].zb_board_id,power,true, true);
        }         
      
      }// End_for

    }// End_if1
    else  if( data.indexOf("RESULT = {\"ZbStatus3\":[{\"Device\":") != -1){// Start_if2
    
      data = data.substring(data.indexOf("{\"Device\":", data.indexOf("ZbStatus3\":[")), data.indexOf("}]}") + 1);
      
      uint8_t iz_len = data.length() + 1;
      char cz_json[iz_len + 1];
      data.toCharArray(cz_json, iz_len);         

      //Serial.println(cz_json);    
       
      DeserializationError error = deserializeJson(doc, cz_json);
      if (error){
        imprimeln(F("Falha no parsing do JSON: "));
        imprimeln(error.c_str());
        return;
      }
          
      const char *short_id = doc["Device"];
      const char *name = doc["Name"];
      uint8_t power = doc["Power"];
      boolean reachable = doc["Reachable"];
      String valorHexadecimal = doc["IEEEAddr"]; // 0xA4C1 38E342BDFB48
     
      // TODO: Rvere atribuicao do tipo
      uint32_t zb_board_id = ZbDEVICE_ID(valorHexadecimal, lAMPADA); 

      
      for (uint8_t cont = 0; (cont < QTD_DEVS_ZIGBEE); cont++){ // Start_for1

        if (strcmp(arr_zb_devs[cont].short_id, short_id) == 0 && arr_zb_devs[cont].short_id[1] == 'x'){
          strcpy(arr_zb_devs[cont].name, name);
          arr_zb_devs[cont].power = power;
          arr_zb_devs[cont].zb_board_id = zb_board_id;
          arr_zb_devs[cont].Reachable = reachable;

          //Serial.printf("[%i] = %s -- %i\n", cont, short_id, zb_board_id); 

          char czjson[JSON_DOC_SIZE]; 
          strcpy(czjson, CriarJSON(data2spiffs.DT, "SAC", name, arr_zb_devs[cont].zb_board_id, power, WiFi.channel()));
          //czjson[strlen(czjson)] = '\0';
          //esp_now_send(novoMAC, (uint8_t *)czjson, sizeof(czjson));
          #if defined(ESP8266) 
            callback_rx_esp_now(0, (uint8_t *)czjson, strlen(czjson));
          #elif defined(ESP32) 
            callback_rx_esp_now(0, (uint8_t *)czjson, strlen(czjson));
          #endif    
          delay(10);
          break;
        }
             
      } // End_for18 

    }

  }

  //////////////////////////////////////////////////////////////////////////////
  // Obter determinado digito numa sequencia de numeros (de tras para frente) //
  //////////////////////////////////////////////////////////////////////////////
  uint8_t obterDigitoInvertido(uint32_t numero, uint8_t posicao) {    
    uint8_t posicaoInvertida = 0;
    while (posicaoInvertida < posicao && numero > 0) {
        //uint8_t digito = numero % 10; // Obter o último dígito
        numero /= 10; // Remover o último dígito
        posicaoInvertida++;
    }
    
    // Se a posição desejada for maior que o número de dígitos
    if (posicaoInvertida < posicao) {
      return -1; // Valor inválido
    }
    
    // O último dígito restante é o dígito na posição desejada
    return numero % 10;
  }


/////////////////////////////
// Funçaão para ler spiffs //
/////////////////////////////
void LerZbSPIFFS(zbtimers2SPIFFS *ddv, const char* nome_arquivo){
  //LittleFS.begin(); 
  File arquivo = LittleFS.open(nome_arquivo, "r");
  arquivo.read((byte *)ddv, sizeof(*ddv));
  arquivo.close();    
}


//////////////////////////////////
// Funçaão para escrever spiffs //
//////////////////////////////////
void EscreverZbSPIFFS(zbtimers2SPIFFS *ddv, const char* nome_arquivo){
  File arquivo = LittleFS.open(nome_arquivo, "w");
  arquivo.write((byte *)ddv, sizeof(*ddv));
  arquivo.close();
}

/////////////////////////////////////////////
// Execyutar Timers de Dispositivos ZigBee //
/////////////////////////////////////////////
void ExecutarTimerZb(const char* nome_arquivo){

  // Carregar dados existentes no array "timersZB"
  LerZbSPIFFS(timersZB, nome_arquivo);
 
  for (uint8_t index = 0; index < QTD_DEVS_ZIGBEE; index++){   

    if ( timersZB[index].weekDays[weekday() - 1] == '1' && timersZB[index].activar == 1 && year() > YEAR_COMPARE) {
      
      String horas = String(timersZB[index].hora_minutos[0]) + String(timersZB[index].hora_minutos[1]);
      String minutos = String(timersZB[index].hora_minutos[3]) + String(timersZB[index].hora_minutos[4]);
        
      uint8_t hours = horas.toInt();
      uint8_t minuts = minutos.toInt();
      
      if (hour() == hours && minute() == minuts && second() <= 3) {
        char data[47] = "";    
        sprintf(data,"ZbSend {\"Device\":\"%s\",\"Send\":{\"Power\":\"%d\"}}", timersZB[index].short_id, timersZB[index].action);
        Serial.write(data, sizeof(data)); 
        Serial.write('\n'); 
        break;         
      }
    }         
  }
  
}
  
#endif


  



 