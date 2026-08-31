#pragma once

#include <cstdint>

#include "../../core/battery/battery_pack.h"
#include "../../core/fault/fault_reason.h"

/*
 * ==========================================================
 * BLE Protocol — Serialização dos dados do BMS
 * ==========================================================
 * Converte os dados de domínio (BatteryPack, FaultInfo, etc.)
 * em payloads binários para as características GATT.
 *
 * Escalas definidas (LI-Ion BMS):
 *   - Tensão    : 0.01 V    (uint16)
 *   - Corrente  : 0.01 A    (int16, sinal)
 *   - Temperatura : 0.1 °C  (int16)
 *   - SOC/SOH   : %         (uint8)
 * ==========================================================
 */

// Tamanho máximo dos payloads serializados
constexpr std::uint8_t BLE_PAYLOAD_MAX = 20;

class BLEProtocol
{
public:
    // Empacota tensão em uint16 (escala 0.01 V)
    static std::uint16_t packFloat(float value, float scale);

    // Serializa dados do pack (voltage, current, soc, soh) em buffer.
    // @return número de bytes escritos.
    static std::uint8_t encodeBattery(const BatteryPack& pack, std::uint8_t* out);

    // Serializa estado do BMS (1 byte).
    static std::uint8_t encodeState(std::uint8_t state, std::uint8_t* out);

    // Serializa falha ativa (código, razão, fonte, valor, limite).
    static std::uint8_t encodeFault(const FaultInfo& fi, std::uint8_t* out);

    // Serializa temperaturas (média, máx, mín).
    static std::uint8_t encodeTemperature(float avg, float maxV, float minV, std::uint8_t* out);

    // Serializa corrente e tensão de shunt.
    static std::uint8_t encodeCurrent(float currentA, float shuntMv, std::uint8_t* out);

    // Serializa limites de configuração.
    static std::uint8_t encodeConfig(float maxV, float minV, float maxT, float maxI, std::uint8_t* out);

    // Serializa um registro de log (timestamp + código).
    static std::uint8_t encodeLog(std::uint32_t timestamp, std::uint16_t code, std::uint8_t* out);
};
