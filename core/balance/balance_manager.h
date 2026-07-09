#pragma once

#include "../battery/battery_manager.h"
#include "../protection/protection_manager.h"

#include "balance_types.h"

#define MAX_CELLS 16

struct BalanceInfo
{
    BalanceState state;
    bool cellEnabled[MAX_CELLS];
    float deltaVoltage;
    int activeCell;
};

class BalanceManager
{
public:
    void init();

    void update(const PackData& pack,
                const ProtectionManager& protection,
                const BatteryManager& battery);

    BalanceInfo getStatus() const;

private:
    BalanceInfo info;

    void disableAll();

    void selectCell(const BatteryManager& battery);

    bool canBalance(const PackData& pack,
                    const ProtectionManager& protection);
};

