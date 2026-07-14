#pragma once

#include "fault_flags.h"
#include "../battery/battery_pack.h"

class FaultManager
{
public:
    void init();

    void clear();

    void evaluate(const BatteryPack& pack);

    void addFault(FaultFlag fault);

    bool hasFault() const;

    bool hasFault(FaultFlag fault) const;

    uint32_t getFaults() const;

private:
    uint32_t flags = FAULT_NONE;
};




