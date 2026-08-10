#include "fault_manager.h"

#include <limits>

#include "fault_registry.h"
#include "../../system/clock/clock.h"

void FaultManager::init()
{
    // Mantém compatível com o padrão dos outros managers.
    clear();

    activeCount_ = 0;
    logSink_ = nullptr;
    storageSink_ = nullptr;
}

/* ==========================================================
 * Injeção de dependência (sinks)
 * ========================================================== */
void FaultManager::setLogSink(IFaultLogSink* sink)
{
    logSink_ = sink;
}

void FaultManager::setStorageSink(IFaultStorageSink* sink)
{
    storageSink_ = sink;
}

/* ==========================================================
 * API DTC
 * ========================================================== */

void FaultManager::raiseFault(FaultCode code)
{
    const FaultSeverity sev = FaultRegistry::findSeverity(code);
    raiseFault(code, sev, FaultState::ACTIVE, FAULT_SOURCE_GLOBAL, 0.0f, 0.0f);
}

void FaultManager::raiseFault(FaultCode code, FaultSeverity severity, FaultState state)
{
    raiseFault(code, severity, state, FAULT_SOURCE_GLOBAL, 0.0f, 0.0f);
}

void FaultManager::raiseFault(FaultCode code,
                               FaultSeverity severity,
                               FaultState state,
                               std::uint8_t source,
                               float measuredValue,
                               float limit)
{
    if (code == FaultCode::NONE)
    {
        return;
    }

    // Se já está ativo, incrementa ocorrência e atualiza (não duplica).
    const std::size_t idx = findActive(code);
    if (idx < activeCount_)
    {
        FaultEvent& existing = activeFaults_[idx];
        ++existing.occurrence;
        existing.state = state;
        existing.severity = severity;
        existing.source = source;
        existing.measuredValue = measuredValue;
        existing.limit = limit;
        publishToLog(existing);
        persistToStorage(existing);
        return;
    }

    // Novo evento ativo.
    if (activeCount_ < MAX_ACTIVE_FAULTS)
    {
        FaultEvent& ev = activeFaults_[activeCount_];
        ev.code = code;
        ev.severity = severity;
        ev.state = state;
        ev.timestamp = static_cast<std::uint32_t>(Clock::millis());
        ev.occurrence = 1;
        ev.source = source;
        ev.measuredValue = measuredValue;
        ev.limit = limit;
        ev.setDescription(FaultRegistry::findDescription(code));
        ++activeCount_;

        publishToLog(ev);
        persistToStorage(ev);
    }
}

void FaultManager::clearFault(FaultCode code)
{
    const std::size_t idx = findActive(code);
    if (idx >= activeCount_)
    {
        return;
    }

    FaultEvent& ev = activeFaults_[idx];
    ev.state = FaultState::CLEARED;
    ev.timestamp = 0; // marcado para atualização no sink/logger

    publishToLog(ev);
    persistToStorage(ev);

    // Remove do conjunto ativo (shift para trás).
    --activeCount_;
    for (std::size_t i = idx; i < activeCount_; ++i)
    {
        activeFaults_[i] = activeFaults_[i + 1u];
    }
    activeFaults_[activeCount_] = FaultEvent{};
}

bool FaultManager::hasFault() const
{
    return activeCount_ > 0;
}

bool FaultManager::hasFault(FaultCode code) const
{
    return findActive(code) < activeCount_;
}

std::size_t FaultManager::getActiveFaults() const
{
    return activeCount_;
}

bool FaultManager::getActiveFault(std::size_t index, FaultEvent& out) const
{
    if (index >= activeCount_)
    {
        return false;
    }
    out = activeFaults_[index];
    return true;
}

/* ==========================================================
 * Internos
 * ========================================================== */

std::size_t FaultManager::findActive(FaultCode code) const
{
    for (std::size_t i = 0; i < activeCount_; ++i)
    {
        if (activeFaults_[i].code == code)
        {
            return i;
        }
    }
    return MAX_ACTIVE_FAULTS;
}

void FaultManager::publishToLog(const FaultEvent& event)
{
    if (logSink_ != nullptr)
    {
        logSink_->onFaultLogged(event);
    }
}

void FaultManager::persistToStorage(const FaultEvent& event)
{
    if (storageSink_ != nullptr)
    {
        storageSink_->onFaultPersist(event);
    }
}

/* ==========================================================
 * API legada (compatibilidade)
 * ========================================================== */

void FaultManager::clear()
{
    flags = FAULT_NONE;
    faultInfo_ = FaultInfo{};
}

void FaultManager::addFault(FaultFlag fault)
{
    flags |= static_cast<std::uint32_t>(fault);
}

bool FaultManager::hasFault(FaultFlag fault) const
{
    return (flags & static_cast<std::uint32_t>(fault)) != 0u;
}

std::uint32_t FaultManager::getFaults() const
{
    return flags;
}

static std::uint8_t findMaxCellIndex1Based(const BatteryPack& pack)
{
    float maxV = -std::numeric_limits<float>::infinity();
    std::uint8_t bestCell0 = 0;

    for (std::uint8_t i = 0; i < PACK_CELL_COUNT; ++i)
    {
        const float v = pack.cells[i].voltage;
        if (pack.cells[i].valid && v > maxV)
        {
            maxV = v;
            bestCell0 = i; // 0-based interno
        }
    }

    return static_cast<std::uint8_t>(bestCell0 + 1);
}

static std::uint8_t findMinCellIndex1Based(const BatteryPack& pack)
{
    float minV = std::numeric_limits<float>::infinity();
    std::uint8_t bestCell0 = 0;

    for (std::uint8_t i = 0; i < PACK_CELL_COUNT; ++i)
    {
        const float v = pack.cells[i].voltage;
        if (pack.cells[i].valid && v < minV)
        {
            minV = v;
            bestCell0 = i; // 0-based interno
        }
    }

    return static_cast<std::uint8_t>(bestCell0 + 1);
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
    if (hasFault(FAULT_CELL_OVERVOLTAGE))
    {
        faultInfo_.active = true;
        faultInfo_.reason = FaultReason::CELL_OVERVOLTAGE;
        faultInfo_.code = 0x0101;
        faultInfo_.source = findMaxCellIndex1Based(pack);
        faultInfo_.value = pack.cells[faultInfo_.source - 1u].voltage;
        faultInfo_.limit = CELL_OVERVOLTAGE_LIMIT;
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
        return;
    }

    if (hasFault(FAULT_OVER_CURRENT))
    {
        faultInfo_.active = true;
        faultInfo_.reason = FaultReason::OVERCURRENT_CHARGE;
        faultInfo_.code = 0x0301;
        faultInfo_.source = 0xFF;
        faultInfo_.value = pack.current;
        faultInfo_.limit = MAX_CHARGE_CURRENT;
        return;
    }
}
