#pragma once

#include <cstdint>

#include "../../core/battery/battery_pack.h"
#include "../../core/fault/fault_manager.h"

enum class BmsState
{
    INIT,
    IDLE,
    CHARGING,
    DISCHARGING,
    BALANCING,
    FAULT,
    SHUTDOWN
};

class BmsStateManager
{
public:
    void init();

    void update(const BatteryPack& pack, const FaultManager& faults);

    BmsState getState() const;

    const char* toString() const;

private:
    BmsState state_ = BmsState::INIT;

    // Regras iniciais
    static constexpr float CURRENT_THRESHOLD_A = 0.2f;

    bool isFaultCritical(const FaultManager& faults) const;
    bool isCharging(const BatteryPack& pack) const;
    bool isDischarging(const BatteryPack& pack) const;
    bool isBalancing(const BatteryPack& pack) const;
};

