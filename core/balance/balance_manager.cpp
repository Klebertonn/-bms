#include "balance_manager.h"

#include "../../config/config.h"

// Limites (stubs) caso ainda não estejam definidos na config.
#ifndef BALANCE_START_DELTA
#define BALANCE_START_DELTA 0.020f
#endif
#ifndef BALANCE_STOP_DELTA
#define BALANCE_STOP_DELTA 0.010f
#endif
#ifndef MAX_BALANCE_TEMP
#define MAX_BALANCE_TEMP 45.0f
#endif
#ifndef MAX_BALANCE_TIME_MS
#define MAX_BALANCE_TIME_MS 600000
#endif

void BalanceManager::init()
{
    info.state = BalanceState::IDLE;
    disableAll();
    info.activeCell = -1;

    info.deltaVoltage = 0.0f;
}

BalanceInfo BalanceManager::getStatus() const
{
    return info;
}

bool BalanceManager::canBalance(const PackData& pack,
                                const ProtectionManager& protection)
{
    if (protection.isFault())
        return false;

    if (pack.temperature > MAX_BALANCE_TEMP)
        return false;

    // Gate simples baseado na variação de tensão entre células
    if (pack.deltaVoltage < BALANCE_START_DELTA)
        return false;

    return true;
}

void BalanceManager::selectCell(const BatteryManager& battery)
{
    float highest = 0.0f;
    int cell = -1;

    for (int i = 0; i < MAX_CELLS; i++)
    {
        float v = battery.getCellVoltage(i);

        if (v > highest)
        {
            highest = v;
            cell = i;
        }
    }

    disableAll();

    if (cell >= 0)
    {
        info.cellEnabled[cell] = true;
        info.activeCell = cell;
    }
}

void BalanceManager::update(const PackData& pack,
                             const ProtectionManager& protection,
                             const BatteryManager& battery)
{
    info.deltaVoltage = pack.deltaVoltage;

    if (!canBalance(pack, protection))
    {
        disableAll();
        info.state = BalanceState::PAUSED;
        return;
    }

    // Exemplo inicial: seleciona apenas a célula de maior tensão.
    selectCell(battery);
    info.state = BalanceState::BALANCING;
}

void BalanceManager::disableAll()
{
    for (int i = 0; i < MAX_CELLS; i++)
        info.cellEnabled[i] = false;

    info.activeCell = -1;
}

