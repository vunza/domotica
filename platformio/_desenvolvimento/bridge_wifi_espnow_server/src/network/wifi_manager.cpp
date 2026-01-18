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
void WiFiManager::criar_ap(const char* ssid, const char* password, uint8_t channel) {
    WiFi.softAP(ssid, password, channel);
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
    snprintf(buffer, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  } else {
    snprintf(buffer, 13, "%02x%02x%02x%02x%02x%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  }
}


String WiFiManager::extrairIdDoTopico(const String& topico) {
    // Encontra a posição da primeira barra '/'
    uint8_t primeiraBarra = topico.indexOf('/');
    // Se não encontrar ou não houver conteúdo após a barra, retorna vazio
    if (primeiraBarra == -1 || topico.length() <= 0) {
        return "";
    }
    
    // Encontra a posição da segunda barra, começando após a primeira
    int segundaBarra = topico.indexOf('/', primeiraBarra + 1);
    
    // Se não encontrar a segunda barra, retorna vazio
    if (segundaBarra == -1) {
        return "";
    }
    
    // Extrai o conteúdo entre as duas barras
    // primeiraBarra+1 = início após a primeira barra
    // segundaBarra = posição da segunda barra (não inclusiva)
    return topico.substring(primeiraBarra + 1, segundaBarra);
}



String WiFiManager::formatarMac(const String& macSemSeparador) {
    if (macSemSeparador.length() != 12) {
        return "INVALIDO";
    }
    
    // Converte String para array de char temporário
    const char* str = macSemSeparador.c_str();
    
    // Buffer para resultado: 6 grupos de 2 chars + 5 separadores + 1 null
    char buffer[18]; // 12 chars + 5 ':' + null terminator
    
    // Formata diretamente com sprintf e %X (hexadecimal maiúsculo)
    // Lê cada par de caracteres como hexadecimal
    unsigned int bytes[6];
    sscanf(str, "%2x%2x%2x%2x%2x%2x", 
           &bytes[0], &bytes[1], &bytes[2], 
           &bytes[3], &bytes[4], &bytes[5]);
    
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X",
            bytes[0], bytes[1], bytes[2],
            bytes[3], bytes[4], bytes[5]);
    
    return String(buffer);
}