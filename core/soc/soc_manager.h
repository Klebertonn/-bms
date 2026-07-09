#pragma once

#include "soc_types.h"
#include "coulomb_counter.h"

#include "../battery/battery_manager.h"

class SOCManager
{
public:
    void init();

    void update(const PackData& pack,
                float deltaTimeSeconds);

    SOCData getData() const;

private:
    SOCData data{};
    CoulombCounter counter;

    // Estimações
    void calculateSOC(const PackData& pack);
    void calculateSOH();
    void calculateCycles();
    void calculateEnergy(const PackData& pack);

    // Utilitários
    float clampPercent(float x) const;
};

