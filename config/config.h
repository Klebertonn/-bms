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


/*
 * ==========================================================
 * Configuração de Hardware (ESP32 real)
 * ==========================================================
 * Todos os pinos e endereços de hardware ficam centralizados aqui.
 * A HAL (src/hal/esp32/) consome apenas estas constantes.
 * Alterar um pino = alterar somente este arquivo.
 *
 * Apenas compilado no target esp32dev (via build_src_filter).
 * Não afeta o ambiente native (desktop/mock).
 * ==========================================================
 */

constexpr std::uint8_t   HW_I2C_SDA_PIN       = 21;
constexpr std::uint8_t   HW_I2C_SCL_PIN       = 22;

constexpr std::uint8_t   HW_MOSFET_CHARGE_PIN = 26;
constexpr std::uint8_t   HW_MOSFET_DISCHARGE_PIN = 27;
constexpr std::uint8_t   HW_MOSFET_BALANCE_PIN   = 14;

constexpr std::uint8_t   HW_RELAY_PIN         = 32;

constexpr std::uint8_t   HW_CAN_TX_PIN        = 5;
constexpr std::uint8_t   HW_CAN_RX_PIN        = 4;

constexpr std::uint8_t   HW_NTC_ADC_CHANNEL   = 0;   // ADC1_CH0 (GPIO 36)

constexpr std::uint8_t   HW_CAN_TX_GPIO       = HW_CAN_TX_PIN;
constexpr std::uint8_t   HW_CAN_RX_GPIO       = HW_CAN_RX_PIN;

// Endereços I²C
constexpr std::uint8_t   HW_OLED_I2C_ADDR     = 0x3C;
constexpr std::uint8_t   HW_INA219_I2C_ADDR   = 0x40;
constexpr std::uint8_t   HW_MAX17048_I2C_ADDR = 0x36;

// NTC: parâmetros da série Steinhart-Hart (exemplo)
constexpr float HW_NTC_SERIES_RESISTOR_OHM = 10000.0f;
constexpr float HW_NTC_NOMINAL_RESISTOR_OHM = 10000.0f;
constexpr float HW_NTC_NOMINAL_TEMP_C = 25.0f;
constexpr float HW_NTC_B_COEFFICIENT = 3950.0f;

// INA219: resistência do shunt (Ω) e corrente máx (A)
constexpr float HW_INA219_SHUNT_OHM = 0.001f;
constexpr float HW_INA219_MAX_CURRENT_A = 30.0f;

// OLED: dimensões (SSD1306)
constexpr std::uint16_t HW_OLED_WIDTH = 128;
constexpr std::uint16_t HW_OLED_HEIGHT = 64;
