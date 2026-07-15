#pragma once


#include "../../core/battery/battery_manager.h"
#include "../../core/current/current_manager.h"
#include "../../core/temperature/temperature_manager.h"
#include "../../core/protection/protection_manager.h"
#include "../../core/balance/balance_manager.h"
#include "../../core/fault/fault_manager.h"
#include "../../core/state/bms_state_manager.h"
#include "../../core/mosfet/mosfet_controller.h"
#include "../../core/fault/fault_history.h"

#include "../../system/logger/logger.h"






class App
{
public:

    bool init();

    void update();



private:

    void printTelemetry(const BatteryPack& pack);
    void printFaultHistory();




    BatteryManager battery_{};


    CurrentManager current_{};


    TemperatureManager temperature_{};


    ProtectionManager protection_{};


    FaultManager fault_{};

    BmsStateManager stateManager_{};

    BalanceManager balance_{};

    MosfetController mosfet_{};

    FaultHistory faultHistory_{};

    // last pushed fault snapshot (avoid repeating the same event)
    bool lastFaultActive_ = false;
    FaultReason lastFaultReason_ = FaultReason::NONE;
    std::uint16_t lastFaultCode_ = 0;
    std::uint8_t lastFaultSource_ = 0xFF;
};




