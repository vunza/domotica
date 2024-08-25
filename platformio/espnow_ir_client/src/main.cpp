#include <headers.h>

/*
          BUTTON    IRsend    IRrecv   LedLink_i   MCU
---------------------------------------------------------------- 
ATHOM     GPIO0     GPIO4     GPIO5    GPIO13      ESP8266 2M 
ESP8266   GPIO13    GPIO14    GPIO5    GPIO4       ESP8285 2M 
(Adaptado)
*/

#define PIN_RX 5
#define PIN_TX 14
#define PIN_LED_READ 2 // 14 para ATHOM
#define PIN_BUTTON 13                  
#define PIN_LED 4
#define QTD_BOARDS 10                      // Número máximo de Clientes ESP
//#define NOME_OTA "ESP-OTA"
#define DATA_SIZE 50
#define MSG_LENGTH 15
#define MAC_STR_SIZE 18
#define TEMPO_ENVIAR_ALIVE 5                // Segundos
#define QTD_TMRS 16                         // Número máximo de Timers
#define JSON_DOC_SIZE 248                   // Tamanho para documentos JSON (DynamicJsonDocument)
#define DT_STR_SIZE 21                      // Tamanho da string contendo a data e hora do Device
#define freq_khz 38                         // Frequência de InfraRed
#define MAC_BYTE_SIZE 6                     // Tamanho do buffer de MAC Address 
#define MAX_NAME_LENGTH 24                  // Comprimento do Nome dos dispositivos (64 - 1)
#define SSID_STR_SIZE 31                    // Tamanho da string contendo o nome de SSID
#define PWD_STR_SIZE 31                     // Tamanho da string contendo a senha
#define MAX_MOD_SIZE 11   
#define MAX_WIFI_CH 13
#define YEAR_COMPARE 2022
#define MAX_TIME_CONN 15                   // Tempo, em segundos, para conexão à rede WiFi  
#define TEMPO_ENVIAR_ALIVE 5               // Tempo em segundos
#define TMP_CTRL_SERVER_ALIVE 60           // Tempo em segundos
#define IP_MODE_SIZE 8                     // Tamanho do buff P/ modos de ip nos modos WIFI_STA e WIFI_AP  
#define EEPROM_START_ADDR 0
#define FILE_DATA_SPIFFS "/data.txt"
#define TYPE_SIZE 5                        // Tamanho do BUFF para o tipo de dispositivo
#define MAX_ID_DIGITS 7
#define IR_TYPE_SIZE 24                    // SONY, NEC, MITSUBISHI_HEAVY_152, etc...
#define NOME_CTRL_REMOTO_SIZE 4                 // ACn, TVn, VTn, ... (n = Número)
#define NOME_TCLA_SIZE 4                        // PWR, ONE, ...
#define QTD_COD_IR 100                     // TOTAL DECÓDIGOS IR A GUARDAR 
#define QTD_AMOSTRAS_IR 5                 // Quantidade de amostras de código IR 
#define JSON_DOC_RAW_SIZE 512*3
#define PASSIVE_SLAVE_ID 0
#define ID_BROADCAST_ESPNOW 9999999         // ID para broadcast aos dispositivos ESP-NOW

#define DEBUG 0
#define IS_PASSIVE_MASTER 0
#define SWAP_UART 0
#define INVERT_PIN 0
#define TELNET 0

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
  char Modo[MAX_MOD_SIZE];        // SIZE = 7 ???                    // OTA, OPRA, OPRS, OTA
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
  uint8_t TIPO; // lAMPADA = 0, VENTILADOR = 1, ARCONDICIONADO = 2, ELECTROBOMBA = 3
  WiFiMode_t wifi_mode;

} data2EEPROM;

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



//uint8_t broadcastAddress[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC}; // Testes
uint8_t broadcastAddress[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xB0}; // Produção
AsyncEventSource eventos("/eventos");
boolean read_ir_code = false; 
uint64_t cont_send_alive = 0;
IRsend irsend(PIN_TX);
const char* CMND = "cmnd";               // Comandos recebidos por via do Javascript
uint64_t cont_alive = 0;
unsigned long ctrl_flash_led = 0;  
unsigned long cont_exec_timer = 0;
AsyncWebServer server(8090);
uint64_t ctl_btn_pulso = 0;
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
//data2SPIFFS data2spiffs = {0};
// TODO: Tornar dinâmica as escolhas abaixo (xx_ip_mode)
const char* ap_ip_mode = "DEFAULT";     // IP_MODE = [DEFAULT | CUSTOM]
const char* sta_ip_mode = "DHCP";       // STA_IP_MODE = [DHCP | STATIC]
#define EEPROM_TIMERS_ADDR sizeof(data2EEPROM) + 2
//data2SPIFFS array_cod_ir[QTD_COD_IR];
char nome_ctrl_remoto[NOME_CTRL_REMOTO_SIZE];
char nome_tecla[NOME_TCLA_SIZE];
uint8_t ir_samples = 0;
uint32_t id_cliente_ws = 0;
StaticJsonDocument <JSON_DOC_SIZE> json_doc;


// ==================== start of TUNEABLE PARAMETERS ====================
// An IR detector/demodulator is connected to GPIO pin 14
// e.g. D5 on a NodeMCU board.
// Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.
// Note: GPIO 14 won't work on the ESP32-C3 as it causes the board to reboot.
#ifdef ARDUINO_ESP32C3_DEV
const uint8_t kRecvPin = PIN_RX; // 14 on a ESP32-C3 causes a boot loop.
#else                            // ARDUINO_ESP32C3_DEV
const uint8_t kRecvPin = PIN_RX;
#endif                           // ARDUINO_ESP32C3_DEV

// As this program is a special purpose capture/decoder, let us use a larger
// than normal buffer so we can handle Air Conditioner remote codes.
const uint16_t kCaptureBufferSize = 1024;

// kTimeout is the Nr. of milli-Seconds of no-more-data before we consider a
// message ended.
// This parameter is an interesting trade-off. The longer the timeout, the more
// complex a message it can capture. e.g. Some device protocols will send
// multiple message packets in quick succession, like Air Conditioner remotes.
// Air Coniditioner protocols often have a considerable gap (20-40+ms) between
// packets.
// The downside of a large timeout value is a lot of less complex protocols
// send multiple messages when the remote's button is held down. The gap between
// them is often also around 20+ms. This can result in the raw data be 2-3+
// times larger than needed as it has captured 2-3+ messages in a single
// capture. Setting a low timeout value can resolve this.
// So, choosing the best kTimeout value for your use particular case is
// quite nuanced. Good luck and happy hunting.
// NOTE: Don't exceed kMaxTimeoutMs. Typically 130ms.
#if DECODE_AC
// Some A/C units have gaps in their protocols of ~40ms. e.g. Kelvinator
// A value this large may swallow repeats of some protocols
const uint8_t kTimeout = 50;
#else  // DECODE_AC
// Suits most messages, while not swallowing many repeats.
const uint8_t kTimeout = 15;
#endif // DECODE_AC
// Alternatives:
// const uint8_t kTimeout = 90;
// Suits messages with big gaps like XMP-1 & some aircon units, but can
// accidentally swallow repeated messages in the rawData[] output.
//
// const uint8_t kTimeout = kMaxTimeoutMs;
// This will set it to our currently allowed maximum.
// Values this high are problematic because it is roughly the typical boundary
// where most messages repeat.
// e.g. It will stop decoding a message and start sending it to serial at
//      precisely the time when the next message is likely to be transmitted,
//      and may miss it.

// Set the smallest sized "UNKNOWN" message packets we actually care about.
// This value helps reduce the false-positive detection rate of IR background
// noise as real messages. The chances of background IR noise getting detected
// as a message increases with the length of the kTimeout value. (See above)
// The downside of setting this message too large is you can miss some valid
// short messages for protocols that this library doesn't yet decode.
//
// Set higher if you get lots of random short UNKNOWN messages when nothing
// should be sending a message.
// Set lower if you are sure your setup is working, but it doesn't see messages
// from your device. (e.g. Other IR remotes work.)
// NOTE: Set this value very high to effectively turn off UNKNOWN detection.
const uint8_t kMinUnknownSize = 12;

// How much percentage lee way do we give to incoming signals in order to match
// it?
// e.g. +/- 25% (default) to an expected value of 500 would mean matching a
//      value between 375 & 625 inclusive.
// Note: Default is 25(%). Going to a value >= 50(%) will cause some protocols
//       to no longer match correctly. In normal situations you probably do not
//       need to adjust this value. Typically that's when the library detects
//       your remote's message some of the time, but not all of the time.
const uint8_t kTolerancePercentage = kTolerance; // kTolerance is normally 25%

// Legacy (No longer supported!)
//
// Change to `true` if you miss/need the old "Raw Timing[]" display.
#define LEGACY_TIMING_INFO false
// ==================== end of TUNEABLE PARAMETERS ====================

// Use turn on the save buffer feature for more complete capture coverage.
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results results;




// ICACHE_RAM_ATTR IRAM_ATTR
void IRAM_ATTR attInterruptFallPIN();
void IRAM_ATTR attInterruptRisePIN();


// Mudar Modo de Operação
void TrocarModoOperacao(const char* cz_modo);

// Limpar EEPROM
void LimpaDadosDispositivo();

// Limpara ArquivoSPIFFS com pin_sttate, DT e Array Alive 
void ResetSPIFFS();

// Funçaão para ler spiffs 
//void LerSPIFFS(data2SPIFFS ddv[], const char* nome_arquivo);

// Funçaão para escrever spiffs
//void EscreverSPIFFS(data2SPIFFS ddv[], const char* nome_arquivo);

// Limpar configurações de Temporizadores
void ResetTimers();

// CallBack dados Enviados ESP-NOW
void callback_tx_esp_now(uint8_t *mac_addr, uint8_t status);  // Para ESP8266
//void callback_tx_esp_now(const uint8_t *mac_addr, esp_now_send_status_t status); // Para ESP32

// CallBack dados Recebidos ESP-NOW
void callback_rx_esp_now(uint8_t * mac, uint8_t *incomingData, uint8_t len); // Para ESP8266
//void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len);// Para ESP32


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

// Função para tratar Eventos (ws)
void configuraEventos();

// Função para lerr código IR
// TODO: Mudar String por cnst char*
//String ReadCodigoIR();


// troca o estado de PIN e guarda dados (estado, data e hora) na SPIFFS
//void TogglePIN();

// Consulta de Timers 
void ConsultarTimer(const char* nome_arquivo, uint8_t index);

// Configuracão de timers 
void ConfigurarTimer(const char* cz_json, const char* nome_arquivo);

// Ler Código IR
void lerCodgoIR(decode_results *results);


// Verifica se a tecla indicada está programada
boolean verificaTecla(const char* arquivoSPIFFS, const char* nome_ctrl_remoto, const char* nome_tecla);

// Eliminar linha num arquivo SPIFFS
//void removeLineFromFile(const char* filename, const char* lineToRemove);
void removeLineFromFile(const char* inputFilename, const char* outputFilename, uint8_t lineNumber);


// Enviar comando para alterar estado do PIN do Dispositivo PASSIVO 
void mudarEstadoPINPassivo(uint8_t estado, uint32_t dev_id);

// Aumentar a Potencia de Transmissao do WiFi
void aumentarPotenciaTxWiFi();

// Processar dados recebidos via Telne
#if TELNET == 1
  void LeerTelnet() ;
#endif


/////////////
// setup() //
/////////////
void setup() { 
  
   // Init Serial Monitor
  Serial.begin(115200);

  #if SWAP_UART == 1
    Serial.swap(); // Para alguns Wemos D1 Mini GPIO15(D8,TX) and GPIO13(D7,RX)          
  #endif

  while(!Serial);
  

  // Configurar PIN's
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);    
  pinMode(PIN_RX, INPUT);  
  pinMode(PIN_TX, OUTPUT);   
  pinMode(PIN_LED_READ, OUTPUT);      
  
 
  // Set pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptFallPIN, FALLING);  

  // Inicializa EEPROM
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(EEPROM_START_ADDR, data2eeprom);


  // Toma o ID do dev e adiciona ao mesmo o TIPO de dev correspondente
  BOARD_ID = ESP.getChipId();
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
  }  


  if (!LittleFS.exists(FILE_DATA_SPIFFS)) {
    imprime("ERRO: O arquivo: \"");
    imprime(FILE_DATA_SPIFFS);
    imprimeln("\" Não existe!!!");

    File arquivoA = LittleFS.open(FILE_DATA_SPIFFS, "w");
    arquivoA.close();
  }
  else{
    File arquivo = LittleFS.open(FILE_DATA_SPIFFS, "r");
    imprime("Tamano do atquivo SPIFFS: ");
    imprime(FILE_DATA_SPIFFS);
    imprime(" ");
    imprime((size_t)arquivo.size());
    imprimeln(" bytes.");
    arquivo.close(); 
  }



   // Se não estiver configurado
  if( strcmp(data2eeprom.CONFIGURADO, "OK") != 0 ){
    LimpaDadosDispositivo();
    ResetTimers();
    ResetSPIFFS();
    ESP.restart();
  }

/*#if DEBUG == 1
  FSInfo fs_info;
  LittleFS.info(fs_info);
  char msg[64];
  sprintf(msg, "\nSPIFFS Disponível: %i bytes\nSPIFFS Usada: %i bytes\n\n",fs_info.totalBytes, fs_info.usedBytes);
  imprimeln(msg);
#endif*/

 
  // Checa o modo de operação
  if ( strcmp(data2eeprom.Modo, "OTA") == 0)  {

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
    AsyncElegantOTA.begin(&server); 


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

       
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
      const char* dta = ("data:image/x-icon;base64,AAAB AAAAA");
      request->send(200, "image/x-icon", dta);        
    });


    server.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
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


     server.on("/chmod", HTTP_GET, [](AsyncWebServerRequest * request) {
      data2EEPROM data2eeprom = {0};
      EEPROM.get(EEPROM_START_ADDR, data2eeprom);
      if( strcmp(data2eeprom.Modo, "OTA") == 0 ){
        TrocarModoOperacao("OPRA"); // TODO: Voltar no modo anterior  
      } 
      else{
        TrocarModoOperacao("OTA");   
      }
    });


    server.on("/info", HTTP_GET, [](AsyncWebServerRequest * request) {
     
      File arquivo = LittleFS.open(FILE_DATA_SPIFFS, "r");
      uint32_t f_size = (size_t)arquivo.size();
      arquivo.close();

      //size_t fileSizeKB = f_size / 1024;

      //Memória Flash Total:
      uint32_t flashSize = ESP.getFlashChipRealSize();      
      flashSize /= 1024;

       //Memória RAM Total:
      uint32_t ramSize = ESP.getFreeContStack();     
      ramSize /= 1024;
      // Obter a quantidade de memória livre
      uint32_t freeHeap = ESP.getFreeHeap();
      // Calcular a quantidade de memória usada
      //uint32_t usedMemory = ramSize - freeHeap;
      // Converter para kilobytes
      //uint32_t usedMemoryKB = usedMemory / 1024;
      freeHeap /= 1024;

      FSInfo fs_info;
      LittleFS.info(fs_info);

      // Obter o tamanho total do sistema de arquivos LittleFS
      uint32_t totalSize = fs_info.totalBytes;

      // Obter o espaço livre disponível no sistema de arquivos LittleFS
      uint32_t freeSpace = fs_info.totalBytes - fs_info.usedBytes;
      uint32_t usedSPIFFSSpaceKB = fs_info.usedBytes / 1024;

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

    // Exibir/Baixar arquivo SPIFFS
    server.on("/spiffs", HTTP_GET, [](AsyncWebServerRequest * request) {
      boolean download = false;
      if (LittleFS.exists(FILE_DATA_SPIFFS)) {
        File file = LittleFS.open(FILE_DATA_SPIFFS, "r");
        AsyncWebServerResponse *response = request->beginResponse(file, FILE_DATA_SPIFFS, String(), download);
        request->send(response);
        file.close();
      } else {
        request->send(404, "text/plain", "Arquivo não encontrado!");
      } 
    });

    // Processa comandos recebidos através de requisições HTTP/GET 
      server.on("/cm", HTTP_GET, [](AsyncWebServerRequest * request) {

        //char cpy_cz_json[JSON_DOC_SIZE];
        json_doc.clear();
        char cz_json[JSON_DOC_SIZE];       
      
        if (request->hasParam(CMND)) {
          strncpy(cz_json, request->getParam(CMND)->value().c_str(), sizeof(cz_json));
          cz_json[strlen(cz_json)] = '\0';
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
      server.begin();  
      cont_alive = millis();

      // Inicializa Telnet  
      #if TELNET == 1
        TelnetStream.begin();
      #endif

  }
  else if(strcmp(data2eeprom.Modo, "OPR") == 0){
    digitalWrite(PIN_LED, HIGH);
    // TODO: Implementação futura
    // Restabelecer estado do PIN 
   

    // Initializing the ESP-NOW
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Init ESP-NOW
    if (esp_now_init() != 0) {
      imprimeln(F("Error initializing ESP-NOW"));
      return;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);  
    esp_now_register_send_cb(callback_tx_esp_now);
    esp_now_register_recv_cb(callback_rx_esp_now);

    cont_alive = millis();
    cont_exec_timer = millis();
    cont_send_alive = millis();
  }

  // Configura Eventos
  configuraEventos();  

  // Aumenta Potencia de Tx do WiFi
  aumentarPotenciaTxWiFi(); 
  
} // FIM de setup()




/////////////
//  loop() //
/////////////
void loop() { 

   // Ler código IR
  if (irrecv.decode(&results) && read_ir_code == true) {  // Verifica se há um código IR recebido
   
    lerCodgoIR(&results); 
  
  }




   // Checa o modo de operação
  if ( strcmp(data2eeprom.Modo, "OTA") == 0)  {
    // Tratar OTA
    //ArduinoOTA.handle();

     // Piscar o LED cada X tempo
    if( (millis() - ctrl_flash_led) >= 200){
      digitalWrite(PIN_LED, !digitalRead(PIN_LED));
      ctrl_flash_led = millis();

      // TODO: Substituir eventos por ws
      /*char cz_json[JSON_DOC_SIZE];
      sprintf(cz_json, "{\"Nome\":\"%s\",\"MAC\":\"%s\"}", data2eeprom.nome, WiFi.macAddress().c_str());
      eventos.send(cz_json, "mac_addr", millis());*/
    }  

    #if TELNET == 1
      LeerTelnet();  
      delay(2);
    #endif  

  }
  else if(strcmp(data2eeprom.Modo, "OPR") == 0 && read_ir_code == false){
      digitalWrite(PIN_LED, HIGH);
     // Verificar se o servidor está ligado.
    if( (millis() - ctrl_server_alive)/1000 >= TMP_CTRL_SERVER_ALIVE ){
      // Entra em modo de emparelhamento.  
      strncpy(pairingStatus, "PRT", sizeof(pairingStatus));
      //imprimeln(pairingStatus); 
      ctrl_server_alive = millis();
    }

  
    // auto emparelhamento
    if ( strcmp(autoPairing(), "PAIR_PAIRED") == 0) { 

      // Envia msg alive cada X tempo.
      if( (millis() - cont_alive)/1000 >= TEMPO_ENVIAR_ALIVE){
          
        cont_alive = millis();  

        // Correcção da Data
        if (year() < YEAR_COMPARE){
          char czjson[JSON_DOC_SIZE]; 
          strcpy(czjson, CriarJSON("", "GDT", WiFi.macAddress().c_str(), BOARD_ID, 2, WiFi.channel()));
          //czjson[strlen(czjson)] = '\0';
          esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));
        }


        // Checar Timer cada X tempo
        if( (millis() - cont_exec_timer)/1000 > 1 ){
          cont_exec_timer = millis();
          ExecutarTimer(FILE_DATA_SPIFFS);       
        }
        
      }  
    }


    // Envia msg alive cada X tempo.
    if( (millis() - cont_send_alive)/1000 >= TEMPO_ENVIAR_ALIVE ){    
      
      char czjson[JSON_DOC_SIZE];   

      uint8_t estado = 2;
        
      EEPROM.get(EEPROM_START_ADDR, data2eeprom);  

      // Envia resposta para actualização de estado nas páginas web.
     
      char cz_dt[DT_STR_SIZE];
      ObterDTActual(cz_dt);
      strcpy(czjson, CriarJSON(cz_dt, "SAC", data2eeprom.nome, BOARD_ID, estado, WiFi.channel()));
      //czjson[strlen(czjson)] = '\0';
      esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));
      
      cont_send_alive = millis();       
    }    

  }
  else if(strcmp(data2eeprom.Modo, "OPR") == 0 && read_ir_code == true){
     // Piscar o LED cada X tempo
    if( (millis() - ctrl_flash_led) >= 200){
      digitalWrite(PIN_LED, !digitalRead(PIN_LED));
      ctrl_flash_led = millis();
    }    
  }
 
}// FIM do loop()



/////////////////////////////////////////
// Atecnção à interrupção do PIN GPIO0 //
/////////////////////////////////////////
// ICACHE_RAM_ATTR IRAM_ATTR
void IRAM_ATTR attInterruptFallPIN() {  
  noInterrupts();
  ctl_btn_pulso = millis();
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptRisePIN, RISING);  
  interrupts();
}


/////////////////////////////////////////
// Atecnção à interrupção do PIN GPIO0 //
/////////////////////////////////////////
// ICACHE_RAM_ATTR IRAM_ATTR
void IRAM_ATTR attInterruptRisePIN(){
  noInterrupts();
  uint32_t tmp = ((millis() - ctl_btn_pulso));    
  if( tmp >= 3000 ){
    if( strcmp(data2eeprom.Modo, "OPR") == 0 ){      
      TrocarModoOperacao("OTA"); // TODO: Voltar no modo anterior  
    }
    else if( strcmp(data2eeprom.Modo, "OTA") == 0 ){     
      TrocarModoOperacao("OPR"); // TODO: Voltar no modo anterior  
    }
    
  } 
  else if( tmp < 3000 && tmp >= 250 && strcmp(data2eeprom.Modo, "OPR") == 0){
   
    /*char cz_dt[DT_STR_SIZE];
    char czjson[JSON_DOC_SIZE];
    data2EEPROM data2eeprom = {0};
    */

    
    
    /*Serial.println("Enviando código IR...");
    uint64_t hex_code = (uint64_t) strtoull("80E0E0", 0, 16);
    irsend.sendMidea24(hex_code); // Envia o código IR
    delay(1000);                  // Aguarda 1 segundo antes de enviar novamente
    */


    /*InfraRed ir_code(PIN_RX, PIN_TX, PIN_LED_READ);
    uint16_t rawDataOnOff[99] = {9008, 4584,  514, 1718,  512, 604,  510, 602,  512, 602,  512, 602,  512, 602,  512, 602,  512, 602,  514, 602,  564, 1664,  542, 1688,  568, 1662,  564, 1666,  568, 1662,  568, 1660,  570, 1638,  592, 1658,  570, 1660,  572, 542,  570, 544,  568, 546,  572, 544,  568, 548,  570, 546,  566, 546,  572, 544,  568, 1660,  570, 1660,  568, 1660,  570, 1660,  568, 1654,  574, 1662,  566, 1664,  512, 1716,  514, 602,  510, 604,  512, 604,  508, 604,  510, 604,  510, 604,  510, 602,  512, 604,  512, 1718,  514, 1716,  512, 1718,  510, 1716,  514, 1718,  512, 1718,  510};
    ir_code.EnviarCodigoIR(&rawDataOnOff[0], 99, freq_khz); */

    /*uint16_t rawSignal[] = {9054, 4550,  550, 1684,  548, 606,  510, 606,  508, 606,  508, 606,  508, 606,  510, 606,  508, 606,  510, 604,  510, 1682,  548, 1682,  548, 1680,  550, 1682,  548, 1680,  552, 1682,  548, 1682,  548, 1682,  548, 1684,  548, 606,  508, 1682,  548, 570,  546, 606,  510, 604,  510, 606,  508, 606,  510, 606,  510, 1684,  548, 606,  508, 1684,  548, 1682,  548, 1682,  548, 1682,  548, 1682,  548, 1682,  548, 606,  508, 1682,  548, 570,  544, 606,  508, 606,  508, 606,  510, 606,  510, 606,  510, 1682,  548, 606,  510, 1676,  548, 1688,  548, 1682,  548, 1682,  548};
    //uint16_t rawSignal[] = {4526,2276,273,842,273,303,254,285,273,302,254,303,254,285,273,303,255,285,272,303,255,840,275,841,274,842,273,841,274,841,274,842,273,842,273,842,274,842,273,285,272,841,274,303,254,287,270,303,254,303,254,303,254,286,271,841,274,303,254,841,274,841,274,842,274,842,273,842,274,841,274,303,254,842,273,286,272,285,272,303,255,285,272,286,271,286,271,842,273,303,254,841,274,842,273,841,274,842,273};
    uint16_t signalLen = sizeof(rawSignal) / sizeof(rawSignal[0]);
    irsend.sendRaw(rawSignal, signalLen, freq_khz);*/




    /*ObterDTActual(cz_dt);
    EEPROM.get(EEPROM_START_ADDR, data2eeprom);
    LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);

    //strncpy(data2spiffs.DT, cz_dt, sizeof(cz_dt));
    char nome[MAX_NAME_LENGTH];
    strncpy(nome, data2eeprom.nome, sizeof(nome));
    EscreverSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);

    uint8_t pin_state = 2;   
    // Envia resposta para actualização de estado nas páginas web.
    strcpy(czjson, CriarJSON(cz_dt, "TGA", nome, BOARD_ID, pin_state, WiFi.channel()));
    //imprimeln(czjson);
    esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));*/

  }

  ctl_btn_pulso = millis();
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptFallPIN, FALLING);  

}




////////////////////////////////
// Função para tratar Eventos //
////////////////////////////////
void configuraEventos() {
  eventos.onConnect([](AsyncEventSourceClient * client) {
    if (client->lastId()) {
      //imprimef("Conexões Clinte. Id: %u\n", client->lastId());
    }

    // and set reconnect delay to 1 second
    client->send("(Re)Conectado, ao Servidor ESP-XX", NULL, millis(), 1000);    
  });

  server.addHandler(&eventos);
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
      QTD_BOARDS,                   // MAX_CONNECTIONS
      false                         // true -> rede_default (192.168.4.1/24); false -> Rede definida
    };

    WiFi.softAPConfig(dados_rede.ip, dados_rede.gateway, dados_rede.subnet);
    WiFi.softAP(_ssid, _pwd, dados_rede.CHANNEL, dados_rede.HIDE_SSID, dados_rede.MAX_CONNECTIONS);   
    
  }
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
  strncpy(data2eeprom.CONFIGURADO, "OK", sizeof(data2eeprom.CONFIGURADO));
  data2eeprom.CONFIGURADO[strlen(data2eeprom.CONFIGURADO)] = '\0';
  memset(data2eeprom.ip, 0, sizeof(data2eeprom.ip));
  memset(data2eeprom.gateway, 0, sizeof(data2eeprom.gateway));
  memset(data2eeprom.subnet, 0, sizeof(data2eeprom.subnet));
  memset(data2eeprom.dns1, 0, sizeof(data2eeprom.dns1));
  memset(data2eeprom.dns2, 0, sizeof(data2eeprom.dns2)); 
  //strncpy(data2eeprom.SOCKSERVERADDR, "", sizeof(data2eeprom.SOCKSERVERADDR));
  //data2eeprom.SOCKSERVERPORT = 0;
  data2eeprom.wifi_mode = WIFI_AP_STA;

  data2eeprom.TIPO = HUB_IR; 
 
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


///////////////////////////////////////////////////////////
// Conforma a data e a hora numa stiring (H:mm:ss d m Y) //
///////////////////////////////////////////////////////////
void ObterDTActual(char* cz_dt){  
  //sprintf(cz_data_hora, "%02d:%02d:%02d %02d/%02d/%d", hour(), minute(), second(), day(), month(), year());
  sprintf(cz_dt, "%d:%02d:%02d, %d/%d/%d", hour(), minute(), second(), day(), month(), year()); 
}




////////////////////////////////////////////////////////////
// Limpara ArquivoSPIFFS com pin_sttate, DT e Array Alive //
////////////////////////////////////////////////////////////
void ResetSPIFFS(){
   LittleFS.remove(FILE_DATA_SPIFFS);
   LittleFS.open(FILE_DATA_SPIFFS, "w"); 
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
void callback_tx_esp_now(uint8_t *mac_addr, uint8_t status) {  // Para ESP8266
  //void callback_tx_esp_now(const uint8_t *mac_addr, esp_now_send_status_t status) {  // Para ESP32

  /*char macStr[MAC_STR_SIZE];
  imprime("Destino: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  imprime(macStr);
  imprime(" Status:\t");
  imprime(status == ESP_NOW_SEND_SUCCESS ? "Successo." : "Falha: ");
  */
}



//////////////////////////////////////////////
// Callback, ESP-NOW, when data is received //
//////////////////////////////////////////////
void callback_rx_esp_now(uint8_t * mac, uint8_t *incomingData, uint8_t len) { // Para ESP8266
  //void callback_rx_esp_now(const uint8_t * mac, const uint8_t *incomingData, int len){ // Para ESP32
  
  //data2SPIFFS data2spiffs = {0};
  data2EEPROM data2eeprom = {0}; 
  char czjson[JSON_DOC_SIZE];
  //StaticJsonDocument <JSON_DOC_SIZE> dadosJson; 
  json_doc.clear();
  deserializeJson(json_doc, (const char*)incomingData);

  EEPROM.get(EEPROM_START_ADDR, data2eeprom);
  //LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);

  if (json_doc.isNull()){
    imprimeln("JSON inválido/nulo!");
    return;
  }

  //imprimeln((const char*)incomingData);

  const char* cz_tipo_msg = json_doc["MSG"];
  const char* cz_cmd = json_doc["CMD"];
  const char* cz_mac = json_doc["MAC"];  
  uint32_t iz_id_dev = json_doc["ID"];
  //uint8_t iz_pin_st = json_doc["PST"];
  uint8_t iz_canal_wifi = json_doc["WCH"];
  id_cliente_ws = iz_canal_wifi;

  uint8_t* uz_mac = converteMacString2Byte(cz_mac);

  if( strcmp(cz_tipo_msg, "PNG") == 0){
    esp_now_add_peer(uz_mac, ESP_NOW_ROLE_COMBO, iz_canal_wifi, NULL, 0);
    strncpy(pairingStatus, "PAIR_PAIRED", sizeof(pairingStatus) );
    memcpy(seve_server_mac, uz_mac, 6);
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
    if(strcmp(cz_cmd, "TGL") == 0 && iz_id_dev == BOARD_ID){      
      // TODO: Implementação futura
      //TogglePIN();    
    }
    else if(strcmp(cz_cmd, "TGG") == 0 && iz_id_dev == BOARD_ID){      
      //digitalWrite(PIN_LED, iz_pin_st);
    }
    else if(strcmp(cz_cmd, "GRLOTA") == 0 && data2eeprom.TIPO == PASSIVO){
      TrocarModoOperacao("OTA");
    }
  }
  else if(strcmp(cz_cmd, "UPT") == 0 ){   
    // Correcção da Data
    /*if (year() < YEAR_COMPARE){     
      setTime((uint32_t)json_doc["MSG"]);      
    }*/   

    // Envia resposta para actualização de estado nas páginas web (código reaproveitado).    
    /*uint8_t pin_state = digitalRead(PIN_RELAY);
#if INVERT_PIN == 1
    pin_state = !digitalRead(PIN_RELAY);
#endif*/

    uint8_t pin_state = 2;
    //data2spiffs.DT
    char cz_dt[DT_STR_SIZE];
    ObterDTActual(cz_dt);
    strcpy(czjson, CriarJSON(cz_dt, "TGA", data2eeprom.nome, BOARD_ID, pin_state, WiFi.channel())); 
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
  else if( strcmp(cz_cmd, "STM") == 0 && iz_id_dev == BOARD_ID){      
    ConfigurarTimer(cz_tipo_msg, FILE_DATA_SPIFFS); 
    //imprimeln(cz_tipo_msg);   
  }
  else if( strcmp(cz_cmd, "RSD") == 0 && iz_id_dev == BOARD_ID){       
     strncpy(data2eeprom.CONFIGURADO, "NO", sizeof(data2eeprom.CONFIGURADO));
     EEPROM.put(EEPROM_START_ADDR, data2eeprom);
     EEPROM.commit();
     delay(10);
     ESP.restart(); 
  }
  else if( strcmp(cz_cmd, "ADS") == 0 && iz_id_dev == BOARD_ID){
    char cz_dt_tmr[DT_STR_SIZE];
    sprintf(cz_dt_tmr, "%s:%s:%s, %d/%d/%d", "__", "__", "__", day(), month(), year()); 
   
    strcpy(czjson, CriarJSON(cz_dt_tmr, "SAC", data2eeprom.nome, BOARD_ID, 2, 2));
    //czjson[strlen(czjson)] = '\0';      
    esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));
  }
  else if( strcmp(cz_cmd, "RNM") == 0 && iz_id_dev == BOARD_ID){      
    //imprimeln((const char*)incomingData);
    strncpy(data2eeprom.nome, cz_mac, sizeof(data2eeprom.nome));
    EEPROM.put(EEPROM_START_ADDR, data2eeprom);
    EEPROM.commit();
    delay(10);


    /*uint8_t pin_state = digitalRead(PIN_RELAY);
#if INVERT_PIN == 1
    pin_state = !digitalRead(PIN_RELAY);
#endif*/

    uint8_t pin_state = 2;

    //StaticJsonDocument <JSON_DOC_SIZE> json_doc; 
    json_doc.clear();
    json_doc["MSG"] = "";//data2spiffs.DT;  
    json_doc["CMD"] = "RAN";
    json_doc["ID"] = BOARD_ID;  
    json_doc["PST"] = pin_state;
    json_doc["WCH"] = 0;
    json_doc["MAC"] = cz_mac;
    serializeJson(json_doc, json_array);
    json_array[strlen(json_array)] = '\0';
     
    esp_now_send(seve_server_mac, (uint8_t *)json_array, sizeof(json_array)); 
  }
  else if( strcmp(cz_cmd, "ENABLE_OTA") == 0 && iz_id_dev == BOARD_ID){      
    //Serial.println((const char*)incomingData);
    TrocarModoOperacao("OTA");
  }
  else if( strcmp(cz_cmd, "SIR") == 0 && iz_id_dev == BOARD_ID){

    strncpy(nome_ctrl_remoto, cz_tipo_msg, NOME_CTRL_REMOTO_SIZE); // Comando
    strncpy(nome_tecla, cz_mac, NOME_TCLA_SIZE); // Tecla 

    // (*J*) JSON_DOC_RAW_SIZE deve ser o suficientemente grande
    DynamicJsonDocument json_doc(JSON_DOC_RAW_SIZE);
    File arquivoR = LittleFS.open(FILE_DATA_SPIFFS, "r");
    boolean registada = false;
    json_doc.clear();
    
    while (arquivoR.available()){
      String line = arquivoR.readStringUntil('\n');
      //if(line.length() <= 0 ) return;
      deserializeJson(json_doc, line);
      const char* comando = json_doc["COMANDO"];
      const char* tecla = json_doc["TECLA"];
      int8_t tipo = json_doc["TIPO"];
      uint8_t bytes = json_doc["BYTES"];
     

      if (strcmp(comando, nome_ctrl_remoto) == 0 && strcmp(tecla, nome_tecla) == 0){
       
        if(tipo == UNKNOWN){

          String codigo = json_doc["CODIGO"];

          // Converter cadeia de caracteres de números em array   
          unsigned int data_num = 0;
            uint16_t data[bytes];
            // loop as long as a comma is found in the string
            while(codigo.indexOf(",") != -1 ){
              // take the substring from the start to the first occurence of a comma, convert it to int and save it in the array
              data[ data_num ] = codigo.substring(0, codigo.indexOf(",")).toInt();
              data_num++; // increment our data counter
              //cut the data string after the first occurence of a comma
              codigo = codigo.substring(codigo.indexOf(",") + 1);
            }
            // get the last value out of the string, which as no more commas in it
            data[ data_num ] = codigo.toInt();            
            //uint16_t dataLen = sizeof(data) / sizeof(data[0]);
            irsend.sendRaw(data, bytes, freq_khz);
        }
        else{
          const char* codigo = json_doc["CODIGO"];
          uint64_t hex_code = (uint64_t) strtoull(codigo, 0, 16);
          // TODO: Rever esta lógica
          if( tipo == MIDEA24){
            irsend.sendMidea24(hex_code);
          }
          else if( tipo == MIDEA){
            irsend.sendMidea(hex_code);
          }
          else if(tipo == RC5){
            irsend.sendRC5(hex_code);
          }
          else if(tipo == RC6){
            irsend.sendRC6(hex_code);
          }
          else if(tipo == NEC){
            irsend.sendNEC(hex_code);
          }
          else if(tipo == SONY){
            irsend.sendSony(hex_code);
          }
          else if(tipo == SAMSUNG){
            irsend.sendSAMSUNG(hex_code);
          }
          else if(tipo == JVC){
            irsend.sendJVC(hex_code);
          }
           else if(tipo == WHYNTER){
            irsend.sendWhynter(hex_code);
          }
          else if(tipo == AIWA_RC_T501){
            irsend.sendAiwaRCT501(hex_code);
          }
          else if(tipo == LG){
            irsend.sendLG(hex_code);
          }
          else if(tipo == MITSUBISHI){
            irsend.sendMitsubishi(hex_code);
          }
          else if(tipo == DISH){
            irsend.sendDISH(hex_code);
          }
          else if(tipo == DISH){
            irsend.sendDISH(hex_code);
          }  
          else if(tipo == SYMPHONY){
            irsend.sendSymphony(hex_code);
          }        
          
        }
        
        registada = true;
        break;
      }
      else{
         registada = false;
      }
      
    }
   
    arquivoR.close();

    if(registada == false){
  
      json_doc.clear();
      json_doc["MSG"] = nome_tecla;
      json_doc["CMD"] = "KNP";
      json_doc["ID"] = BOARD_ID;
      json_doc["PST"] = 2;
      json_doc["WCH"] = id_cliente_ws;
      json_doc["MAC"] = "";
      serializeJson(json_doc, json_array);
      esp_now_send(seve_server_mac, (uint8_t *)json_array, sizeof(json_array));

      //imprimeln(json_array);
    }
    
  }
  else if( strcmp(cz_cmd, "RIR") == 0 && iz_id_dev == BOARD_ID){  

    const char* comando = json_doc["MSG"];
    const char* tecla = json_doc["MAC"];    
     
    strncpy(nome_ctrl_remoto, comando, NOME_CTRL_REMOTO_SIZE); // Comando
    strncpy(nome_tecla, tecla, NOME_TCLA_SIZE); // Tecla 

    // Verifica se a tecla já está programada
    boolean teclaProgramada = verificaTecla(FILE_DATA_SPIFFS, nome_ctrl_remoto, nome_tecla);
  
    if(teclaProgramada == true){
      // Envia, ao Servidor, mensagem Key Already Programed
      //StaticJsonDocument<JSON_DOC_SIZE> json_doc;
      json_doc.clear();
      // Envia, ao Servidor, mensagem Key Already Programed
      json_doc["MSG"] = nome_tecla;
      json_doc["CMD"] = "KAP";
      json_doc["ID"] = BOARD_ID;
      json_doc["PST"] = 2;
      json_doc["WCH"] = id_cliente_ws;
      json_doc["MAC"] = "";
      serializeJson(json_doc, json_array);
      json_array[strlen(json_array)] = '\0';
      esp_now_send(seve_server_mac, (uint8_t *)json_array, sizeof(json_array));
      // imprimeln(F("A tecla indicada já está programada!"));

      return;
    }

        // Inicializa o receptor IR
    #if DECODE_HASH
        // Ignore messages with less than minimum on or off pulses.
        irrecv.setUnknownThreshold(kMinUnknownSize);
    #endif // DECODE_HASH

    irrecv.setTolerance(kTolerancePercentage); // Override the default tolerance.
    irrecv.enableIRIn();                       // Start the receiver    
    read_ir_code = true;
    ctrl_flash_led = millis();

    //strncpy(nome_ctrl_remoto, cz_tipo_msg, NOME_CTRL_REMOTO_SIZE); // Comando
    //strncpy(nome_tecla, cz_mac, NOME_TCLA_SIZE); // Tecla
    //imprimeln((const char*)incomingData);
  }
  else if( strcmp(cz_cmd, "CKS") == 0 && iz_id_dev == BOARD_ID){  

    const char* comando = json_doc["MSG"];
    const char* tecla = json_doc["MAC"];
    char key_state[4];
     
    strncpy(nome_ctrl_remoto, comando, NOME_CTRL_REMOTO_SIZE); // Comando
    strncpy(nome_tecla, tecla, NOME_TCLA_SIZE); // Tecla 

    // Verifica se a tecla já está programada
    boolean teclaProgramada = verificaTecla(FILE_DATA_SPIFFS, nome_ctrl_remoto, nome_tecla);

    if(teclaProgramada == true){
      strcpy(key_state, "KAP");
    }
    else{
      strcpy(key_state, "KNP");
    }

    // Envia, ao Servidor, mensagem Key Already Programed
    //StaticJsonDocument<JSON_DOC_SIZE> json_doc;
    json_doc.clear();
    // Envia, ao Servidor, mensagem Key Already Programed
    json_doc["MSG"] = nome_tecla;
    json_doc["CMD"] = key_state;
    json_doc["ID"] = BOARD_ID;
    json_doc["PST"] = 2;
    json_doc["WCH"] = id_cliente_ws;
    json_doc["MAC"] = "";
    serializeJson(json_doc, json_array);
    json_array[strlen(json_array)] = '\0';
    esp_now_send(seve_server_mac, (uint8_t *)json_array, sizeof(json_array));
    // imprimeln(F("A tecla indicada já está programada!"));
    //imprimeln(json_array);
  }
  else if( strcmp(cz_cmd, "SRI") == 0 && iz_id_dev == BOARD_ID){         
    irrecv.disableIRIn();                       // Stop the receiver    
    read_ir_code = false;
    ctrl_flash_led = 0;      
    //Serial.println((const char*)incomingData);
  }
  else if( strcmp(cz_cmd, "UPK") == 0 && iz_id_dev == BOARD_ID){ 

    const char* comando = json_doc["MSG"];
    const char* tecla = json_doc["MAC"];
                
    strncpy(nome_ctrl_remoto, comando, NOME_CTRL_REMOTO_SIZE); // Comando
    strncpy(nome_tecla, tecla, NOME_TCLA_SIZE); // Tecla 

    //StaticJsonDocument<JSON_DOC_SIZE> json_doc;
    json_doc.clear();
    File arquivoR = LittleFS.open(FILE_DATA_SPIFFS, "r");
    int currentLine = 0;

    // lê o arquivo linha por linha e procura pela linha de interesse
    while (arquivoR.available()){
        String line = arquivoR.readStringUntil('\n');
        currentLine++;
        if ( line.indexOf(comando) != -1 &&  line.indexOf(tecla) != -1) {
          break;
        }
    }
    arquivoR.close();

    // Eliminar linha indicada
    //imprimeln(currentLine);
    removeLineFromFile(FILE_DATA_SPIFFS, "/tmp.txt", currentLine);


    // Enviar resposta ao Servidor
    json_doc["MSG"] = "";
    json_doc["CMD"] = "UKA";
    json_doc["ID"] = BOARD_ID;
    json_doc["PST"] = 2;
    json_doc["WCH"] = id_cliente_ws;
    json_doc["MAC"] = "";
    serializeJson(json_doc, json_array);
    json_array[strlen(json_array)] = '\0';
    esp_now_send(seve_server_mac, (uint8_t *)json_array, sizeof(json_array));
    
    //imprimeln(json_array);
  }
  else if( strcmp(cz_cmd, "GRC") == 0 && iz_id_dev == BOARD_ID){  

    //StaticJsonDocument<JSON_DOC_SIZE> json_doc;
    json_doc.clear();
    File arquivoR = LittleFS.open(FILE_DATA_SPIFFS, "r");
    String szComandos;
    
    if (!arquivoR) {
      Serial.println("Falha ao abrir os arquivos.");
      return;
    }
    
    while (arquivoR.available()){
      String line = arquivoR.readStringUntil('\n');   
      //imprimeln(line);          
      deserializeJson(json_doc, line);
      
      char comando[4];
      if(json_doc.containsKey("COMANDO") && !json_doc["COMANDO"].isNull()){
        strcpy(comando, json_doc["COMANDO"]);
      }
      else{
        strcpy(comando, "");
      }
     
      if ( strcmp(comando, "") != 0 && szComandos.indexOf(comando) == -1){
        szComandos += comando;
        szComandos += ",";
      }
      
    }

    arquivoR.close();
    
    if(szComandos.length() <= 0){
      return;
    }

    // Remover a última víorgula
    int lastIndex = szComandos.lastIndexOf(',');
    if (lastIndex != -1) {
      szComandos = szComandos.substring(0, lastIndex) + szComandos.substring(lastIndex + 1);
    }

    json_doc.clear();
    json_doc["MSG"] = szComandos;
    json_doc["CMD"] = "GRA";
    json_doc["ID"] = BOARD_ID;
    json_doc["PST"] = 2;
    json_doc["WCH"] = 0;
    json_doc["MAC"] = "";
    serializeJson(json_doc, json_array);
    esp_now_send(seve_server_mac, (uint8_t *)json_array, sizeof(json_array));
    
    //imprimeln(json_array); 
    
  }


  free(uz_mac);
  
}// end callback_rx_esp_now(...)



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
    WiFi.mode(WIFI_STA);
    wifi_promiscuous_enable(1);
    wifi_set_channel(channel);
    wifi_promiscuous_enable(0);
    WiFi.disconnect();

    // Init ESP-NOW
    if (esp_now_init() != 0) {
      imprimeln(F("Error initializing ESP-NOW"));
    }

    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_register_send_cb(callback_tx_esp_now);
    esp_now_register_recv_cb(callback_rx_esp_now);
    
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
           
      strcpy(czjson, CriarJSON("PAL", "", WiFi.macAddress().c_str(), BOARD_ID, 2, channel));
      //czjson[strlen(czjson)] = '\0';
      esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));  
      //imprimeln(pairingStatus); 
      ctrl_send_alive = millis();
    }
    
  }
  
  return pairingStatus;

} 


/////////////////////
//Criar dados JSON //
/////////////////////
const char* CriarJSON(const char* MSG_TYPE, const char* CMD, const char* MAC, uint32_t ID, int8_t PIN_STT, uint8_t WIFI_CH){
  
  DynamicJsonDocument json_doc(JSON_DOC_SIZE);  

  json_doc["MSG"] = MSG_TYPE;
  json_doc["CMD"] = CMD;
  json_doc["MAC"] = MAC;
  json_doc["ID"] = ID;
  json_doc["PST"] = PIN_STT;
  json_doc["WCH"] = WIFI_CH;   

  serializeJson(json_doc, json_array);

  return json_array;
}



//////////////////////////////
// Executar acções do Timer //
//////////////////////////////
void ExecutarTimer(const char* nome_arquivo) {

  // Para tirar e guardar dados EEPROM e SPIFFS
  /*data2SPIFFS data2spiffs = {0};
  data2EEPROM data2eeprom = {0};
  EEPROM.get(EEPROM_START_ADDR, data2eeprom);

  // Tira Timers da EEPROM
  timers2EEPROM tmrs[QTD_TMRS];
  EEPROM.get(EEPROM_TIMERS_ADDR, tmrs);

  for (uint8_t index = 0; index < QTD_TMRS; index++) {

    // Verficar se o dia de semana, do momento, está programado, timer activado
    if ( tmrs[index].weekDays[weekday() - 1] == '1' && tmrs[index].activar == 1 && year() > YEAR_COMPARE) {
      if (hour() == tmrs[index].horas && minute() == tmrs[index].minutos && second() <= 5) {
        digitalWrite(PIN_RELAY, tmrs[index].action);
    #if INVERT_PIN == 1
        digitalWrite(PIN_RELAY, !tmrs[index].action);
    #endif    
        digitalWrite(PIN_LED, digitalRead(PIN_RELAY)); // O inverso do PIN_RELAY
                
        char cz_dt[DT_STR_SIZE];
       
        ObterDTActual(cz_dt);        
        LerSPIFFS(&data2spiffs, nome_arquivo);

        data2spiffs.estado_pin = digitalRead(PIN_RELAY);
        strncpy(data2spiffs.DT, cz_dt, sizeof(cz_dt));
        char nome[MAX_NAME_LENGTH];
        strncpy(nome, data2eeprom.nome, sizeof(nome));

        EscreverSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);

        uint8_t pin_state = digitalRead(PIN_RELAY);
#if INVERT_PIN == 1
        pin_state = !digitalRead(PIN_RELAY);
#endif
        // Envia resposta para actualização de estado nas páginas web.
        char czjson[JSON_DOC_SIZE]; 
        strcpy(czjson, CriarJSON(cz_dt, "TGA", data2eeprom.nome, BOARD_ID, pin_state, WiFi.channel()));
        // imprimeln(czjson);
        esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));
      }
    }
  }*/

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
  json_doc.clear();
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

}


////////////////////////////
// Configuracão de timers //
////////////////////////////
void ConfigurarTimer(const char* cz_json, const char* nome_arquivo) {

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



//////////////////////////////////////////////////////////////////////////
// troca o estado de PIN e guarda dados (estado, data e hora) na SPIFFS //
//////////////////////////////////////////////////////////////////////////
void TogglePIN(){

  /*char czjson[JSON_DOC_SIZE];
  char cz_dt[DT_STR_SIZE];

  data2EEPROM data2eeprom = {0};
  EEPROM.get(EEPROM_START_ADDR, data2eeprom);

  data2SPIFFS data2spiffs = {0};
  digitalWrite(PIN_RELAY, !digitalRead(PIN_RELAY));
  digitalWrite(PIN_LED, digitalRead(PIN_RELAY)); // O inverso do PIN_RELAY

  ObterDTActual(cz_dt);

  LerSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);

  data2spiffs.estado_pin = digitalRead(PIN_RELAY);
  strncpy(data2spiffs.DT, cz_dt, sizeof(cz_dt));
  char nome[MAX_NAME_LENGTH];
  strncpy(nome, data2eeprom.nome, sizeof(nome));

  EscreverSPIFFS(&data2spiffs, FILE_DATA_SPIFFS);

  uint8_t pin_state = digitalRead(PIN_RELAY);
#if INVERT_PIN == 1
  pin_state = !digitalRead(PIN_RELAY);
#endif
  // Envia resposta para actualização de estado nas páginas web.
  strcpy(czjson, CriarJSON(cz_dt, "TGA", nome, BOARD_ID, pin_state, WiFi.channel()));
  //imprimeln(czjson);
  esp_now_send(seve_server_mac, (uint8_t *)czjson, strlen(czjson));*/

  // Envia comando para muidanca do estado do PIN do Dispositivo passivo associado  
#if IS_PASSIVE_MASTER == 1
    if(data2eeprom.TIPO != PASSIVO){
      char czjson[JSON_DOC_SIZE];
      uint8_t pin_state = digitalRead(PIN_RELAY);
      strcpy(czjson, CriarJSON("DATA", "TGG", "", PASSIVE_SLAVE_ID, pin_state, WiFi.channel()));     
      //czjson[strlen(czjson)] = '\0';
      esp_now_send(0, (uint8_t *)czjson, strlen(czjson));  
    }                  
#endif

}



///////////////////
// Ler Código IR //
///////////////////
void lerCodgoIR(decode_results *results) {
  
  /*if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  } else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");
  } else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  } else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  } else if (results->decode_type == RC5X) {
    Serial.print("Decoded RC5X: ");
  } else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  } else if (results->decode_type == RCMM) {
    Serial.print("Decoded RCMM: ");
  } else if (results->decode_type == PANASONIC) {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->address, HEX);
    Serial.print(" Value: ");
  } else if (results->decode_type == LG) {
    Serial.print("Decoded LG: ");
  } else if (results->decode_type == JVC) {
    Serial.print("Decoded JVC: ");
  } else if (results->decode_type == AIWA_RC_T501) {
    Serial.print("Decoded AIWA RC T501: ");
  } else if (results->decode_type == WHYNTER) {
    Serial.print("Decoded Whynter: ");
  } else if (results->decode_type == NIKAI) {
    Serial.print("Decoded Nikai: ");
  }
  else if (results->decode_type == MIDEA) {
    Serial.print("Decoded MIDEA: ");
  }
  else if (results->decode_type == MIDEA24) {
    Serial.print("Decoded MIDEA24: ");
  }
  else if (results->decode_type == SYMPHONY) {
    Serial.print("Decoded SYMPHONY: ");
  }
  else{
    uint8_t size_em_bytes = (results->bits/4) + 1;
    //uint8_t size_em_bits = results->bits;
    char cz_hex[size_em_bytes];
    sprintf(cz_hex, "%llX", results->value);
    int8_t tipo = results->decode_type;
    Serial.println(tipo);
  }*/

  uint8_t size_em_bytes = (results->bits/4) + 1;
  uint8_t size_em_bits = results->bits;
  char cz_hex[size_em_bytes];
  sprintf(cz_hex, "%llX", results->value);
  int8_t tipo = results->decode_type;



  // TODO: Fazer aqui as Verificações necessárias (códigos IR, tipo, etc.)
  if(tipo == UNUSED){ 
    if(ir_samples < QTD_AMOSTRAS_IR){
      ir_samples++;
    }
    else{
      //StaticJsonDocument<JSON_DOC_SIZE> json_doc;
      json_doc.clear();
      json_doc.clear();
      json_doc["MSG"] = "";
      json_doc["CMD"] = "USD";
      json_doc["ID"] = BOARD_ID;
      json_doc["PST"] = 2;
      json_doc["WCH"] = id_cliente_ws;
      json_doc["MAC"] = "";
      serializeJson(json_doc, json_array);
      json_array[strlen(json_array)] = '\0';
      esp_now_send(seve_server_mac, (uint8_t *)json_array, sizeof(json_array));

      ir_samples = 0;
      irrecv.disableIRIn(); // Stop the receiver
      read_ir_code = false;
      ctrl_flash_led = 0;
    }
  }
  else if( tipo > UNUSED && size_em_bits >= 4 ){ // KNOWN
    //StaticJsonDocument<JSON_DOC_SIZE> json_doc;
    json_doc.clear();
    StaticJsonDocument<JSON_DOC_SIZE> jsonDoc;
    String sz_json = "";
    jsonDoc["COMANDO"] = nome_ctrl_remoto;
    jsonDoc["TECLA"] = nome_tecla;
    jsonDoc["CODIGO"] = cz_hex;
    jsonDoc["BYTES"] = size_em_bytes;
    jsonDoc["TIPO"] = tipo;
    //serializeJson(jsonDoc, json_array);
    serializeJson(jsonDoc, sz_json);

    File arquivoA = LittleFS.open(FILE_DATA_SPIFFS, "a");
    arquivoA.println(sz_json);
    arquivoA.close();

    // Envia, ao Servidor, mensagem Key Programed Successfully
    json_doc["MSG"] = nome_tecla;
    json_doc["CMD"] = "KPG";
    json_doc["ID"] = BOARD_ID;
    json_doc["PST"] = 2;
    json_doc["WCH"] = id_cliente_ws;
    json_doc["MAC"] = "";
    serializeJson(json_doc, json_array);
    json_array[strlen(json_array)] = '\0';
    esp_now_send(seve_server_mac, (uint8_t *)json_array, sizeof(json_array));
    // imprimeln(F("Tecla programada com sucesso."));
  }
  else if(tipo == UNKNOWN && size_em_bits >= 4){ // UNKNOWN
    uint16_t raw_len = results->rawlen;
    if(ir_samples < QTD_AMOSTRAS_IR){
      ir_samples++;
    }
    else if(raw_len <= 150){ // max raw length 150 word (byte*2)
      String output;
      for (uint16_t i = 1; i < results->rawlen; i++) {
          uint32_t usecs;
          for (usecs = results->rawbuf[i] * kRawTick; usecs > UINT16_MAX;
              usecs -= UINT16_MAX) {
            output += uint64ToString(UINT16_MAX);
            if (i % 2)
              output += F(",0,");
            else
              output += F(",0,");
          }
          output += uint64ToString(usecs, 10);
          if (i < results->rawlen - 1)
            output += kCommaSpaceStr;            // ',' not needed on the last one
          if (i % 2 == 0) output += ' ';         // Extra if it was even.
      }
      
      //StaticJsonDocument<JSON_DOC_SIZE> json_doc;
      json_doc.clear();
      // (*J*) JSON_DOC_RAW_SIZE deve ser o suficientemente grande
      DynamicJsonDocument jsonDoc(JSON_DOC_RAW_SIZE);
      String sz_json = "";
      output.trim();
      jsonDoc["COMANDO"] = nome_ctrl_remoto;
      jsonDoc["TECLA"] = nome_tecla;
      jsonDoc["CODIGO"] = output.c_str();
      jsonDoc["BYTES"] = results->rawlen;
      jsonDoc["TIPO"] = tipo;
      serializeJson(jsonDoc, sz_json);
      Serial.println(output);

      File arquivoA = LittleFS.open(FILE_DATA_SPIFFS, "a");
      arquivoA.println(sz_json);
      arquivoA.close();

      // Envia, ao Servidor, mensagem Key Programed Successfully
      json_doc["MSG"] = nome_tecla;
      json_doc["CMD"] = "KPG";
      json_doc["ID"] = BOARD_ID;
      json_doc["PST"] = 2;
      json_doc["WCH"] = id_cliente_ws;
      json_doc["MAC"] = "";
      serializeJson(json_doc, json_array);
      json_array[strlen(json_array)] = '\0';
      esp_now_send(seve_server_mac, (uint8_t *)json_array, sizeof(json_array));
      // imprimeln(F("Tecla programada com sucesso."));
      
    }
    else{
      //Serial.println(size_em_bits);
      ir_samples = QTD_AMOSTRAS_IR - 4;
    }
  } 

}



//////////////////////////////////////////////////
// Verifica se a tecla indicada está programada //
//////////////////////////////////////////////////
boolean verificaTecla(const char* arquivoSPIFFS, const char* nome_ctrl_remoto, const char* nome_tecla){
  
  //StaticJsonDocument<JSON_DOC_SIZE> json_doc;
  json_doc.clear();
  File arquivoR = LittleFS.open(arquivoSPIFFS, "r");
  boolean teclaProgramada = false;

  // Verifica se a tecla já está programada
  while (arquivoR.available()){
    
    String line = arquivoR.readStringUntil('\n');
       
    if(line.length() > 0 ){ 
      deserializeJson(json_doc, line);
      const char* comando = json_doc["COMANDO"];
      const char* tecla = json_doc["TECLA"];
      /*const char* codigo = json_doc["CODIGO"];
      uint8_t bytes = json_doc["BYTES"];
      uint8_t tipo = json_doc["TIPO"];*/

      if( strcmp(comando, nome_ctrl_remoto) == 0 && strcmp(tecla, nome_tecla) == 0){
        teclaProgramada = true;
        break;
      }
      else{
        teclaProgramada = false;
      }
    }
   
  }
   
  arquivoR.close();

  return teclaProgramada;

}



///////////////////////////////////////
// Eliminar linha num arquivo SPIFFS //
///////////////////////////////////////
void removeLineFromFile(const char* inputFilename, const char* outputFilename, uint8_t lineNumber) {
  File inputFile = LittleFS.open(inputFilename, "r");
  File outputFile = LittleFS.open(outputFilename, "w");

  if (!inputFile || !outputFile) {
    Serial.println("Falha ao abrir os arquivos.");
    return;
  }

  uint8_t currentLine = 1;
  String line;

  while (inputFile.available()) {
    line = inputFile.readStringUntil('\n');

    // Copia todas as linhas, exceto a linha a ser removida
    if (currentLine != lineNumber) {
      outputFile.println(line);
    }
    currentLine++;
  }

  inputFile.close();
  outputFile.close();

  LittleFS.remove(inputFilename);
  LittleFS.rename(outputFilename, inputFilename);
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