#include "app.h"

#include "../../core/fault/fault_registry.h"
#include "../../system/clock/clock.h"
#include <cstdio>

/* ==========================================================
 * Adaptadores de saída do FaultManager (Dependency Injection)
 * ========================================================== */

/* FaultManager -> Logger */
void FaultLogSinkAdapter::onFaultLogged(const FaultEvent& event)
{
    const char* desc     = FaultRegistry::codeToString(event.code);
    const char* stateStr = faultStateToString(event.state);
    const char* sevStr   = faultSeverityToString(event.severity);

    std::printf(
        "[FAULT] code=%s | state=%s | sev=%s | occ=%lu | desc=%s\n",
        desc, stateStr, sevStr,
        static_cast<unsigned long>(event.occurrence),
        event.description);
}

/* FaultManager -> FaultStorage */
void FaultStorageSinkAdapter::setStorage(FaultStorage* storage)
{
    storage_ = storage;
}

bool FaultStorageSinkAdapter::onFaultPersist(const FaultEvent& event)
{
    if (storage_ == nullptr)
    {
        return false;
    }

    return storage_->appendEvent(event);
}

static const char* protectionStateToString(ProtectionState state)
{
    switch (state)
    {
        case ProtectionState::NORMAL:
            return "NORMAL";

        case ProtectionState::OVER_VOLTAGE:
            return "OVER_VOLTAGE";

        case ProtectionState::UNDER_VOLTAGE:
            return "UNDER_VOLTAGE";

        case ProtectionState::OVER_TEMPERATURE:
            return "OVER_TEMPERATURE";

        case ProtectionState::UNDER_TEMPERATURE:
            return "UNDER_TEMPERATURE";

        case ProtectionState::OVER_CURRENT_CHARGE:
            return "OVER_CURRENT_CHARGE";

        case ProtectionState::OVER_CURRENT_DISCHARGE:
            return "OVER_CURRENT_DISCHARGE";

        case ProtectionState::SHORT_CIRCUIT:
            return "SHORT_CIRCUIT";

        case ProtectionState::SENSOR_FAILURE:
            return "SENSOR_FAILURE";

        case ProtectionState::UNKNOWN:
        default:
            return "UNKNOWN";
    }
}

/* ==========================================================
 * Mapeamento FaultCode (DTC) -> FaultReason (legado) + código 0x????.
 * Tabela mínima para as falhas detectadas pelo ProtectionManager.
 * ========================================================== */
static FaultReason faultCodeToReason(FaultCode code, std::uint16_t& outCode)
{
    switch (code)
    {
        case FaultCode::BMS_CELL_OVERVOLTAGE:   outCode = 0x0101; return FaultReason::CELL_OVERVOLTAGE;
        case FaultCode::BMS_CELL_UNDERVOLTAGE:  outCode = 0x0102; return FaultReason::CELL_UNDERVOLTAGE;
        case FaultCode::BMS_OVER_TEMPERATURE:   outCode = 0x0201; return FaultReason::OVERTEMPERATURE;
        case FaultCode::BMS_LOW_TEMPERATURE:    outCode = 0x0202; return FaultReason::UNDERTEMPERATURE;
        case FaultCode::BMS_OVER_CURRENT:       outCode = 0x0301; return FaultReason::OVERCURRENT_CHARGE;
        case FaultCode::BMS_SHORT_CIRCUIT:      outCode = 0x0302; return FaultReason::OVERCURRENT_DISCHARGE;
        case FaultCode::BMS_SENSOR_FAILURE:     outCode = 0x0401; return FaultReason::SENSOR_FAILURE;
        default:                                 outCode = 0x0000; return FaultReason::NONE;
    }
}

void App::printTelemetry(const BatteryPack& pack)
{
    printf("\n");
    printf("=============== BMS TELEMETRY ===============\n");

    printf("PACK       : %.2f V\n", pack.totalVoltage);
    printf("AVG CELL   : %.3f V\n", pack.averageVoltage);
    printf("MIN CELL   : %.3f V\n", pack.minVoltage);
    printf("MAX CELL   : %.3f V\n", pack.maxVoltage);
    printf("DELTA      : %.0f mV\n", pack.deltaVoltage * 1000.0f);

    printf("\n");

    printf("CURRENT    : %.2f A\n", pack.current);
    printf("POWER      : %.2f W\n", pack.power);

    printf("\n");

    printf("TEMP       : %.1f C\n", pack.averageTemperature);

    printf("\n");

    printf("SOC        : %u %%\n", pack.soc);
    printf("SOH        : %u %%\n", pack.soh);

    printf("\n");

    printf("CHARGING   : %s\n", pack.charging ? "YES" : "NO");
    printf("DISCHARGE  : %s\n", pack.discharging ? "YES" : "NO");
    printf("BALANCING  : %s\n", pack.balancing ? "ON" : "OFF");

    printf("\n");

    printf("=============================================\n\n");
}

void App::setMosfetDriver(IMosfetDriver* driver)
{
    if (driver != nullptr)
    {
        mosfetDriver_ = driver;
    }
}

bool App::init()
{
    fault_.init();
    fault_.setLogSink(&faultLogSink_);
    faultStorageSink_.setStorage(&faultStorage_);
    fault_.setStorageSink(&faultStorageSink_);

    printf("Loading Fault History...\n");

    faultStorage_.init();

    constexpr bool CLEAR_FAULT_HISTORY_ON_BOOT = false;
    if (CLEAR_FAULT_HISTORY_ON_BOOT)
    {
        (void)faultStorage_.clear();
    }

    std::vector<FaultInfo> persistedHistory;
    faultStorage_.load(persistedHistory);

    for (const auto& fault : persistedHistory)
    {
        faultHistory_.push(fault);
    }

    if (persistedHistory.empty())
    {
        printf("No previous events.\n");
    }
    else
    {
        printf("History loaded.\n");
        printf("Events found: %u\n", static_cast<unsigned>(persistedHistory.size()));
    }

    mosfetDriver_->init();

    battery_.init();
    protection_.init();

    bmsStateMachine_.init();

    // Sprint 5.2 — Passo 2: avança BOOT -> INIT -> SELF_TEST -> READY na inicialização.
    bmsStateMachine_.update();
    bmsStateMachine_.update();
    bmsStateMachine_.update();

    balance_.init();
    temperature_.init();
    current_.init();

    // Sprint Heartbeat — inicializa o heartbeat do firmware.
    heartbeat_.init();

    return true;
}

void App::printFaultHistory()
{
    printf("\n=============== FAULT HISTORY ===============\n\n");

    const size_t n = faultHistory_.size();
    printf("EVENTS : %u\n\n", static_cast<unsigned>(n));

    for (size_t i = 0; i < n; ++i)
    {
        const FaultInfo& fi = faultHistory_.at(i);

        printf("#%03u\n", static_cast<unsigned>(i + 1u));

        printf("TIME        : %u ms\n", static_cast<unsigned>(fi.timestamp));
        printf("FAULT       : %s\n", faultReasonToString(fi.reason));
        printf("CODE        : 0x%04X\n", fi.code);
        printf("CELL        : %u\n", static_cast<unsigned>(fi.source));
        printf("VALUE       : %.3f V\n", fi.value);
        printf("LIMIT       : %.3f V\n\n", fi.limit);
    }

    printf("==============================================\n\n");
}

void App::update()
{
    // Contadores para status periódico (modo terminal estável)
    static std::uint64_t lastStatusMs = 0;
    static std::uint64_t lastIndustrialPrintMs = 0;
    static std::uint64_t loopCount = 0;
    static bool terminalHeaderPrinted = false;
    ++loopCount;

    //-------------------------------------------------
    // Battery
    //-------------------------------------------------

    battery_.update();

    BatteryPack& pack = battery_.getPack();

    //-------------------------------------------------
    // Current
    //-------------------------------------------------

    current_.update(pack.totalVoltage);

    CurrentData currentData = current_.getData();

    battery_.setCurrent(currentData.currentA);
    pack.current = currentData.currentA;
    pack.power = pack.totalVoltage * pack.current;

    //-------------------------------------------------
    // Temperature
    //-------------------------------------------------

    temperature_.update();

    TemperatureData temperatureData = temperature_.getData();

    pack.averageTemperature = temperatureData.averageTemperature;
    pack.maxTemperature = temperatureData.maxTemperature;
    pack.minTemperature = temperatureData.minTemperature;

    //-------------------------------------------------
    // Fault / Protection
    //-------------------------------------------------

    protection_.update(pack, fault_);

    //-------------------------------------------------
    // BMS State Machine
    //-------------------------------------------------

    bmsStateMachine_.update();

    //-------------------------------------------------
    // BMS State
    //-------------------------------------------------

    stateManager_.update(pack, fault_);

    mosfetDriver_->applyState(stateManager_.getState());

    pack.charging = mosfetDriver_->chargeEnabled();
    pack.discharging = mosfetDriver_->dischargeEnabled();
    pack.balancing = mosfetDriver_->balanceEnabled();

    //-------------------------------------------------
    // Balance
    //-------------------------------------------------

    balance_.update(pack, protection_, battery_);
    (void)balance_.getStatus();

    //-------------------------------------------------
    // Status periódico (a cada 5 s)
    //-------------------------------------------------

    const std::uint64_t nowMs = Clock::millis();
    if ((nowMs - lastStatusMs) >= 5000)
    {
        lastStatusMs = nowMs;

        printf("\n");
        printf("=============== BMS STATUS ===============\n");
        printf("UPTIME       : %llu ms\n", static_cast<unsigned long long>(nowMs));
        printf("LOOPS        : %llu\n", static_cast<unsigned long long>(loopCount));
        printf("STATE        : %s\n", bmsStateMachine_.toString());
        printf("PACK VOLTAGE : %.2f V\n", pack.totalVoltage);
        printf("MIN CELL     : %.3f V\n", pack.minVoltage);
        printf("MAX CELL     : %.3f V\n", pack.maxVoltage);
        printf("CELL DELTA   : %.0f mV\n", pack.deltaVoltage * 1000.0f);
        printf("CURRENT      : %.2f A\n", pack.current);
        printf("TEMPERATURE  : %.1f C\n", pack.averageTemperature);
        printf("SOC          : %u %%\n", static_cast<unsigned>(pack.soc));
        printf("SOH          : %u %%\n", static_cast<unsigned>(pack.soh));

        // Falhas ativas (DTC)
        const std::size_t activeFaults = fault_.getActiveFaults();
        printf("ACTIVE FAULTS: %u\n", static_cast<unsigned>(activeFaults));
        for (std::size_t i = 0; i < activeFaults; ++i)
        {
            FaultEvent ev;
            if (fault_.getActiveFault(i, ev))
            {
                printf("  - %s\n", FaultRegistry::codeToString(ev.code));
            }
        }

        // Heartbeat
        const HeartbeatData& hb = heartbeat_.data();
        printf("HEARTBEAT    : ALIVE=%s UPTIME=%u ms SOC=%u%% TEMP=%.1f C\n",
               hb.alive ? "YES" : "NO",
               static_cast<unsigned>(hb.uptimeMs),
               static_cast<unsigned>(hb.soc),
               hb.temperatureC);

        printf("==========================================\n\n");
    }

    //-------------------------------------------------
    // Edge detection DTC -> FaultHistory + FaultStorage
    //-------------------------------------------------

    static std::size_t lastActiveCount = 0;
    const std::size_t activeCount = fault_.getActiveFaults();

    if (activeCount > lastActiveCount)
    {
        // Nova falha detectada — pega o primeiro evento ativo
        FaultEvent ev;
        if (fault_.getActiveFault(0, ev))
        {
            std::uint16_t legacyCode = 0;
            const FaultReason reason = faultCodeToReason(ev.code, legacyCode);

            FaultInfo snapshot;
            snapshot.active = true;
            snapshot.reason = reason;
            snapshot.code = legacyCode;
            snapshot.source = ev.source;
            snapshot.value = ev.measuredValue;
            snapshot.limit = ev.limit;
            snapshot.timestamp = ev.timestamp;

            // Deduplicação
            bool alreadyExists = false;
            const size_t n = faultHistory_.size();
            for (size_t i = 0; i < n; ++i)
            {
                const FaultInfo& prev = faultHistory_.at(i);
                if (prev.code == snapshot.code && prev.reason == snapshot.reason && prev.source == snapshot.source)
                {
                    alreadyExists = true;
                    break;
                }
            }

            if (!alreadyExists)
            {
                faultHistory_.push(snapshot);
                faultStorage_.append(snapshot);
                printf("[STORAGE] Fault persisted.\n");
            }

            lastFaultActive_ = true;
            lastFaultReason_ = reason;
            lastFaultCode_ = legacyCode;
            lastFaultSource_ = ev.source;
        }
    }
    else if (activeCount == 0 && lastActiveCount > 0)
    {
        // Todas as falhas foram limpas
        lastFaultActive_ = false;
        lastFaultReason_ = FaultReason::NONE;
        lastFaultCode_ = 0;
        lastFaultSource_ = 0xFF;
    }

    lastActiveCount = activeCount;

    //-------------------------------------------------
    // BMS INDUSTRIAL (exibição quando há falha ativa)
    //-------------------------------------------------

    if (activeCount > 0 && (nowMs - lastIndustrialPrintMs) >= 2000)
    {
        lastIndustrialPrintMs = nowMs;

        FaultEvent fe;
        if (fault_.getActiveFault(0, fe))
        {
            std::uint16_t legacyCode = 0;
            const FaultReason reason = faultCodeToReason(fe.code, legacyCode);

            printf("\nFAULT=%s CODE=0x%04X CELL=%u MEASURED=%.3fV LIMIT=%.3fV CHARGE=%s DISCHARGE=%s BALANCE=%s\n",
                   faultReasonToString(reason),
                   legacyCode,
                   static_cast<unsigned>(fe.source),
                   fe.measuredValue,
                   fe.limit,
                   mosfetDriver_->chargeEnabled() ? "ON" : "OFF",
                   mosfetDriver_->dischargeEnabled() ? "ON" : "OFF",
                   mosfetDriver_->balanceEnabled() ? "ON" : "OFF");
        }
    }

    //-------------------------------------------------
    // Proteção (logs de apoio)
    //-------------------------------------------------

    char protectionLog[200];

    const char* bmsStateStr = "IDLE";

    switch (stateManager_.getState())
    {
        case BmsState::INIT:
            bmsStateStr = "INIT";
            break;
        case BmsState::IDLE:
            bmsStateStr = "IDLE";
            break;
        case BmsState::CHARGING:
            bmsStateStr = "CHARGING";
            break;
        case BmsState::DISCHARGING:
            bmsStateStr = "DISCHARGING";
            break;
        case BmsState::BALANCING:
            bmsStateStr = "BALANCING";
            break;
        case BmsState::FAULT:
            bmsStateStr = "FAULT";
            break;
        case BmsState::SHUTDOWN:
            bmsStateStr = "SHUTDOWN";
            break;
        default:
            bmsStateStr = "IDLE";
            break;
    }

    (void)bmsStateStr;

    snprintf(
        protectionLog,
        sizeof(protectionLog),
        "PROTECTION %s | CHARGE MOSFET %s | DISCHARGE MOSFET %s",
        protectionStateToString(protection_.getState()),
        protection_.chargeEnabled() ? "ON" : "OFF",
        protection_.dischargeEnabled() ? "ON" : "OFF"
    );

    // Fault History em intervalo longo para manter o terminal estável.
    if ((nowMs - lastStatusMs) >= 15000)
    {
        printFaultHistory();
        lastStatusMs = nowMs;
    }

    //-------------------------------------------------
    // Heartbeat
    //-------------------------------------------------

    heartbeat_.setState(bmsStateMachine_.toString());
    heartbeat_.setSOC(pack.soc);
    heartbeat_.setTemperature(pack.averageTemperature);

    heartbeat_.update();
}