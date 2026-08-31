#include "mock_adc.h"

float MockADC::readCell(int index)
{
    switch (index)
    {
        case 0:
            return 4.30f;   // <-- Sobretensão

        case 1:
            return 4.18f;

        case 2:
            return 4.19f;

        default:
            return 0.0f;
    }
}

static MockADC adcMock;

// Variável global usada pelo BatteryManager
IADC& adc = adcMock;