// INA226 current and voltage sensor
#include "INA226Sensor.h"

INA226Sensor::INA226Sensor(uint8_t address): ina(address), current_mA(0), voltage_V(0) {}

bool INA226Sensor::begin() {
    Wire.begin();

    if(!ina.init()) {
        imprimeln(F("Erro ao inicializar INA226!"));
        return false;
    }

    // Ajuste opcional: média, tempo de conversão etc.
    // ina.setAverage(INA226_AVG_16);
    // ina.setConversionTime(INA226_CONV_TIME_1100);

    return true;
}

void INA226Sensor::update() {
    ina.readAndClearFlags();
    current_mA = ina.getCurrent_mA();
    voltage_V = ina.getBusVoltage_V();
}

float INA226Sensor::readCurrent() {
    return current_mA;
}

float INA226Sensor::readVoltage() {
    return voltage_V;
}

