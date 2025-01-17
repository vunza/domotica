
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


#if DEBUG == 1
#define imprime(x) Serial.print(x)
#define imprimeln(x) Serial.println(x)
#else
#define imprime(x)
#define imprimeln(x)
#endif

  
uint8_t macSTA[6] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};
uint8_t macAP[6]  = {0x02, 0x00, 0x00, 0x00, 0x00, 0x02};   

const char* ssid = "TPLINK";  
const char* password = "gregorio@2012"; 
 
/*esp_now_peer_info_t slave;
const esp_now_peer_info_t *peer = &slave;

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void prntmac(const uint8_t *mac_addr);
*/


// Função de callback para receber dados via ESP-NOW
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  Serial.print("Dados recebidos de: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", mac[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
  Serial.print("Mensagem: ");
  for (int i = 0; i < len; i++) {
    Serial.print((char)incomingData[i]);
  }
  Serial.println();
}

/////////////
// setup() //
/////////////
void setup() {
  // Inicializar Serial
  Serial.begin(115200);
  
  // Aguardar pela inicializacao da Serual
  while (!Serial){    
  }
  

  // Inicializar Wi-Fi 
  /*RedeWifi redeWfi("ESP12E", "123456789", "WIFI_AP");
  redeWfi.AlterarMacAP(macAP);
  redeWfi.CriaRedeWifi("");*/

  RedeWifi redeWfi(ssid, password, "WIFI_STA");  //modo: WIFI_AP_STA | WIFI_STA | WIFI_AP
  redeWfi.AlterarMacSTA(macSTA);                    // Altera MC no modo STA
  redeWfi.ConectaRedeWifi("");                      // STA_IP_MODE = [DHCP | STATIC]
   
  

  // Configurar ESP-NOW
  /*if (esp_now_init() == ESP_OK) {
    Serial.println("ESP-NOW inicializado com sucesso!");
  } else {
    Serial.println("Erro ao inicializar ESP-NOW.");
    ESP.restart();
  }   

  // Configurar callback para receber dados
  esp_now_register_recv_cb(onDataRecv);

  // Configurar Peer para enviar dados
  uint8_t peerMAC[] = {0x24, 0x6F, 0x28, 0x12, 0x34, 0x56};  // Substituir pelo MAC do destinatário
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerMAC, 6);
  peerInfo.channel = WiFi.channel();  // Usar o mesmo canal do Wi-Fi
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) == ESP_OK) {
    Serial.println("Peer adicionado com sucesso!");
  } else {
    Serial.println("Erro ao adicionar Peer.");
  }*/
}

////////////
// loop() //
////////////
void loop() {
  
  // Enviar dados via ESP-NOW
  /*uint8_t data[] = "Olá, ESP-NOW!";
  uint8_t peerMAC[] = {0x24, 0x6F, 0x28, 0x12, 0x34, 0x56};  // Substituir pelo MAC do destinatário

  esp_err_t result = esp_now_send(peerMAC, data, sizeof(data));

  if (result == ESP_OK) {
    Serial.println("Mensagem enviada com sucesso!");
  } else {
    Serial.printf("Erro ao enviar mensagem: %d\n", result);
  }

  delay(2000);  // Aguarde 2 segundos antes de enviar novamente*/
}
