#ifndef CONSTANTES_H
#define CONSTANTES_H

#define EEPROM_START_ADDR 0
#define DEVICE_NAME_SIZE 24
#define EEPROM_SIZE 512
#define INA226_I2C_ADDRESS 0x40 
#define JS_UART_DATA_SIZE 64
#define MQTT_BUFFER_SIZE 512

// Configurações específicas para cada plataforma
#ifdef ESP8266 
  #define CHANNEL_SCAN_MAX 14  // ESP8266 suporta até canal 14
#else  
  #define CHANNEL_SCAN_MAX 13  // ESP32 geralmente até canal 13
#endif


#endif // CONSTANTES_H