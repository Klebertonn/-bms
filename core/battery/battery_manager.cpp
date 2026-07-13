
#include "battery_manager.h"

#include "../../lib/hal/adc/adc_interface.h"
#include "../../lib/system/logger/logger.h"

extern IADC& adc;

void BatteryManager::init() {
    for (std::uint8_t i = 0; i < PACK_CELL_COUNT; ++i) {
        pack.cells[i].voltage = 0.0f;
        pack.cells[i].temperature = 0.0f;
        pack.cells[i].balancing = false;
        pack.cells[i].valid = false;
    }

    pack.totalVoltage = 0.0f;
    pack.averageVoltage = 0.0f;
    pack.minVoltage = 0.0f;
    pack.maxVoltage = 0.0f;
    pack.deltaVoltage = 0.0f;

    pack.current = 0.0f;
    pack.power = 0.0f;

    pack.averageTemperature = 0.0f;
    pack.maxTemperature = 0.0f;
    pack.minTemperature = 0.0f;

    pack.soc = 0;
    pack.soh = 100;

    pack.charging = false;
    pack.discharging = false;
    pack.balancing = false;

    pack.valid = false;
}

void BatteryManager::update() {
    // Orquestrador (domínio do pack): tensões + agregados + SOC/flags dependentes
    // Entradas (current/temperatures) são fornecidas via setCurrent/setTemperatures pelo App.

    // ======================
    // LEITURA DAS CÉLULAS
    // ======================
    for (std::uint8_t i = 0; i < PACK_CELL_COUNT; ++i) {
        const float voltage = adc.readCell(i);
        pack.cells[i].voltage = voltage;
        pack.cells[i].valid =
            voltage >= MIN_CELL_VOLTAGE &&
            voltage <= MAX_CELL_VOLTAGE;
    }

    validateCells();

    // Calcula agregado e tensões (min/max/delta/total, valid)
    calculatePack();


    // Potência depende de corrente e tensão do pack atualizados (domínio BatteryManager)
    pack.power = pack.totalVoltage * pack.current;

    // SOC é calculado com base na tensão (por enquanto, modelo simples)
    calculateSOC();
}

float BatteryManager::getCellVoltage(int index) const {
    if (index < 0 || index >= static_cast<int>(PACK_CELL_COUNT)) return 0.0f;
    return pack.cells[static_cast<std::uint8_t>(index)].voltage;
}

BatteryPack& BatteryManager::getPack() {
    return pack;
}

const BatteryPack& BatteryManager::getPack() const {
    return pack;
}

// ======================
// CÁLCULO DO PACK
// ======================
void BatteryManager::calculatePack() {
    float sum = 0.0f;

    // init com base na célula 0 (sem gambiarra)
    pack.minVoltage = pack.cells[0].voltage;
    pack.maxVoltage = pack.cells[0].voltage;

    pack.valid = pack.cells[0].valid;

    sum += pack.cells[0].voltage;

    for (std::uint8_t i = 1; i < PACK_CELL_COUNT; ++i) {
        const float v = pack.cells[i].voltage;
        sum += v;

        if (v < pack.minVoltage) pack.minVoltage = v;
        if (v > pack.maxVoltage) pack.maxVoltage = v;

        if (!pack.cells[i].valid) {
            pack.valid = false;
        }
    }

    // Temperaturas ainda não têm leitura dedicada via HAL
    // (mantêm zeros por enquanto)

    pack.totalVoltage = sum;
    pack.averageVoltage = sum / static_cast<float>(PACK_CELL_COUNT);
    pack.deltaVoltage = pack.maxVoltage - pack.minVoltage;
}

// ======================
// Domínio: Setters (App apenas fornece entradas)
// ======================
void BatteryManager::setCurrent(float current) {
    pack.current = current;
}

void BatteryManager::setTemperatures(float averageTemperature, float minTemperature, float maxTemperature) {
    pack.averageTemperature = averageTemperature;
    pack.maxTemperature = maxTemperature;
}

void BatteryManager::setTemperature(float temperature) {
    pack.averageTemperature = temperature;
    pack.minTemperature = temperature;
    pack.maxTemperature = temperature;
}

void BatteryManager::setSOC(uint8_t soc) {
    pack.soc = soc;
}

void BatteryManager::setSOH(uint8_t soh) {
    pack.soh = soh;
}

void BatteryManager::setCharging(bool enabled) {
    pack.charging = enabled;
}

void BatteryManager::setDischarging(bool enabled) {
    pack.discharging = enabled;
}

// ======================
// SOC (versão inicial)
// ======================
void BatteryManager::validateCells() {
    for (std::uint8_t i = 0; i < PACK_CELL_COUNT; ++i) {
        BatteryCell& cell = pack.cells[i];

        // Reset de falhas
        cell.valid = cell.valid; // mantém coerência

        // Flags de over/under-voltage
        // (BatteryCell no modelo atual não expõe overVoltage/underVoltage como flags).
        // Por enquanto, não impacta a telemetria agregada/prints.

    }
}

void BatteryManager::calculateSOC() {

    // Estimativa simplificada baseada em PACK_FULL_VOLTAGE e PACK_EMPTY_VOLTAGE
    const float v = pack.totalVoltage;

    // clamp
    if (v <= PACK_EMPTY_VOLTAGE) {
        pack.soc = 0;
        return;
    }

    if (v >= PACK_FULL_VOLTAGE) {
        pack.soc = 100;
        return;
    }

    // linear (0..100)
    const float ratio = (v - PACK_EMPTY_VOLTAGE) / (PACK_FULL_VOLTAGE - PACK_EMPTY_VOLTAGE);
    pack.soc = static_cast<std::uint8_t>(ratio * 100.0f);
}



