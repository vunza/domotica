
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


#if defined(ESP8266) 
#define PIN_LED 2//19//23 
#elif defined(ESP32) 
#define PIN_LED 23//19//2
#endif

#define TIME_PING_REQUEST 5                 // Tempo para enviar estado do dispositivo ao servidor
#define TMP_CHECK_SERVER_ALIVE 5            // Tempo para checar servidor

#define DEBUG 1

#if DEBUG == 1
#define imprime(x) Serial.print(x)
#define imprimeln(x) Serial.println(x)
#else
#define imprime(x)
#define imprimeln(x)
#endif


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
  if( set_device_pin == true ){  

    set_device_pin = false;

    Payload pld = {};
    pld.tipo_msg = DATA;   
    pld.canal_wifi = remote_wifi_channel;
    pld.estado_pin = get_pin_state;
    memcpy(pld.nome_dispositivo, DEVICE_NAME, sizeof(DEVICE_NAME));
    memcpy(pld.mac_cliente, localMac, sizeof(localMac));     
    esp_now_send(broadcastAddress, (uint8_t *)&pld, sizeof(pld));

    #if defined(ESP8266)      
      digitalWrite(PIN_LED, !get_pin_state); // logica invertida
    #elif defined(ESP32) 
       digitalWrite(PIN_LED, get_pin_state);      
    #endif  
       
  } 


  if((millis() - ctrl_time_ping_request)/1000 >= TIME_PING_REQUEST ){   

    // Reemparelhar
    if(ping_couter > TMP_CHECK_SERVER_ALIVE){   
      device_paired = false;   
      ReEmparelhar();
    }
     
    // Enviar "PING_REQUEST" para controlo da disponibilidade do servidor
    Payload pld = {};
    pld.tipo_msg = PING_REQUEST;      
    memcpy(pld.mac_cliente, localMac, sizeof(localMac));   
    memcpy(pld.nome_dispositivo, DEVICE_NAME, sizeof(DEVICE_NAME)); 
    esp_now_send(broadcastAddress, (uint8_t *)&pld, sizeof(pld));    
     
    // Actualiza Variaveis 
    ping_couter++;
    ctrl_time_ping_request = millis(); 

  }


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