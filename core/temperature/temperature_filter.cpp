#include "temperature_filter.h"

void TemperatureFilter::init()
{
    filtered = 25.0f;
}

float TemperatureFilter::update(float sample)
{
    filtered = alpha * sample + (1.0f - alpha) * filtered;
    return filtered;
}

