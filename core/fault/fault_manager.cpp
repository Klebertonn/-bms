#include "fault_manager.h"

#include <limits>

void FaultManager::init()
{
    // Mantém compatível com o padrão dos outros managers.
    clear();
}

void FaultManager::clear()
{
    flags = FAULT_NONE;
    faultInfo_ = FaultInfo{};
}

void FaultManager::addFault(FaultFlag fault)
{
    flags |= static_cast<uint32_t>(fault);
}

bool FaultManager::hasFault() const
{
    return flags != FAULT_NONE;
}

bool FaultManager::hasFault(FaultFlag fault) const
{
    return (flags & static_cast<uint32_t>(fault)) != 0u;
}

uint32_t FaultManager::getFaults() const
{
    return flags;
}

static uint8_t findMaxCellIndex1Based(const BatteryPack& pack)
{
    float maxV = -std::numeric_limits<float>::infinity();
    uint8_t bestCell0 = 0;

    for (uint8_t i = 0; i < PACK_CELL_COUNT; ++i)
    {
        const float v = pack.cells[i].voltage;
        if (pack.cells[i].valid && v > maxV)
        {
            maxV = v;
            bestCell0 = i; // 0-based interno
        }
    }

    // Telemetria industrial geralmente usa CELL 1..N.
    return static_cast<uint8_t>(bestCell0 + 1);
}

static uint8_t findMinCellIndex1Based(const BatteryPack& pack)
{
    float minV = std::numeric_limits<float>::infinity();
    uint8_t bestCell0 = 0;

    for (uint8_t i = 0; i < PACK_CELL_COUNT; ++i)
    {
        const float v = pack.cells[i].voltage;
        if (pack.cells[i].valid && v < minV)
        {
            minV = v;
            bestCell0 = i; // 0-based interno
        }
    }

    return static_cast<uint8_t>(bestCell0 + 1);
}

void FaultManager::evaluate(const BatteryPack& pack)
{
    clear();

    // 1) Detecta faults em flags (compatibilidade)
    if (pack.maxVoltage > CELL_OVERVOLTAGE_LIMIT)
    {
        addFault(FAULT_CELL_OVERVOLTAGE);
    }

    if (pack.minVoltage < CELL_UNDERVOLTAGE_LIMIT)
    {
        addFault(FAULT_CELL_UNDERVOLTAGE);
    }

    if (pack.averageTemperature > MAX_TEMPERATURE)
    {
        addFault(FAULT_OVER_TEMPERATURE);
    }

    if (pack.current > MAX_CHARGE_CURRENT)
    {
        addFault(FAULT_OVER_CURRENT);
    }

    // 2) Deriva FaultInfo (prioridade simples: primeiro fault detectado)
    // Para manter comportamento determinístico, usamos ordem fixa.

    if (hasFault(FAULT_CELL_OVERVOLTAGE))
    {
        faultInfo_.active = true;
        faultInfo_.reason = FaultReason::CELL_OVERVOLTAGE;
        faultInfo_.code = 0x0101;
        faultInfo_.source = findMaxCellIndex1Based(pack);
        // value/limit devem usar o índice 0-based interno do pack.
        faultInfo_.value = pack.cells[faultInfo_.source - 1u].voltage;
        faultInfo_.limit = CELL_OVERVOLTAGE_LIMIT;

        // timestamp será definido/persistido pelo chamador (ex.: App::update ao salvar no FaultStorage)
        return;
    }

    if (hasFault(FAULT_CELL_UNDERVOLTAGE))

    {
        faultInfo_.active = true;
        faultInfo_.reason = FaultReason::CELL_UNDERVOLTAGE;
        faultInfo_.code = 0x0102;
        faultInfo_.source = findMinCellIndex1Based(pack);
        faultInfo_.value = pack.cells[faultInfo_.source - 1u].voltage;

        faultInfo_.limit = CELL_UNDERVOLTAGE_LIMIT;
        // timestamp será definido/persistido pelo chamador (ex.: App::update ao salvar no FaultStorage)
        return;
    }

    if (hasFault(FAULT_OVER_TEMPERATURE))

    {
        faultInfo_.active = true;
        faultInfo_.reason = FaultReason::OVERTEMPERATURE;
        faultInfo_.code = 0x0201;
        faultInfo_.source = 0xFF;
        faultInfo_.value = pack.averageTemperature;
        faultInfo_.limit = MAX_TEMPERATURE;
        // timestamp será definido/persistido pelo chamador (ex.: App::update ao salvar no FaultStorage)
        return;
    }

    if (hasFault(FAULT_OVER_CURRENT))

    {
        // Nesta fase, o FaultFlag OVER_CURRENT não diferencia carga/descarga.
        faultInfo_.active = true;
        faultInfo_.reason = FaultReason::OVERCURRENT_CHARGE;
        faultInfo_.code = 0x0301;
        faultInfo_.source = 0xFF;
        faultInfo_.value = pack.current;
        faultInfo_.limit = MAX_CHARGE_CURRENT;
        // timestamp será definido/persistido pelo chamador (ex.: App::update ao salvar no FaultStorage)
        return;
    }

    // Se chegou aqui, sem FaultInfo mas com flags (ou flags não mapeadas).

    // Mantém ativo=false.
}



