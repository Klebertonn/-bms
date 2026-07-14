#include "fault_manager.h"

void FaultManager::init()
{
    // Mantém compatível com o padrão dos outros managers.
    clear();
}

void FaultManager::clear()
{
    flags = FAULT_NONE;
}

void FaultManager::addFault(FaultFlag fault)
{
    flags |= static_cast<uint32_t>(fault);
}

bool FaultManager::hasFault() const
{
    return flags != FAULT_NONE;
}

bool FaultManager::hasFault(FaultFlag fault) const
{
    return (flags & static_cast<uint32_t>(fault)) != 0u;
}

uint32_t FaultManager::getFaults() const
{
    return flags;
}

void FaultManager::evaluate(const BatteryPack& pack)
{
    clear();

    if (pack.maxVoltage > CELL_OVERVOLTAGE_LIMIT)
    {
        addFault(FAULT_CELL_OVERVOLTAGE);
    }

    if (pack.minVoltage < CELL_UNDERVOLTAGE_LIMIT)
    {
        addFault(FAULT_CELL_UNDERVOLTAGE);
    }

    if (pack.averageTemperature > MAX_TEMPERATURE)
    {
        addFault(FAULT_OVER_TEMPERATURE);
    }

    if (pack.current > MAX_CHARGE_CURRENT)
    {
        addFault(FAULT_OVER_CURRENT);
    }

    // NOTA: os demais faults (UNDER_TEMPERATURE, SENSOR_ERROR, etc)
    // serão adicionados nas próximas etapas.
}


