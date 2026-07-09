#pragma once

#include "temperature_types.h"
#include "temperature_filter.h"

class TemperatureManager
{
public:
    void init();

    void update();

    TemperatureData getData() const;

private:
    TemperatureData data{};

    TemperatureFilter filters[MAX_TEMP_SENSORS];

    void calculateStatistics();
    void detectFailures();
};

