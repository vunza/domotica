// src/network/wifi_manager.cpp
#include "wifi_manager.h"

// Conectar ao ponto de acesso WiFi
void WiFiManager::connect(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        imprimeln(F("Conectando ao WiFi..."));
    }

    // Configurar reconexão automática e persistência
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    
    imprimeln(F("\nConectado ao WiFi!"));    
    imprimeln(WiFi.localIP());
    imprime(F("Canal WiFi: "));
    imprimeln(String(WiFi.channel()));
}


// Criar ponto de acesso WiFi
void WiFiManager::criar_ap(const char* ssid, const char* password) {
    WiFi.softAP(ssid, password);
    imprimeln(F("Ponto de acesso criado"));
    imprimeln(WiFi.softAPIP());
}


// Checar conexão WiFi periodicamente
void WiFiManager::checkConnection() {     
    if (millis() - lastCheck >= 10000) {
        if (WiFi.status() != WL_CONNECTED) {
            imprimeln(F("Conexão WiFi perdida!"));
            //TODO Implementar reconexão
        }

        lastCheck = millis();
    }
}

// Obtém o endereço MAC do dispositivo Wi-Fi.
void WiFiManager::getMacAddress(char* buffer, bool withColons) {
  uint8_t mac[6];
  WiFi.macAddress(mac);

  if (withColons) {
    snprintf(buffer, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  } else {
    snprintf(buffer, 13, "%02x%02x%02x%02x%02x%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  }
}