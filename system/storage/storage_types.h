#pragma once

#include <stdint.h>

struct StorageData
{
    uint32_t version = 0;

    float soc = 0.0f;
    float soh = 0.0f;

    uint32_t cycleCount = 0;

    float batteryCapacityAh = 0.0f;

    float overVoltageLimit = 0.0f;
    float underVoltageLimit = 0.0f;
    float overTemperatureLimit = 0.0f;
    float overCurrentLimit = 0.0f;

    uint32_t crc = 0;
};

