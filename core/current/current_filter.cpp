#include "current_filter.h"

void CurrentFilter::init()
{
    filtered = 0.0f;
}

float CurrentFilter::update(float sample)
{
    filtered = alpha * sample + (1.0f - alpha) * filtered;
    return filtered;
}

