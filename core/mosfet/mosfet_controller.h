#pragma once

#include "../../core/state/bms_state_manager.h"

class MosfetController
{
public:
    void init();

    // Estado industrial (derivado de BmsState)
    void applyState(BmsState state);

    // Para telemetria
    bool chargeEnabled() const { return chargeMosfet_; }
    bool dischargeEnabled() const { return dischargeMosfet_; }
    bool balanceEnabled() const { return balanceMosfet_; }

private:
    // Mock/placeholder: apenas guarda flags.
    bool chargeMosfet_ = false;
    bool dischargeMosfet_ = false;
    bool balanceMosfet_ = false;
};

