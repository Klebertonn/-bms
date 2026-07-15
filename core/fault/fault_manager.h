#pragma once

#include "fault_flags.h"
#include "fault_reason.h"
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

    // Novo: detalhe industrial da falha.
    const FaultInfo& getFaultInfo() const { return faultInfo_; }

private:
    uint32_t flags = FAULT_NONE;
    FaultInfo faultInfo_{};
};






