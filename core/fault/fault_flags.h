#pragma once

#include <cstdint>

enum FaultFlag : uint32_t
{
    FAULT_NONE = 0u,

    FAULT_CELL_OVERVOLTAGE = (1u << 0),
    FAULT_CELL_UNDERVOLTAGE = (1u << 1),

    FAULT_OVER_TEMPERATURE = (1u << 2),
    FAULT_UNDER_TEMPERATURE = (1u << 3),

    FAULT_OVER_CURRENT = (1u << 4),

    FAULT_SENSOR_ERROR = (1u << 5)
};


