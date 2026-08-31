#pragma once

#include <stdint.h>

#ifndef BMS_MAX_CELLS
#define BMS_MAX_CELLS 16
#endif

#include "battery_pack.h"

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

    BatteryPack& getPack();
    const BatteryPack& getPack() const;

    // Domínio: App apenas fornece entradas; BatteryManager monta/atualiza o BatteryPack.
    void setCurrent(float current);
    void setTemperatures(float averageTemperature, float minTemperature, float maxTemperature);
    void setTemperature(float temperature); // compat (mapeia para average)
    void setSOC(uint8_t soc);
    void setSOH(uint8_t soh);

    void setCharging(bool enabled);
    void setDischarging(bool enabled);

private:
    BatteryPack pack{};

    void validateCells();

    void calculatePack();
    void calculateSOC();
};




