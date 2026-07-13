#include "lib/hal/temperature/temperature_interface.h"

class MockTemperatureSensor : public ITemperatureSensor
{
public:
    float readTemperature(int sensorIndex) override
    {
        (void)sensorIndex;
        return 25.0f;
    }
};

static MockTemperatureSensor g_mock;

ITemperatureSensor& temperatureSensor = g_mock;

