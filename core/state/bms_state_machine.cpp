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

const char* BmsStateMachine::toString() const
{
    switch (currentState_)
    {
        case BmsIndustrialState::BOOT:
            return "BOOT";
        case BmsIndustrialState::INIT:
            return "INIT";
        case BmsIndustrialState::SELF_TEST:
            return "SELF_TEST";
        case BmsIndustrialState::READY:
            return "READY";
        case BmsIndustrialState::IDLE:
            return "IDLE";
        case BmsIndustrialState::CHARGING:
            return "CHARGING";
        case BmsIndustrialState::DISCHARGING:
            return "DISCHARGING";
        case BmsIndustrialState::BALANCING:
            return "BALANCING";
        case BmsIndustrialState::FAULT:
            return "FAULT";
        case BmsIndustrialState::RECOVERY:
            return "RECOVERY";
        case BmsIndustrialState::SHUTDOWN:
            return "SHUTDOWN";
        default:
            return "UNKNOWN";
    }
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
            break;

case BmsIndustrialState::SELF_TEST:
            // Executa o Power-On Self Test (POST).
            // PASS  -> READY
            // FAIL  -> FAULT
            selfTest_.run();
            if (selfTest_.passed())
            {
                transitionTo(BmsIndustrialState::READY);
            }
            else
            {
                transitionTo(BmsIndustrialState::FAULT);
            }
            break;

        case BmsIndustrialState::READY:
            break;

        case BmsIndustrialState::CHARGING:
            break;

        case BmsIndustrialState::DISCHARGING:
            break;

        case BmsIndustrialState::BALANCING:
            break;

        case BmsIndustrialState::FAULT:
            break;

        case BmsIndustrialState::RECOVERY:
            break;

        case BmsIndustrialState::SHUTDOWN:
            break;
    }
}


