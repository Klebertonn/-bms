#pragma once

#include "lib/hal/adc/adc_interface.h"

class MockADC : public IADC
{
public:

    float readCell(int index) override;
};