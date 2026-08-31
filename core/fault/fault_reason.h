#pragma once

#include <cstdint>

enum class FaultReason
{
    NONE = 0,

    CELL_OVERVOLTAGE,
    CELL_UNDERVOLTAGE,

    PACK_OVERVOLTAGE,
    PACK_UNDERVOLTAGE,

    OVERCURRENT_CHARGE,
    OVERCURRENT_DISCHARGE,

    OVERTEMPERATURE,
    UNDERTEMPERATURE,

    ADC_FAILURE,
    SENSOR_FAILURE,
    COMMUNICATION_TIMEOUT
};

static inline const char* faultReasonToString(FaultReason reason)
{
    switch (reason)
    {
        case FaultReason::CELL_OVERVOLTAGE:
            return "CELL_OVERVOLTAGE";
        case FaultReason::CELL_UNDERVOLTAGE:
            return "CELL_UNDERVOLTAGE";
        case FaultReason::PACK_OVERVOLTAGE:
            return "PACK_OVERVOLTAGE";
        case FaultReason::PACK_UNDERVOLTAGE:
            return "PACK_UNDERVOLTAGE";
        case FaultReason::OVERCURRENT_CHARGE:
            return "OVERCURRENT_CHARGE";
        case FaultReason::OVERCURRENT_DISCHARGE:
            return "OVERCURRENT_DISCHARGE";
        case FaultReason::OVERTEMPERATURE:
            return "OVERTEMPERATURE";
        case FaultReason::UNDERTEMPERATURE:
            return "UNDERTEMPERATURE";
        case FaultReason::ADC_FAILURE:
            return "ADC_FAILURE";
        case FaultReason::SENSOR_FAILURE:
            return "SENSOR_FAILURE";
        case FaultReason::COMMUNICATION_TIMEOUT:
            return "COMMUNICATION_TIMEOUT";
        case FaultReason::NONE:
        default:
            return "NONE";
    }
}

struct FaultInfo
{
    bool active = false;

    FaultReason reason = FaultReason::NONE;

    // Código pronto para telemetria/can.
    // (Ex.: 0x0101 para CELL_OVERVOLTAGE)
    std::uint16_t code = 0;

    // Fonte: célula (1..N) quando aplicável; caso contrário 0xFF
    std::uint8_t source = 0xFF;

    float value = 0.0f;
    float limit = 0.0f;

    // Milliseconds desde o boot (ou primeiro evento)
    std::uint32_t timestamp = 0;
};


