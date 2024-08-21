

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


////////////
// MACROS //
////////////
#define SHELLY1L 0
#define M5_1C_86 0
#define POWER320D 0
#define BISTABLE_RELAY 0
#define ESPNOW_TASMOTA_ZIGBEE_BRIDGE 1
#define SWAP_UART 0
#define HIDEN_SERVER 0
#define IS_PASSIVE_MASTER 0
#define PASSIVE_SLAVE_ID 0 //4164129
#define DEBUG 0
#define TELNET 0
#define CTRLDIAS 0

/////////////////////////////////////
// Configuração do pin a controlar //
/////////////////////////////////////
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

///////////////////////
// Constantes Gerais //
///////////////////////
#define QTD_TMRS_ESPNOW 16                 // Número máximo de Timers para dispozitivos ESP-NOW
                                           // (*J*) Deve coincidir com a do Javascript    

#define  QTD_TMRS_ZB 4                     // Número máximo de Timers para dispozitivos ZIGBEE 
                                           // (*J*) Deve coincidir com a do Javascript   
#if defined(ESP8266) 
  #define QTD_BOARDS_ESPNOW 7           // Número máximo de Clientes ESP
#elif defined(ESP32) 
  #define QTD_BOARDS_ESPNOW 7             // Número máximo de Clientes ESP
#endif

#define QTD_DEVS_ZIGBEE 6                  // Número máximo de Dispositivos ZigBee
#define QTD_BOARDS  (QTD_DEVS_ZIGBEE + QTD_BOARDS_ESPNOW)

#define MAC_BYTE_SIZE 6                     // Tamanho do buffer de MAC Address 
#define MAC_STR_LENGTH 19
#define MAX_NAME_LENGTH 24                  // Comprimento do Nome dos dispositivos (24 - 1)
#define JSON_DOC_SIZE 249                   // 250 - Max para esp-now (249 + 1bye de fim de cadeia)
#define ZB_JSON_DOC_SIZE 512                // Tamnho do json recebido do tasmota  
#define ZB_SHORT_ID 7                       // Tamnho do short id dos dispositivos zigbee 
#define DT_STR_SIZE 21                      // Tamanho da string contendo a data e hora do Device
#define SSID_STR_SIZE 31                    // Tamanho da string contendo o nome de SSID
#define PWD_STR_SIZE 31                     // Tamanho da string contendo a senha
#define MAX_TIME_CONN 30                    // Tempo, em segundos, para conexão à rede WiFi  
#define MAX_MOD_SIZE 5    
#define STR_CFG_SIZE 3  
#define YEAR_COMPARE 2023
#define HORA_MINUTOS 6
#define MAX_WIFI_CH 13
#define ALIVE_TIMEOUT 45//15                     // Tempo em segundos
#define TEMPO_ENVIAR_ALIVE (ALIVE_TIMEOUT / 3)  // Tempo em segundos
#define IP_MODE_SIZE 8                        // Tamanho do buff P/ modos de ip nos modos WIFI_STA e WIFI_AP  
#define EEPROM_START_ADDR 0
//#define SOCKSERVERADDR_SIZE 50
//#define NOME_OTA "SRVR_CLT_WS"
#define FILE_DATA_SPIFFS "/data.txt"
#define FILE_ZBTMRS_SPIFFS "/zbtimers.txt"
#define TYPE_SIZE 5                         // Tamanho do BUFF para o tipo de dispositivo
#define MAX_ID_DIGITS 7
#define DBOUNCE_TIME 200 // Tempo em millisec
#define ID_BROADCAST_ESPNOW 9999999         // ID para broadcast aos dispositivos ESP-NOW
#if DEBUG == 1
#define imprime(x) Serial.print(x)
#define imprimeln(x) Serial.println(x)
#else
#define imprime(x)
#define imprimeln(x)
#endif




//////////////////////////////////////////
// Estrutura e Array p/ controlar alive //
//////////////////////////////////////////
typedef struct Alive{
  uint32_t board_id;
  uint32_t temp_alive;
  char board_name[MAX_NAME_LENGTH];
  char board_hist[DT_STR_SIZE];
  uint32_t BOARDS_ID[QTD_BOARDS]; // TODO: Rever quantidade de boards
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
  //uint32_t BOARDS_ID[QTD_BOARDS]; // TODO: Rever quantidade de boards
  char DT[DT_STR_SIZE];
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
  char BackupModo[MAX_MOD_SIZE]; 
  char SSID[SSID_STR_SIZE];
  char PWD[PWD_STR_SIZE];
  byte ip[4];
  byte gateway[4];
  byte subnet[4];
  byte dns1[4];
  byte dns2[4];
  /*char SOCKSERVERADDR[SOCKSERVERADDR_SIZE];
  uint16_t SOCKSERVERPORT;*/  
  char AP_STA_IP_MODE[IP_MODE_SIZE];  // AP_STA_IP_MODE = [DEFAULT | CUSTOM | DHCP | STATIC]
  char CONFIGURADO[STR_CFG_SIZE];
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

#if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1 
  //#define EEPROM_SIZE ( 2 + sizeof(data2EEPROM))
  #define EEPROM_SIZE ( 2 + sizeof(data2EEPROM) + (sizeof(timers2EEPROM) * QTD_TMRS_ESPNOW))
#else
  #define EEPROM_SIZE ( 2 + sizeof(data2EEPROM) + (sizeof(timers2EEPROM) * QTD_TMRS_ESPNOW))
#endif


//////////////////////////
// Tipo de Dispositivos //
//////////////////////////
enum DEV_TYPES{
  lAMPADA = 1, 
  HUB_IR, 
  E_BOMBA,
  BRIDGE_ZB,
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



///////////////////////  
// Variáveis Globais //
///////////////////////
AsyncWebSocket ws_obj("/ws");
AsyncWebSocket wss_obj("/wss");
AsyncWebServerRequest * requisicao;
uint8_t WIFI_CH = 0;
char json_array[JSON_DOC_SIZE];
char aux_dt[DT_STR_SIZE];
uint32_t BOARD_ID;
AsyncWebServer servidorHTTP(8090);
const char* CMND = "cmnd";               // Comandos recebidos por via do Javascript
uint32_t ctl_btn_pulso = 0;
uint64_t ctrl_flash_led = 0;
unsigned long cont_exec_timer = 0;
unsigned long ctl_clientes_offline = 0;
uint64_t cont_send_alive = 0;
data2EEPROM data2eeprom = {0};
data2SPIFFS data2spiffs = {0};
Alive array_alive[QTD_BOARDS];
unsigned long contador = 0; 
// TODO: Tornar dinâmica as escolhas abaixo (xx_ip_mode)
const char* ap_ip_mode = "DEFAULT";     // IP_MODE = [DEFAULT | CUSTOM]
const char* sta_ip_mode = "DHCP";       // STA_IP_MODE = [DHCP | STATIC]
#define EEPROM_TIMERS_ADDR sizeof(data2EEPROM) + 2
uint8_t iz_conex = 0;
//uint8_t novoMAC[MAC_BYTE_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xB0};//Produção
//uint8_t novoMAC[MAC_BYTE_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};//Testes

uint8_t novoMAC[MAC_BYTE_SIZE] = {0x78, 0xE3, 0x6D, 0x12, 0x29, 0x30};//Testes

uint8_t uz_MACADDR[MAC_BYTE_SIZE];
char cz_MACADDR[18];
uint32_t id_cliente_ws = 0;
//char* cz_array_json;//[DOC_DEVS_INFO_SIZE];//String cz_array_json = "";
//char historico_servidor[DT_STR_SIZE] = "00:00:00, 00/00/0000";
String sz_tunnel_ngrok;
uint8_t save_pin_state = 0;
//DynamicJsonDocument json_doc(JSON_DOC_SIZE);
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
  uint8_t iz_wifi_state = 0;
  char get_short_id[SHORT_DEV_ID_SIZE];

  //DynamicJsonDocument zb_json_doc(ZB_JSON_DOC_SIZE);
  StaticJsonDocument <ZB_JSON_DOC_SIZE> zb_json_doc;   
  
  zbtimers2SPIFFS timersZB[QTD_DEVS_ZIGBEE];

#endif

#if defined(ESP32) 
  esp_now_peer_info_t peerInfo;
  uint32_t esp32_get_uz_id;
#endif


#ifdef ESP32
  uint8_t enderecos_mac[][MAC_BYTE_SIZE] { {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, };
  uint32_t devs_id[QTD_BOARDS] = {0}; 
  uint8_t num_enderecos_mac = sizeof(enderecos_mac) / sizeof(enderecos_mac[0]);
#endif


// this part goes at the beginning of your code, before "setup"
#if CTRLDIAS == 1
byte hh = 0;
byte mi = 0;
byte ss = 0;                // hours, minutes, seconds
unsigned int dddd = 0;      // days (for if you want a calendar)
unsigned long lastTick = 0; // time that the clock last "ticked"
char timestring[25];        // for output
#endif


// Funçaão para tratar dados recebidos de clientes WebSocket JS 
void handleingIncomingData(void *arg, uint8_t *data, size_t len, uint32_t client_id);


// Callback para ebventos provenientes de clientes WebSocket JS
void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, 
             void * arg, uint8_t *data, size_t len);

void onEvent1(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, 
             void * arg, uint8_t *data, size_t len);             



// CallBack rx/tx ESP-NOW
#if defined(ESP8266) 
 void callback_tx_esp_now(uint8_t *mac_addr, uint8_t status); 
 void callback_rx_esp_now(uint8_t * mac, uint8_t *incomingData, uint8_t len);
#elif defined(ESP32) 
  void callback_tx_esp_now(const uint8_t *mac_addr, esp_now_send_status_t status);
  void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len);
#endif
 


// Converter MAC (string para Byte array)
uint8_t* converteMacString2Byte(const char* cz_mac);


// Convert IP de string para bytes
void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base);


// Limpar EEPROM
void LimpaDadosDispositivo();


// Mudar Modo de Operação
void TrocarModoOperacao(const char* cz_modo);


// Tira, da EEPROM, dados do Dispositivo
const char* ObterDadosDispositivo();


//Tira da EEPROM, dados do Dispositivo
void cb_scan_wifi(uint8_t networksFound);


//Criar dados JSON
const char* CriarJSON(const char* MSG_TYPE, const char* CMD, const char* MAC, uint32_t ID, int8_t PIN_STT, uint32_t WIFI_CH);


//Enviar dados ao Servidor e clientes WebSocket
void SendData2WebSocket(uint32_t board_id, uint8_t pin_state, const char* DT, const char* nome, const char* dateTime, uint8_t action);

// Conforma a data e a hora numa stiring (H:mm:ss d m Y)
void ObterDTActual(char* cz_dt);


// troca o estado de PIN e guarda dados (estado, data e hora) na EEPROM
void TogglePIN();


// Executar acções do Timer
void ExecutarTimer(const char* nome_arquivo);


// Configuracão de timers 
void ConfigurarTimer(const char* cz_json, const char* nome_arquivo);


// Processa/sincronoza a data e hora, no farmato epoch OVERLOAD 
void SincronizaDataHora(uint32_t unix_date_time);

// Consulta de Timers
void ConsultarTimer(const char* nome_arquivo, uint8_t index);

// Limpar configurações de Temporizadores
//void ResetTimers();


// Funçaão para ler spiffs 
void LerSPIFFS(data2SPIFFS *ddv, const char* nome_arquivo);


// Funçaão para escrever spiffs
void EscreverSPIFFS(data2SPIFFS *ddv, const char* nome_arquivo);


// Limpara ArquivoSPIFFS com pin_sttate, DT e Array Alive 
void ResetSPIFFS();


//Checa se um array deinteiros oelemento indicado 
int8_t ArrayHasElement(uint32_t array[], uint32_t element);


// Criar ID 
uint32_t CriarDEVICE_ID();

// Actualiza Data e Hora atraves de Servidor NTP
bool actualizaDTfromNTP(const char* servidor_ntp);

// Enviar comando para alterar estado do PIN do Dispositivo PASSIVO 
void mudarEstadoPINPassivo(uint8_t estado, uint32_t dev_id);

// Aumentar a Potencia de Transmissao do WiFi
void aumentarPotenciaTxWiFi();


// troca o estado de PIN para Relay bistable
#if BISTABLE_RELAY == 1
  void ToggleBISTABLE(uint8_t pin_on, uint8_t pin_off);
#endif  

// Liga/Desliga Relay bistable 
#if BISTABLE_RELAY == 1
  void OnOffBISTABLE(uint8_t estado, uint8_t pin_on, uint8_t pin_off);
#endif


#if defined(ESP32) 
  void guardar_endereco_mac(uint8_t newMAC[6],  uint32_t dev_id);
#endif

// Consulta Estado de dispositivos Zigbee
#if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1 
  uint8_t ctl_send_zbstatus_3 = 0;
  //boolean send_zb_status_3 = false;
  uint32_t ZbDEVICE_ID(String zbmac,  DEV_TYPES type);
  void processarMSG(String dados);
  int8_t ArrayZbHasElement(ZigbeeDevices arr[], const char* element);
  //int8_t ArrayZbTimerHasElement(zbtimers2SPIFFS arr[], const char* element);
  uint8_t obterDigitoInvertido(uint32_t numero, uint8_t posicao);
  void LerZbSPIFFS(zbtimers2SPIFFS *ddv, const char* nome_arquivo);
  void EscreverZbSPIFFS(zbtimers2SPIFFS *ddv, const char* nome_arquivo);
  void ExecutarTimerZb(const char* nome_arquivo); 
  void guardarActualizarTimerZigBee( uint8_t offset, uint8_t iz_output, uint8_t iz_action, uint8_t iz_enable, uint32_t iz_id, const char *hor_min, const char *weedays, const char* short_id);

#endif


// Processar dados recebidos via Telne
#if TELNET == 1
  void LerTelnet() ;
#endif


// ICACHE_RAM_ATTR IRAM_ATTR
void IRAM_ATTR attInterruptChangePIN();
void IRAM_ATTR attInterruptFallPIN();
void IRAM_ATTR attInterruptRisePIN();


/////////////
// setup() //
/////////////
void setup() {

  // Comfigurações iniciais.
  // Init Serial Monitor  
  #if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1
    Serial.begin(115200);     
  #else
    Serial.begin(115200);
  #endif

  
  #if defined(ESP8266) 
    #if SWAP_UART == 1
      Serial.swap(); // Para alguns Wemos D1 Mini GPIO15(D8,TX) and GPIO13(D7,RX)          
    #endif
  #endif

  delay(1000);//while(!Serial);


  //sprintf(cz_novoMAC, "%02X:%02X:%02X:%02X:%02X:%02X", novoMAC[0], novoMAC[1], novoMAC[2], novoMAC[3], novoMAC[4], novoMAC[5]);

  // Configurar PIN's  
  #if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 0
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

  BOARD_ID = CriarDEVICE_ID();


  // Initialize SPIFFS
  if (!LittleFS.begin()) {
    imprimeln(F("Erro de montagem do SPIFFS!"));
    return;
  }
  else{
    imprimeln(F("\nSPIFFS OK.\n"));   
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
    imprime("Tamanho do arquivo SPIFFS: ");
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
    //ResetTimers();
    ResetSPIFFS();
    ESP.restart();
  }
 


  // Resposta para comandos/Opções inálidas
  servidorHTTP.onNotFound([](AsyncWebServerRequest *request){
    const char* html = "<div style='padding:5px; text-align: center; position: absolute; width: 100%; margin: auto;'><div style='height:45px; padding:15px;'><font size='3px', color='red'><h1>ERRO:</h1></font></div><div style='display: inline;'><font color='blue', size='4px'></font><font color='', size='4px'>Comando/Opção Inválido!</font></div></div>";
    //request->send(404, "text/html", html);
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", html);
    response->addHeader("Content-Type", "text/html; charset=utf-8");
    request->send(response);
  });

 
  if( strcmp(data2eeprom.Modo, "OTA") == 0 ){

    RedeWifi redeAP(data2eeprom.nome, "123456789");
    redeAP.CriaRedeWifi(data2eeprom.AP_STA_IP_MODE); // IP_MODE = [DEFAULT | CUSTOM]

    // Inicializar o ElegantOTA
    AsyncElegantOTA.begin(&servidorHTTP);      

      // Processa comandos recebidos através de requisições HTTP/GET 
      servidorHTTP.on("/cm", HTTP_GET, [](AsyncWebServerRequest * request) {
        //char cpy_cz_json[JSON_DOC_SIZE];
        char cz_json[JSON_DOC_SIZE];       
      
        if (request->hasParam(CMND)) {
          strncpy(cz_json, request->getParam(CMND)->value().c_str(), sizeof(cz_json));

          deserializeJson(json_doc, cz_json);
          const char* cz_cmd = json_doc["Comando"];                         

          if (json_doc.isNull()) {
            return;
          }

         
          if( strcmp(cz_cmd, "CANCEL_OTA") == 0 ){
              //TelnetStream.println(data2eeprom.BackupModo);           
              TrocarModoOperacao(data2eeprom.BackupModo);     
              ResetSPIFFS();           
              request->send(200, "text/plain",  "OK");
              ESP.restart();
          }  
          else if( strcmp(cz_cmd, "GET_DEV_NAME") == 0 ){                                        
            request->send(200, "text/plain",  data2eeprom.nome);
          }            
          else if( strcmp(cz_cmd, "RENAME_DEV") == 0 ){                
            data2EEPROM data2eeprom = {0}; 
            EEPROM.get(EEPROM_START_ADDR, data2eeprom);   
            const char* cz_nome = json_doc["Nome"];
            strncpy(data2eeprom.nome, cz_nome, sizeof(data2eeprom.nome)); 
            data2eeprom.nome[strlen(data2eeprom.nome)] = '\0';       
            EEPROM.put(EEPROM_START_ADDR, data2eeprom);
            EEPROM.commit();
            #if defined(ESP8266) 
              delay(5);
            #elif defined(ESP32) 
              vTaskDelay(5);
            #endif                      
            request->send(200, "text/plain",  "OK");
          }  
          else if( strcmp(cz_cmd, "RESET_DEVICE") == 0 ){
            LimpaDadosDispositivo();
            //ResetTimers();
            ResetSPIFFS();
            ESP.restart();
            request->send(200, "text/plain",  "");
          }  

          json_doc.clear();

        }        
      });  

    // Route for root / web page
    servidorHTTP.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {    
      request->send(LittleFS, "/update.html", "text/html");
    });
    
    // Route to load style.css file
    servidorHTTP.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(LittleFS, "/style.css", "text/css");
    });

    // Route to load script.js file
    servidorHTTP.on("/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(LittleFS, "/script.js", "text/javascript");
    });

    servidorHTTP.on("/sair", HTTP_GET, [](AsyncWebServerRequest * request) {
      // Reestabelece o modo anterior ao OTA
      //ResetSPIFFS();
      //TelnetStream.println(data2eeprom.BackupModo);
      TrocarModoOperacao(data2eeprom.BackupModo);      
      request->send(200, "text/plain",  "Sair do modo OTA.");  
    });


     servidorHTTP.on("/chmod", HTTP_GET, [](AsyncWebServerRequest * request) {
      data2EEPROM data2eeprom = {0};
      EEPROM.get(EEPROM_START_ADDR, data2eeprom);
      if( strcmp(data2eeprom.Modo, "OTA") == 0 ){
        TrocarModoOperacao(data2eeprom.BackupModo); 
      } 
      else{
        TrocarModoOperacao("OTA");   
      }
    });


  }    
  else{
    //TODO: Descomentar esta linha na versao definitiva
    servidorHTTP.on("/update", HTTP_GET, [](AsyncWebServerRequest * request) {
      const char* html = "<div style='padding:5px; text-align: center; position: absolute; width: 100%; margin: auto;'><div style='height:45px; padding:15px;'><font size='3px', color='blue'><h1>NOTA:</h1></font></div><div style='display: inline;'><font color='blue', size='4px'></font><font color='', size='4px'>Este modo não permite actualização do firmeware...</font></div></div>";
      //request->send(404, "text/html", html);
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", html);
      response->addHeader("Content-Type", "text/html; charset=utf-8");
      request->send(response);
    });

    if( strcmp(data2eeprom.Modo, "OPRA") == 0 ){
      RedeWifi redeAP(data2eeprom.nome, "123456789");
      redeAP.CriaRedeWifi(ap_ip_mode); // AP_IP_MODE = [DEFAULT | CUSTOM]      
    }
    else if( strcmp(data2eeprom.Modo, "OPRS") == 0 ){     
      RedeWifi redeWiFi(data2eeprom.SSID, data2eeprom.PWD);
      redeWiFi.ConectaRedeWifi(sta_ip_mode);// STA_IP_MODE = [DHCP | STATIC]
    }

    #ifdef ESP32 
      WiFi.setTxPower(WIFI_POWER_19_5dBm); // Sets WiFi RF power output to highest level, highest RF power usage
    #else  
      WiFi.setOutputPower(20); // Sets WiFi RF power output to highest level, highest RF power usage
    #endif

    servidorHTTP.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
      if( strcmp(data2eeprom.CONFIGURADO, "OK") != 0 ){
        LimpaDadosDispositivo();
        //ResetTimers();
        ResetSPIFFS();
        ESP.restart();
      }
      else{
        strncpy(data2eeprom.CONFIGURADO, "NO", sizeof(data2eeprom.CONFIGURADO));
        data2eeprom.CONFIGURADO[strlen(data2eeprom.CONFIGURADO)] = '\0';
        EEPROM.put(EEPROM_START_ADDR, data2eeprom);
        EEPROM.commit();
        #if defined(ESP8266) 
          delay(5);
        #elif defined(ESP32) 
          vTaskDelay(5);
        #endif
        ESP.restart(); 
      }
      
    });

    servidorHTTP.on("/restart", HTTP_GET, [](AsyncWebServerRequest * request) {
      ESP.restart();
    });

    #if defined(ESP32) 

       

      servidorHTTP.on("/macs", HTTP_GET, [](AsyncWebServerRequest * request) {
        String sx_mac = "<b>ENDEREÇOS MAC REGISTADOS:</b><br>";
        sx_mac += "<b>============================</b><br>";
        for (uint8_t i = 1; i < num_enderecos_mac; i++) {
          char cz_mac[18];
          snprintf(cz_mac, sizeof(cz_mac), "%02X:%02X:%02X:%02X:%02X:%02X",
                  enderecos_mac[i][0], enderecos_mac[i][1], enderecos_mac[i][2], 
                  enderecos_mac[i][3], enderecos_mac[i][4], enderecos_mac[i][5]);
          sx_mac += String(cz_mac) += "<br>";    
        }
       
        AsyncWebServerResponse *response = request->beginResponse(200, "text/html", sx_mac);
        response->addHeader("Content-Type", "text/html; charset=utf-8");
        request->send(response);        
      });
    #endif

  
    servidorHTTP.on("/chmod", HTTP_GET, [](AsyncWebServerRequest * request) {
      data2EEPROM data2eeprom = {0};
      EEPROM.get(EEPROM_START_ADDR, data2eeprom);
      if( strcmp(data2eeprom.Modo, "OTA") == 0 ){
        TrocarModoOperacao(data2eeprom.BackupModo); 
      } 
      else{
        TrocarModoOperacao("OTA");   
      }
    });

   
     // Guardar tunel NGROK, enviado pelo Servidor local ngrok
    servidorHTTP.on("/tunel", HTTP_GET, [](AsyncWebServerRequest * request) {

      if (request->hasParam(CMND)) {   
          sz_tunnel_ngrok = request->getParam(CMND)->value().c_str();
          /*const char* canal = request->getParam(CMND)->value().c_str();
          uint8_t num_bytes = strlen(canal);  
          sz_tunnel_ngrok = (char *) malloc(num_bytes*sizeof(char));      
          strcpy(sz_tunnel_ngrok, canal);*/ 
          request->send(200, "text/plain",  "Tunel recebido.");        
      }    

    });


    // Comando para aos dispositivos do tipo PASSIVO entrarem no modo OTA
    servidorHTTP.on("/grlota", HTTP_GET, [](AsyncWebServerRequest * request) {
      
      char czjson[JSON_DOC_SIZE];
      strcpy(czjson, CriarJSON("DATA", "GRLOTA", "", 0, 2, WiFi.channel()));
      
      esp_now_send(0, (uint8_t *)czjson, strlen(czjson)); 

      #if defined(ESP8266) 
        esp_now_send(0, (uint8_t *)czjson, strlen(czjson));  
      #elif defined(ESP32) 
        // cont = 1 -- xq o eleento 0 não conta
        for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
          esp_now_send(enderecos_mac[cont], (uint8_t *)czjson, strlen(czjson));
          delay(1);
        }
      #endif   
      
      const char* html = "<div style='padding:5px; text-align: center; position: absolute; width: 100%; margin: auto;'><div style='height:45px; padding:15px;'><font size='3px', color='blue'><h1>NOTA:</h1></font></div><div style='display: inline;'><font color='blue', size='4px'></font><font color='', size='4px'>Comando (modo OTA) enviado aos Disposotivos do Tipo PASSIVO.</font></div></div>";
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", html);
      response->addHeader("Content-Type", "text/html; charset=utf-8");
      request->send(response); 
    });


    // Servir tunel NGROK aos Clientes
    servidorHTTP.on("/canal", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(200, "text/plain",  sz_tunnel_ngrok);  
    });

    /*servidorHTTP.on("/power", HTTP_GET, [](AsyncWebServerRequest * request) {
      WiFiClient wifiClient;
      HTTPClient http;    
      http.begin(wifiClient, "http://192.168.0.249/cm?cmnd=Status%208");     
      uint16_t httpCode = http.GET();      
      String response = http.getString();                            
      http.end();        
      request->send(200, "text/plain",  response);     
    });*/

    servidorHTTP.on("/info", HTTP_GET, [](AsyncWebServerRequest * request) {      
      
    File arquivo = LittleFS.open(FILE_DATA_SPIFFS, "r");
    uint32_t f_size = (size_t)arquivo.size();
    arquivo.close();

    File arquivozb = LittleFS.open(FILE_ZBTMRS_SPIFFS, "r");
    uint32_t f_sizezb = (size_t)arquivozb.size();
    arquivozb.close();

    

    // size_t fileSizeKB = f_size / 1024;

    #if defined(ESP8266) 
      // Memória Flash Total:
      uint32_t flashSize = ESP.getFlashChipRealSize();
      flashSize /= 1024;
      // Memória RAM Total:
      uint32_t ramSize = ESP.getFlashChipRealSize() - ESP.getFreeHeap();
      // uint32_t ramSize = ESP.getFreeContStack();
      ramSize /= 1024;
      // Obter a quantidade de memória livre
      uint32_t freeHeap = ESP.getFreeHeap();
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

    // To display the time in the serial monitor:
    #if CTRLDIAS == 1
    sprintf(timestring,"%d days %02d:%02d:%02d", dddd, hh, mi, ss);
    #else
      char timestring[25];        // for output
     sprintf(timestring,"0");
    #endif

    char msg[512];
    sprintf(
      msg, "<br>ID: %d<br>MAC: %s<br><br>Número de conexões WS: %d<br><br>FLASH Total: %d KB<br>RAM Total: %d KB<br>RAM Livre: %d KB<br><br>SPIFFS Total: %d KB<br>Arquivos (%s): %d Bytes<br>SPIFFS Usado: %d KB<br>SPIFFS Livre: %d KB<br><br>Uptime: %s<br><br>",
      BOARD_ID, cz_MACADDR, (uint8_t)ws_obj.count(), flashSize, ramSize, freeHeap, totalSPIFFSSizeKB, FILE_ZBTMRS_SPIFFS "  "  FILE_DATA_SPIFFS, (f_sizezb + f_size), usedSPIFFSSpaceKB, freeSPIFFSSpaceKB, timestring);
      msg[strlen(msg)] = '\0';
      String str_1 = "<div style='align-items: center; text-align: center; position: absolute; width: 80%; margin: auto;'><div style='height:40px;'><font size='3px', color='blue'><u><h1>Dados do Servidor:</h1></u></font></div><div style='position:relative; margin: auto; max-width:75%; text-align: left;'><font size='3px'>";

      str_1 += msg + String("</font></div></div>"); 

      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", str_1);
      response->addHeader("Content-Type", "text/html; charset=utf-8");
      request->send(response);
    });


    //servidorHTTP.on("/devs", HTTP_GET, [](AsyncWebServerRequest * request) {    
       //request->send(200, "application/json", cz_array_json);
    //});



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

    // Serve favicon  
    servidorHTTP.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
      const char* dta = ("data:image/x-icon;base64,AAAB AAAAA");
      request->send(200, "image/x-icon", dta);        
    });


    // Processar procura de redes WiFi
      servidorHTTP.on("/scan_wifi", HTTP_GET, [](AsyncWebServerRequest *request){       
        requisicao = request;  
        
        #if defined(ESP8266) 
          WiFi.scanNetworksAsync(cb_scan_wifi);  
        #elif defined(ESP32)
          esp_task_wdt_init(30, false);
          //DynamicJsonDocument json_doc(JSON_DOC_SIZE);        
          StaticJsonDocument <JSON_DOC_SIZE> json_doc;
          uint8_t networksFound = WiFi.scanNetworks();
          char cz_key[4]; 
          for (uint8_t i = 0; i < networksFound; ++i){
            sprintf(cz_key, "%d", i);
            cz_key[strlen(cz_key)] = '\0';
            JsonObject rede = json_doc.createNestedObject(cz_key);
            rede["SSID"] = WiFi.SSID(i);
            vTaskDelay(1);           
          }

          memset(json_array, 0, sizeof(json_array));  
          serializeJson(json_doc, json_array);        
          requisicao->send(200, "text/json", json_array);
        #endif    
          
        //request->send(200, "text/json", "{\"0\":{\"SSID\":\"TPLINK1\"},\"1\":{\"SSID\":\"SONOFF\"}}");  
      });
      

      // Processa comandos recebidos através de requisições HTTP/GET 
      servidorHTTP.on("/cm", HTTP_GET, [](AsyncWebServerRequest * request) {
        //char cpy_cz_json[JSON_DOC_SIZE];
        char cz_json[JSON_DOC_SIZE];       
      
        if (request->hasParam(CMND)) {
          strncpy(cz_json, request->getParam(CMND)->value().c_str(), sizeof(cz_json));
         
          deserializeJson(json_doc, cz_json);
          const char* cz_cmd = json_doc["Comando"];                         

          if (json_doc.isNull()) {
            return;
          }

          if( strcmp(cz_cmd, "SET_WIFI_CFG") == 0 ){

            // Guarda as configurações, para acesso à rede WiFi.
            EEPROM.get(EEPROM_START_ADDR, data2eeprom);

            const char* cz_modo = json_doc["Modo"]; 
            const char* cz_ssid = json_doc["SSID"];
            const char* cz_pwd = json_doc["PWD"];
            const char* cz_ip = json_doc["IP"];
            const char* cz_gateway = json_doc["Gateway"];
            const char* cz_subnet = json_doc["Subnet"];
            const char* cz_dns1 = json_doc["Dns1"];
            const char* cz_dns2 = json_doc["Dns2"];
            //const char* cz_ws_addr = json_doc["WsAddr"];
            //uint16_t ws_port = json_doc["WsPort"];

            byte bz_ip[4];
            byte bz_gateway[4];
            byte bz_subnet[4];
            byte bz_dns1[4];
            byte bz_dns2[4];
            parseBytes(cz_ip, '.', bz_ip, 4, 10);
            parseBytes(cz_gateway, '.', bz_gateway, 4, 10);
            parseBytes(cz_subnet, '.', bz_subnet, 4, 10);  
            parseBytes(cz_dns1, '.', bz_dns1, 4, 10);   
            parseBytes(cz_dns2, '.', bz_dns2, 4, 10); 

            strncpy(data2eeprom.SSID, cz_ssid, sizeof(data2eeprom.SSID));
            data2eeprom.SSID[strlen(data2eeprom.SSID)] = '\0';
            strncpy(data2eeprom.PWD, cz_pwd, sizeof(data2eeprom.PWD));
            data2eeprom.PWD[strlen(data2eeprom.PWD)] = '\0';
            memcpy(data2eeprom.ip, bz_ip, sizeof(data2eeprom.ip));
            memcpy(data2eeprom.gateway, bz_gateway, sizeof(data2eeprom.gateway));
            memcpy(data2eeprom.subnet, bz_subnet, sizeof(data2eeprom.subnet));
            memcpy(data2eeprom.dns1, bz_dns1, sizeof(data2eeprom.dns1));
            memcpy(data2eeprom.dns2, bz_dns2, sizeof(data2eeprom.dns2));
            strncpy(data2eeprom.Modo, cz_modo, sizeof(data2eeprom.Modo));
            data2eeprom.Modo[strlen(data2eeprom.Modo)] = '\0';   
            //strncpy(data2eeprom.SOCKSERVERADDR, cz_ws_addr, sizeof(data2eeprom.SOCKSERVERADDR));
            //data2eeprom.SOCKSERVERPORT = ws_port;  
           
            EEPROM.put(EEPROM_START_ADDR, data2eeprom);
            EEPROM.commit();
            
            delay(5);

            request->send(200, "text/plain", "Operação Concluída.");  
            json_doc.clear();   
            // Reinicializa o Dispositivo
            ESP.restart();     
          }  
          else if( strcmp(cz_cmd, "GET_WIFI_CFG") == 0 ){
            request->send(200, "text/json",  ObterDadosDispositivo());
          }
          else if( strcmp(cz_cmd, "PERMITJOIN") == 0 ){   

            char data[15] = "ZbPermitJoin 1";    
            Serial.write(data, sizeof(data)); 
            Serial.write('\n'); 
                                                
            request->send(200, "text/plain",  "PERMITJOIN");
          }  
          else if( strcmp(cz_cmd, "RESET_DEVICE") == 0 ){
            LimpaDadosDispositivo();
            //ResetTimers();
            ResetSPIFFS();
            ESP.restart();
            request->send(200, "text/plain",  "");
          }
          else if( strcmp(cz_cmd, "GET_DEV_NAME") == 0 ){                                   
            request->send(200, "text/plain",  data2eeprom.nome);
          }  
          
        }        
      });  
         

    WIFI_CH = WiFi.channel();
    imprime(F("Canal: "));
    imprimeln(WIFI_CH);
    imprime(F("ID: "));
    imprimeln(BOARD_ID); 

    
    #if defined(ESP32) 
      //Force espnow to use a specific channel
      esp_wifi_set_promiscuous(true);
      esp_wifi_set_channel(WIFI_CH, WIFI_SECOND_CHAN_NONE);
      esp_wifi_set_promiscuous(false);
    #endif
  
    
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

     
    // WebSoket Server
    ws_obj.onEvent(onEvent);
    servidorHTTP.addHandler(&ws_obj);
    
    wss_obj.onEvent(onEvent1);
    servidorHTTP.addHandler(&wss_obj);

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
    ctl_clientes_offline = millis();
    
  }

  // Preenche array para controlo de alive
  for (uint8_t index = 0; index < QTD_BOARDS; index++){
    if(index == 0){
      array_alive[index].board_id = BOARD_ID;
      array_alive[index].temp_alive = 0;
    }
    else{
      array_alive[index].board_id =  255;
      array_alive[index].temp_alive =  0;
    }    
  }

  // Preencher Array com dados do SPIFFS
  for (uint8_t cont = 0; cont < QTD_BOARDS; cont++){
    if (array_alive[0].BOARDS_ID[cont] != 255){
      array_alive[cont].board_id = array_alive[0].BOARDS_ID[cont];
      array_alive[cont].temp_alive = 0;
    }
  }

  // TODO: Apagar/comentar esta linha na versao definitiva
  //AsyncElegantOTA.begin(&servidorHTTP); 
  
  // Start server
  servidorHTTP.begin(); 
  contador = millis(); 

  // TODO: Debug only
  //ArduinoOTA.setHostname("OTA_D1MINI");
  //ArduinoOTA.begin();
  
  // Inicializa Telnet  
  #if TELNET == 1
    TelnetStream.begin();
  #endif

  // Aumenta Potencia de Tx do WiFi
  aumentarPotenciaTxWiFi(); 

  
}



////////////
// loop() //
////////////
void loop() {
// TODO: Debug only
//ArduinoOTA.handle();

 //////////////////////////////////////////
 //                                      // 
 //////////////////////////////////////////
 if( strcmp(data2eeprom.Modo, "OPRS") == 0 || strcmp(data2eeprom.Modo, "OPRA") == 0 ){
    
    if(exec_toggle == true){
      exec_toggle = false; 
      TogglePIN();      
    }

     
    
    // Checar Timer cada X tempo
    if( (millis() - cont_exec_timer)/1000 > 1 ){
      cont_exec_timer = millis();      
      //ExecutarTimer(FILE_DATA_SPIFFS);  
      #if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1  
        ExecutarTimerZb(FILE_ZBTMRS_SPIFFS); 
      #endif    

      // WebSocket
      ws_obj.cleanupClients();
      wss_obj.cleanupClients();

      #if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1      

      while (Serial.available() > 0){

        static String data;
        char rx_byte = Serial.read();
        data += rx_byte;

        if (rx_byte == '\n' && procesing_uart_data == false) {
          processarMSG(data);
          data = "";
        }
      }

    #endif 

    }



    // Envia msg alive cada X tempo.
    if( (millis() - cont_send_alive)/1000 >= TEMPO_ENVIAR_ALIVE ){  
      uint8_t estado = 0;
      #if POWER320D == 1
        estado = relayState;
      #else
        estado = digitalRead(PIN_RELAY); 
      #endif  
      EEPROM.get(EEPROM_START_ADDR, data2eeprom);
      SendData2WebSocket(BOARD_ID, estado, "SAS", data2eeprom.nome, "", 0);

      // Actualizar DT, se necessśrio, primeiro tenta pelo NTP depois pelos clientes WebSocket/Navegadores
      if (year() < YEAR_COMPARE){ 
        if(actualizaDTfromNTP("pool.ntp.org") == false){
          SendData2WebSocket(BOARD_ID, estado, "UDT", "","", 0);
        }         
      }


      // Controlar RAM livre
      #if defined(ESP8266)      
        // Obter a quantidade de memória livre
        uint32_t freeHeap = ESP.getFreeHeap();
        freeHeap /= 1024;      
      #elif defined(ESP32)      
        // Obter a quantidade de memória livre
        uint32_t freeHeap = ESP.getFreeHeap();
        freeHeap /= 1024;     
      #endif

      if(freeHeap <= 29){
        ESP.restart();
      }


      /*#if TELNET == 1

      data2SPIFFS data2spiffs = {0};
      LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);
      
      FSInfo info;
      LittleFS.info(info);

      File arquivo = LittleFS.open(FILE_DATA_SPIFFS, "r");
      uint32_t f_size = (size_t)arquivo.size();
      arquivo.close();

      // To display the time in the serial monitor:
      #if CTRLDIAS == 1
      sprintf(timestring,"%d days %02d:%02d:%02d", dddd, hh, mi, ss);
      #else
        char timestring[25];        // for output
      sprintf(timestring,"");
      #endif

      TelnetStream.printf("  %u:%u  %s\n", hour(), minute(), timestring);
      TelnetStream.printf("  ======================\n");
      TelnetStream.printf("  Tamanho do Arquivo: %s -- %u bytes\n", FILE_DATA_SPIFFS, f_size);

      TelnetStream.printf("  Espaço total: %u KB\n", info.totalBytes/1024);
      TelnetStream.printf("  Espaço usado: %u KB\n", info.usedBytes/1024);
      TelnetStream.printf("  Espaço livre: %u KB\n", (info.totalBytes - info.usedBytes)/1024);

      TelnetStream.printf("  RAM livre: %u KB\n", ESP.getFreeHeap()/1024);
      TelnetStream.printf("\n\n");
      #elif DEBUG == 1
      data2SPIFFS data2spiffs = {0};
      LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);
      
      FSInfo info;
      LittleFS.info(info);

      File arquivo = LittleFS.open(FILE_DATA_SPIFFS, "r");
      uint32_t f_size = (size_t)arquivo.size();
      arquivo.close();

      // To display the time in the serial monitor:
      sprintf(timestring,"%d days %02d:%02d:%02d", dddd, hh, mi, ss);


      Serial.printf("  %u:%u  %s\n", hour(), minute(), timestring);
      Serial.printf("  ======================\n");
      Serial.printf("  Tamanho do Arquivo: %s -- %u bytes\n", FILE_DATA_SPIFFS, f_size);

      Serial.printf("  Espaço total: %u KB\n", info.totalBytes/1024);
      Serial.printf("  Espaço usado: %u KB\n", info.usedBytes/1024);
      Serial.printf("  Espaço livre: %u KB\n", (info.totalBytes - info.usedBytes)/1024);

      Serial.printf("  RAM livre: %u KB\n", ESP.getFreeHeap()/1024);
   
      Serial.printf("\n\n");
      #endif */ 


      // Envia ZbStatus3 a todos dispositivos
      /*
        for (uint8_t cont = 0; cont < QTD_DEVS_ZIGBEE; cont++){
                
          if ( strcmp(arr_zb_devs[cont].short_id, "") != 0){ 
            char data[17];
            sprintf(data, "ZbStatus3 %s", arr_zb_devs[cont].short_id);  
            Serial.write(data, strlen(data)); 
            Serial.write('\n');
            delay(50);
          }

        }  

        send_zb_status_3 = false;

      }*/

     
      // Solicita lista de dispositivos emparelhados e dados detalhados dos dispositivos registados
      #if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1
        
        Serial.write("ZbStatus", 8);      
        Serial.write('\n');  

        // Envia ZbStatus3 ao dispositivo indicado
        if (ctl_send_zbstatus_3 < QTD_DEVS_ZIGBEE){
          String str = String(arr_zb_devs[ctl_send_zbstatus_3].short_id);      
          if ( str.indexOf("0x") != -1 ){ 
            char data[17];
            sprintf(data, "ZbStatus3 %s", arr_zb_devs[ctl_send_zbstatus_3].short_id);  
            Serial.write(data, strlen(data)); 
            Serial.write('\n');
          } 

          ctl_send_zbstatus_3++;

        }  
        else{
          ctl_send_zbstatus_3 = 0;
        }

      #endif    

      cont_send_alive = millis();       
    } 



    // Controlo OFFLINE
    if( (millis() - ctl_clientes_offline)/1000 >= 5 ){
      // Preencher Array com dados do SPIFFS
      LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);
      // Preencher Array com dados do SPIFFS
      for (uint8_t cont = 0; cont < QTD_BOARDS; cont++){
        if (array_alive[0].BOARDS_ID[cont] != 255){
          array_alive[cont].board_id = array_alive[0].BOARDS_ID[cont];         
        }
      }

     

      for(uint8_t cont = 0; cont < QTD_BOARDS; cont++){        
         unsigned long tmp = now() - array_alive[cont].temp_alive;

        if( tmp >= ALIVE_TIMEOUT && array_alive[cont].board_id != 255){   

          // Obter marca de devices zigbee, de tras para frente
          uint8_t quarto = obterDigitoInvertido(array_alive[cont].board_id, 1);
          uint8_t quinto = obterDigitoInvertido(array_alive[cont].board_id, 2); 
          if(quarto != 0 && quinto != 0){// Se nao for ZicBee envia OFF
            SendData2WebSocket(array_alive[cont].board_id, 2, "OFF", array_alive[cont].board_name, "", 0);  
          
            char czjson[JSON_DOC_SIZE]; 
            strcpy(czjson, CriarJSON("", "ADS", "", array_alive[cont].board_id, 2, WiFi.channel())); 
             
            #if defined(ESP8266) 
              esp_now_send(0, (uint8_t *)czjson, strlen(czjson));
            #elif defined(ESP32) 
              // cont = 1 -- xq o eleento 0 não conta
              for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
                if( devs_id[cont] == esp32_get_uz_id ) esp_now_send(enderecos_mac[cont], (uint8_t *)czjson, strlen(czjson));
              }
            #endif      
          }          
        }
      
      } 
      

      // TODO: Checar hora e data 
      /*char dt[21];
      ObterDTActual(dt);
      Serial.println(dt);*/

      ctl_clientes_offline = millis();

      //LittleFS.end();
    }// FIM de Controlo OFFLINE
    
  }
  else if( strcmp(data2eeprom.Modo, "OTA") == 0 ){     
    // Piscar o LED cada X tempo
    if( (millis() - ctrl_flash_led) >= 200){
      digitalWrite(PIN_LED, !digitalRead(PIN_LED));
      ctrl_flash_led = millis();
    }    
  }

  #if TELNET == 1
    LerTelnet();  
    yield();
  #endif

  

  #if defined(ESP32) 
    vTaskDelay(100);
  #endif
  
  
} // FIM do loop()



/////////////////////////////////////////
// Atecnção à interrupção do PIN GPIO0 //
/////////////////////////////////////////
// ICACHE_RAM_ATTR IRAM_ATTR
void IRAM_ATTR attInterruptFallPIN() {  

  noInterrupts();  

  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptRisePIN, RISING);
  
  #if SHELLY1L == 0     
    ctl_btn_pulso = millis();
  #elif SHELLY1L == 1 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > DBOUNCE_TIME){      
      //exec_toggle = true; 
      //TelnetStream.println("FALL");   
      TelnetStream.print(digitalRead(PIN_BUTTON));
      TelnetStream.print(" -- ");
      TelnetStream.println("FALL");    
    }
    last_interrupt_time = interrupt_time;
  #endif
  interrupts();
}


///////////////////////////////////////////////////
// Atecnção à interrupção do PIN GPIO4 SHELLY 1L //
///////////////////////////////////////////////////
void IRAM_ATTR  attInterruptChangePIN(){  

  #if SHELLY1L == 1 

  noInterrupts();

  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > DBOUNCE_TIME){
    //exec_toggle = true; 
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptFallPIN, FALLING);  
    //attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptRisePIN, RISING); 
    //TelnetStream.println("CHANGE");
    TelnetStream.print(digitalRead(PIN_BUTTON));
    TelnetStream.print(" -- ");
    TelnetStream.println("CHANGE");
  }
  last_interrupt_time = interrupt_time;

  interrupts();

  
    //noInterrupts();
    
    /*if(digitalRead(PIN_BUTTON)){
      exec_toggle = true; 
      TelnetStream.println("itreept2");
    }else{
      attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptRisePIN, RISING);  
      exec_toggle = true; 
      TelnetStream.println("itreept3");
    }*/

    //interrupts();
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
    data2EEPROM data2eeprom = {0};
    EEPROM.get(EEPROM_START_ADDR, data2eeprom);
    
    if( tmp >= 3000 ){  
      if( strcmp(data2eeprom.Modo, "OTA") == 0 ){
        TrocarModoOperacao(data2eeprom.BackupModo);   
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
    interrupts(); 

  #elif SHELLY1L == 1 
    noInterrupts();

    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    // If interrupts come faster than 200ms, assume it's a bounce and ignore
    if (interrupt_time - last_interrupt_time > DBOUNCE_TIME){
      /*attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptFallPIN, FALLING);  
      exec_toggle = true; 
      TelnetStream.println("RISE");*/
      if(save_pin_state != digitalRead(PIN_BUTTON)){
        exec_toggle = true; 
        //save_pin_state = digitalRead(PIN_BUTTON);
        TelnetStream.print(digitalRead(PIN_BUTTON));
        TelnetStream.print(" -- ");
        TelnetStream.println("RISE");   
      }
      else{
        if(save_pin_state == 0 && digitalRead(PIN_BUTTON) == 1){
          exec_toggle = true; 
        }
        TelnetStream.println("FALSE RISE"); 
        TelnetStream.print(now()); 
        TelnetStream.print(" -- ACTUAL -- "); 
        TelnetStream.println(digitalRead(PIN_BUTTON));
        TelnetStream.print(now()); 
        TelnetStream.print(" -- ULTIMO -- "); 
        TelnetStream.println(save_pin_state);
      }
      
    }
    last_interrupt_time = interrupt_time;

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

#if defined(ESP8266)
  WiFi.mode(WIFI_STA);  
  // For Soft Access Point (AP) Mode
  // wifi_set_macaddr(SOFTAP_IF, &novoMAC[0]);
  wifi_set_macaddr(0, &novoMAC[0]);
#elif defined(ESP32)
  WiFi.mode(WIFI_AP_STA); 
  bool ssid_hidden  = true;
  uint8_t wifi_channel = 1;
  WiFi.softAP(data2eeprom.nome, "123456789", wifi_channel, ssid_hidden); 
  
  /*WiFi.mode(WIFI_STA);
  Serial.print("\nOLD ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());
  esp_wifi_set_mac(WIFI_IF_STA, &novoMAC[0]);
  Serial.print("NEW ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());*/


  //WiFi.mode(WIFI_AP_STA);  
  // ESP32 Board add-on after version > 1.0.5
  //esp_wifi_set_mac(WIFI_IF_STA, &novoMAC[0]);
  // ESP32 Board add-on before version < 1.0.5
  //esp_wifi_set_mac(ESP_IF_WIFI_AP, &novoMAC[0]);
#endif

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
    #if defined(ESP8266) 
      delay(1000);
    #elif defined(ESP32) 
      vTaskDelay(500);
    #endif
    imprimeln("Conectando-se a: " + (String)_ssid + " ... " + String(contador++) + " Seg");
  }
  

  if (WiFi.status() == WL_CONNECTED) {
    // Guarda MAC address    
    #if defined(ESP8266) 
      memcpy(uz_MACADDR, converteMacString2Byte(WiFi.macAddress().c_str()), sizeof(uz_MACADDR));
      strcpy(cz_MACADDR, WiFi.macAddress().c_str()); 
    #elif defined(ESP32) 
      memcpy(uz_MACADDR, converteMacString2Byte(WiFi.macAddress().c_str()), sizeof(uz_MACADDR));
      strcpy(cz_MACADDR, WiFi.macAddress().c_str());
    #endif     

    imprimeln("");
    imprimeln(F("================="));
    imprimeln(F("Rede WiFi Ligada."));
    imprimeln(F("================="));
    imprimeln("SSID: " + WiFi.SSID() + "\nIP: " + WiFi.localIP().toString());
    imprime("MAC: ");  
    imprimeln(cz_MACADDR);   
     

    // Reconectar a Rede WiFi
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    // Actualizar Hora e data usando servido NTP
    // For WAT UTC +1.00 : 1 * 60 * 60 : 3600
    actualizaDTfromNTP("pool.ntp.org");    

  }
  else{
    imprimeln("");
    imprimeln(F("Rede WiFi Indisponível!"));
    TrocarModoOperacao("OPRA"); 
  }

}




/////////////////////////////
// Criar Rede Wifi AP_MODE //
/////////////////////////////
void RedeWifi::CriaRedeWifi(const char* AP_IP_MODE) {// AP_IP_MODE = [DEFAULT | CUSTOM]
  
  WiFi.mode(WIFI_AP_STA);
  
#if defined(ESP8266)
  // For Soft Access Point (AP) Mode
  //wifi_set_macaddr(SOFTAP_IF, &novoMAC[0]);
  wifi_set_macaddr(0, &novoMAC[0]);
#elif defined(ESP32)
  // ESP32 Board add-on after version > 1.0.5
  esp_wifi_set_mac(WIFI_IF_AP, &novoMAC[0]);
  // ESP32 Board add-on before version < 1.0.5
  //esp_wifi_set_mac(ESP_IF_WIFI_STA, &novoMAC[0]);
#endif

  char aux[IP_MODE_SIZE];
  if( strcmp(AP_IP_MODE, "") == 0){
    strncpy(aux, "DEFAULT", IP_MODE_SIZE);
  }
  else{
    strncpy(aux, AP_IP_MODE, IP_MODE_SIZE);
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
      QTD_BOARDS_ESPNOW,            // MAX_CONNECTIONS
      false                         // true -> rede_default (192.168.4.1/24); false -> Rede definida
    };

    WiFi.softAPConfig(dados_rede.ip, dados_rede.gateway, dados_rede.subnet);
    WiFi.softAP(_ssid, _pwd, dados_rede.CHANNEL, dados_rede.HIDE_SSID, dados_rede.MAX_CONNECTIONS);   
  }

  memcpy(uz_MACADDR, converteMacString2Byte(WiFi.softAPmacAddress().c_str()), sizeof(uz_MACADDR));
  strcpy(cz_MACADDR, WiFi.softAPmacAddress().c_str());

  imprimeln("");  
  imprimeln(F("======================="));
  imprimeln(F("Ponto de Acesso Criado."));
  imprimeln(F("======================="));
  imprimeln("SSID: " + WiFi.softAPSSID() + "\nIP: " + WiFi.softAPIP().toString());
  imprime("MAC: ");
  imprimeln(cz_MACADDR);
   
}







////////////////////////////////////////////
// Converter MAC (string para Byte array) //
////////////////////////////////////////////
uint8_t* converteMacString2Byte(const char* cz_mac) {

  //static uint8_t MAC[6];
  uint8_t* MAC = (uint8_t*)calloc(6, sizeof(uint8_t));
  char* ptr;

  MAC[0] = strtol(cz_mac, &ptr, HEX );
  for ( uint8_t i = 1; i < 6; i++ ) {
    MAC[i] = strtol(ptr + 1, &ptr, HEX );
  }

  return MAC;
}




///////////////////////////////////////////////////////////
// Conforma a data e a hora numa stiring (H:mm:ss d m Y) //
///////////////////////////////////////////////////////////
void ObterDTActual(char* cz_dt){  
  //sprintf(cz_data_hora, "%02d:%02d:%02d %02d/%02d/%d", hour(), minute(), second(), day(), month(), year());
  sprintf(cz_dt, "%d:%02d:%02d, %d/%d/%d", hour(), minute(), second(), day(), month(), year()); 
}



////////////////////////////////////
//Convert IP de string para bytes //
////////////////////////////////////
void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
    for (int i = 0; i < maxBytes; i++) {
        bytes[i] = strtoul(str, NULL, base);  // Convert byte
        str = strchr(str, sep);               // Find next separator
        if (str == NULL || *str == '\0') {
            break;                            // No more separators, exit
        }
        str++;                                // Point to next character after separator
    }
}




////////////////////////////////////////////
// Limppa EEPROM, altera modo de operação //
////////////////////////////////////////////
void LimpaDadosDispositivo(){  

  data2EEPROM data2eeprom = {0};
  memset(&data2eeprom, 0, sizeof(data2eeprom));
  EEPROM.put(EEPROM_START_ADDR, data2eeprom);  
  EEPROM.commit();
  #if defined(ESP8266) 
    delay(10);
  #elif defined(ESP32) 
    vTaskDelay(5);
  #endif

  EEPROM.get(EEPROM_START_ADDR, data2eeprom); 

  // Guardar informações padrão na EEPROM.
  if( strcmp(data2eeprom.CONFIGURADO, "OK") == 0 && ( strcmp(data2eeprom.Modo, "OPRS") == 0 || strcmp(data2eeprom.Modo, "OPRA") == 0) ){
    strncpy(data2eeprom.Modo, data2eeprom.BackupModo, sizeof(data2eeprom.Modo));
    data2eeprom.Modo[strlen(data2eeprom.Modo)] = '\0';   
  }
  else{
    strncpy(data2eeprom.Modo, "OPRA", sizeof(data2eeprom.Modo));
    data2eeprom.Modo[strlen(data2eeprom.Modo)] = '\0'; 
  }
  
  strncpy(data2eeprom.SSID, "", sizeof(data2eeprom.SSID)); 
  data2eeprom.SSID[strlen(data2eeprom.SSID)] = '\0';
  strncpy(data2eeprom.PWD, "", sizeof(data2eeprom.PWD));
  data2eeprom.PWD[strlen(data2eeprom.PWD)] = '\0';
  strncpy(data2eeprom.nome, "[Nome]", sizeof(data2eeprom.nome));
  data2eeprom.nome[strlen(data2eeprom.nome)] = '\0';   
  strncpy(data2eeprom.AP_STA_IP_MODE, "", sizeof(data2eeprom.AP_STA_IP_MODE));
  data2eeprom.AP_STA_IP_MODE[strlen(data2eeprom.AP_STA_IP_MODE)] = '\0';
  strncpy(data2eeprom.CONFIGURADO, "OK", sizeof(data2eeprom.CONFIGURADO));
  data2eeprom.CONFIGURADO[strlen(data2eeprom.CONFIGURADO)] = '\0';
  memset(data2eeprom.ip, 0, sizeof(data2eeprom.ip));
  memset(data2eeprom.gateway, 0, sizeof(data2eeprom.gateway));
  memset(data2eeprom.subnet, 0, sizeof(data2eeprom.subnet));
  memset(data2eeprom.dns1, 0, sizeof(data2eeprom.dns1));
  memset(data2eeprom.dns2, 0, sizeof(data2eeprom.dns2)); 
  //strncpy(data2eeprom.SOCKSERVERADDR, "", sizeof(data2eeprom.SOCKSERVERADDR));
  //data2eeprom.SOCKSERVERPORT = 0;

  
  #if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1
    data2eeprom.TIPO = BRIDGE_ZB;
  #else
    data2eeprom.TIPO = lAMPADA;//BRIGE_ZB, E_BOMBA;//HUB_IR; // Lâmpada por defeito
  #endif
 
  EEPROM.put(EEPROM_START_ADDR, data2eeprom);  
  EEPROM.commit();
  #if defined(ESP8266) 
    delay(10);
  #elif defined(ESP32) 
    vTaskDelay(5);
  #endif 

}




//////////////////////////////////////////////
// Mudar Modo de Operação (OTA, OPRA, OPRS) //
//////////////////////////////////////////////
void TrocarModoOperacao(const char* cz_modo){
  data2EEPROM data2eeprom = {0};
  EEPROM.get(EEPROM_START_ADDR, data2eeprom);

  // Guarda o modo actual antes de passar para o modo OTA
  if( strcmp(cz_modo, "OTA") == 0){
    strncpy(data2eeprom.BackupModo, data2eeprom.Modo, sizeof(data2eeprom.BackupModo));
    data2eeprom.BackupModo[strlen(data2eeprom.BackupModo)] = '\0';   
  }
  else{
    strncpy(data2eeprom.BackupModo, cz_modo, sizeof(data2eeprom.BackupModo));
    data2eeprom.BackupModo[strlen(data2eeprom.BackupModo)] = '\0';   
  }
  //TelnetStream.println(data2eeprom.BackupModo);
  //strncpy(device_data.save_Modo, device_data.Modo, sizeof(device_data.Modo));
  strncpy(data2eeprom.Modo, cz_modo, sizeof(data2eeprom.Modo));
  data2eeprom.Modo[strlen(data2eeprom.Modo)] = '\0';     
  EEPROM.put(EEPROM_START_ADDR, data2eeprom);
  EEPROM.commit();
   #if defined(ESP8266) 
    delay(100);
  #elif defined(ESP32) 
    vTaskDelay(10);
  #endif
 
  ESP.restart();
}




//////////////////////////////////////////
// Tira da EEPROM, dados do Dispositivo //
//////////////////////////////////////////
const char* ObterDadosDispositivo(){

  data2EEPROM data2eeprom = {0};
 
  char aux[26];   
  
  EEPROM.get(EEPROM_START_ADDR, data2eeprom);

  strncpy(json_array, "", JSON_DOC_SIZE);
  //json_array[JSON_DOC_SIZE - 1] = '\0';
  json_doc["Modo"] = data2eeprom.Modo;
  json_doc["SSID"] = data2eeprom.SSID; 
  json_doc["PWD"] = data2eeprom.PWD;

  IPAddress ip(data2eeprom.ip);
  sprintf(aux, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
   aux[strlen(aux)] = '\0';
  json_doc["IP"] = aux;

  IPAddress gw(data2eeprom.gateway);
  sprintf(aux, "%d.%d.%d.%d", gw[0], gw[1], gw[2], gw[3]);
   aux[strlen(aux)] = '\0';
  json_doc["GW"] = aux;

  IPAddress msk(data2eeprom.subnet);
  sprintf(aux, "%d.%d.%d.%d", msk[0], msk[1], msk[2], msk[3]);
   aux[strlen(aux)] = '\0';
  json_doc["MSK"] = aux;

  IPAddress dns1(data2eeprom.dns1);
  sprintf(aux, "%d.%d.%d.%d", dns1[0], dns1[1], dns1[2], dns1[3]);
  aux[strlen(aux)] = '\0';
  json_doc["DNS1"] = aux;

  IPAddress dns2(data2eeprom.dns2);
  sprintf(aux, "%d.%d.%d.%d", dns2[0], dns2[1], dns2[2], dns2[3]);
  aux[strlen(aux)] = '\0';
  json_doc["DNS2"] = aux;

  //json_doc["SCKADDR"] = data2eeprom.SOCKSERVERADDR;
  //json_doc["SCKPRT"] = data2eeprom.SOCKSERVERPORT;
  
  serializeJson(json_doc, json_array);  
 
  json_doc.clear();      
  return json_array;
}



//////////////////////////////////////////
// Tira da EEPROM, dados do Dispositivo //
//////////////////////////////////////////
void cb_scan_wifi(uint8_t networksFound){

    char cz_key[4];
  
    for (uint8_t i = 0; i < networksFound; ++i){
      sprintf(cz_key, "%d", i);
      cz_key[strlen(cz_key)] = '\0';
      JsonObject rede = json_doc.createNestedObject(cz_key);
      rede["SSID"] = WiFi.SSID(i);
    }

    serializeJson(json_doc, json_array);    
    json_doc.clear();      
    requisicao->send(200, "text/json", json_array); 

}


//////////////////////////////////////////////
// Callback, ESP-NOW, when data is received //
//////////////////////////////////////////////
#if defined(ESP8266) 
  void callback_rx_esp_now(uint8_t * mac, uint8_t *incomingData, uint8_t len) { // Para ESP8266
  
  char czjson[JSON_DOC_SIZE];
   
  deserializeJson(json_doc, (const char*)incomingData);
   
  if (json_doc.isNull()){
    imprimeln(F("JSON inválido/nulo!"));
    return;
  } 
  
  const char* cz_tipo_msg = json_doc["MSG"];
  const char* cz_cmd = json_doc["CMD"];
  const char* cz_mac = json_doc["MAC"];  
  uint32_t iz_id_dev = json_doc["ID"];
  uint8_t iz_pin_state = json_doc["PST"];
  uint32_t iz_canal_wifi = json_doc["WCH"];
  id_cliente_ws = iz_canal_wifi;


  uint8_t* uz_mac = converteMacString2Byte(cz_mac);

  bool exists = esp_now_is_peer_exist(uz_mac);

  if( strcmp(cz_tipo_msg, "PRT") == 0){

    if(iz_id_dev == 0) return; // Abortar se Id for 0   

    int8_t index = ArrayHasElement(array_alive[0].BOARDS_ID, iz_id_dev);    

    if (index != -1 && index != -2){      
      array_alive[0].BOARDS_ID[index] = iz_id_dev;   
    }

    // Se o dev não está registado
    if (!exists && (iz_canal_wifi <= MAX_WIFI_CH && iz_id_dev != BOARD_ID) ) {       
      
      // Adiciona par
      esp_now_add_peer(uz_mac, ESP_NOW_ROLE_SLAVE, WiFi.channel(), NULL, 0);
      strcpy(czjson, CriarJSON("PNG", "", cz_MACADDR, BOARD_ID, 2, WiFi.channel()));
     
      esp_now_send(uz_mac, (uint8_t *)czjson, strlen(czjson)); 

      // Envia, aos clientes WS javascript, os ID dos clientes registados
      if (index != -2) SendData2WebSocket(0, 2, "REG", "", "", 0); 
      
    }
    else if (exists && (iz_canal_wifi <= MAX_WIFI_CH && iz_id_dev != BOARD_ID) ){            
      // Envia resposta      
      strcpy(czjson, CriarJSON("PNG", "", cz_MACADDR, BOARD_ID, 2, WiFi.channel()));
      
      esp_now_send(uz_mac, (uint8_t *) czjson, strlen(czjson)); 

      // Envia, aos clientes WS javascript, os ID dos clientes registados
      if (index != -2) SendData2WebSocket(0, 2, "REG", "", "", 0);    

    }

  }
  else if( strcmp(cz_cmd, "PRZ") == 0){   
   
    if(iz_id_dev == 0) return; // Abortar se Id for 0   

    int8_t index = ArrayHasElement(array_alive[0].BOARDS_ID, iz_id_dev);    

    if (index != -1 && index != -2){      
      array_alive[0].BOARDS_ID[index] = iz_id_dev;         
    }

    // Se o dev não está registado
    if (!exists && (iz_canal_wifi <= MAX_WIFI_CH && iz_id_dev != BOARD_ID) ) {      

      // Envia, aos clientes WS javascript, os ID dos clientes registados
      if (index != -2) SendData2WebSocket(0, 2, "REG", "", "", 0); 
      
    }
    else if (exists && (iz_canal_wifi <= MAX_WIFI_CH && iz_id_dev != BOARD_ID) ){            
      // Envia, aos clientes WS javascript, os ID dos clientes registados
      if (index != -2) SendData2WebSocket(0, 2, "REG", "", "", 0);  
    }

  }
  else if( strcmp(cz_tipo_msg, "PAL") == 0){

    if (!exists && (iz_canal_wifi <= MAX_WIFI_CH && iz_id_dev != BOARD_ID) ) {
 
      // Adiciona par    
      esp_now_add_peer( uz_mac, ESP_NOW_ROLE_SLAVE, WiFi.channel(), NULL, 0);
      strcpy(czjson, CriarJSON("PRP", "", cz_MACADDR, BOARD_ID, 2, WiFi.channel()));
      
      esp_now_send(uz_mac, (uint8_t *) czjson, strlen(czjson));      
    }
    else{
      // Envia resposta
      strcpy(czjson, CriarJSON("ALA", "", cz_MACADDR, BOARD_ID, 2, WiFi.channel()));
     
      esp_now_send(uz_mac, (uint8_t *)czjson, strlen(czjson));       
    }
  }
  else if( strcmp(cz_cmd, "TGA") == 0){   
    //TelnetStream.println((const char*)incomingData);
    SendData2WebSocket(iz_id_dev, iz_pin_state, cz_tipo_msg, cz_mac, "", 0);

    // Actualiza o historico do device na struct
    for (uint8_t cont = 0; cont < QTD_BOARDS; cont++){
      if (array_alive[cont].board_id == iz_id_dev){
        strcpy(array_alive[cont].board_hist, cz_tipo_msg);
        array_alive[cont].board_hist[strlen(array_alive[cont].board_hist)] = '\0';
      }
    }
  }  
  else if( strcmp(cz_cmd, "TGG") == 0){     
    mudarEstadoPINPassivo(iz_pin_state, iz_id_dev);     
  }  
  else if( strcmp(cz_cmd, "GDT") == 0){
    // Prepara JSON 
    json_doc.clear();     
    json_doc["MSG"] = "";
    json_doc["CMD"] = "SDT";
    json_doc["MAC"] = cz_MACADDR;
    json_doc["ID"] = BOARD_ID;
    json_doc["PST"] = 2;
    json_doc["WCH"] = 0;
    json_doc["DT"] = now();
    serializeJson(json_doc, czjson);
      
    // Envia resposta
    esp_now_send(uz_mac, (uint8_t *)czjson, strlen(czjson)); 
   
    json_doc.clear();
  }
  else if( strcmp(cz_cmd, "TCA") == 0){    
    // Difunde mensagem aos Clinetes WS da LAN.      
    ws_obj.textAll((const char*)incomingData);
    wss_obj.textAll((const char*)incomingData);
  }
  else if( strcmp(cz_cmd, "RAN") == 0){   
    SendData2WebSocket(iz_id_dev, iz_pin_state, cz_tipo_msg, cz_mac, "", 0);     
  }
  else if( strcmp(cz_cmd, "SAC") == 0){// SET ALIVE ClIENT   
       
    int8_t index = ArrayHasElement(array_alive[0].BOARDS_ID, iz_id_dev);

    if(index != -1 && index != -2 && index < QTD_BOARDS && iz_id_dev != 0){
      array_alive[0].BOARDS_ID[index] = iz_id_dev;  
      array_alive[index].board_id =  iz_id_dev;
    } 
    else{
      if(index >= QTD_BOARDS){ // TODO: Analisar/Rever logica
        //ResetSPIFFS();
        // Preenche array com 255 e coloca 0 ID do Servidor na primeira posição
        for (uint8_t index = 0; index < QTD_BOARDS; index++){
          array_alive[0].BOARDS_ID[index] = 255;
          array_alive[index].board_id =  255;
          array_alive[index].temp_alive =  0;
        }
        
        array_alive[0].BOARDS_ID[0] = CriarDEVICE_ID();// Servidor
        array_alive[0].board_id = CriarDEVICE_ID();
        array_alive[0].temp_alive = 0;
      }
    }

    strcpy(aux_dt, cz_tipo_msg);
    aux_dt[strlen(aux_dt)] = '\0';
    SendData2WebSocket(iz_id_dev, iz_pin_state, "SAC", cz_mac, cz_tipo_msg, iz_canal_wifi); 

    // Guardar tempos Alive dos clientes   
    for(uint8_t cont = 0; cont < QTD_BOARDS; cont++){
      if(array_alive[cont].board_id == iz_id_dev && array_alive[cont].board_id != 255){
        array_alive[cont].temp_alive = now();
      }
    } 
  }
  else if (strcmp(cz_cmd, "NRG") == 0 && strcmp(cz_tipo_msg, "SNS") == 0){
    // Repassa aos Browsers a informacao sobre a energia
    SendData2WebSocket(iz_id_dev, iz_pin_state, "SNS", cz_mac, "", 0); 
  }
  else if( strcmp(cz_cmd, "SAP") == 0){// SET ALIVE PASSIVE ClIENT  
    //TelnetStream.println((const char*)incomingData);     
    mudarEstadoPINPassivo(iz_pin_state, iz_id_dev); 
  }
  else if (strcmp(cz_cmd, "KNP") == 0){
    SendData2WebSocket(iz_id_dev, iz_pin_state, "KNP", cz_tipo_msg, "", 0); 
  }
  else if (strcmp(cz_cmd, "KAP") == 0){
    SendData2WebSocket(iz_id_dev, iz_pin_state, "KAP", cz_tipo_msg, "", 0); 
  }
  else if (strcmp(cz_cmd, "KPG") == 0){
    SendData2WebSocket(iz_id_dev, iz_pin_state, "KPG", cz_tipo_msg, "", 0); 
  }
  else if( strcmp(cz_cmd, "GRA") == 0){   
    SendData2WebSocket(iz_id_dev, iz_pin_state, cz_cmd, cz_tipo_msg, "", 0);     
  }  
  else if( strcmp(cz_cmd, "UKA") == 0){   
    SendData2WebSocket(iz_id_dev, iz_pin_state, cz_cmd, cz_tipo_msg, "", 0);     
  }  
  else if( strcmp(cz_cmd, "USD") == 0){   
    SendData2WebSocket(iz_id_dev, iz_pin_state, cz_cmd, cz_tipo_msg, "", 0);     
  }  
  else if( strcmp(cz_cmd, "SWS") == 0){   
    const char* cz_ip = json_doc["MAC"];     
    uint8_t iz_wifi_state = json_doc["PST"];  
    char jsondata[48];
    
    sprintf(jsondata,"{\"WifiState\":%d,\"IPAddress\":\"%s\"}", iz_wifi_state, cz_ip);
    SendData2WebSocket(0, 0, "SWS", jsondata, "", 0);    
  }  

  free(uz_mac);
  json_doc.clear();
  
}// end callback_rx_esp_now(...)
#elif defined(ESP32) 
  void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len){ 
  
  Serial.println((const char*)incomingData);
  
  char czjson[JSON_DOC_SIZE];
   
  deserializeJson(json_doc, (const char*)incomingData);
    
  if (json_doc.isNull()){
    imprimeln(F("JSON inválido/nulo!"));
    return;
  } 
   
  const char* cz_tipo_msg = json_doc["MSG"];
  const char* cz_cmd = json_doc["CMD"];
  const char* cz_mac = json_doc["MAC"];  
  uint32_t iz_id_dev = json_doc["ID"];
  uint8_t iz_pin_state = json_doc["PST"];
  uint32_t iz_canal_wifi = json_doc["WCH"];
  id_cliente_ws = iz_canal_wifi;

  uint8_t* uz_mac = converteMacString2Byte(cz_mac);

  bool exists = esp_now_is_peer_exist(uz_mac);

  if( strcmp(cz_tipo_msg, "PRT") == 0){       
    // Guarda BOARD_ID no array/SPIFFS (caso não esteja)
    //LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);  
    if(iz_id_dev == 0) return; // Abortar se Id for 0   
    
    int8_t index = ArrayHasElement(array_alive[0].BOARDS_ID, iz_id_dev);    

    if (index != -1 && index != -2){      
      array_alive[0].BOARDS_ID[index] = iz_id_dev;   
      //EscreverSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);      
    }

    // Se o dev não está registado
    if (!exists && (iz_canal_wifi <= MAX_WIFI_CH && iz_id_dev != BOARD_ID) && iz_canal_wifi == WIFI_CH) {   
       
      memcpy(peerInfo.peer_addr, uz_mac, 6);
      peerInfo.channel = WIFI_CH;
      peerInfo.encrypt = false;

      if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        imprimeln(F("Falha ao adicionar par (1)"));
      }
      else{
        guardar_endereco_mac(uz_mac, iz_id_dev);
      }
      
      strcpy(czjson, CriarJSON("PNG", "", cz_MACADDR, BOARD_ID, 2, WiFi.channel()));
       
      esp_now_send(uz_mac, (uint8_t *)czjson, strlen(czjson)); 
     

      // Envia, aos clientes WS javascript, os ID dos clientes registados
      if (index != -2) SendData2WebSocket(0, 2, "REG", "", "", 0); 
      
    }
    else if (exists && (iz_canal_wifi <= MAX_WIFI_CH && iz_id_dev != BOARD_ID) ){            
      // Envia resposta      
      strcpy(czjson, CriarJSON("PNG", "", cz_MACADDR, BOARD_ID, 2, WiFi.channel()));
      
      esp_now_send(uz_mac, (uint8_t *) czjson, strlen(czjson)); 

      // Envia, aos clientes WS javascript, os ID dos clientes registados
      if (index != -2) SendData2WebSocket(0, 2, "REG", "", "", 0);    

    }

  }
  else if( strcmp(cz_cmd, "PRZ") == 0){

    if(iz_id_dev == 0) return; // Abortar se Id for 0   

    int8_t index = ArrayHasElement(array_alive[0].BOARDS_ID, iz_id_dev);    

    if (index != -1 && index != -2){      
      array_alive[0].BOARDS_ID[index] = iz_id_dev;         
    }

    // Se o dev não está registado
    if (!exists && (iz_canal_wifi <= MAX_WIFI_CH && iz_id_dev != BOARD_ID) ) {      

      // Envia, aos clientes WS javascript, os ID dos clientes registados
      if (index != -2) SendData2WebSocket(0, 2, "REG", "", "", 0); 
      
    }
    else if (exists && (iz_canal_wifi <= MAX_WIFI_CH && iz_id_dev != BOARD_ID) ){            
      // Envia, aos clientes WS javascript, os ID dos clientes registados
      if (index != -2) SendData2WebSocket(0, 2, "REG", "", "", 0);  
    }

  }
  else if( strcmp(cz_tipo_msg, "PAL") == 0){

    if (!exists && (iz_canal_wifi <= MAX_WIFI_CH && iz_id_dev != BOARD_ID) && iz_canal_wifi == WiFi.channel()) {     
      memcpy(peerInfo.peer_addr, uz_mac, 6);
      peerInfo.channel = WIFI_CH;
      peerInfo.encrypt = false;

      if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        imprimeln(F("Falha ao adicionar par (2)"));
      }
      else{
        guardar_endereco_mac(uz_mac, iz_id_dev);
      }

      strcpy(czjson, CriarJSON("PRP", "", cz_MACADDR, BOARD_ID, 2, WiFi.channel()));

      esp_now_send(uz_mac, (uint8_t *) czjson, strlen(czjson)); 
     
    }
    else{
      // Envia resposta
      strcpy(czjson, CriarJSON("ALA", "", cz_MACADDR, BOARD_ID, 2, WiFi.channel()));
      
      esp_now_send(uz_mac, (uint8_t *)czjson, strlen(czjson)); 
    }
  }
  else if( strcmp(cz_cmd, "TGA") == 0){   
    //TelnetStream.println((const char*)incomingData);
    SendData2WebSocket(iz_id_dev, iz_pin_state, cz_tipo_msg, cz_mac, "", 0);

    // Actualiza o historico do device na struct
    for (uint8_t cont = 0; cont < QTD_BOARDS; cont++){
      if (array_alive[cont].board_id == iz_id_dev){
        strcpy(array_alive[cont].board_hist, cz_tipo_msg);
        array_alive[cont].board_hist[strlen(array_alive[cont].board_hist)] = '\0';
      }
    }
  }  
  else if( strcmp(cz_cmd, "TGG") == 0){     
    mudarEstadoPINPassivo(iz_pin_state, iz_id_dev);     
  }  
  else if( strcmp(cz_cmd, "GDT") == 0){

    // Prepara JSON 
    json_doc.clear();     
    json_doc["MSG"] = "";
    json_doc["CMD"] = "SDT";
    json_doc["MAC"] = cz_MACADDR;
    json_doc["ID"] = BOARD_ID;
    json_doc["PST"] = 2;
    json_doc["WCH"] = 0;
    json_doc["DT"] = now();
    serializeJson(json_doc, czjson);
      
    // Envia resposta
    esp_now_send(uz_mac, (uint8_t *)czjson, strlen(czjson)); 
   
    json_doc.clear();
  }
  else if( strcmp(cz_cmd, "TCA") == 0){
    
    // Difunde mensagem aos Clinetes WS da LAN.
    ws_obj.textAll((const char*)incomingData);
    wss_obj.textAll((const char*)incomingData);    
  }
  else if( strcmp(cz_cmd, "RAN") == 0){   
    SendData2WebSocket(iz_id_dev, iz_pin_state, cz_tipo_msg, cz_mac, "", 0);     
  }
  else if( strcmp(cz_cmd, "SAC") == 0){// SET ALIVE ClIENT   
    // Guarda BOARD_ID no array/SPIFFS (caso não esteja)
    //LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);    
    
    int8_t index = ArrayHasElement(array_alive[0].BOARDS_ID, iz_id_dev);

    if(index != -1 && index != -2 && index < QTD_BOARDS && iz_id_dev != 0){
      array_alive[0].BOARDS_ID[index] = iz_id_dev;  
      array_alive[index].board_id =  iz_id_dev;
      //EscreverSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);  
    } 
    else{
      if(index >= QTD_BOARDS){ // TODO: Analisar/Rever logica
        //ResetSPIFFS();
        // Preenche array com 255 e coloca 0 ID do Servidor na primeira posição
        for (uint8_t index = 0; index < QTD_BOARDS; index++){
          array_alive[0].BOARDS_ID[index] = 255;
          array_alive[index].board_id =  255;
          array_alive[index].temp_alive =  0;
        }
        
        array_alive[0].BOARDS_ID[0] = CriarDEVICE_ID();
        array_alive[0].board_id = CriarDEVICE_ID();
        array_alive[0].temp_alive = 0;
      }
    }

    strcpy(aux_dt, cz_tipo_msg);
    aux_dt[strlen(aux_dt)] = '\0';
    SendData2WebSocket(iz_id_dev, iz_pin_state, "SAC", cz_mac, cz_tipo_msg, iz_canal_wifi); 

    // Guardar tempos Alive dos clientes   
    for(uint8_t cont = 0; cont < QTD_BOARDS; cont++){
      if(array_alive[cont].board_id == iz_id_dev && array_alive[cont].board_id != 255){
        array_alive[cont].temp_alive = now();
      }
    } 
  }
  else if (strcmp(cz_cmd, "NRG") == 0 && strcmp(cz_tipo_msg, "SNS") == 0){
    // Repassa aos Browsers a informacao sobre a energia
    SendData2WebSocket(iz_id_dev, iz_pin_state, "SNS", cz_mac, "", 0); 
  }
  else if( strcmp(cz_cmd, "SAP") == 0){// SET ALIVE PASSIVE ClIENT  
    //TelnetStream.println((const char*)incomingData);     
    mudarEstadoPINPassivo(iz_pin_state, iz_id_dev); 
  }
  else if (strcmp(cz_cmd, "KNP") == 0){
    SendData2WebSocket(iz_id_dev, iz_pin_state, "KNP", cz_tipo_msg, "", 0); 
  }
  else if (strcmp(cz_cmd, "KAP") == 0){
    SendData2WebSocket(iz_id_dev, iz_pin_state, "KAP", cz_tipo_msg, "", 0); 
  }
  else if (strcmp(cz_cmd, "KPG") == 0){
    SendData2WebSocket(iz_id_dev, iz_pin_state, "KPG", cz_tipo_msg, "", 0); 
  }
  else if( strcmp(cz_cmd, "GRA") == 0){   
    SendData2WebSocket(iz_id_dev, iz_pin_state, cz_cmd, cz_tipo_msg, "", 0);     
  }  
  else if( strcmp(cz_cmd, "UKA") == 0){   
    SendData2WebSocket(iz_id_dev, iz_pin_state, cz_cmd, cz_tipo_msg, "", 0);     
  }  
  else if( strcmp(cz_cmd, "USD") == 0){   
    SendData2WebSocket(iz_id_dev, iz_pin_state, cz_cmd, cz_tipo_msg, "", 0);     
  }
  else if( strcmp(cz_cmd, "SWS") == 0){   
    const char* cz_ip = json_doc["MAC"];     
    uint8_t iz_wifi_state = json_doc["PST"];  
    char jsondata[48];
    
    sprintf(jsondata,"{\"WifiState\":%d,\"IPAddress\":\"%s\"}", iz_wifi_state, cz_ip);
    SendData2WebSocket(0, 0, "SWS", jsondata, "", 0);    
  }    

  free(uz_mac);
  json_doc.clear();
  
}// end callback_rx_esp_now(...)
#endif



//////////////////////////////////////////
// Callback, ESP-NOW, when data is sent //
//////////////////////////////////////////
#if defined(ESP8266) 
  void callback_tx_esp_now(uint8_t *mac_addr, uint8_t status) { 
    /* char macStr[18];
    //imprime("Destino: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.println(macStr);
    imprime(" Status:\t");
   imprime(status == ESP_NOW_SEND_SUCCESS ? "Successo." : "Falha: ");*/
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

 



/////////////////////
//Criar dados JSON //
/////////////////////
const char* CriarJSON(const char* MSG_TYPE, const char* CMD, const char* MAC, uint32_t ID, int8_t PIN_STT, uint32_t WIFI_CH){
  
  json_doc.clear();
  if( strcmp(CMD, "REG") == 0 ){
    // Envia lista dos ID dos devices 
    //LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS); 
    JsonObject root = json_doc.to<JsonObject>();
    JsonArray arr = root.createNestedArray("MSG_TYPE");

    for(uint8_t cont = 0; cont < QTD_BOARDS; cont++){
      if(array_alive[0].BOARDS_ID[cont] != 255){
      //if(device_data.BOARDS_ID[cont] != 255 && data2spiffs.BOARDS_ID[cont] >= 0){
        arr.add(array_alive[0].BOARDS_ID[cont]);        
      }     
    } 

    //LittleFS.end();
  }
  else{
    // Envia estado dos devices       
    json_doc["MSG"] = MSG_TYPE;
  }
  
  
  json_doc["CMD"] = CMD;
  json_doc["MAC"] = MAC;
  json_doc["ID"] = ID;
  json_doc["PST"] = PIN_STT;
  json_doc["WCH"] = WIFI_CH;

  serializeJson(json_doc, json_array);
  return json_array;
}


///////////////////////////////////////////////////
// Enviar dados ao Servidor e clientes WebSocket //
///////////////////////////////////////////////////
void SendData2WebSocket(uint32_t board_id, uint8_t pin_state, const char* DT, const char* nome, const char* dateTime, uint8_t action){
 
  char czjson[JSON_DOC_SIZE];
  boolean to_broad_cast = false;

  if( strcmp(DT, "REG") == 0 ){ // REG -- REGIST DEVICES
    // Envia lista dos ID dos devices   
    #if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1
      
      Serial.write("ZbStatus", 8);      
      Serial.write('\n'); 

      delay(100);      

      for (uint8_t cont = 0; cont < QTD_DEVS_ZIGBEE; cont++){
        String str = String(arr_zb_devs[cont].short_id);        
        if ( str.indexOf("0x") != -1 ){ 
          char data[17];
          sprintf(data, "ZbStatus3 %s", arr_zb_devs[cont].short_id);  
          Serial.write(data, strlen(data)); 
          Serial.write('\n');
          delay(50);
        } 
      } 

    #endif    
    
    
    strcpy(czjson, CriarJSON("", DT, "", 0, 0, 0));
   
    to_broad_cast = true;        
  }
  else if( strcmp(DT, "ECHO") == 0 ){ // ECHO -- SEND BACK TO WEB CLIENT
    // Envia lista dos ID dos devices   
    strcpy(czjson, CriarJSON(nome, DT, "", 0, 0, 0));
    
    to_broad_cast = true;        
  }
  else if( strcmp(DT, "SNS") == 0 ){ // SNS/NRG -- ESTADO ENERGIA
    // Envia Parametros da Energia   
    strcpy(czjson, CriarJSON("SNS", "NRG", nome, board_id, pin_state, 0));
    
    to_broad_cast = true;  
    //Serial.println(czjson);      
  }
  else if( strcmp(DT, "SWS") == 0 ){ // SWS -- ESTADO WIFI TASMOTA
    // Envia Parametros da Energia   
    strcpy(czjson, CriarJSON(nome, "SWS", "", 0, 0, 0));
    
    to_broad_cast = true;  
    //Serial.println(czjson);      
  }
  else if( strcmp(DT, "SAS") == 0 ){ // SAS -- SET ALIVE SERVER 
    // Reporta o estado do servidor (Alive)   
    uint32_t alive = now(); 
    data2SPIFFS data2spiffs = {0};
    LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);  

    if((array_alive[0].BOARDS_ID[0]) == 0 && board_id == BOARD_ID){
      array_alive[0].BOARDS_ID[0] = BOARD_ID;
    }

    EscreverSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);

    LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);  

    strcpy(czjson, CriarJSON(data2spiffs.DT, DT, nome, board_id, pin_state, alive));
    //strcpy(czjson, CriarJSON(historico_servidor, DT, nome, board_id, pin_state, alive));
      
    // Guarda tempo alive do servidor
    array_alive[0].board_id =  board_id;
    array_alive[0].temp_alive =  alive;
    to_broad_cast = true;
    //LittleFS.end();
  } 
  else if( strcmp(DT, "SAC") == 0 && board_id != PASSIVE_SLAVE_ID){   
    // Reporta o estado dos clientes (Alive)   
    to_broad_cast = true;
    //uint32_t alive = now();   

    // Processar horas do Timer
    char cz_dt[DT_STR_SIZE];
    ObterDTActual(cz_dt);
    cz_dt[strlen(cz_dt)] = '\0';  

    uint8_t hora_menor = 0;
    uint8_t minuto_menor = 0; 
    uint8_t guarda_indice = 0; 
    uint8_t accao_timer = 0;
    
    // Obter marca de devices zigbee, de tras para frente
    uint8_t quarto = obterDigitoInvertido(board_id, 1);
    uint8_t quinto = obterDigitoInvertido(board_id, 2); 

    if(quarto == 0 && quinto == 0){ // Disp[ositivos Zigbee
      
      // Obter os dados do Timer correspondente
      LerZbSPIFFS(timersZB, FILE_ZBTMRS_SPIFFS);    
      char short_id[ZB_SHORT_ID];
                  
      // Obtem ID do dispositivos, naestructura "ZigbeeDevices"
      for (uint8_t index = 0; index < QTD_DEVS_ZIGBEE; index++){
        if(arr_zb_devs[index].zb_board_id == board_id){
          strcpy(short_id, arr_zb_devs[index].short_id);          
          break;
        }
      }

      // Obtem a posicao do elemento no array das estructuras "zbtimers2SPIFFS"
      for (uint8_t indice = 0; indice < QTD_DEVS_ZIGBEE; indice++){
        if( timersZB[indice].weekDays[weekday() - 1] == '1' &&  strcmp(timersZB[indice].short_id, short_id) == 0 && strcmp(timersZB[indice].short_id, "") != 0 && timersZB[indice].activar == 1){
          String horas = String(timersZB[indice].hora_minutos[0]) + String(timersZB[indice].hora_minutos[1]);
          String minutos = String(timersZB[indice].hora_minutos[3]) + String(timersZB[indice].hora_minutos[4]);
          if( ((horas.toInt()*60) + minutos.toInt()) > ((hour()*60) + minute()) ){
            hora_menor = horas.toInt();
            minuto_menor = minutos.toInt();
            guarda_indice = indice;  
            break;  
          }                    
        }       
      } 

      // Procurar hora menor
      for (uint8_t i = (guarda_indice + 1); i < QTD_DEVS_ZIGBEE; i++) { // Percorre o array a partir do segundo elemento
        if( timersZB[i].weekDays[weekday() - 1] == '1' &&  strcmp(timersZB[i].short_id, short_id) == 0 && strcmp(timersZB[i].short_id, "") != 0 && timersZB[i].activar == 1){
          String horas = String(timersZB[i].hora_minutos[0]) + String(timersZB[i].hora_minutos[1]);
          String minutos = String(timersZB[i].hora_minutos[3]) + String(timersZB[i].hora_minutos[4]);
          if ( ((horas.toInt()*60) + minutos.toInt()) < ((hora_menor)*60) + minuto_menor) { 
            hora_menor = horas.toInt();
            minuto_menor = minutos.toInt();
            guarda_indice = i;            
          }                
        }         
      }

      if(((hora_menor*60) + minuto_menor) > ((hour()*60) + minute())){
        sprintf(cz_dt, "%d:%02d:%02d, %d/%d/%d", hora_menor, minuto_menor, 0, day(), month(), year()); 
        accao_timer = timersZB[guarda_indice].action;
      }  
      else{
        sprintf(cz_dt, "%s:%s:%s, %d/%d/%d", "__", "__", "__", day(), month(), year()); 
        accao_timer = 2;
      }
    }  
    else{ // Disp[ositivos WiFi/ESP-NOW      
      strcpy(cz_dt, dateTime);
      accao_timer = action;
    }   

    // accao_timer -- accao, serve para alterar a cor dos icons do historico
    strcpy(czjson, CriarJSON(cz_dt, DT, nome, board_id, pin_state, accao_timer)); 
    //strcpy(czjson, CriarJSON(cz_dt, DT, nome, board_id, pin_state, alive));   
      
  }  
  else if( strcmp(DT, "UDT") == 0){
    to_broad_cast = true;
    strcpy(czjson, CriarJSON("", "UDT", "", board_id, 2, 0));
    
  }
  else if (strcmp(DT, "OFF") == 0){  
    to_broad_cast = true;

    char historico[DT_STR_SIZE];
    
    for(uint8_t cont = 0; cont < QTD_BOARDS; cont++){
      if(array_alive[cont].board_id == board_id){
        strcpy(historico, array_alive[cont].board_hist); 
        historico[strlen(historico)] = '\0';
      }
    }

    
    #if ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 1 
      // Obter marca de devices zigbee, de tras para frente
      uint8_t quarto = obterDigitoInvertido(board_id, 1);
      uint8_t quinto = obterDigitoInvertido(board_id, 2); 

      if(quarto == 0 && quinto == 0){ // Disp[ositivos Zigbee
        // Processar horas do Timer
        char cz_dt[DT_STR_SIZE];
        ObterDTActual(cz_dt);
        cz_dt[strlen(cz_dt)] = '\0';  

        strcpy(historico, cz_dt);
      }
    #endif

    strcpy(czjson, CriarJSON(historico, "OFF", nome, board_id, 2, 0));
    
  
  }
  else if (strcmp(DT, "GRA") == 0){  
    to_broad_cast = true;
    strcpy(czjson, CriarJSON(nome, "GRA", "", board_id, 2, 0));
   
  }
  else if (strcmp(DT, "USD") == 0){  
    to_broad_cast = false;
    strcpy(czjson, CriarJSON(nome, "USD", "", board_id, 2, 0));
   
  }
  else if (strcmp(DT, "UKA") == 0){  
    to_broad_cast = false;
    strcpy(czjson, CriarJSON(nome, "UKA", "", board_id, 2, 0));
  }
  else if (strcmp(DT, "KNP") == 0){  
    to_broad_cast = false;
    strcpy(czjson, CriarJSON(nome, "KNP", "", board_id, 2, 0));
   
  }
  else if (strcmp(DT, "KAP") == 0){  
    to_broad_cast = false;
    strcpy(czjson, CriarJSON(nome, "KAP", "", board_id, 2, 0));
    
  }
  else if (strcmp(DT, "KPG") == 0){  
    to_broad_cast = false;
    strcpy(czjson, CriarJSON(nome, "KPG", "", board_id, 2, 0));
  
  }
  else{
    // Envia estado dos devices    
    uint32_t alive = now();    
    to_broad_cast = true;    
    if(board_id == PASSIVE_SLAVE_ID) return;
    strcpy(czjson, CriarJSON(DT, "STT", nome, board_id, pin_state, alive));
  }
  
 
  // Envia mensagem aos Clinetes WS da LAN.
  //ws_obj.textAll(czjson);
  if(to_broad_cast == true){
    ws_obj.textAll(czjson);
    wss_obj.textAll(czjson);    
  }
  else{
    ws_obj.text(id_cliente_ws, czjson);
    wss_obj.text(id_cliente_ws, czjson);
  }

}




//////////////////////////////////////////////////////////////////////////
// troca o estado de PIN e guarda dados (estado, data e hora) na EEPROM //
//////////////////////////////////////////////////////////////////////////
void TogglePIN(){
   
    data2EEPROM data2eeprom = {0};
    data2SPIFFS data2spiffs = {0};
    EEPROM.get(EEPROM_START_ADDR, data2eeprom);   
    
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
    
    char cz_dt[DT_STR_SIZE];
    ObterDTActual(cz_dt);
    cz_dt[strlen(cz_dt)] = '\0';  

    LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);
    strncpy(data2spiffs.DT, cz_dt, sizeof(cz_dt));   
    data2spiffs.DT[strlen(data2spiffs.DT)] = '\0';  
    
    #if POWER320D == 1 
      data2spiffs.estado_pin = relayState;
    #else 
      data2spiffs.estado_pin = digitalRead(PIN_RELAY);
    #endif

    EscreverSPIFFS(&data2spiffs, FILE_DATA_SPIFFS); 
    
    #if POWER320D == 1
      SendData2WebSocket(BOARD_ID, relayState, cz_dt, data2eeprom.nome, "", 0);
    #else 
      SendData2WebSocket(BOARD_ID, digitalRead(PIN_RELAY), cz_dt, data2eeprom.nome, "", 0);
    #endif

    // Actualiza o historico do servidor
    for (uint8_t cont = 0; cont < QTD_BOARDS; cont++){
      if (array_alive[cont].board_id == BOARD_ID){
        strcpy(array_alive[cont].board_hist, cz_dt);   
        array_alive[cont].board_hist[strlen(array_alive[cont].board_hist)] = '\0';     
      }
    }


    #if IS_PASSIVE_MASTER == 1
      if(data2eeprom.TIPO != PASSIVO){
        #if POWER320D == 1 
          mudarEstadoPINPassivo(relayState, PASSIVE_SLAVE_ID);
        #else
          mudarEstadoPINPassivo(digitalRead(PIN_RELAY), PASSIVE_SLAVE_ID);
        #endif 
      }                  
    #endif  
}


////////////////////////////
// Configuracão de timers //
////////////////////////////
void ConfigurarTimer(const char* cz_json, const char* nome_arquivo) {
  
  deserializeJson(json_doc, cz_json); 

  // Tira Timers da EEPROM
  timers2EEPROM timers[QTD_TMRS_ESPNOW];
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
  #if defined(ESP8266) 
    delay(10);
  #elif defined(ESP32) 
    vTaskDelay(5);
  #endif
  json_doc.clear();
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
  timers2EEPROM tmrs[QTD_TMRS_ESPNOW];
  EEPROM.get(EEPROM_TIMERS_ADDR, tmrs);
 

  for (uint8_t index = 0; index < QTD_TMRS_ESPNOW; index++) {

    // Verficar se o dia de semana, do momento, está programado, timer activado
    if ( tmrs[index].weekDays[weekday() - 1] == '1' && tmrs[index].activar == 1 && year() > YEAR_COMPARE) {
      if (hour() == tmrs[index].horas && minute() == tmrs[index].minutos && second() <= 5) {
        #if POWER320D == 1   
          OnOffBISTABLE(tmrs[index].action, BISTABLE_ON, BISTABLE_OFF);
        #else
          digitalWrite(PIN_RELAY, tmrs[index].action);
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
       
        LerSPIFFS(&data2spiffs, nome_arquivo);

        #if POWER320D == 1 
          data2spiffs.estado_pin = relayState;
        #else
          data2spiffs.estado_pin = digitalRead(PIN_RELAY);
        #endif

        strncpy(data2spiffs.DT, cz_dt, sizeof(cz_dt));
        data2spiffs.DT[strlen(data2spiffs.DT)] = '\0';
        //strncpy(historico_servidor, cz_dt, sizeof(cz_dt));

        EscreverSPIFFS(&data2spiffs, nome_arquivo);
        
        #if POWER320D == 1 
          SendData2WebSocket(BOARD_ID, relayState, cz_dt, data2eeprom.nome, "", 0);  
        #else
          SendData2WebSocket(BOARD_ID, digitalRead(PIN_RELAY), cz_dt, data2eeprom.nome, "", 0); 
        #endif
      }
    }
  }

}



//////////////////////////////////////////////////////////////////
// Processa/sincronoza a data e hora, no farmato epoch OVERLOAD //
//////////////////////////////////////////////////////////////////
void SincronizaDataHora(uint32_t unix_date_time) {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 284036400; // Jan 1 1979

  // Actualizar Data/Hora do Servidor
  pctime = unix_date_time + 3600; // + 3600 para aumentar 1 Hora (está a dar uma hora a menos)
  if ( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
    setTime(pctime); // Sync Arduino clock to the time received on the serial port
  }
}



////////////////////////
// Consulta de Timers //
////////////////////////
void ConsultarTimer(const char* nome_arquivo, uint8_t index) {

  char aux_hor[6];
  char aux_min[6];
  char hora[6];
  timers2EEPROM tmrs[QTD_TMRS_ESPNOW];
  EEPROM.get(EEPROM_TIMERS_ADDR, tmrs);

  uint8_t enable = tmrs[index].activar;
  uint8_t saida = tmrs[index].output;
  uint8_t accao = tmrs[index].action;
  char dias_semana[8];
  strncpy(dias_semana, tmrs[index].weekDays, 8);

  
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
    
  memset(json_array, 0, sizeof(json_array));  
  json_doc["CMD"] = "TCG";
  json_doc["Enable"] = enable;
  json_doc["Time"] = hora;
  json_doc["WeekDays"] = dias_semana;
  json_doc["Output"] = saida;
  json_doc["Action"] = accao;  
  serializeJson(json_doc, json_array);    
  json_doc.clear();
}




////////////////////////////////////////////
// Limpar configurações de Temporizadores //
////////////////////////////////////////////
/*void ResetTimers(){
 
  timers2EEPROM timers[QTD_TMRS_ESPNOW];

  for (uint8_t index = 0; index < QTD_TMRS_ESPNOW; index++){
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
   #if defined(ESP8266) 
    delay(10);
  #elif defined(ESP32) 
    vTaskDelay(5);
  #endif

}*/




//////////////////////////////////////////////////////////////
// Checa se um array de inteiros contem o elemento indicado //
//////////////////////////////////////////////////////////////
int8_t ArrayHasElement(uint32_t array[], uint32_t element){

  uint8_t cont = 0;
  
  // Checar o tipo de disposotivo
  uint8_t lastDigit = element % 10;
  if(lastDigit == 9){
  
    return -2; // Se for do tipo PASSIVO
  } 

  for (; cont < QTD_BOARDS; cont++){

    if(array[cont] == 255) break;

    if (array[cont] == element){
      return -1;
    }   
  }

  return cont;
}



/////////////////////////////
// Funçaão para ler spiffs //
/////////////////////////////
void LerSPIFFS(data2SPIFFS *ddv, const char* nome_arquivo){
  //LittleFS.begin(); 
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
  //LittleFS.end(); 
}




//////////////////////////////////////////////////////////////////
// Funçaão para tratar dados recebidos de clientes WebSocket JS //
//////////////////////////////////////////////////////////////////
void handleingIncomingData(void *arg, uint8_t *rx_data, size_t len, uint32_t client_id) {
 
  data2SPIFFS data2spiffs = {0};
  data2EEPROM data2eeprom = {0};
  LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);
  EEPROM.get(EEPROM_START_ADDR ,data2eeprom);

  AwsFrameInfo *info = (AwsFrameInfo*)arg; 

  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    
    char czjson[JSON_DOC_SIZE];
    json_doc.clear();
    memset(json_array, 0, sizeof(json_array));  
    DeserializationError erro = deserializeJson(json_doc, (const char*)rx_data);    

    if (json_doc.isNull() || erro){
      imprimeln(F("JSON inválido/nulo!"));    
      return;
    }
  
     
    const char* cz_cmd = json_doc["CMD"];
    uint32_t uz_id = json_doc["ID"];
    //esp32_get_uz_id = uz_id;

    // Obter marca de devices zigbee, de tras para frente
    uint8_t quarto = obterDigitoInvertido(uz_id, 1);
    uint8_t quinto = obterDigitoInvertido(uz_id, 2); 
         
    if( strcmp(cz_cmd, "TGL") == 0 ){     
     
      if(uz_id == BOARD_ID){ // Se for o servidor
        exec_toggle = true;           
      }
      else if(strcmp(cz_cmd, "TGL") == 0 && uz_id != BOARD_ID && quarto == 0 && quinto == 0){
       
        for (uint8_t cont = 0; (cont < QTD_DEVS_ZIGBEE); cont++){ // Start_for1

          if (arr_zb_devs[cont].zb_board_id == uz_id){
            char data[52] = "ZbSend {\"Device\":\""; 
            strcat(data, arr_zb_devs[cont].short_id); 
            strcat(data, "\",\"Send\":{\"Power\":\"TOGGLE\"}}");           
            Serial.write(data, sizeof(data));        
            Serial.write('\n');   

            delay(10);  

            sprintf(data, "ZbStatus3 %s", arr_zb_devs[cont].short_id);  
            Serial.write(data, 16); 
            Serial.write('\n');     

            break;
          }
        }
      }
      else if( uz_id != BOARD_ID){
        strcpy(czjson, CriarJSON("DATA", "TGL", "", uz_id, 2, WiFi.channel()));         

        #if defined(ESP8266) 
          esp_now_send(0, (uint8_t *)czjson, strlen(czjson));
        #elif defined(ESP32) 
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], (uint8_t *)czjson, strlen(czjson));
            
          }
        #endif       
          
      }      
    }
    else if( strcmp(cz_cmd, "TWN") == 0 ){
      
      if(uz_id == BOARD_ID){
        // Ligar WiFi Tasmota
        char cmnd[] = "WiFi ON";        
        Serial.write(cmnd, strlen(cmnd));      
        Serial.write('\n');        
      }
      else  if(uz_id != BOARD_ID){
        strcpy(czjson, CriarJSON("DATA", "TWN", "", uz_id, 2, WiFi.channel()));         

        #if defined(ESP8266) 
          esp_now_send(0, (uint8_t *)czjson, strlen(czjson));
        #elif defined(ESP32) 
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], (uint8_t *)czjson, strlen(czjson));
           
          }
        #endif            
      }     
    }
    else if( strcmp(cz_cmd, "TWF") == 0 ){
      
      if(uz_id == BOARD_ID){
        // Ligar WiFi Tasmota
        char cmnd[] = "WiFi OFF";        
        Serial.write(cmnd, strlen(cmnd));      
        Serial.write('\n');        
      }
      else  if(uz_id != BOARD_ID){
        strcpy(czjson, CriarJSON("DATA", "TWF", "", uz_id, 2, WiFi.channel()));         

        #if defined(ESP8266) 
          esp_now_send(0, (uint8_t *)czjson, strlen(czjson));
        #elif defined(ESP32) 
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], (uint8_t *)czjson, strlen(czjson));
           
          }
        #endif            
      }     
    }
    else if( strcmp(cz_cmd, "GWS") == 0 ){
      
      if(uz_id == BOARD_ID){
        // Solicita estado do WiFi Tasmota
        Serial.write("WiFi", 5);      
        Serial.write('\n'); 
        delay(100);
        // Solicita dados da conexao WiFi Tasmota
        Serial.write("Status 5", 9);      
        Serial.write('\n'); 
      }
      else  if(uz_id != BOARD_ID){
        strcpy(czjson, CriarJSON("DATA", "GWS", "", uz_id, 2, WiFi.channel()));         
        
        #if defined(ESP8266) 
          esp_now_send(0, (uint8_t *)czjson, strlen(czjson));
        #elif defined(ESP32) 
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], (uint8_t *)czjson, strlen(czjson));
           
          }
        #endif            
      }     
    }
    else if(strcmp(cz_cmd, "TCW") == 0){ // Conecta a wifi por dhcp
      if(uz_id == BOARD_ID){

        char cmnd[512];
        const char* cz_ssid = json_doc["MSG"];
        const char* cz_senha = json_doc["MAC"];

        sprintf(cmnd, "Backlog SSID1 %s; Password1 %s; Ipaddress1 0.0.0.0; restart 1", cz_ssid, cz_senha);
        Serial.write(cmnd, strlen(cmnd));      
        Serial.write('\n'); 
       
      }
      else  if(uz_id != BOARD_ID){
        const char* cz_ssid = json_doc["MSG"];
        const char* cz_senha = json_doc["MAC"];

        // MSG = ssid
        // MAC = seha
        strcpy(czjson, CriarJSON(cz_ssid, "TCW", cz_senha, uz_id, 2, WiFi.channel()));         
        
        #if defined(ESP8266) 
          esp_now_send(0, (uint8_t *)czjson, strlen(czjson));
        #elif defined(ESP32) 
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], (uint8_t *)czjson, strlen(czjson));
            
          }
        #endif            
      }           
    }  
    // Simula estado OFF do dispositivo criado a partir da indexedDB
    else if( strcmp(cz_cmd, "AFO") == 0 ){ 
      uint32_t uz_id = json_doc["ID"];
      const char* name = json_doc["MAC"];
      SendData2WebSocket(uz_id, 2, "OFF", name, "", 0);
    }  
    else if( strcmp(cz_cmd, "UPT") == 0 ){          
      // Correcção da Data
      if (year() < YEAR_COMPARE){    
        uint32_t linuxtime = json_doc["MSG"]; 
        SincronizaDataHora(linuxtime);    
      }

      SendData2WebSocket(0, 0, "REG", "", "", 0); 

      // Actualiza, nos clientes WS javascript, estado do Servidor
      #if POWER320D == 1
        SendData2WebSocket(BOARD_ID, relayState, data2spiffs.DT, data2eeprom.nome, "", 0); 
      #else
        SendData2WebSocket(BOARD_ID, digitalRead(PIN_RELAY), data2spiffs.DT, data2eeprom.nome, "", 0); 
      #endif

      // Repassa a soliitação de UPDATE aos demais dispositivos
      strcpy(czjson, CriarJSON("", cz_cmd, "", 0, 2, WiFi.channel())); 
    
      #if defined(ESP8266) 
          esp_now_send(0, (uint8_t *)czjson, strlen(czjson));
        #elif defined(ESP32) 
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], (uint8_t *)czjson, strlen(czjson));
            
          }
        #endif       
    }
    else if( strcmp(cz_cmd, "SDT") == 0 ){
      SincronizaDataHora((uint32_t)json_doc["MSG"]);
    }    
    else if( strcmp(cz_cmd, "STM") == 0 ){

      if( uz_id == BOARD_ID ){
        //ConfigurarTimer((const char*)data, FILE_DATA_SPIFFS);
      }// Se é um dispositivo zigbee
      else if(uz_id != BOARD_ID && quarto == 0 && quinto == 0){

        deserializeJson(json_doc, (const char*)rx_data); 
 
        // Actualiza regsidto indicado
        uint8_t offset = json_doc["Timer"];
        uint8_t iz_output = json_doc["Output"];
        uint8_t iz_action = json_doc["Action"];
        uint8_t iz_enable = json_doc["Enable"];
        uint32_t iz_id = json_doc["ID"];
        const char* hor_min = json_doc["Time"];
        const char* weedays = json_doc["WeekDays"];
        char short_id[ZB_SHORT_ID];

        //Limita o numero de Timers para dispozitivos ZigBee
        if( offset > QTD_TMRS_ZB){
          return;
        }
        
        // Obtem short_id do dispositivos
        for (uint8_t index = 0; index < QTD_DEVS_ZIGBEE; index++){
          if(arr_zb_devs[index].zb_board_id == iz_id){
            strcpy(short_id, arr_zb_devs[index].short_id); 
            guardarActualizarTimerZigBee( offset, iz_output, iz_action, iz_enable, iz_id, hor_min, weedays, short_id);           
            break;
          }         
        }
       
      }
      else if(uz_id != BOARD_ID){   
        
        // Repassa a soliitaçã de SET_TIMER aos demais dispositivos
        strcpy(czjson, CriarJSON((const char*)rx_data, cz_cmd, "", uz_id, 2, WiFi.channel())); 
    
        #if defined(ESP8266) 
          esp_now_send(0, (uint8_t *)czjson, strlen(czjson));
        #elif defined(ESP32) 
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], (uint8_t *)czjson, strlen(czjson));
          
          }
        #endif      
      }      
    }
    else if( strcmp(cz_cmd, "SHT") == 0 ){ // Strat_if_SHT

       deserializeJson(json_doc, (const char*)rx_data); 
 
        // Actualiza regsidto indicado
        uint8_t offset = json_doc["Timer"];
        uint8_t iz_output = json_doc["Output"];
        uint8_t iz_action = json_doc["Action"];
        uint8_t iz_enable = json_doc["Enable"];
        const char* hor_min = json_doc["Time"];
        const char* weedays = json_doc["WeekDays"];        
               
        // Envia cfg Timer em todos dispositivos ZigBee
        for (uint8_t indice = 0; indice < QTD_DEVS_ZIGBEE; indice++){
          if(arr_zb_devs[indice].zb_board_id > 0 && offset <= QTD_TMRS_ZB ){           
            guardarActualizarTimerZigBee( offset, iz_output, iz_action, iz_enable, arr_zb_devs[indice].zb_board_id, hor_min, weedays, arr_zb_devs[indice].short_id);
          }         
        }  


        // Alterar ID do dispositivo no JSON, enviar para dispositivos ESP-NOW
        #define AUX_JSON_SIZE 110
        char json_array[AUX_JSON_SIZE];
        json_doc["ID"] = ID_BROADCAST_ESPNOW;
        json_doc["CMD"] = "STM";
        json_doc["Timer"] = offset;
        json_doc["Enable"] = iz_enable;
        json_doc["Time"] = hor_min;
        json_doc["WeekDays"] = weedays;
        json_doc["Output"] = iz_output;
        json_doc["Action"] = iz_action;
        memset(json_array, 0, sizeof(json_array));  
        serializeJson(json_doc, json_array);
        
        strcpy(czjson, CriarJSON(json_array, "STM", "", ID_BROADCAST_ESPNOW, 2, WiFi.channel()));
   

        #if defined(ESP8266)
          esp_now_send(0, (uint8_t *)czjson, strlen(czjson));
        #elif defined(ESP32)
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], (uint8_t *)czjson, strlen(czjson));
          } 
        #endif             
          

    }// End_if_SHT */       
    else if( strcmp(cz_cmd, "GTM") == 0 ){    
      if( uz_id == BOARD_ID && ESPNOW_TASMOTA_ZIGBEE_BRIDGE == 0){        
        ws_obj.textAll(json_array);
        wss_obj.textAll(json_array);
      }// Se é um ldispositivo Zigbee  
      else if(uz_id != BOARD_ID && quarto == 0 && quinto == 0){              
        
        char short_id[ZB_SHORT_ID];
        uint8_t indice = 0; 
        uint8_t iz_timer = json_doc["Timer"];
        
        // Obtem ID do dispositivos, naestructura "ZigbeeDevices"
        for (uint8_t index = 0; index < QTD_DEVS_ZIGBEE; index++){
          if(arr_zb_devs[index].zb_board_id == uz_id){
            strcpy(short_id, arr_zb_devs[index].short_id);                   
            break;
          }
        }

        // Obtem a posicao do elemento no array das estructuras "zbtimers2SPIFFS"
        for (uint8_t index = 0; index < QTD_DEVS_ZIGBEE; index++){
          if(strcmp(timersZB[index].short_id, short_id) == 0 && timersZB[index].timer == iz_timer){
            indice = index;  
            break;
          }
          else{
            indice = 255;
          }
        }        

      
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
        memset(json_array, 0, sizeof(json_array));  
        serializeJson(json_doc, json_array);
        uint8_t total_data_lenght = strlen(json_array);      
        ws_obj.textAll(json_array, total_data_lenght);
        wss_obj.textAll(json_array, total_data_lenght);          
        json_doc.clear(); 
      }
      else if(uz_id != BOARD_ID && quarto != 0 && quinto != 0){
        // Repassa a soliitaçã de GET_TIMER aos demais dispositivos
       #if defined(ESP8266)           
          esp_now_send(0, rx_data, len);         
        #elif defined(ESP32) 
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], rx_data, len);
          }
        #endif      
      }     
    }
    else if( strcmp(cz_cmd, "RSD") == 0 ){
      if( uz_id == BOARD_ID ){        
        strncpy(data2eeprom.CONFIGURADO, "NO", sizeof(data2eeprom.CONFIGURADO));
        data2eeprom.CONFIGURADO[strlen(data2eeprom.CONFIGURADO)] = '\0';
        EEPROM.put(EEPROM_START_ADDR, data2eeprom);
        EEPROM.commit();
        #if defined(ESP8266) 
          delay(10);
        #elif defined(ESP32) 
          vTaskDelay(5);
        #endif
        ESP.restart(); 
      }  
      else  if( uz_id != BOARD_ID ){
        // Repassa a soliitaçã de RESET_TIMERS aos demais dispositivos
        strcpy(czjson, CriarJSON("", cz_cmd, "", uz_id, 2, WiFi.channel())); 
        #if defined(ESP8266) 
          esp_now_send(0, (uint8_t *)czjson, strlen(czjson));
        #elif defined(ESP32) 
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], (uint8_t *)czjson, strlen(czjson));
            
          }
        #endif      
      }       
    }
    else if( strcmp(cz_cmd, "RNM") == 0 ){
      
      if( uz_id == BOARD_ID ){         
        data2SPIFFS data2spiffs = {0};
        data2EEPROM data2eeprom = {0}; 
        EEPROM.get(EEPROM_START_ADDR, data2eeprom);   
        const char* cz_nome = json_doc["MAC"];
        strncpy(data2eeprom.nome, cz_nome, sizeof(data2eeprom.nome));              
        EEPROM.put(EEPROM_START_ADDR, data2eeprom);
        EEPROM.commit();
        
        LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);
        SendData2WebSocket(BOARD_ID, data2spiffs.estado_pin, data2spiffs.DT, data2eeprom.nome, "", 0);
      }  
      else if( strcmp(cz_cmd, "RNM") == 0 && uz_id != BOARD_ID && quarto == 0 && quinto == 0){
        
        for (uint8_t cont = 0; (cont < QTD_DEVS_ZIGBEE); cont++){ // Start_for1

          if ( arr_zb_devs[cont].zb_board_id == uz_id){
            const char* cz_new_name = json_doc["MAC"];
            String cmnd = "ZbName " + String(arr_zb_devs[cont].short_id) + "," + String(cz_new_name);
            uint8_t var_len = cmnd.length() + 1;
            char data[var_len];
            cmnd.toCharArray(data, var_len);             
            Serial.write(data, var_len);        
            Serial.write('\n'); 

            delay(10);  

            sprintf(data, "ZbStatus3 %s", arr_zb_devs[cont].short_id);  
            Serial.write(data, 16); 
            Serial.write('\n'); 
           
            break;
          }
        }
      }
      else  if( uz_id != BOARD_ID ){
        // Repassa a soliitaçã de SET_DEV_NAME aos demais dispositivos     
        #if defined(ESP8266)           
          esp_now_send(0, rx_data, len);
          delay(10);                            
        #elif defined(ESP32) 
         
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ){
              esp_now_send(enderecos_mac[cont], rx_data, len);
              vTaskDelay(1);
            }             
          }
        #endif      
      }       
    }   
    else if( strcmp(cz_cmd, "ENABLE_OTA") == 0 ){
      if( uz_id == BOARD_ID ){ 
        TrocarModoOperacao("OTA");   
      }  
      else  if( uz_id != BOARD_ID ){
        // Repassa a soliitaçã de ENABLE_OTA aos demais dispositivos     
        #if defined(ESP8266) 
          esp_now_send(0, rx_data, len);
        #elif defined(ESP32) 
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], rx_data, len);           
          }
        #endif      
      }       
    }
    else if( strcmp(cz_cmd, "SIR") == 0 ){
      if( uz_id != BOARD_ID ){
        // Repassa a soliitaçã de SIR aos demais dispositivos   
        const char* cz_id_btn = json_doc["MAC"];
         const char* cz_aparelho = json_doc["MSG"];
        strcpy(czjson, CriarJSON(cz_aparelho, cz_cmd, cz_id_btn, uz_id, 2, client_id));   
        #if defined(ESP8266) 
          esp_now_send(0, (uint8_t *)czjson, strlen(czjson));
        #elif defined(ESP32) 
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], (uint8_t *)czjson, strlen(czjson));
          }
        #endif      
      }       
    }
    else if( strcmp(cz_cmd, "RIR") == 0 ){
      if( uz_id != BOARD_ID ){
        // Repassa a soliitaçã de RIR aos demais dispositivos   
        const char* cz_id_btn = json_doc["MAC"];
        const char* cz_aparelho = json_doc["MSG"];
        strcpy(czjson, CriarJSON(cz_aparelho, cz_cmd, cz_id_btn, uz_id, 2, client_id));         
        #if defined(ESP8266) 
          esp_now_send(0, (uint8_t *)czjson, strlen(czjson));
        #elif defined(ESP32) 
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], (uint8_t *)czjson, strlen(czjson));
          }
        #endif      
      }       
    }
    else if( strcmp(cz_cmd, "CKS") == 0 ){
      if( uz_id != BOARD_ID ){
        // Repassa a soliitaçã de RIR aos demais dispositivos   
        const char* cz_id_btn = json_doc["MAC"];
        const char* cz_aparelho = json_doc["MSG"];
        strcpy(czjson, CriarJSON(cz_aparelho, cz_cmd, cz_id_btn, uz_id, 2, client_id)); 
        #if defined(ESP8266) 
          esp_now_send(0, (uint8_t *)czjson, strlen(czjson));
        #elif defined(ESP32) 
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], (uint8_t *)czjson, strlen(czjson));
            
          }
        #endif      
      }       
    }
    else if( strcmp(cz_cmd, "UPK") == 0 ){
      if( uz_id != BOARD_ID ){
        // Repassa a soliitaçã de RIR aos demais dispositivos   
        const char* cz_id_btn = json_doc["MAC"];
        const char* cz_aparelho = json_doc["MSG"];
        strcpy(czjson, CriarJSON(cz_aparelho, cz_cmd, cz_id_btn, uz_id, 2, client_id));  
        #if defined(ESP8266) 
          esp_now_send(0, (uint8_t *)czjson, strlen(czjson));
        #elif defined(ESP32) 
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], (uint8_t *)czjson, strlen(czjson));
            
          }
        #endif      
      }       
    }
    else if( strcmp(cz_cmd, "SRI") == 0 ){
      if( uz_id != BOARD_ID ){
        // Repassa a soliitaçã de SRI aos demais dispositivos   
        strcpy(czjson, CriarJSON("", cz_cmd, "", uz_id, 2, WiFi.channel())); 
        #if defined(ESP8266) 
          esp_now_send(0, rx_data, len);
        #elif defined(ESP32) 
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], rx_data, len);
            
          }
        #endif      
      }       
    }
    else if( strcmp(cz_cmd, "GRC") == 0 ){
      if( uz_id != BOARD_ID ){
        // Repassa a soliitaçã de GRC aos demais dispositivos   
        strcpy(czjson, CriarJSON("", cz_cmd, "", uz_id, 2, WiFi.channel()));  
        #if defined(ESP8266) 
          esp_now_send(0, rx_data, len);
        #elif defined(ESP32) 
          // cont = 1 -- xq o eleento 0 não conta
          for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
            if( devs_id[cont] == uz_id ) esp_now_send(enderecos_mac[cont], rx_data, len);
            
          }
        #endif      
      }       
    }
    else if( strcmp(cz_cmd, "DST") == 0){  

      //#define DOC_DEVS_INFO_SIZE 2048             
      //#define DOC_DEVS_INFO_SIZE (ESP.getMaxFreeBlockSize() - 512)

      DeserializationError error = deserializeJson(json_doc, json_doc["MSG"]);
      
      if (error){        
        imprimeln(F("Erro ao analisar JSON: "));
        imprimeln(error.c_str());
        return;
      }
     
      const char *cz_nome = json_doc["Nome"];
      uint32_t iz_id = json_doc["Id"];

      // Actualiza o nome do device na struct
      for (uint8_t cont = 0; cont < QTD_BOARDS; cont++){
        if (array_alive[cont].board_id == iz_id){
          strcpy(array_alive[cont].board_name, cz_nome);
          array_alive[cont].board_name[strlen(array_alive[cont].board_name)] = '\0';
          break;
        }
      }
      
      json_doc.clear();
    }  

    json_doc.clear();

  }

  //LittleFS.end();

}// Fim de handleingIncomingData()




//////////////////////////////////////////////////////////////////
// Callback para ebventos provenientes de clientes WebSocket JS //
//////////////////////////////////////////////////////////////////
void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  
  if (type == WS_EVT_CONNECT) {    

    iz_conex = (uint8_t)ws_obj.count();

    // Limita o número máximo de conexões ao Servidor WS.
    // (4 Clientes simultâneos, em função da alteraçã feita em "lib/ESPAsyncWebServer/src/AsyncWebSocket.h")
    if( iz_conex > (DEFAULT_MAX_WS_CLIENTS - 1)){
   
      json_doc["MSG"] = "";
      json_doc["CMD"] = "END";
      json_doc["MAC"] = "";
      json_doc["ID"] = 0;
      json_doc["PST"] = 2;
      json_doc["WCH"] = 0;

      memset(json_array, 0, sizeof(json_array));  
      serializeJson(json_doc, json_array);
      uint8_t data_lenght = strlen(json_array);
      client->text(json_array, data_lenght);
      client->close();
      json_doc.clear();
    }

  #if DEBUG == 1
    char cz_msg[128]; 
    sprintf(cz_msg, "Número de conexões WS = %d\nRAM = %d\n", iz_conex, ESP.getFreeHeap());
    imprimeln(cz_msg); 
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

    // Envia lista dos ID dos devices  
    SendData2WebSocket(0, 0, "REG", "", "", 0); 
  }
  else if(type == WS_EVT_DATA){    
    handleingIncomingData(arg, data, len, client->id());
  }
  else if(type == WS_EVT_DISCONNECT){   
  #if DEBUG == 1
    char cz_msg[128]; 
    iz_conex = (uint8_t)ws_obj.count(); 
    sprintf(cz_msg, "Número de conexões WS = %d\nRAM = %d\n", iz_conex, ESP.getFreeHeap());
    imprimeln(cz_msg);
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
  }
}



void onEvent1(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  
  if (type == WS_EVT_CONNECT) {    

    iz_conex = (uint8_t)wss_obj.count();

    // Limita o número máximo de conexões ao Servidor WS.
    // (4 Clientes simultâneos, em função da alteraçã feita em "lib/ESPAsyncWebServer/src/AsyncWebSocket.h")
    if( iz_conex > (DEFAULT_MAX_WS_CLIENTS - 1)){
     
      json_doc["MSG"] = "";
      json_doc["CMD"] = "END";
      json_doc["MAC"] = "";
      json_doc["ID"] = 0;
      json_doc["PST"] = 2;
      json_doc["WCH"] = 0;

      memset(json_array, 0, sizeof(json_array));  
      serializeJson(json_doc, json_array);
      uint8_t data_lenght = strlen(json_array);
      client->text(json_array, data_lenght);
      client->close();
      json_doc.clear();
    }

  #if DEBUG == 1
    char cz_msg[128]; 
    sprintf(cz_msg, "Número de conexões WS = %d\nRAM = %d\n", iz_conex, ESP.getFreeHeap());
    imprimeln(cz_msg);  
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

    // Envia lista dos ID dos devices  
    SendData2WebSocket(0, 0, "REG", "", "", 0); 
  }
  else if(type == WS_EVT_DATA){    
    handleingIncomingData(arg, data, len, client->id());
  }
  else if(type == WS_EVT_DISCONNECT){   
  #if DEBUG == 1
    char cz_msg[128]; 
    iz_conex = (uint8_t)wss_obj.count(); 
    sprintf(cz_msg, "Número de conexões WS = %d\nRAM = %d\n", iz_conex, ESP.getFreeHeap());
    imprimeln(cz_msg);
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
  }
  
}


////////////////////////////////////////////////////////////
// Limpara ArquivoSPIFFS com pin_sttate, DT e Array Alive //
////////////////////////////////////////////////////////////
void ResetSPIFFS(){
    LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);

    strncpy(data2spiffs.DT, "00:00:00, 00/00/0000", sizeof(data2spiffs.DT));
    data2spiffs.DT[strlen(data2spiffs.DT)] = '\0';
    data2spiffs.estado_pin = 0;

    // Preenche array com 255 e coloca 0 ID do Servidor na primeira posição
    for (uint8_t index = 0; index < QTD_BOARDS; index++){
      array_alive[0].BOARDS_ID[index] = 255;
      array_alive[index].board_id =  255;
      array_alive[index].temp_alive =  0;
    }
    
    array_alive[0].BOARDS_ID[0] = CriarDEVICE_ID();
    array_alive[0].board_id = CriarDEVICE_ID();
    array_alive[0].temp_alive = 0;

    EscreverSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);
     #if defined(ESP8266) 
        delay(10);
      #elif defined(ESP32) 
        vTaskDelay(5);
      #endif  

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

//////////////////////////////////////////////////////////////////////
// Enviar comando para alterar estado do PIN do Dispositivo PASSIVO //
//////////////////////////////////////////////////////////////////////
void mudarEstadoPINPassivo(uint8_t estado, uint32_t dev_id){
  char czjson[JSON_DOC_SIZE];      
  strcpy(czjson, CriarJSON("DATA", "TGG", "", dev_id, estado, WiFi.channel()));     
  #if defined(ESP8266) 
    esp_now_send(0, (uint8_t *)czjson, strlen(czjson));  
  #elif defined(ESP32) 
    // cont = 1 -- xq o eleento 0 não conta
    for(uint8_t cont = 1; cont < num_enderecos_mac; cont++){
      if( devs_id[cont] == dev_id ) esp_now_send(enderecos_mac[cont], (uint8_t *)czjson, strlen(czjson));
      
    }
  #endif   
}
   



/////////////////////////////////////////
// Processar dados recebidos via Telne //
/////////////////////////////////////////
#if TELNET == 1
  void LerTelnet() {
    data2SPIFFS data2spiffs = {0};
    if (TelnetStream.available()) {
      char Letra = TelnetStream.read();
      switch (Letra) {
        case 'R':
          TelnetStream.stop();
           #if defined(ESP8266) 
            delay(100);
          #elif defined(ESP32) 
            vTaskDelay(10);
          #endif
          ESP.restart();
          break;
        case 'C':
          TelnetStream.println("Adios, adios");
          TelnetStream.stop();
          break;
        case 'D':
        case 'd':
          LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);

          TelnetStream.println("Dados do SPIFFS:");
          TelnetStream.println("================");
          for(uint8_t cont = 0; cont < QTD_BOARDS; cont++){
            TelnetStream.print(cont);
            TelnetStream.print(" -- ");
            TelnetStream.println(array_alive[0].BOARDS_ID[cont]);        
          } 

          TelnetStream.print("Hitorico: ");
          TelnetStream.println(data2spiffs.DT);
          TelnetStream.print("Estado do Pin: ");
          TelnetStream.println(data2spiffs.estado_pin);

          TelnetStream.println("Dados da EEPROM:");
          TelnetStream.println("================");
          TelnetStream.print("Configuração: ");
          TelnetStream.println(data2eeprom.CONFIGURADO);
          TelnetStream.print("Modo: ");
          TelnetStream.println(data2eeprom.Modo);
          TelnetStream.print("Nome: ");
          TelnetStream.println(data2eeprom.nome);
          TelnetStream.print("Tipo: ");
          TelnetStream.println(data2eeprom.TIPO);
          break;
      }
    }
  }
#endif



#ifdef ESP32
  void guardar_endereco_mac(uint8_t newMAC[6], uint32_t dev_id) {
      // Verificar se o endereço MAC já existe no array
      for (uint8_t i = 0; i < num_enderecos_mac; i++) {
          if (memcmp(newMAC, enderecos_mac[i], 6) == 0) {
              // O endereço MAC já existe, portanto, não é necessário adicioná-lo
              return;
          }
      }

      // Adicionar o novo endereço MAC ao array se não existir
      if (num_enderecos_mac < QTD_BOARDS) {
          memcpy(enderecos_mac[num_enderecos_mac], newMAC, 6);
          devs_id[num_enderecos_mac] = dev_id;
          num_enderecos_mac++;
      }
  }
#endif


///////////////////////////////////////////////////
// Actualiza Data e Hora atraves de Servidor NTP //
///////////////////////////////////////////////////
bool actualizaDTfromNTP(const char* servidor_ntp){
  WiFiClient client;
  WiFiUDP ntpUDP; 
  if (client.connect(servidor_ntp, 80)) {
    NTPClient timeClient(ntpUDP, servidor_ntp, 3600, 60000);
    timeClient.begin();
    timeClient.update();
    SincronizaDataHora(timeClient.getEpochTime() - 3600);
    client.stop();
    return true;
  } 
  else{
    return false;
  }
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
 
    // Processar resposta do comando "WiFi"
    if( data.indexOf("RESULT = {\"Wifi\":\"") != -1){  

      procesing_uart_data = true;

      uint8_t startIndex = data.indexOf("{");
      uint8_t endIndex = data.indexOf("}", startIndex);

      data = data.substring(startIndex, endIndex + 1);                   
       
      deserializeJson(zb_json_doc, data);

      const char* cz_wifi_state = zb_json_doc["Wifi"];

      if(strcmp(cz_wifi_state, "ON") == 0){
        iz_wifi_state = 1;
      } 
      else  if(strcmp(cz_wifi_state, "OFF") == 0){
        iz_wifi_state = 0;
      }          
       
      procesing_uart_data = false;    

    }
    // Processar res[posta do comando "Status 5"
    else if( data.indexOf("STATUS5 = {\"StatusNET\":{\"") != -1){  

      procesing_uart_data = true;

      uint8_t startIndex = data.indexOf("{");
      
      data = data.substring(startIndex);               
       
      deserializeJson(zb_json_doc, data);

      const char* cz_ip = zb_json_doc["StatusNET"]["IPAddress"];  

      char jsondata[48];
      sprintf(jsondata,"{\"WifiState\":%d,\"IPAddress\":\"%s\"}", iz_wifi_state, cz_ip);
      SendData2WebSocket(0, 0, "SWS", jsondata, "", 0);      

      procesing_uart_data = false;

    }
    else if( data.indexOf("RESULT = {\"ZbStatus1\":[{") != -1){  

      procesing_uart_data = true;

      uint8_t startIndex = data.indexOf("[");
      uint8_t endIndex = data.indexOf("]", startIndex);

      data = data.substring(startIndex, endIndex + 1);
      uint8_t iz_len = data.length() + 1;
      char cz_json[iz_len + 1];
      data.toCharArray(cz_json, iz_len);
              
      DeserializationError error = deserializeJson(zb_json_doc, cz_json);
      if (error) {
        imprimeln(F("Falha no parsing do JSON: "));
        imprimeln(error.c_str());
        return;
      }

      // Verificar se o JSON é um array
      if (!zb_json_doc.is<JsonArray>()) {
        imprimeln(F("JSON não é um array."));
        return;
      }

      JsonArray array = zb_json_doc.as<JsonArray>();
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

      procesing_uart_data = false;

    }
    else if (data.indexOf("SENSOR = {\"ZbReceived\":{\"") != -1 && data.indexOf("\"Power\"") != -1 ){// Start_if1
      
      procesing_uart_data = true;
 
      data = data.substring(data.indexOf("{\"Device\":", data.indexOf("ZbReceived\":{")), data.indexOf("}}}") + 1);
   
      DeserializationError error = deserializeJson(zb_json_doc, data);
      if (error){
        imprimeln(F("Falha no parsing do JSON: "));
        imprimeln(error.c_str());
        return;
      }
     

      const char *short_id = zb_json_doc["Device"];
      strcpy(get_short_id, short_id);
      const char *name = zb_json_doc["Name"];
      uint8_t power = zb_json_doc["Power"];   
      boolean reachable = zb_json_doc["Reachable"];        
      char czjson[JSON_DOC_SIZE]; 
           
      for (uint8_t cont = 0; (cont < QTD_DEVS_ZIGBEE); cont++){// Star_for
               
        if (arr_zb_devs[cont].zb_board_id > 0 && strcmp(arr_zb_devs[cont].short_id, short_id) == 0 && strcmp(arr_zb_devs[cont].short_id, "") != 0){         

          // Registo do dispositivo nos browser
          int8_t index = ArrayHasElement(array_alive[0].BOARDS_ID, arr_zb_devs[cont].zb_board_id); 
          if (index != -1 && index != -2){      
            array_alive[0].BOARDS_ID[index] = arr_zb_devs[cont].zb_board_id;   
            SendData2WebSocket(0, 2, "REG", "", "", 0);                      
          } 
         
          // Enviar mensagem "OFF" dos disp[ositivos ZigBee desligados]
          if( reachable == false && zb_json_doc.containsKey("Reachable")){
            SendData2WebSocket(arr_zb_devs[cont].zb_board_id, 2, "OFF", name, "", 0);  
            
            char data[17] = "";    
            sprintf(data, "ZbStatus3 %s", arr_zb_devs[cont].short_id);  
            Serial.write(data, 16); 
            Serial.write('\n');            
          } 

          strcpy(czjson, CriarJSON(data2spiffs.DT, "PRZ", name, arr_zb_devs[cont].zb_board_id, power, WiFi.channel()));
          
          #if defined(ESP8266) 
            callback_rx_esp_now(0, (uint8_t *)czjson, strlen(czjson));
          #elif defined(ESP32) 
            callback_rx_esp_now(0, (uint8_t *)czjson, strlen(czjson));
          #endif    
         
          char cz_dt[DT_STR_SIZE];
          ObterDTActual(cz_dt);
          cz_dt[strlen(cz_dt)] = '\0';            
                   
          strcpy(czjson, CriarJSON(cz_dt, "SAC", arr_zb_devs[cont].name, arr_zb_devs[cont].zb_board_id, power, WiFi.channel()));
          
          #if defined(ESP8266) 
            callback_rx_esp_now(0, (uint8_t *)czjson, strlen(czjson));
          #elif defined(ESP32) 
            callback_rx_esp_now(0, (uint8_t *)czjson, strlen(czjson));
          #endif    
          

          // Actualiza informacoes os devices
          strcpy(arr_zb_devs[cont].name, name);
          arr_zb_devs[cont].power = power;
          arr_zb_devs[cont].Reachable = reachable;
        }   
       
      
      }// End_for

      procesing_uart_data = false;

    }// End_if1
    else  if( data.indexOf("RESULT = {\"ZbStatus3\":[{\"Device\":") != -1){// Start_if2
    
      procesing_uart_data = true;

      data = data.substring(data.indexOf("{"));
 
      DeserializationError error = deserializeJson(zb_json_doc, data);
      if (error){
        imprimeln(F("Falha no parsing do JSON: "));
        imprimeln(error.c_str());
        return;
      }
          
      const char *short_id = zb_json_doc["ZbStatus3"][0]["Device"];
      const char *name = zb_json_doc["ZbStatus3"][0]["Name"];
      uint8_t power = zb_json_doc["ZbStatus3"][0]["Power"];
      boolean reachable = zb_json_doc["ZbStatus3"][0]["Reachable"];
      String valorHexadecimal = zb_json_doc["ZbStatus3"][0]["IEEEAddr"]; // 0xA4C1 38E342BDFB48          

      // TODO: Rvere atribuicao do tipo
      uint32_t zb_board_id = ZbDEVICE_ID(valorHexadecimal, lAMPADA); 
                  
      for (uint8_t cont = 0; (cont < QTD_DEVS_ZIGBEE); cont++){ // Start_for1

        if (strcmp(arr_zb_devs[cont].short_id, short_id) == 0 && arr_zb_devs[cont].short_id[1] == 'x'){
          strcpy(arr_zb_devs[cont].name, name);
          arr_zb_devs[cont].power = power;
          arr_zb_devs[cont].zb_board_id = zb_board_id;
          arr_zb_devs[cont].Reachable = reachable;

          // Registo do dispositivo nos browser
          int8_t index = ArrayHasElement(array_alive[0].BOARDS_ID, zb_board_id); 
          if (index != -1 && index != -2){      
            array_alive[0].BOARDS_ID[index] = zb_board_id;   
            SendData2WebSocket(0, 2, "REG", "", "", 0);             
          }   

          // Enviar mensagem "OFF" dos disp[ositivos ZigBee desligados]
          if( reachable == false){
            SendData2WebSocket(zb_board_id, 2, "OFF", name, "", 0);  

            char data[17] = "";    
            sprintf(data, "ZbStatus3 %s", arr_zb_devs[cont].short_id);  
            Serial.write(data, 16); 
            Serial.write('\n'); 
          }
          else{
            char czjson[JSON_DOC_SIZE]; 
            char cz_dt[DT_STR_SIZE];
            ObterDTActual(cz_dt);
            cz_dt[strlen(cz_dt)] = '\0';

            strcpy(czjson, CriarJSON(cz_dt, "SAC", name, zb_board_id, power, WiFi.channel()));
            
            // Envioar "STT" dos dispositivos ZigBee                        
            SendData2WebSocket(arr_zb_devs[cont].zb_board_id, power, cz_dt, name, "", 0);
            
            #if defined(ESP8266) 
              callback_rx_esp_now(0, (uint8_t *)czjson, strlen(czjson));
            #elif defined(ESP32) 
              callback_rx_esp_now(0, (uint8_t *)czjson, strlen(czjson));
            #endif    
            delay(10);
          }          
          
          break;
        }
             
      } // End_for18 

      procesing_uart_data = false;

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
  arquivo.read((byte *)ddv, sizeof(*ddv)*QTD_DEVS_ZIGBEE);
  arquivo.close();    
}


//////////////////////////////////
// Funçaão para escrever spiffs //
//////////////////////////////////
void EscreverZbSPIFFS(zbtimers2SPIFFS *ddv, const char* nome_arquivo){
  File arquivo = LittleFS.open(nome_arquivo, "w");
  arquivo.write((byte *)ddv, sizeof(*ddv)*QTD_DEVS_ZIGBEE);
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
      
      if (hour() == hours && minute() == minuts && second() <= 5) {
        char data[47] = "";    
        sprintf(data,"ZbSend {\"Device\":\"%s\",\"Send\":{\"Power\":\"%d\"}}", timersZB[index].short_id, timersZB[index].action);
        Serial.write(data, strlen(data)); 
        Serial.write('\n'); 
        
        delay(5);   

        sprintf(data, "ZbStatus3 %s", timersZB[index].short_id);  
        Serial.write(data, 16); 
        Serial.write('\n');        
      }
    }         
  }  
}


//////////////////////////////////////////////////////
// Guardar/Actualizar Timers de Dispositivos ZigBee //
//////////////////////////////////////////////////////
void guardarActualizarTimerZigBee( uint8_t offset, uint8_t iz_output, uint8_t iz_action, uint8_t iz_enable, uint32_t iz_id, const char *hor_min, const char *weedays, const char* short_id){

  // Guarda/Actualiza Timers
  for (uint8_t index = 0; index < QTD_DEVS_ZIGBEE; index++){
    // Carregar dados existentes no array "timersZB"
    LerZbSPIFFS(timersZB, FILE_ZBTMRS_SPIFFS);

    // Se o array esta vazio, preencher a primeira posicao
    if (strcmp(timersZB[index].short_id, "") == 0 && index == 0){
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
    else if (strcmp(timersZB[index].short_id, short_id) == 0 && timersZB[index].timer == offset){
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
    else if (strcmp(timersZB[index].short_id, short_id) != 0 && strcmp(timersZB[index].short_id, "") == 0){
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
}
  
#endif

