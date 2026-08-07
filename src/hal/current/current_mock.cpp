#include "../../../lib/hal/current/current_interface.h"


class MockCurrentSensor : public ICurrentSensor
{
public:

    bool begin() override
    {
        // Mock: sempre inicializado.
        return true;
    }

    float readCurrentA() override
    {
        return 2.35f;
    }


    float readShuntVoltageMv() override
    {
        return 25.0f;
    }
};


static MockCurrentSensor sensor;


ICurrentSensor& currentSensor = sensor;