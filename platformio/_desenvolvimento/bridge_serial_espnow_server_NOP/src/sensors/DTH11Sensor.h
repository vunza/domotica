// DHT11 temperature and humidity sensor
#ifndef DHT11SENSOR_H
#define DHT11SENSOR_H

#include <Arduino.h>
#include <Adafruit_Sensor.h>  
#include <DHT.h>
#include <DHT_U.h>
#include "ctrl_debug.h"

class DHT11Sensor {
    public:
        DHT11Sensor(uint8_t pin, uint8_t type);
        void begin();
        float getTemperature();
        float getHumidity();

    private:
        DHT dht;
};

#endif
