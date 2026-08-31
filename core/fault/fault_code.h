#pragma once

#include <cstdint>

/*
 * ==========================================================
 * DTC – Diagnostic Trouble Code
 * ==========================================================
 * Códigos padronizados de falha do sistema BMS.
 *
 * Cada código identifica de forma única uma condição de falha.
 * Valores são estáveis: uma vez publicados, não mudam de significado.
 *
 * Este arquivo pertence ao domínio (core/fault) e NÃO depende de
 * hardware, ESP32, HAL ou drivers.
 * ==========================================================
 */

enum class FaultCode : std::uint16_t
{
    NONE = 0,

    /* ------------------- Battery ------------------- */
    BMS_BAT_OVERVOLTAGE,    // Bateria sobretensão
    BMS_BAT_UNDERVOLTAGE,   // Bateria subtensão

    /* ------------------- Cell ------------------- */
    BMS_CELL_OVERVOLTAGE,   // Célula sobretensão
    BMS_CELL_UNDERVOLTAGE,  // Célula subtensão

    /* ------------------- Temperature ------------------- */
    BMS_OVER_TEMPERATURE,   // Temperatura acima do limite
    BMS_LOW_TEMPERATURE,    // Temperatura abaixo do limite

    /* ------------------- Current ------------------- */
    BMS_OVER_CURRENT,       // Sobrecorrente
    BMS_SHORT_CIRCUIT,      // Curto-circuito

    /* ------------------- Communication ------------------- */
    BMS_CAN_FAILURE,        // Falha de comunicação CAN
    BMS_SENSOR_FAILURE,     // Falha de sensor

    /* ------------------- Storage ------------------- */
    BMS_STORAGE_FAILURE     // Falha de armazenamento
};
