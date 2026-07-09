#include "battery_manager.h"

#include "../../lib/hal/adc/adc_interface.h"
#include "../../lib/system/logger/logger.h"

extern IADC& adc;

void BatteryManager::init() {
    for (int i = 0; i < BMS_MAX_CELLS; i++) {
        cells[i].voltage = 0.0f;
    }

    pack.totalVoltage = 0.0f;
    pack.minVoltage = 0.0f;
    pack.maxVoltage = 0.0f;
    pack.deltaVoltage = 0.0f;
    pack.current = 0.0f;
    pack.temperature = 0.0f;
    pack.soc = 0;
}

void BatteryManager::update() {
    // ======================
    // LEITURA DAS CÉLULAS
    // ======================
    for (int i = 0; i < BMS_MAX_CELLS; i++) {
        cells[i].voltage = adc.readCell(i);
    }

    calculatePack();
    calculateSOC();
}

float BatteryManager::getCellVoltage(int index) const {
    if (index < 0 || index >= BMS_MAX_CELLS) return 0.0f;
    return cells[index].voltage;
}

PackData BatteryManager::getPackData() const {
    return pack;
}

// ======================
// CÁLCULO DO PACK
// ======================
void BatteryManager::calculatePack() {
    float sum = 0.0f;
    pack.minVoltage = 10.0f; // valor inicial
    pack.maxVoltage = 0.0f;

    for (int i = 0; i < BMS_MAX_CELLS; i++) {
        float v = cells[i].voltage;
        sum += v;

        if (v < pack.minVoltage) pack.minVoltage = v;
        if (v > pack.maxVoltage) pack.maxVoltage = v;
    }

    pack.totalVoltage = sum;
    pack.deltaVoltage = pack.maxVoltage - pack.minVoltage;
}

// ======================
// SOC (versão inicial)
// ======================
void BatteryManager::calculateSOC() {
    // Versão simples (estimativa por tensão do pack)
    float v = pack.totalVoltage;

    if (v >= 12.6f) pack.soc = 100;
    else if (v >= 12.0f) pack.soc = 75;
    else if (v >= 11.5f) pack.soc = 50;
    else if (v >= 11.0f) pack.soc = 25;
    else pack.soc = 5;
}

