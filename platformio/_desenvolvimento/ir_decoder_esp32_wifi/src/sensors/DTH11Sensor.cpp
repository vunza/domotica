// DHT11 temperature and humidity sensor
#include "sensors/DTH11Sensor.h"

DHT11Sensor::DHT11Sensor(uint8_t pin, uint8_t type) : dht(pin, type) {}

void DHT11Sensor::begin() {
    dht.begin();
}

float DHT11Sensor::getTemperature() {
    return dht.readTemperature();
}

float DHT11Sensor::getHumidity() {
    return dht.readHumidity();
}
