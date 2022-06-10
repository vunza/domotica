
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <Time.h>
#include "creds.h"

#define OTA_Host_Name "SLAMPHER-R2"

// Configuração do pin a controlar
#define PIN_RELAY 15  //GPIO12 dio SLAMPHER -- //GPIO15 do ESP12E
#define PIN_BUTTON 0  //GPIO0 do SLAMPHER
#define PIN_LED 2    //GPIO13 do SLAMPHER   -- // GPIO2 do ESP12E


const char* CMND = "cmnd";               // Comandos recebidos por via do Javascript
String get_txt_json = "";
uint8_t start_addr = 0;
uint16_t eeprom_size = 512;
//String permisso_fechar = "false";


// Estrutura para programar Timers/Alarmes
typedef struct timers {
  uint8_t activar;
  uint8_t horas;
  uint8_t minutos;
  char weekDays[8];
  uint8_t output;
  uint8_t action;
  uint16_t end_struct; // Caracter extra necessśrio no fim da estructura
} Timers;

// Offset/posição para guardar instância da Struct no arquivo, por defeito 1;
uint8_t timerNumber = 1;


#define MAX_NAME_LENGTH 64          // Comprimento do Nome dos dispositivos (64 - 1)

// Estrutura para guardar dados do device
typedef struct dados {
  uint8_t estado_pin;
  char nome[MAX_NAME_LENGTH];
  uint16_t end_struct; // Caracter extra necessśrio no fim da estructura
} Dados;


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);



// Função para atribuir valores aos "placeholders" criados no arquivo HTML, %PIN% no caso.
String processor(const String& var) {

  /*if (var == "NM2") {
    Dados dds = {};    
    EEPROM.get(start_addr, dds);
    return  String(dds.nome);
  }
  else if (var == "NM3") {
    Dados dds = {};    
    EEPROM.get(start_addr, dds);
    return  String(dds.nome);
  }*/

  return String();
}


void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  delay(250);
  Serial.println("");

  // Configurações e inicialização do PIN
  configurarPIN(PIN_RELAY);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // Configuração do PIN_BUTTON como entrada e com pull-up
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  // Set pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), attInterruptPIN_BUTTON, RISING);


  // Inicialização da memória EEPROM
  inicilaizaEEPROM(eeprom_size);

  // Reestabelece o último estado do PIN
  restorePinState(PIN_RELAY, start_addr);

  // Inicializa Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(staticIP, gateway, subnet);
  WiFi.softAP(ap_ssid, ap_password, CHANNEL, HIDE_SSID, MAX_CONNECTIONS);
  //WiFi.softAP(const char* ssid, const char* passphrase, int channel, int ssid_hidden, int max_connection);
  server.begin();


  // Configuração da rede WiFi, para utiliazaçã com OTA em fasse de desenvolvimento
  //configuraRedeWiFi(wifi_ssid, wifi_pwd);


  // Obter hostnome, (*J*) deve ser feita depois da configutação da rede
  //NOME = obetrHostName();
  //ID = obetrHostName();
  //Serial.println(NOME);


  // Refgistar cliente
  get_txt_json = criarTextoJSON(digitalRead(PIN_RELAY));


  // Configuração e inicialição do OTA
  ArduinoOTA.setHostname(OTA_Host_Name);
  ArduinoOTA.begin();


  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }


  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });


  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to load script.js file
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/script.js", "text/javascript");
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
    String dta = F("data:image/x-icon;base64,AAAB AAAAA");
    request->send(200, "image/x-icon", dta.c_str() );
  });






  /////////////////////////////////////////////////////////////////
  // Processa comandos recebidos através de requisições HTTP/GET //
  /////////////////////////////////////////////////////////////////
  server.on("/cm", HTTP_GET, [](AsyncWebServerRequest * request) {

    String cmnd = "";

    if (request->hasParam(CMND)) {
      cmnd = request->getParam(CMND)->value();
      //Serial.println(cmnd.c_str());   // Debug
    }

    // Mudança do estado do PIN
    if ( cmnd.indexOf("Toggle") >= 0) {

      digitalWrite(PIN_RELAY, !digitalRead(PIN_RELAY));
      digitalWrite(PIN_LED, digitalRead(PIN_RELAY));

      get_txt_json = criarTextoJSON(digitalRead(PIN_RELAY));

      guardarEstadoEEPROM(start_addr, digitalRead(PIN_RELAY));

      // Reenvia o texto JSON
      request->send_P(200, "text/plain", get_txt_json.c_str());
    }
    else if ( cmnd.indexOf("STATE") >= 0) {// Actualizar Estado do PIN

      // Envia como resposta a data e hora actualizada
      request->send_P(200, "text/plain", criarTextoJSON(digitalRead(PIN_RELAY)).c_str());
    }
    else if ( cmnd.indexOf("ResetearTimers") >= 0) {

      // Envia como resposta a data e hora actualizada
      request->send_P(200, "text/plain", criaTimers().c_str());
    }
    else if ( cmnd.indexOf("{\"Enable\":1") >= 0 ) {
      // Configuração de Timer //

      configurarTimer(cmnd);

      // Envia como resposta a data e hora actualizada
      request->send_P(200, "text/plain", "OK");
    }
    else if ( cmnd.indexOf("Timer") >= 0  && cmnd.indexOf("{\"Enable\":1") == -1 ) {
      // Consulata do TimerX //
      // Obter o número do Timer
      String timer_num = cmnd.substring(5);
      timer_num.trim();
      uint8_t tmr_nmbr = timer_num.toInt();

      // Forçar pesquisa de timer válido
      if(tmr_nmbr >= 1 && tmr_nmbr <= QTD_TMRS ){
        tmr_nmbr = tmr_nmbr;
      }
      else{
        tmr_nmbr = 1;
      }

      //Serial.println(tmr_nmbr);

      request->send_P(200, "text/plain", consultaConfigTimer(tmr_nmbr).c_str());
    }
    else if ( cmnd.indexOf("Time") >= 0 ) {
      // Consulatar/Actualizar Hora //
      // Separa os números da palavra "Time" --> Time 1651668177 --> 1651668177
      uint8_t indx = cmnd.indexOf("Time");
      String unix_time = cmnd.substring(indx + 5).c_str();

      if (unix_time.length() == 10) { // Actualiza
        sincronizaDataHora(unix_time);
        // Envia como resposta a data e hora actualizada
        request->send_P(200, "text/plain", textoDataHora().c_str());
        //Serial.println(textoDataHora().c_str());   // Debug
      }
      else { // Consulata
        // Envia como resposta a data e hora actualizada
        request->send_P(200, "text/plain", textoDataHora().c_str());
        //Serial.println(textoDataHora().c_str());   // Debug
      }
    }
    else if ( cmnd.indexOf("DeviceName") >= 0 ) {
      // Consulatar/Actualizar Nome //
      // Verifica se o caomando tem argumento --> DeviceName XXXXXX...XXX
      uint8_t indx = cmnd.indexOf("DeviceName");
      String new_name = cmnd.substring(indx + 11).c_str();

      if (new_name.length() > 0) { // Actualiza

        char aux_char[MAX_NAME_LENGTH];
        uint16_t var_len = new_name.length() + 1;
        new_name.toCharArray(aux_char, var_len);

        String txt_json = actualizarNome(aux_char, start_addr);

        // Envia como resposta um texto json com o nome do device
        request->send_P(200, "text/plain", txt_json.c_str());
      }
      else { // Consulata

        String txt_json = consultarNome(start_addr);

        // Envia como resposta um texto json com o nome do device
        request->send_P(200, "text/plain", txt_json.c_str());
      }
    }
       
  });


  // Start server
  server.begin();

}



void loop() {

  // Tratar OTA
  ArduinoOTA.handle();


  executarTimer();

  delay(1000);

}


// Configuração da rede WiFi, para utiliazaçã com OTA
/*void configuraRedeWiFi(const char* ssid, const char* pwd){
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, pwd);

  uint8_t cont = 0;

  while (WiFi.status() != WL_CONNECTED){
    delay(1000);

    if( cont >= 10 ){
      break;
    }

    cont++;
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  }*/


// Obter Hostname do device
String obetrHostName() {
  return WiFi.hostname().c_str();
}




// Funçãopara para criar texto JSON
String criarTextoJSON(byte estado) {

  String IP = WiFi.localIP().toString().c_str();
  String Power = "";
  String sz_json;

  // Conformar estado
  if (estado) Power = "ON"; else  Power = "OFF";

  // Create object, texto com formato json, serialização
  DynamicJsonDocument doc(226);

  //doc["Nome"] = NOME;
  doc["POWER"] = Power;

  serializeJson(doc, sz_json);

  return sz_json;
}


// Guardar dados na EEPROM
void guardarEstadoEEPROM(uint8_t addr, byte estado) {

  // Guarda dados anteriores (os que se pretendem preservar)
  Dados old_dds = {};
  EEPROM.get(addr, old_dds);
  char nom_disp[MAX_NAME_LENGTH];
  strncpy( nom_disp, old_dds.nome, sizeof(old_dds.nome) );

  Dados new_dds = {};
  new_dds.estado_pin = estado;
  strncpy( new_dds.nome, nom_disp, sizeof(new_dds.nome) );

  // Guarda dados na EEPROM (anteriores e novos)
  EEPROM.put(addr, new_dds);
  EEPROM.commit();

}



// Reestabelece o estado do PIN
void restorePinState(byte pin, uint8_t eeprom_addr) {

  // Le dado da EEPROM, último estado do PIN

  Dados dds = {};
  EEPROM.get(eeprom_addr, dds);
  uint8_t last_state = dds.estado_pin;

  digitalWrite(pin, last_state);
  digitalWrite(PIN_LED, digitalRead(pin));
}



// Configura e inicilaiza o PIN a controlar
void configurarPIN(byte pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}



// Inicialização da memória EEPROM
void inicilaizaEEPROM(uint16_t eeprom_size) {
  EEPROM.begin(eeprom_size);  //Initialize EEPROM
  delay(20);
}



// Conforma a data e a hora numa stiring (H:mm:ss d m Y)
String textoDataHora() {

  String aux_seg = "";
  String aux_min = "";
  if (minute() < 10 ) aux_min = "0" + String(minute()); else aux_min = String(minute());
  if (second() < 10 ) aux_seg = "0" + String(second()); else aux_seg = String(second());

  String data_hora = String(hour()) + ":" + aux_min + ":" + aux_seg;
  data_hora += " " + String(day()) + "/" + String(month()) + "/" + String(year());

  return data_hora;
}


// Exibe data e hora na consola serial
/*void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
  }


  String printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
  }*/


// Processa/sincronoza a data e hora, no farmato epoch
void sincronizaDataHora(String unix_date_time) {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 284036400; // Jan 1 1979

  pctime = unix_date_time.toInt() + 3600; // + 3600 para aumentar 1 Hora (está a dar uma hora a menos)
  if ( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
    setTime(pctime); // Sync Arduino clock to the time received on the serial port
  }
}


// Atecnção à interrupção do PIN GPIO0
ICACHE_RAM_ATTR void attInterruptPIN_BUTTON() {

  digitalWrite(PIN_RELAY, !digitalRead(PIN_RELAY));
  digitalWrite(PIN_LED, digitalRead(PIN_RELAY));
  get_txt_json = criarTextoJSON(digitalRead(PIN_RELAY));
  guardarEstadoEEPROM(start_addr, digitalRead(PIN_RELAY));

}


// Cria instâncias do Timers e os guarda num arquivo SPIFFS
String criaTimers() {

  // Criar 16 instâncias, vazias, da Estructura "Rimers"
  Timers t = {};

  // Elimina o arquivo
  const char* f_name = "/timers.txt";
  File file = SPIFFS.open(f_name, "r");
  if ( file ) SPIFFS.remove(f_name);
  file.close();

  // Cria novo arquivo
  file = SPIFFS.open(f_name, "a");

  //Serial.println(file.size()); // Debug

  for (int i = 0; i < QTD_TMRS; i++) {
    file.write( (uint8_t*)&t, sizeof(struct timers));
  }

  /*
    Serial.print("Tamnaho do arquivo: "); // Debug
    Serial.print(file.size()); // Debug
    Serial.println(" bytes"); // Debug
  */

  file.close();

  return "Timers Reseteados!";

}


////////////////////////////
// Configuracão de timers //
////////////////////////////
void configurarTimer(String cmnd) {
  // Obter o número do Timer
  String timer_num = cmnd.substring(cmnd.indexOf(";") + 1,  cmnd.indexOf("{"));
  timer_num.trim();
  timer_num = timer_num.substring(5);
  timerNumber = timer_num.toInt();

  //Serial.print(timerNumber); // Debug

  // Guardar partes do JSON, do Timer, no Array e este último no arquivo SPIFFS
  uint8_t indx = cmnd.indexOf("{");
  String str_json = cmnd.substring(indx).c_str();

  char buff[256];
  int var_len = str_json.length() + 1;
  str_json.toCharArray(buff, var_len);

  DynamicJsonDocument doc_rx(256);
  deserializeJson(doc_rx, buff);

  // Abre o arquivo para Leitura + Escritura (r+)
  File myFile = SPIFFS.open("/timers.txt", "r+");

  // Lee a instância a ser alterada
  Timers tmr;
  timerNumber = timerNumber - 1;

  // Altera a instância indiacada (configuração do TimerX)
  tmr.activar = String(doc_rx["Enable"]).toInt();
  tmr.horas = String(doc_rx["Time"]).substring(0, 2).toInt();
  tmr.minutos = String(doc_rx["Time"]).substring(3, 5).toInt();
  strncpy( tmr.weekDays, doc_rx["Days"], sizeof(tmr.weekDays) );
  tmr.output = String(doc_rx["Output"]).toInt();
  tmr.action = String(doc_rx["Action"]).toInt();

  //Serial.println(tmr.action); // Debug

  // Guarda os dados na instância correspondente
  myFile.seek(timerNumber * sizeof(struct timers), SeekSet); // Desloca para posição correpondente ao TimerX
  myFile.write( (uint8_t*)&tmr, sizeof(struct timers));
  myFile.close();
}


// Executar acções do Timer
void executarTimer() {
  for (uint8_t cont = 1; cont <= 16; cont++) {

    File myFile = SPIFFS.open("/timers.txt", "r");
    Timers tmr = {};
    uint8_t offset = cont;

    if ( offset > 0 ) {

      offset = offset - 1;
      myFile.seek(offset * sizeof(struct timers), SeekSet); // Desloca para posição correpondente ao TimerX
      myFile.read((uint8_t*)&tmr, sizeof(struct timers));
      myFile.close();
      //Serial.printf( "Dados: %d, %d, %c, %d\n", tmr.horas, tmr.minutos, tmr.weekDays[1], tmr.action );
      //Serial.printf( "Timer%d: %d, %d, %d, %s, %d\n", (offset + 1), tmr.activar, tmr.horas, tmr.minutos, tmr.weekDays, tmr.action );

      // Verficar se o dia de semana, do momento, está programado, timer activado
      if ( tmr.weekDays[weekday() - 1] == '1' && tmr.activar == 1) {
        //Serial.println(hour() + " -- " + tmr.horas);
        //-------------------------------------------------//
        if (hour() == tmr.horas && minute() == tmr.minutos) {
          //Serial.println(hour() + " -- " + tmr.horas);
          // (*J*) Se tiver mais de uma saida a lógica altera, deverá usar-se "tmr.output"
          digitalWrite(PIN_RELAY, tmr.action);
          digitalWrite(PIN_LED, digitalRead(PIN_RELAY));
          get_txt_json = criarTextoJSON(digitalRead(PIN_RELAY));
          guardarEstadoEEPROM(start_addr, digitalRead(PIN_RELAY));
        }
        //-------------------------------------------------//

      }

    }
  }
}



// Consuta configuração dum Timer
String consultaConfigTimer(uint8_t timer_num) {

  File myFile = SPIFFS.open("/timers.txt", "r");
  Timers tmr = {};
  uint8_t offset = timer_num;

  offset = offset - 1;
  myFile.seek(offset * sizeof(struct timers), SeekSet); // Desloca para posição correpondente ao TimerX
  myFile.read((uint8_t*)&tmr, sizeof(struct timers));
  myFile.close();

  String timer = "Timer" + String((offset + 1));
  String enable = String(tmr.activar);

  String aux_hor, aux_min;
  if (tmr.horas <= 9) aux_hor = "0" + String(tmr.horas); else aux_hor = String(tmr.horas);
  if (tmr.minutos <= 9) aux_min = "0" + String(tmr.minutos); else aux_min = String(tmr.minutos);
  String hora = aux_hor + ":" + aux_min;

  String dias = String(tmr.weekDays);
  String saida = String(tmr.output);
  String accao = String(tmr.action);

  String txt_json = "{\"" + timer + "\":{\"Enable\":" + enable;
  txt_json += ",\"Time\":\"" + hora + "\",\"Window\":0,\"Days\":\"" + dias;
  txt_json += "\",\"Repeat\":1,\"Output\":" + saida + ",\"Action\":" + accao + "}}";

  //Serial.printf( "Timer%d: %d, %d, %d, %s, %d\n", (offset + 1), tmr.activar, tmr.horas, tmr.minutos, tmr.weekDays, tmr.action );
  //Serial.printf("%s, %s, %s, %s, %s, %s",timer, enable, hora, dias, saida, accao);
  //Serial.println(txt_json);

  return txt_json;
}



// Actualiza o nome do Device
String actualizarNome(const char* new_name, uint8_t eeprom_start_addr) {

  // Guarda dados anteriores (os que se pretendem preservar)
  Dados old_dds = {};
  EEPROM.get(eeprom_start_addr, old_dds);
  uint8_t pin_state = old_dds.estado_pin;

  Dados new_dds = {};
  new_dds.estado_pin = pin_state;
  strncpy( new_dds.nome, new_name, sizeof(new_dds.nome) );

  // Guarda dados na EEPROM (anteriores e novos)
  EEPROM.put(eeprom_start_addr, new_dds);
  EEPROM.commit();

  String str_json = "{\"DeviceName\":\"" + String(new_name) + "\"}";
  return str_json;
}


// Actualiza o nome do Device
String consultarNome(uint8_t eeprom_start_addr) {

  Dados dds = {};

  // Lee dados da EEPROM
  EEPROM.get(eeprom_start_addr, dds);

  String str_json = "{\"DeviceName\":\"" + String(dds.nome) + "\"}";
  return str_json;
}
