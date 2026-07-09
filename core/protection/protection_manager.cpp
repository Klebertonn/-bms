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

void ProtectionManager::update(const PackData& pack)
{
    state = ProtectionState::NORMAL;

    checkVoltage(pack);
    checkTemperature(pack);
    checkCurrent(pack);

    applyProtection();
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

void ProtectionManager::checkVoltage(const PackData& pack)
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

void ProtectionManager::checkTemperature(const PackData& pack)
{
    if (state != ProtectionState::NORMAL)
        return;

    if (pack.temperature > MAX_TEMPERATURE)
    {
        state = ProtectionState::OVER_TEMPERATURE;

        // Logger::warning("OTP detected");
        return;
    }

    if (pack.temperature < MIN_TEMPERATURE)
    {
        state = ProtectionState::UNDER_TEMPERATURE;

        // Logger::warning("UTP detected");
    }
}

void ProtectionManager::checkCurrent(const PackData& pack)
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

