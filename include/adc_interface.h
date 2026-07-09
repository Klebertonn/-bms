#pragma once

#include <stdint.h>

class IADC
{
public:

    virtual ~IADC() = default;

    virtual float readVoltage(uint8_t channel) = 0;
};
