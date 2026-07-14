#pragma once


#include "../../core/battery/battery_manager.h"
#include "../../core/current/current_manager.h"
#include "../../core/temperature/temperature_manager.h"
#include "../../core/protection/protection_manager.h"
#include "../../core/balance/balance_manager.h"
#include "../../core/fault/fault_manager.h"
#include "../../core/state/bms_state_manager.h"
#include "../../core/mosfet/mosfet_controller.h"





#include "../../system/logger/logger.h"





class App
{
public:

    bool init();

    void update();



private:

    void printTelemetry(const BatteryPack& pack);

    Logger logger_{};


    BatteryManager battery_{};


    CurrentManager current_{};


    TemperatureManager temperature_{};


    ProtectionManager protection_{};


    FaultManager fault_{};

    BmsStateManager stateManager_{};

    BalanceManager balance_{};

    MosfetController mosfet_{};
};



