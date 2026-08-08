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

void ProtectionManager::init()
{
    state = ProtectionState::NORMAL;

    chargeMosfet = true;
    dischargeMosfet = true;
}

void ProtectionManager::update(
    BatteryPack& pack,
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
            faults.raiseFault(FaultCode::BMS_CELL_OVERVOLTAGE);
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
            faults.raiseFault(FaultCode::BMS_CELL_UNDERVOLTAGE);
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
            faults.raiseFault(FaultCode::BMS_OVER_TEMPERATURE);
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
            faults.raiseFault(FaultCode::BMS_LOW_TEMPERATURE);
        }
    }
    else
    {
        faults.clearFault(FaultCode::BMS_LOW_TEMPERATURE);
    }

    if (overCurrentCharge || overCurrentDischarge)
    {
        if (!faults.hasFault(FaultCode::BMS_OVER_CURRENT))
        {
            faults.raiseFault(FaultCode::BMS_OVER_CURRENT);
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

