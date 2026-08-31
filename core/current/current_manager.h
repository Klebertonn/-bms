#pragma once

#include "current_types.h"
#include "current_filter.h"

class CurrentManager
{
public:
    void init();

    // packVoltage é usado apenas para calcular potência (P = V * I)
    void update(float packVoltage);

    CurrentData getData() const;

private:
    CurrentData data;
    CurrentFilter filter;

    void determineDirection();
    void calculatePower(float voltage);
};

