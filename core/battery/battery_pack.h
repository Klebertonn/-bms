#pragma once

#include <cstdint>

#include "../../config/config.h"

/*
 * ==========================================================
 * Dados de uma célula
 * ==========================================================
 */

struct BatteryCell
{
    float voltage = 0.0f;
    float temperature = 0.0f;

    bool balancing = false;
    bool valid = false;
};


/*
 * ==========================================================
 * Estado do Pack de Baterias
 * ==========================================================
 */

struct BatteryPack
{
    /*
     * Células
     */
    BatteryCell cells[PACK_CELL_COUNT];

    /*
     * Tensões
     */
    float totalVoltage = 0.0f;
    float averageVoltage = 0.0f;
    float minVoltage = 0.0f;
    float maxVoltage = 0.0f;
    float deltaVoltage = 0.0f;

    /*
     * Corrente e potência
     */
    float current = 0.0f;
    float power = 0.0f;

    /*
     * Temperaturas
     */
    float averageTemperature = 0.0f;
    float maxTemperature = 0.0f;
    float minTemperature = 0.0f;

    /*
     * Estado da bateria
     */
    std::uint8_t soc = 0;
    std::uint8_t soh = 100;

    /*
     * Estado operacional
     */
    bool charging = false;
    bool discharging = false;
    bool balancing = false;

    /*
     * Estado do pack
     */
    bool valid = false;
};