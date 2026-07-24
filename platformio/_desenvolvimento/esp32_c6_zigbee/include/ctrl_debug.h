#ifndef CTRL_DEBUG_H
#define CTRL_DEBUG_H

#include <Arduino.h>

// Ativar mensagens pela serial para depuração
#if ACTIVAR_SERIAL_DEBUG
#define imprime(x) Serial.print(x)
#define imprimeln(x) Serial.println(x)
#define imprimef(format, ...) Serial.printf(format, ##__VA_ARGS__)
//#define imprimefln(format, ...) Serial.printf(format "\n", ##__VA_ARGS__) // Pula uma linha automaticamente
#else
#define imprime(x)
#define imprimeln(x)
#define imprimef(format, ...)
#endif


#endif // COMUM_H