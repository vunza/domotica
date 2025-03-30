
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

#define PIN_LED 2//19//23

#define CTRL_TIME_SEND_STATUS 5    // Tempo para enviar estado do dispositivo ao servidor
#define TMP_SEND_ALIVE 10           // Tempo para checar servidor

#define DEBUG 1

#if DEBUG == 1
#define imprime(x) Serial.print(x)
#define imprimeln(x) Serial.println(x)
#else
#define imprime(x)
#define imprimeln(x)
#endif

unsigned long ctrl_send_alive = 0;

/////////////
// setup() //
/////////////
void setup() {

  #if DEBUG == 1
    // Inicializar Serial
    Serial.begin(115200);
    
    // Aguardar pela inicializacao da Serual
    while (!Serial); 
  #endif

  pinMode(PIN_LED, OUTPUT); 

  // Criar AP 
  //RedeWifi ap_obj("ESP12E", "123456789", "WIFI_AP");
  //ap_obj.AlterarMacAP(macAP);
  //ap_obj.CriaRedeWifi("");

  // Apenas configura o dispositivo como STA (em dispositivos clientes)
  RedeWifi sta_obj("", "", "WIFI_STA");               //modo: WIFI_AP_STA | WIFI_STA | WIFI_AP
  //sta_obj.AlterarMacSTA(macSTA);                    // Altera MC no modo STA
  //sta_obj.ConectaRedeWifi("");                      // STA_IP_MODE = [DHCP | STATIC]
  
  // Configurar inicializar esp-now
  EspNow objespnow;

}// setup()


////////////
// loop() //
////////////
void loop() {  

  // Envia, ao servidor, o estado do didpositivo.
  if( (millis() - ctrl_time_send_status)/1000 >= CTRL_TIME_SEND_STATUS ){     
    
    Payload pld = {};
    pld.tipo_msg = DATA;   
    pld.canal_wifi = remote_wifi_channel;
    pld.estado_pin = digitalRead(PIN_LED);
    memcpy(pld.mac_cliente, localMac, sizeof(localMac));     
    /*esp_now_send(broadcastAddress, (uint8_t *)&pld, sizeof(pld));      
    ctrl_time_send_status = millis();
        
    imprime("Comando: ");
    imprimeln(pld.tipo_msg); 

    digitalWrite(PIN_LED, !digitalRead(PIN_LED)); */         
  } 


  // Enviar mensagem ao servidor para verificar se está disponível.
  if( (millis() - ctrl_send_alive)/1000 >= TMP_SEND_ALIVE ){         
    
  }

  /*
  uint8_t cont = 0;
  uint8_t data[24];
  char buff[16];
  sprintf(buff, "Contador = %d", cont++);
  memcpy(&data, buff, sizeof(data));
  if(cont > 9) cont = 0;
  //esp_err_t result = esp_now_send(broadcastAddress, data, sizeof(data));

  uint8_t result = esp_now_send(broadcastAddress, data, sizeof(data));

  //if (result == ESP_OK) {
  if (result == 0) {
    Serial.println("Mensagem enviada com sucesso");
  } else {
    Serial.println("Erro ao enviar mensagem");
  }

  delay(1000);  // Envia a mensagem a cada 2 segundos
  */

}// loop()



/*#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao inicializar ESP-NOW");
    return;
  }

  esp_wifi_set_channel(11, WIFI_SECOND_CHAN_NONE);

  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  for (int i = 0; i < 6; i++) {
    peerInfo.peer_addr[i] = 0xFF;
  }
  peerInfo.channel = 11;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Erro ao adicionar peer");
    return;
  }
}


uint8_t cont = 0;
void loop() {
  uint8_t data[24];
  char buff[16];
  sprintf(buff, "Contador = %d", cont++);
  memcpy(&data, buff, sizeof(data));
  if(cont > 9) cont = 0;
  esp_err_t result = esp_now_send(broadcastAddress, data, sizeof(data));

  if (result == ESP_OK) {
    Serial.println("Mensagem enviada com sucesso");
  } else {
    Serial.println("Erro ao enviar mensagem");
  }

  delay(1000);  // Envia a mensagem a cada 2 segundos
}*/