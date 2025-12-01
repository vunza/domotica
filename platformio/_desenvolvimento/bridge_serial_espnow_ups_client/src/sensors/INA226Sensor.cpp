// INA226 current and voltage sensor
#include "INA226Sensor.h"

INA226Sensor::INA226Sensor(uint8_t address): ina(address), current_mA(0), voltage_V(0) {}

bool INA226Sensor::begin() {
    Wire.begin();

    if(!ina.init()) {
        imprimeln(F("Erro ao inicializar INA226!"));
        return false;
    }


    ////////////////////////////
    // CALIBRAÇÃO DA CORRENTE //
    ////////////////////////////
    ina.setAverage(INA226_AVERAGE_16);        // Média de 16 amostras (reduz ruído)
    //ina.setConversionTime(CONV_TIME_1100US); // Tempo de conversão
    ina.setMeasureMode(INA226_CONTINUOUS);    // Modo de medição contínua

    float resistenciaShunt = 0.1;   // Resistência de shunt (R100)
    float correnteMaxima = 3.0;     // Corrente máxima esperada em Amperes (ex: 2A)
    
    // Esta função configura o registro CAL interno do chip.
    // É essencial chamá-la antes de setCorrectionFactor().
    ina.setResistorRange(resistenciaShunt, correnteMaxima);  
    delay(2000);
    
    // APLICAÇÃO DO FATOR DE CORREÇÃO (CALIBRAÇÃO FINA)
    float meuFatorDeCorrecao =  1.2334;; // corrente_multimetro / corrente_ina    
    ina.setCorrectionFactor(meuFatorDeCorrecao);    

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

