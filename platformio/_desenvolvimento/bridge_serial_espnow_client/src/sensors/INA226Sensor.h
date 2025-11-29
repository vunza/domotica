// INA226 current and voltage sensor 
#ifndef INA226_SENSOR_H
#define INA226_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <INA226_WE.h>
#include "ctrl_debug.h"

class INA226Sensor {

public:
    INA226Sensor(uint8_t address = 0x40);

    bool begin();                 
    float readCurrent();          
    float readVoltage();          
    void update();                

private:
    INA226_WE ina;
    float current_mA;
    float voltage_V;
};

#endif
