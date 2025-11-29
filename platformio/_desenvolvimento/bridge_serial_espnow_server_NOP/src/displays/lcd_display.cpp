#include "lcd_display.h"

LCDDisplay::LCDDisplay(uint8_t i2c_addr, uint8_t cols, uint8_t rows)
    : lcd(i2c_addr, cols, rows), _cols(cols), _rows(rows) {}


void LCDDisplay::begin() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    Wire.begin();
}


void LCDDisplay::showReadings(float current_mA, float voltage_V) {
    lcd.clear();
    lcd.setCursor(0, 0);  // Coluna 0, linha 0
    lcd.print("I = ");
    lcd.print(current_mA);
    lcd.print(" mA");  

    // Impede que o watchdog timer (WDT) reinicie o microcontrolador.   
    #ifdef ESP32
        vTaskDelay(1);
    #elif defined(ESP8266)
        yield();     
    #endif     
    
    lcd.setCursor(0, 1); // Coluna 0, linha 1
    lcd.print("V = ");
    lcd.print(voltage_V);
    lcd.print(" V");   
    
    // Impede que o watchdog timer (WDT) reinicie o microcontrolador.   
    #ifdef ESP32
        vTaskDelay(1);
    #elif defined(ESP8266)
        yield();     
    #endif     
}