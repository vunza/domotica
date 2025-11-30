#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "ctrl_debug.h"

class LCDDisplay {
public:
    LCDDisplay(uint8_t i2c_addr, uint8_t cols, uint8_t rows);

    void begin();
    void showReadings(float current_mA, float voltage_V);

private:
    LiquidCrystal_I2C lcd;
    uint8_t _cols;
    uint8_t _rows;
};

#endif
