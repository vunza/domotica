#include "espnow_manager.h"


EspNowManager *EspNowManager::instance = nullptr;

EspNowManager::EspNowManager() {
    instance = this;
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
