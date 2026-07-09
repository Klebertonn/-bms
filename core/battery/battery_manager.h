#pragma once

#include <stdint.h>

#ifndef BMS_MAX_CELLS
#define BMS_MAX_CELLS 16
#endif

/// Dados de uma célula
struct CellData {
    float voltage; // [V]
};

/// Dados agregados do pack
struct PackData {
    float totalVoltage;   // [V]
    float minVoltage;     // [V]
    float maxVoltage;     // [V]
    float deltaVoltage;   // [V]
    float current;        // [A] (placeholder inicialmente)
    float temperature;    // [°C] (placeholder inicialmente)
    uint8_t soc;           // [%]
};

/**
 * @brief BatteryManager (CORE)
 *
 * Responsável por:
 * - ler tensões das células via HAL
 * - calcular estatísticas do pack (min/max/delta/sum)
 * - calcular SOC inicial (estimativa simplificada)
 * - manter estado global da bateria para consumo por outros módulos
 *
 * Importante:
 * - Este módulo NÃO implementa proteção ainda.
 * - NÃO corta MOSFET.
 * - NÃO realiza balanceamento.
 * - NÃO envia CAN/BLE.
 */
class BatteryManager {
public:
    void init();
    void update();

    float getCellVoltage(int index) const;
    PackData getPackData() const;

private:
    CellData cells[BMS_MAX_CELLS]{};
    PackData pack{};

    void calculatePack();
    void calculateSOC();
};

