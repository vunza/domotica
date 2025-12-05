#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H

#include <Arduino.h>

#if defined(ESP32)
  #include <EEPROM.h>
#elif defined(ESP8266)
  #include <EEPROM.h>
#else
  #error "Placa não suportada!"
#endif

class EEPROMManager {
public:
    EEPROMManager(size_t size = 512) : _size(size) {}

    void begin() {
        EEPROM.begin(_size);
    }

    /** Escreve um byte na EEPROM */
    void writeByte(int address, uint8_t value) {
        if (address < 0 || address >= _size) return;
        EEPROM.write(address, value);
        EEPROM.commit();
    }

    /** Lê um byte da EEPROM */
    uint8_t readByte(int address) {
        if (address < 0 || address >= _size) return 0;
        return EEPROM.read(address);
    }

    /** Escreve um inteiro (32 bits) */
    void writeInt(int address, int32_t value) {
        if (address < 0 || address + 3 >= _size) return;
        EEPROM.put(address, value);
        EEPROM.commit();
    }

    /** Lê um inteiro (32 bits) */
    int32_t readInt(int address) {
        int32_t value = 0;
        EEPROM.get(address, value);
        return value;
    }

    /** Escreve uma string */
    void writeString(int address, const String &txt) {
        int len = txt.length();
        if (address + len + 1 >= _size) return;

        for (int i = 0; i < len; i++) {
            EEPROM.write(address + i, txt[i]);
        }
        EEPROM.write(address + len, 0); // NULL terminator
        EEPROM.commit();
    }

    /** Lê uma string da EEPROM */
    String readString(int address) {
        String txt = "";
        for (int i = address; i < _size; i++) {
            char c = EEPROM.read(i);
            if (c == 0) break;
            txt += c;
        }
        return txt;
    }

private:
    size_t _size;
};

#endif
