#include "espnow_manager.h"


EspNowManager *EspNowManager::instance = nullptr;

EspNowManager::EspNowManager() {
    instance = this;
    channelFound = false;
    discoveredChannel = 0;
    server_alive_counter = 0;
}

bool EspNowManager::begin(uint8_t channel, bool useAP) {
#if defined(ESP32)

    WiFi.disconnect(true);
    WiFi.mode(useAP ? WIFI_AP : WIFI_STA);    
    delay(100);

    // Ajusta o canal de rádio    
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    if (esp_now_init() != ESP_OK) {
        imprimeln(F("[ESPNOW] ERRO ao iniciar (ESP32)"));
        return false;
    }

    esp_now_register_recv_cb(_onRecv);
    esp_now_register_send_cb(_onSent);

#elif defined(ESP8266)

    WiFi.disconnect();
    WiFi.mode(useAP ? WIFI_AP : WIFI_AP_STA);   
    WiFi.softAP(WiFi.hostname(), "123456789", channel, true); // para que o esp-now funcione
   

    delay(100);

    if (esp_now_init() != 0) {
        imprimeln(F("[ESPNOW] ERRO ao iniciar (ESP8266)"));
        return false;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

    esp_now_register_recv_cb(_onRecv);
    esp_now_register_send_cb(_onSent);

    // Transformar AP em “fantasma” (desativada, mas canal permanece)
    softap_config apcfg;
    wifi_softap_get_config(&apcfg);
    memset(apcfg.ssid, 0, sizeof(apcfg.ssid));  // Ocultar completamente
    apcfg.ssid_len = 0;
    apcfg.ssid_hidden = 1;
    apcfg.max_connection = 0;                 
    apcfg.beacon_interval = 1000;    
    wifi_softap_set_config_current(&apcfg);
    wifi_softap_dhcps_stop(); // Desliga DHCP

#endif

    imprimeln(F("[ESPNOW] Iniciado com sucesso"));
    return true;
}




bool EspNowManager::addPeer(const uint8_t mac[6], uint8_t channel) {

#if defined(ESP32)

    esp_now_peer_info_t peer{};
    memcpy(peer.peer_addr, mac, 6);

    peer.channel  = channel;
    peer.encrypt  = false;
    peer.ifidx    = WIFI_IF_STA;

    if (esp_now_add_peer(&peer) == ESP_OK) return true;

#elif defined(ESP8266)

    uint8_t *m = (uint8_t *)mac;

    if (esp_now_is_peer_exist(m)) return true;

    if (esp_now_add_peer(m, ESP_NOW_ROLE_COMBO, channel, NULL, 0) == 0)
        return true;

#endif

    imprimeln(F("[ESPNOW] Falha ao adicionar peer"));
    return false;
}

bool EspNowManager::send(const uint8_t mac[6], const uint8_t *data, int len) {
#if defined(ESP32)
    return esp_now_send(mac, data, len) == ESP_OK;
#elif defined(ESP8266)
    return esp_now_send((uint8_t*)mac, (uint8_t*)data, len) == 0;
#endif
}


// Obtem canal WiFi configurado no dispositivo
uint8_t EspNowManager::getCurrentWiFiChannel() {
#if defined(ESP8266)
    return wifi_get_channel();
#elif defined(ESP32)
    return WiFi.channel();
#endif
}



// Callback de instância (processa os dados recebidos)
void EspNowManager::onRecvInstance(const uint8_t* mac, const uint8_t* data, int len) {
    
    EspNowData dados_espnow;
    memcpy(&dados_espnow, data, sizeof(EspNowData));

    // Verifica se é uma resposta de canal
    if (strcmp(dados_espnow.msg_type, "CHANNEL_RSP") == 0) {         

        // Termina o scan do vcanal
        channelFound = true;
        // Converte char array a interiro
        char* endPtr;
        discoveredChannel = strtol(dados_espnow.state, &endPtr, 10);

        // Se nao foi encontardo o canal
        if (*endPtr != '\0') {
            discoveredChannel = 0;
        }

        imprime("[ESPNOW] Canal descoberto: ");
        imprimeln(discoveredChannel);       
    }
}



// Callback estática para ESP8266 (discovery espnow channel)
#ifdef ESP8266
void EspNowManager::staticOnRecv(uint8_t* mac, uint8_t* incomingData, uint8_t len) {
    if (instance != nullptr) {
        instance->onRecvInstance(mac, incomingData, len);
    }
}
// Callback estática para ESP32 (discovery espnow channel)
#elif defined(ESP32)
void EspNowManager::staticOnRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
    if (instance != nullptr) {
        instance->onRecvInstance(mac, incomingData, len);
    }
}
#endif



// Solicita canal WiFi do Master
uint8_t EspNowManager::discoverEspNowChannel(uint32_t timeoutMs) {

    EspNowData dados_espnow; 
    uint8_t broadcastMac[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; 
    channelFound = false;
    is_server_alive = false;
    
    strcpy(dados_espnow.msg_type, "CHANNEL_REQ");   
    strcpy(dados_espnow.state, "");     

    for(int8_t channel = 0; channel <= CHANNEL_SCAN_MAX; channel++){
        
        esp_now_deinit();
        
        #if defined(ESP32)
            WiFi.disconnect(true);
            WiFi.mode(WIFI_STA);    
            delay(100);

            // Ajusta o canal de rádio    
            esp_wifi_set_promiscuous(true);
            esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
            esp_wifi_set_promiscuous(false);

            if (esp_now_init() != ESP_OK) {
                imprimeln(F("[ESPNOW] ERRO ao iniciar (ESP32)"));
                return 0;
            }

            esp_now_register_recv_cb(staticOnRecv);           

        #elif defined(ESP8266)

            WiFi.disconnect();
            WiFi.mode(WIFI_AP_STA);   
            WiFi.softAP(WiFi.hostname(), "123456789", channel, true); // para que o esp-now funcione

            delay(100);

            if (esp_now_init() != 0) {
                imprimeln(F("[ESPNOW] ERRO ao iniciar (ESP8266)"));
                return 0;
            }

            esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
            esp_now_register_recv_cb(staticOnRecv);            

            // Transformar AP em “fantasma” (desativada, mas canal permanece)
            softap_config apcfg;
            wifi_softap_get_config(&apcfg);
            memset(apcfg.ssid, 0, sizeof(apcfg.ssid));  // Ocultar completamente
            apcfg.ssid_len = 0;
            apcfg.ssid_hidden = 1;
            apcfg.max_connection = 0;                 
            apcfg.beacon_interval = 1000;    
            wifi_softap_set_config_current(&apcfg);
            wifi_softap_dhcps_stop(); // Desliga DHCP

        #endif

        if(channelFound){           
           break;
           // Cancela ESP-NOW (discovery espnow channel)
           esp_now_deinit();
        }  
        else{

            #if defined(ESP32)
                const char* host_name = WiFi.getHostname();
            #elif defined(ESP8266)  
                String hostNameStr = WiFi.hostname();
                const char* host_name = hostNameStr.c_str();
            #endif  

            strcpy(dados_espnow.mac_server, host_name); // Usar mac_server para levar o nome do ESP
            strcpy(dados_espnow.msg_type, "CHANNEL_REQ");
            strcpy(dados_espnow.mac_client, WiFi.macAddress().c_str());
            esp_now_send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
            imprime(F("Testando o Canal: "));
            imprimeln(channel);            
        }       

        delay(timeoutMs);
    }

    // Cancela ESP-NOW (discovery espnow channel)
    esp_now_deinit();

    return discoveredChannel;
    
}


// Realiza o emparelhamento com um dispositivo mestre via ESP-NOW.
void EspNowManager::emparelharDispositivo(const uint8_t* broadcastMac, uint32_t timeoutMs, boolean wifi_sta_mode){
    
    EspNowData dados_espnow; 

    // Scanea canal Esp-Now
    uint8_t canal = discoverEspNowChannel(timeoutMs);    

    if (canal <= 0) {
        imprimeln(F("Master ESP-NOW não encontrado"));
        is_server_alive = false;
    } else {
        imprime(F("Canal ESP-NOW descoberto: "));
        imprimeln(canal); 
        // Iniciar ESP-NOW (modo normal)
        begin(canal, wifi_sta_mode);
        // broadcast (tudo que enviar vai para todos)
        addPeer(broadcastMac, canal); 
        is_server_alive = true;  
        
        // Solicita estado atual do Dispositivo
        strcpy(dados_espnow.msg_type, "ASK_STATE");          
        strcpy(dados_espnow.mac_client, WiFi.macAddress().c_str());             
        send(broadcastMac, (uint8_t*)&dados_espnow, sizeof(EspNowData));
    }            
}




/* -------------------- CALLBACKS ------------------------ */

void EspNowManager::onReceive(RecvCallback cb) { _recvCB = cb; }
void EspNowManager::onSend(SendCallback cb) { _sendCB = cb; }

#if defined(ESP32)
void EspNowManager::_onRecv(const uint8_t *mac, const uint8_t *data, int len) {
    if (instance && instance->_recvCB)
        instance->_recvCB(mac, data, len);
}

void EspNowManager::_onSent(const uint8_t *mac, esp_now_send_status_t status) {
    if (instance && instance->_sendCB)
        instance->_sendCB(mac, status == ESP_NOW_SEND_SUCCESS);
}
#elif defined(ESP8266)
void EspNowManager::_onRecv(uint8_t *mac, uint8_t *data, uint8_t len) {
    if (instance && instance->_recvCB)
        instance->_recvCB(mac, data, len);
}

void EspNowManager::_onSent(uint8_t *mac, uint8_t status) {
    if (instance && instance->_sendCB)
        instance->_sendCB(mac, status == 0);
}

#endif
