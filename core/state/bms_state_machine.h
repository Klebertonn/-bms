#pragma once

#include <cstdint>

#include "../../core/battery/battery_pack.h"
#include "../../core/fault/fault_manager.h"
#include "self_test.h"

enum class BmsIndustrialState
{
    BOOT,
    INIT,
    SELF_TEST,
    READY,
    IDLE,
    CHARGING,
    DISCHARGING,
    BALANCING,
    FAULT,
    RECOVERY,
    SHUTDOWN
};


class BmsStateMachine
{
public:
    void init();

    // update without parameters for now (state decisions can be expanded later)
    void update();

    void setState(BmsIndustrialState s);

    BmsIndustrialState state() const;

    bool selfTestPassed() const { return selfTest_.passed(); }

    BmsIndustrialState previousState() const { return previousState_; }

    const char* toString() const;

private:
    BmsIndustrialState currentState_ = BmsIndustrialState::BOOT;
    BmsIndustrialState previousState_ = BmsIndustrialState::BOOT;


// Deterministic SELF_TEST duration (Sprint 5.1)
    static constexpr uint32_t SELF_TEST_CYCLES = 3;
    uint32_t selfTestCounter_ = 0;

    // Power-On Self Test (integração SELF_TEST -> READY/FAULT)
    SelfTest selfTest_;

    void transitionTo(BmsIndustrialState s);
};


