#pragma once

#include "../battery/battery_manager.h"
#include "../fault/fault_manager.h"
#include "protection_types.h"


class ProtectionManager
{
public:
    void init();

    void update(const BatteryPack& pack,
                const FaultManager& faults);


    ProtectionState getState() const;

    bool chargeEnabled() const;

    bool dischargeEnabled() const;

    bool isFault() const;

private:
    ProtectionState state = ProtectionState::NORMAL;

    bool chargeMosfet = true;
    bool dischargeMosfet = true;

    void checkVoltage(const BatteryPack& pack);
    void checkTemperature(const BatteryPack& pack);
    void checkCurrent(const BatteryPack& pack);

    void applyProtection();
};



