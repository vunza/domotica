#ifndef CTRL_DEBUG_H
#define CTRL_DEBUG_H

#include <Arduino.h>

// Ativar mensagens pela serial para depuração
#if ACTIVAR_SERIAL_DEBUG
#define imprime(x) Serial.print(x)
#define imprimeln(x) Serial.println(x)
#else
#define imprime(x)
#define imprimeln(x)
#endif


#endif // COMUM_H