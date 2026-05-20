#include "oled_display_128x64.h"

// ---------------- I2C PADRÃO POR PLATAFORMA ----------------
#if defined(ESP8266)
  #define SDA_PIN D2
  #define SCL_PIN D1
#elif defined(ESP32)
  #define SDA_PIN 21
  #define SCL_PIN 22
#endif

OLEDDisplay::OLEDDisplay()
    : display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1) {}

bool OLEDDisplay::begin() {    
    #if defined(ESP8266)
        Wire.begin();
    #elif defined(ESP32)
        Wire.begin(SDA_PIN, SCL_PIN);
    #endif

    delay(100);

    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        return false;
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.display();
    return true;
}

void OLEDDisplay::clear() {
    display.clearDisplay();
}

void OLEDDisplay::printText(uint8_t x, uint8_t y, const char* text, uint8_t size) {
    display.setTextSize(size);
    display.setCursor(x, y);
    display.print(text);
}

void OLEDDisplay::printValue(uint8_t x, uint8_t y, const char* label, int value, const char* suffix, uint8_t size) {
    display.setTextSize(size);
    display.setCursor(x, y);
    display.print(label);
    display.print(value);
    display.print(suffix);
}

void OLEDDisplay::showBattery(uint8_t percent) {
    // Moldura
    display.drawRect(98, 2, 26, 12, SSD1306_WHITE);
    display.drawRect(124, 5, 3, 6, SSD1306_WHITE);

    uint8_t fill = map(percent, 0, 100, 0, 22);
    display.fillRect(100, 4, fill, 8, SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(70, 4);
    display.print(percent);
    display.print("%");
}

void OLEDDisplay::update() {
    display.display();
}
