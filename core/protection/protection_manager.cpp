#include "protection_manager.h"

#include "../../config/config.h"

// Ajuste defensivo: garante que os limites existam mesmo se o include falhar no editor.
#ifndef MAX_CELL_VOLTAGE
#define MAX_CELL_VOLTAGE 4.20f
#endif
#ifndef MIN_CELL_VOLTAGE
#define MIN_CELL_VOLTAGE 3.00f
#endif
#ifndef MAX_TEMPERATURE
#define MAX_TEMPERATURE 60.0f
#endif
#ifndef MIN_TEMPERATURE
#define MIN_TEMPERATURE -20.0f
#endif
#ifndef MAX_CHARGE_CURRENT
#define MAX_CHARGE_CURRENT 30.0f
#endif
#ifndef MAX_DISCHARGE_CURRENT
#define MAX_DISCHARGE_CURRENT 30.0f
#endif


// Nesta etapa, o ProtectionManager controla somente as flags de MOSFET.

/*
 * Localiza o índice (1-based) da célula com a maior tensão válida.
 * Reutilizado para fornecer a origem do DTC de sobretensão.
 */
static std::uint8_t findMaxCellSource(const BatteryPack& pack)
{
    float maxV = -1e30f;
    std::uint8_t best = FAULT_SOURCE_GLOBAL;
    for (std::uint8_t i = 0; i < PACK_CELL_COUNT; ++i)
    {
        if (pack.cells[i].valid && pack.cells[i].voltage > maxV)
        {
            maxV = pack.cells[i].voltage;
            best = static_cast<std::uint8_t>(i + 1u); // 1-based
        }
    }
    return best;
}

/*
 * Localiza o índice (1-based) da célula com a menor tensão válida.
 * Reutilizado para fornecer a origem do DTC de subtensão.
 */
static std::uint8_t findMinCellSource(const BatteryPack& pack)
{
    float minV = 1e30f;
    std::uint8_t best = FAULT_SOURCE_GLOBAL;
    for (std::uint8_t i = 0; i < PACK_CELL_COUNT; ++i)
    {
        if (pack.cells[i].valid && pack.cells[i].voltage < minV)
        {
            minV = pack.cells[i].voltage;
            best = static_cast<std::uint8_t>(i + 1u); // 1-based
        }
    }
    return best;
}

void ProtectionManager::init()
{
    state = ProtectionState::NORMAL;

    chargeMosfet = true;
    dischargeMosfet = true;
}

void ProtectionManager::update(
    const BatteryPack& pack,
    FaultManager& faults)
{
    // =========================================================
    // 1. Detecção das condições físicas
    // =========================================================

    const bool overVoltage =
        pack.maxVoltage > MAX_CELL_VOLTAGE;

    const bool underVoltage =
        pack.minVoltage < MIN_CELL_VOLTAGE;

    const bool overTemperature =
        pack.maxTemperature > MAX_TEMPERATURE;

    const bool underTemperature =
        pack.minTemperature < MIN_TEMPERATURE;

const bool overCurrentCharge =
        pack.current > MAX_CHARGE_CURRENT;

    const bool overCurrentDischarge =
        pack.current < -MAX_DISCHARGE_CURRENT;

    // Curto-circuito: corrente muito acima do limite (ex.: 10x o máximo).
    // Detectado de forma determinística a partir da corrente medida.
    const bool shortCircuit =
        pack.current > (MAX_CHARGE_CURRENT * 10.0f) ||
        pack.current < -(MAX_DISCHARGE_CURRENT * 10.0f);

    // =========================================================
    // 2. Sincronização Protection -> DTC
    //
    // IMPORTANTE:
    // Só chama raiseFault() quando a falha ainda não está ativa.
    // Isso evita incrementar occurrence a cada ciclo do BMS.
    // =========================================================

    if (overVoltage)
    {
        if (!faults.hasFault(FaultCode::BMS_CELL_OVERVOLTAGE))
        {
            const std::uint8_t src = findMaxCellSource(pack);
            const float measured = (src != FAULT_SOURCE_GLOBAL)
                                       ? pack.cells[src - 1u].voltage
                                       : pack.maxVoltage;
            faults.raiseFault(FaultCode::BMS_CELL_OVERVOLTAGE,
                              FaultSeverity::CRITICAL,
                              FaultState::ACTIVE,
                              src, measured, MAX_CELL_VOLTAGE);
        }
    }
    else
    {
        faults.clearFault(FaultCode::BMS_CELL_OVERVOLTAGE);
    }

    if (underVoltage)
    {
        if (!faults.hasFault(FaultCode::BMS_CELL_UNDERVOLTAGE))
        {
            const std::uint8_t src = findMinCellSource(pack);
            const float measured = (src != FAULT_SOURCE_GLOBAL)
                                       ? pack.cells[src - 1u].voltage
                                       : pack.minVoltage;
            faults.raiseFault(FaultCode::BMS_CELL_UNDERVOLTAGE,
                              FaultSeverity::CRITICAL,
                              FaultState::ACTIVE,
                              src, measured, MIN_CELL_VOLTAGE);
        }
    }
    else
    {
        faults.clearFault(FaultCode::BMS_CELL_UNDERVOLTAGE);
    }

    if (overTemperature)
    {
        if (!faults.hasFault(FaultCode::BMS_OVER_TEMPERATURE))
        {
            faults.raiseFault(FaultCode::BMS_OVER_TEMPERATURE,
                              FaultSeverity::CRITICAL,
                              FaultState::ACTIVE,
                              FAULT_SOURCE_GLOBAL,
                              pack.maxTemperature, MAX_TEMPERATURE);
        }
    }
    else
    {
        faults.clearFault(FaultCode::BMS_OVER_TEMPERATURE);
    }

    if (underTemperature)
    {
        if (!faults.hasFault(FaultCode::BMS_LOW_TEMPERATURE))
        {
            faults.raiseFault(FaultCode::BMS_LOW_TEMPERATURE,
                              FaultSeverity::CRITICAL,
                              FaultState::ACTIVE,
                              FAULT_SOURCE_GLOBAL,
                              pack.minTemperature, MIN_TEMPERATURE);
        }
    }
    else
    {
        faults.clearFault(FaultCode::BMS_LOW_TEMPERATURE);
    }

if (shortCircuit)
    {
        if (!faults.hasFault(FaultCode::BMS_SHORT_CIRCUIT))
        {
            const float limit = (pack.current > 0.0f)
                                    ? (MAX_CHARGE_CURRENT * 10.0f)
                                    : (MAX_DISCHARGE_CURRENT * 10.0f);
            faults.raiseFault(FaultCode::BMS_SHORT_CIRCUIT,
                              FaultSeverity::FATAL,
                              FaultState::ACTIVE,
                              FAULT_SOURCE_GLOBAL,
                              pack.current, limit);
        }
    }
    else
    {
        faults.clearFault(FaultCode::BMS_SHORT_CIRCUIT);
    }

    if (overCurrentCharge || overCurrentDischarge)
    {
        if (!faults.hasFault(FaultCode::BMS_OVER_CURRENT))
        {
            const float limit = overCurrentCharge ? MAX_CHARGE_CURRENT
                                                  : MAX_DISCHARGE_CURRENT;
            faults.raiseFault(FaultCode::BMS_OVER_CURRENT,
                              FaultSeverity::CRITICAL,
                              FaultState::ACTIVE,
                              FAULT_SOURCE_GLOBAL,
                              pack.current, limit);
        }
    }
    else
    {
        faults.clearFault(FaultCode::BMS_OVER_CURRENT);
    }

    // =========================================================
    // 3. Estado de proteção
    // Prioridade:
    // OV > UV > temperatura > corrente
    // =========================================================

chargeMosfet = true;
    dischargeMosfet = true;
    state = ProtectionState::NORMAL;

    // Curto-circuito tem a maior prioridade de segurança.
    if (faults.hasFault(FaultCode::BMS_SHORT_CIRCUIT))
    {
        state = ProtectionState::SHORT_CIRCUIT;

        chargeMosfet = false;
        dischargeMosfet = false;
        return;
    }

    if (faults.hasFault(FaultCode::BMS_CELL_OVERVOLTAGE))
    {
        state = ProtectionState::OVER_VOLTAGE;

        chargeMosfet = false;
        dischargeMosfet = true;
        return;
    }

    if (faults.hasFault(FaultCode::BMS_CELL_UNDERVOLTAGE))
    {
        state = ProtectionState::UNDER_VOLTAGE;

        chargeMosfet = true;
        dischargeMosfet = false;
        return;
    }

    if (faults.hasFault(FaultCode::BMS_OVER_TEMPERATURE))
    {
        state = ProtectionState::OVER_TEMPERATURE;

        chargeMosfet = false;
        dischargeMosfet = false;
        return;
    }

    if (faults.hasFault(FaultCode::BMS_LOW_TEMPERATURE))
    {
        state = ProtectionState::UNDER_TEMPERATURE;

        chargeMosfet = false;
        dischargeMosfet = false;
        return;
    }

    if (faults.hasFault(FaultCode::BMS_OVER_CURRENT))
    {
        state = overCurrentCharge
                    ? ProtectionState::OVER_CURRENT_CHARGE
                    : ProtectionState::OVER_CURRENT_DISCHARGE;

        chargeMosfet = false;
        dischargeMosfet = false;
        return;
    }
}





ProtectionState ProtectionManager::getState() const
{
    return state;
}

bool ProtectionManager::chargeEnabled() const
{
    return chargeMosfet;
}

bool ProtectionManager::dischargeEnabled() const
{
    return dischargeMosfet;
}

bool ProtectionManager::isFault() const
{
    return state != ProtectionState::NORMAL;
}

void ProtectionManager::checkVoltage(const BatteryPack& pack)
{
    if (pack.maxVoltage > MAX_CELL_VOLTAGE)

    {
        state = ProtectionState::OVER_VOLTAGE;

        // stub logger: em implementação real, use o ILogger/Logger real
        // Logger::warning("OVP detected");
        return;
    }

    if (pack.minVoltage < MIN_CELL_VOLTAGE)
    {
        state = ProtectionState::UNDER_VOLTAGE;

        // Logger::warning("UVP detected");
    }
}

void ProtectionManager::checkTemperature(const BatteryPack& pack)
{

    if (state != ProtectionState::NORMAL)
        return;

    if (pack.maxTemperature > MAX_TEMPERATURE)
    {
        state = ProtectionState::OVER_TEMPERATURE;


        // Logger::warning("OTP detected");
        return;
    }

    if (pack.minTemperature < MIN_TEMPERATURE)

    {
        state = ProtectionState::UNDER_TEMPERATURE;

        // Logger::warning("UTP detected");
    }
}

void ProtectionManager::checkCurrent(const BatteryPack& pack)
{
    if (state != ProtectionState::NORMAL)
        return;

    if (pack.current > MAX_CHARGE_CURRENT)

    {
        state = ProtectionState::OVER_CURRENT_CHARGE;

        // Logger::warning("OCC detected");
        return;
    }

    if (pack.current < -MAX_DISCHARGE_CURRENT)
    {
        state = ProtectionState::OVER_CURRENT_DISCHARGE;

        // Logger::warning("OCD detected");
    }
}

void ProtectionManager::applyProtection()
{
    switch (state)
    {
    case ProtectionState::NORMAL:
        chargeMosfet = true;
        dischargeMosfet = true;
        break;

    case ProtectionState::OVER_VOLTAGE:
        chargeMosfet = false;
        dischargeMosfet = true;
        break;

    case ProtectionState::UNDER_VOLTAGE:
        chargeMosfet = true;
        dischargeMosfet = false;
        break;

    case ProtectionState::OVER_TEMPERATURE:
    case ProtectionState::OVER_CURRENT_CHARGE:
    case ProtectionState::OVER_CURRENT_DISCHARGE:
    case ProtectionState::SHORT_CIRCUIT:
        chargeMosfet = false;
        dischargeMosfet = false;
        break;

    default:
        chargeMosfet = false;
        dischargeMosfet = false;
        break;
    }
}

