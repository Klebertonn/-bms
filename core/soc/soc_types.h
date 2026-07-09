#pragma once

#include <stdint.h>

struct SOCData
{
    float soc;                 // %
    float soh;                 // %
    float remainingCapacityAh; // Ah
    float fullCapacityAh;      // Ah

    float energyRemainingWh;
    float energyConsumedWh;

    float chargedAh;
    float dischargedAh;

    uint32_t cycleCount;
};

