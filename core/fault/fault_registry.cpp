#include "fault_registry.h"

#include <cstring>

/*
 * ==========================================================
 * Tabela de registro de códigos DTC.
 * Mantida em ordem estável. A posição não muda o significado;
 * o código é a fonte de verdade.
 * ==========================================================
 */
static const FaultMeta kFaultTable[] = {
    /* ---- Battery ---- */
    {FaultCode::BMS_BAT_OVERVOLTAGE,  "Battery over voltage",  FaultSeverity::CRITICAL},
    {FaultCode::BMS_BAT_UNDERVOLTAGE, "Battery under voltage", FaultSeverity::CRITICAL},

    /* ---- Cell ---- */
    {FaultCode::BMS_CELL_OVERVOLTAGE,  "Cell over voltage",  FaultSeverity::CRITICAL},
    {FaultCode::BMS_CELL_UNDERVOLTAGE, "Cell under voltage", FaultSeverity::CRITICAL},

    /* ---- Temperature ---- */
    {FaultCode::BMS_OVER_TEMPERATURE, "Over temperature", FaultSeverity::CRITICAL},
    {FaultCode::BMS_LOW_TEMPERATURE,  "Low temperature",  FaultSeverity::WARNING},

    /* ---- Current ---- */
    {FaultCode::BMS_OVER_CURRENT, "Over current",   FaultSeverity::CRITICAL},
    {FaultCode::BMS_SHORT_CIRCUIT,"Short circuit",  FaultSeverity::FATAL},

    /* ---- Communication ---- */
    {FaultCode::BMS_CAN_FAILURE,   "CAN communication failure", FaultSeverity::ERROR},
    {FaultCode::BMS_SENSOR_FAILURE,"Sensor failure",            FaultSeverity::ERROR},

    /* ---- Storage ---- */
    {FaultCode::BMS_STORAGE_FAILURE, "Storage failure", FaultSeverity::ERROR},
};

const FaultMeta* FaultRegistry::table()
{
    return kFaultTable;
}

std::size_t FaultRegistry::size()
{
    return sizeof(kFaultTable) / sizeof(kFaultTable[0]);
}

std::size_t FaultRegistry::findIndex(FaultCode code)
{
    for (std::size_t i = 0; i < size(); ++i)
    {
        if (kFaultTable[i].code == code)
        {
            return i;
        }
    }
    return NOT_FOUND;
}

const char* FaultRegistry::findDescription(FaultCode code)
{
    const std::size_t idx = findIndex(code);
    if (idx == NOT_FOUND)
    {
        return nullptr;
    }
    return kFaultTable[idx].description;
}

FaultSeverity FaultRegistry::findSeverity(FaultCode code)
{
    const std::size_t idx = findIndex(code);
    if (idx == NOT_FOUND)
    {
        return FaultSeverity::ERROR;
    }
    return kFaultTable[idx].defaultSeverity;
}

const char* FaultRegistry::codeToString(FaultCode code)
{
    const char* desc = findDescription(code);
    return (desc != nullptr) ? desc : "Unknown fault";
}

FaultCode FaultRegistry::stringToCode(const char* text)
{
    if (text == nullptr)
    {
        return FaultCode::NONE;
    }

    for (std::size_t i = 0; i < size(); ++i)
    {
        if (std::strcmp(kFaultTable[i].description, text) == 0)
        {
            return kFaultTable[i].code;
        }
    }

    return FaultCode::NONE;
}
