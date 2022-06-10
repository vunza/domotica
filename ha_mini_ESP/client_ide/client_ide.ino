#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#include <ArduinoOTA.h>

     
HTTPClient http;
WiFiClient wifiClient; 

ESP8266WebServer server(80);


// Configuração do pin a controlar
#define PIN_CTRL 14

#define OTA_Host_Name "WEMOS-D1-R2"

IPAddress staticIP(192, 168, 5, 3); 
IPAddress gateway(192, 168, 5, 1);
IPAddress subnet(255, 255, 255, 0);


void setup(void) { 
  
  Serial.begin(9600);     
  delay(200);
  Serial.println("");
   
  // Configurações e inicialização do PIN
  configurarPIN(PIN_CTRL);

  // Inicialização da memória EEPROM  
  inicilaizaEEPROM(eeprom_size); 

  // Reestabelece o último estado do PIN
  restorePinState(PIN_CTRL, state_addr);

  // Configuraçao e conexão da rede WiFi      
  configuraRedeWiFi(ssid, password);

  // Obter hostnome, (*J*) deve ser feita depois da configutação da rede
  //NOME = obetrHostName();
  //ID = obetrHostName();
  //Serial.println(NOME);
  
  
  // Configuração e inicialição do OTA
  ArduinoOTA.setHostname(OTA_Host_Name);
  ArduinoOTA.begin();


  server.on("/TOGGLE", togglePIN);

  server.enableCORS(true); 
   
  //server.enableCORS(true);
  server.begin();   
  
}



void loop(void) {
  
  // Conectar sempre que estiver disconectado
  if(WiFi.status() != WL_CONNECTED){    
     configuraRedeWiFi(ssid, password);   
  }

  server.handleClient();

   // Tratar OTA
  ArduinoOTA.handle();

}


void togglePIN(){      
  digitalWrite(PIN_CTRL, !digitalRead(PIN_CTRL));
  String estado = digitalRead(PIN_CTRL)?"ON":"OFF";
  server.send(200, "text/html", estado); 
}


// Funçãopara registar-se ao servidor
void registarCliente(String sz_json){  
  
  String payload = "?Registo=" + sz_json;
      
  http.begin(wifiClient, "http://" + server_ip + "/REGISTO/" + payload);  
  httpResponseCode = http.GET();    
  http.end();
  //Serial.println(payload);      
}


// Funçãopara para criar texto JSON
/*String criarTextoJSON(byte estado){
  String IP = WiFi.localIP().toString().c_str();
  String ESTADO = "";

  // Conformar estado
  if(estado) ESTADO = "ON"; else  ESTADO = "OFF";

  // Create object, texto com formato json, serialização
  DynamicJsonDocument doc(1024);

  doc["Id"] = ID;
  doc["Ip"] = IP;
  doc["Nome"] = NOME;
  doc["Tipo"]   = TIPO;
  doc["Estado"] = ESTADO;
  doc["Grupo"] = GRUPO;

  String sz_json;
  serializeJson(doc, sz_json);
  
  return sz_json;      
}*/



// Guardar estado na EEPROM
void guardarEstadoEEPROM(uint8_t addr, byte estado){
  EEPROM.write(addr, estado);
  EEPROM.commit();
}



// Reestabelece o estado do PIN
void restorePinState(byte pin, uint8_t eeprom_addr){
  // Le dado da EEPROM, último estado do PIN
  uint8_t last_state = EEPROM.read(eeprom_addr);
  
  digitalWrite(pin, last_state);
}


// Configuração da rede WiFi
void configuraRedeWiFi(const char* ssid, const char* pwd){
  WiFi.mode(WIFI_STA);
  WiFi.config(staticIP, gateway, subnet);  
  WiFi.begin(ssid, pwd);

  uint8_t cont = 0;
  
  while (WiFi.status() != WL_CONNECTED){
    delay(1000); 
    
    if( cont >= 5 ){
      break; 
    }
    
    cont++;    
  } 

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true); 
}


// Configura e inicilaiza o PIN a controlar
void configurarPIN(byte pin){
  pinMode(pin, OUTPUT);  
  digitalWrite(pin, LOW);  
}


// Inicialização da memória EEPROM
void inicilaizaEEPROM(uint16_t eeprom_size){
  EEPROM.begin(eeprom_size);  //Initialize EEPROM
  delay(20);
}

String obetrHostName(){
  return WiFi.hostname().c_str();
}


// Rotina de interrupção do WDT
void ISR_RegistarCliente(){  
  //ctrl_reg = true;
}
