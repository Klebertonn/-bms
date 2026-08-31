#include "coulomb_counter.h"

void CoulombCounter::init(float capacityAh)
{
    nominalCapacity = capacityAh;
    consumedAh = 0.0f;
    chargedAh = 0.0f;
}

void CoulombCounter::update(float currentA,
                             float dtSeconds)
{
    float deltaAh = currentA * dtSeconds / 3600.0f;

    if (currentA > 0.0f)
        chargedAh += deltaAh;
    else
        consumedAh += -deltaAh;
}

float CoulombCounter::getConsumedAh() const
{
    return consumedAh;
}

float CoulombCounter::getChargedAh() const
{
    return chargedAh;
}

void CoulombCounter::reset()
{
    consumedAh = 0.0f;
    chargedAh = 0.0f;
}

