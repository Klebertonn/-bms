#pragma once

#include "../battery/battery_manager.h"
#include "protection_types.h"

class ProtectionManager
{
public:
    void init();

    void update(const PackData& pack);

    ProtectionState getState() const;

    bool chargeEnabled() const;

    bool dischargeEnabled() const;

    bool isFault() const;

private:
    ProtectionState state = ProtectionState::NORMAL;

    bool chargeMosfet = true;
    bool dischargeMosfet = true;

    void checkVoltage(const PackData& pack);
    void checkTemperature(const PackData& pack);
    void checkCurrent(const PackData& pack);

    void applyProtection();
};

