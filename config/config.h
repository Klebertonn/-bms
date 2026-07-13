#pragma once

#include <cstdint>

/*
 * ==========================================================
 * Configuração Geral do Pack
 * ==========================================================
 */

// Número de células em série
constexpr std::uint8_t PACK_CELL_COUNT = 3;


/*
 * ==========================================================
 * Limites de Tensão por Célula (V)
 * ==========================================================
 */

constexpr float MAX_CELL_VOLTAGE = 4.20f;
constexpr float MIN_CELL_VOLTAGE = 3.00f;

// Validação/saúde por célula (Passo 1.2)
#define CELL_OVERVOLTAGE_LIMIT   4.25f
#define CELL_UNDERVOLTAGE_LIMIT  3.00f


constexpr float CELL_NOMINAL_VOLTAGE = 3.70f;


/*
 * ==========================================================
 * Limites de Temperatura (°C)
 * ==========================================================
 */

constexpr float MAX_TEMPERATURE = 60.0f;
constexpr float MIN_TEMPERATURE = -20.0f;

constexpr float MAX_BALANCE_TEMP = 45.0f;


/*
 * ==========================================================
 * Limites de Corrente (A)
 * ==========================================================
 */

constexpr float MAX_CHARGE_CURRENT = 30.0f;
constexpr float MAX_DISCHARGE_CURRENT = 30.0f;


/*
 * ==========================================================
 * Balanceamento
 * ==========================================================
 */

// Inicia balanceamento quando ΔV ≥ 20 mV
constexpr float BALANCE_START_DELTA = 0.020f;

// Para balanceamento quando ΔV ≤ 10 mV
constexpr float BALANCE_STOP_DELTA = 0.010f;

// Tempo máximo de balanceamento
constexpr std::uint32_t MAX_BALANCE_TIME_MS = 600000;


/*
 * ==========================================================
 * Atualização do Sistema
 * ==========================================================
 */

constexpr std::uint32_t MAIN_LOOP_PERIOD_MS = 100;
constexpr std::uint32_t LOGGER_PERIOD_MS = 1000;
constexpr std::uint32_t TELEMETRY_PERIOD_MS = 500;


/*
 * ==========================================================
 * SOC
 * ==========================================================
 */

constexpr float PACK_FULL_VOLTAGE = 12.60f;
constexpr float PACK_EMPTY_VOLTAGE = 9.00f;