#include "soc_manager.h"

#include "../../config/config.h"

// Defaults (stubs) caso ainda não existam na config.
#ifndef NOMINAL_PACK_VOLTAGE
#define NOMINAL_PACK_VOLTAGE 12.0f
#endif

#ifndef BATTERY_CAPACITY_AH
#define BATTERY_CAPACITY_AH 100.0f
#endif

void SOCManager::init()
{
    counter.init(BATTERY_CAPACITY_AH);

    data.soc = 100.0f;
    data.soh = 100.0f;

    data.fullCapacityAh = BATTERY_CAPACITY_AH;
    data.remainingCapacityAh = BATTERY_CAPACITY_AH;

    data.energyRemainingWh = 0.0f;
    data.energyConsumedWh = 0.0f;

    data.chargedAh = 0.0f;
    data.dischargedAh = 0.0f;

    data.cycleCount = 0;
}

float SOCManager::clampPercent(float x) const
{
    if (x > 100.0f) return 100.0f;
    if (x < 0.0f) return 0.0f;
    return x;
}

void SOCManager::update(const BatteryPack& pack,
                          float dtSeconds)
{
    counter.update(pack.current, dtSeconds);


    // Mantém energia/ciclos sempre atualizados
    calculateSOC(pack);
    calculateSOH();
    calculateEnergy(pack);
    calculateCycles();

    // Atualiza campos derivados
    data.chargedAh = counter.getChargedAh();
    data.dischargedAh = counter.getConsumedAh();

    // Remaining capacity simples (estimativa)
    float consumed = counter.getConsumedAh();
    float remaining = BATTERY_CAPACITY_AH - consumed;
    if (remaining < 0.0f) remaining = 0.0f;
    data.remainingCapacityAh = remaining;
    data.fullCapacityAh = BATTERY_CAPACITY_AH;
}

SOCData SOCManager::getData() const
{
    return data;
}

void SOCManager::calculateSOC(const BatteryPack& pack)
{

    float voltageEstimate = (pack.totalVoltage / NOMINAL_PACK_VOLTAGE) * 100.0f;

    float coulombEstimate =
        100.0f - (counter.getConsumedAh() / BATTERY_CAPACITY_AH) * 100.0f;

    data.soc = (voltageEstimate * 0.30f) + (coulombEstimate * 0.70f);
    data.soc = clampPercent(data.soc);
}

void SOCManager::calculateSOH()
{
    // Estima SOH simplificado como razão entre capacidade atual e nominal.
    // (Nesta etapa, capacity atual ainda é aproximada.)
    data.soh = (data.fullCapacityAh / BATTERY_CAPACITY_AH) * 100.0f;
    data.soh = clampPercent(data.soh);
}

void SOCManager::calculateEnergy(const BatteryPack& pack)
{

    data.energyRemainingWh = pack.totalVoltage * data.remainingCapacityAh;
    data.energyConsumedWh = counter.getConsumedAh() * pack.totalVoltage;
}

void SOCManager::calculateCycles()
{
    float totalConsumedAh = counter.getConsumedAh();
    data.cycleCount = static_cast<uint32_t>(totalConsumedAh / BATTERY_CAPACITY_AH);
}

