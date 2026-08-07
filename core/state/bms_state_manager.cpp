#include "bms_state_manager.h"

void BmsStateManager::init()
{
    state_ = BmsState::INIT;
}

BmsState BmsStateManager::getState() const
{
    return state_;
}

const char* BmsStateManager::toString() const
{
    switch (state_)
    {
        case BmsState::INIT:
            return "INIT";
        case BmsState::IDLE:
            return "IDLE";
        case BmsState::CHARGING:
            return "CHARGING";
        case BmsState::DISCHARGING:
            return "DISCHARGING";
        case BmsState::BALANCING:
            return "BALANCING";
        case BmsState::FAULT:
            return "FAULT";
        case BmsState::SHUTDOWN:
            return "SHUTDOWN";
        default:
            return "UNKNOWN";
    }
}

bool BmsStateManager::isFaultCritical(const FaultManager& faults) const
{
    // Regra inicial: qualquer falha (por enquanto) leva ao estado FAULT.
    return faults.hasFault();
}

bool BmsStateManager::isCharging(const BatteryPack& pack) const
{
    return pack.current > CURRENT_THRESHOLD_A;
}

bool BmsStateManager::isDischarging(const BatteryPack& pack) const
{
    return pack.current < -CURRENT_THRESHOLD_A;
}

bool BmsStateManager::isBalancing(const BatteryPack& pack) const
{
    return pack.balancing;
}

void BmsStateManager::update(const BatteryPack& pack, const FaultManager& faults)
{
    // Transição INIT -> (regra normal)
    if (state_ == BmsState::INIT)
    {
        state_ = BmsState::IDLE;
    }

    if (isFaultCritical(faults))
    {
        state_ = BmsState::FAULT;
        return;
    }

    if (isBalancing(pack))
    {
        state_ = BmsState::BALANCING;
        return;
    }

    if (isCharging(pack))
    {
        state_ = BmsState::CHARGING;
        return;
    }

    if (isDischarging(pack))
    {
        state_ = BmsState::DISCHARGING;
        return;
    }

    state_ = BmsState::IDLE;
}

