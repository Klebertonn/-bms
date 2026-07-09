#include "../../../lib/hal/temperature/temperature_interface.h"

class MockTemperatureSensor : public ITemperatureSensor
{
public:
    float readTemperature(int sensor) override
    {
        switch (sensor)
        {
            case 0: return 24.5f;
            case 1: return 25.0f;
            case 2: return 25.8f;
            case 3: return 26.2f;
            default: return 25.0f;
        }
    }
};

static MockTemperatureSensor sensor;

ITemperatureSensor& temperatureSensor = sensor;