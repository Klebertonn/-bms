#include "bms_state_machine.h"

void BmsStateMachine::init()
{
    currentState_ = BmsIndustrialState::BOOT;
    previousState_ = BmsIndustrialState::BOOT;
    selfTestCounter_ = 0;
}

BmsIndustrialState BmsStateMachine::state() const
{
    return currentState_;
}

void BmsStateMachine::setState(BmsIndustrialState s)
{
    transitionTo(s);
}

void BmsStateMachine::transitionTo(BmsIndustrialState s)
{
    if (s == currentState_)
    {
        return;
    }

    previousState_ = currentState_;
    currentState_ = s;

    // Reset deterministic counters on state changes where relevant.
    if (currentState_ != BmsIndustrialState::SELF_TEST)
    {
        selfTestCounter_ = 0;
    }
}

void BmsStateMachine::update()
{
    switch (currentState_)
    {
        case BmsIndustrialState::BOOT:
            transitionTo(BmsIndustrialState::INIT);
            break;

        case BmsIndustrialState::INIT:
            transitionTo(BmsIndustrialState::SELF_TEST);
            selfTestCounter_ = 0;
            break;

        case BmsIndustrialState::SELF_TEST:
            // Deterministic SELF_TEST duration for Sprint 5.1: 3 update() cycles.
            selfTestCounter_++;
            if (selfTestCounter_ >= SELF_TEST_CYCLES)
            {
                transitionTo(BmsIndustrialState::READY);
            }
            break;

        case BmsIndustrialState::READY:
            // Sub-modes can be decided later; default to IDLE.
            transitionTo(BmsIndustrialState::IDLE);
            break;

        case BmsIndustrialState::IDLE:
        case BmsIndustrialState::CHARGING:
        case BmsIndustrialState::DISCHARGING:
        case BmsIndustrialState::BALANCING:
        case BmsIndustrialState::FAULT:
        case BmsIndustrialState::RECOVERY:
        case BmsIndustrialState::SHUTDOWN:
        default:
            // Keep stable for Sprint 5.1 (architecture base).
            break;
    }
}


