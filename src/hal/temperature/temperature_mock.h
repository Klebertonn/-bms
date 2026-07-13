#pragma once

#include "lib/hal/temperature/temperature_interface.h"

class MockTemperatureSensor : public ITemperatureSensor
{
public:
    float readTemperature(int sensorIndex) override;
};

