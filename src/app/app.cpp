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
    // Persiste o evento DTC no backend real (FaultStorage).
    // O FaultStorage é injetado pela camada de aplicação; o domínio
    // (FaultManager) não conhece a implementação concreta.
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
    // Inicializa o FaultManager e injeta os adaptadores de saída (Dependency Injection).
    // O FaultManager (core) não conhece Logger/Storage; recebe abstrações
    // IFaultLogSink e IFaultStorageSink compostas aqui na camada de aplicação.
    fault_.init();
    fault_.setLogSink(&faultLogSink_);
    faultStorageSink_.setStorage(&faultStorage_);
    fault_.setStorageSink(&faultStorageSink_);

    printf("Loading Fault History...\n");

faultStorage_.init();

    // Política industrial: NÃO limpar o histórico automaticamente no boot.
    // Carregamos o histórico persistido e o restauramos na FaultHistory (RAM).
    // Se a demonstração native precisar de um boot limpo, use
    // CLEAR_FAULT_HISTORY_ON_BOOT = true (comportamento explícito e configurável).
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

    // Inicializa o driver de MOSFET (mock por padrão, ou HAL real no esp32dev).
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
    // Mantém potência do pack consistente com a corrente atual.
    pack.current = currentData.currentA;
    pack.power = pack.totalVoltage * pack.current;

    //-------------------------------------------------
    // Temperature
    //-------------------------------------------------

    temperature_.update();

    TemperatureData temperatureData = temperature_.getData();

    // Atualiza temperaturas no estado compartilhado do BatteryPack
    // Temporariamente, caso ainda não exista temperatura média real no TemperatureData,
    // copiamos maxTemperature para todos os campos.
    pack.averageTemperature = temperatureData.averageTemperature;
    pack.maxTemperature = temperatureData.maxTemperature;
    pack.minTemperature = temperatureData.minTemperature;

//-------------------------------------------------
    // Fault / Protection
    //-------------------------------------------------

    // LEGACY COMPATIBILITY ONLY:
    // fault_.evaluate(pack) está desabilitado como mecanismo primário.
    // A detecção agora é centralizada no ProtectionManager, que chama
    // fault_.raiseFault()/clearFault() para gerar os DTCs.
    // Manter evaluate() desabilitado evita duplicação de DTC entre os
    // dois caminhos de detecção (legado FaultFlag vs novo DTC).
    //
    // fault_.evaluate(pack);

    protection_.update(pack, fault_);

    //-------------------------------------------------
    // BMS State Machine (Sprint 5.2 — Passo 2)
    // A máquina ainda NÃO controla MOSFET; apenas informa seu estado.
    //-------------------------------------------------

    bmsStateMachine_.update();

    //-------------------------------------------------
    // BMS State
    //-------------------------------------------------

    stateManager_.update(pack, fault_);

    // MOSFET é uma ação derivada do estado: State → MOSFET → Telemetria
    mosfetDriver_->applyState(stateManager_.getState());

    // Telemetria lê o estado real dos MOSFETs
    pack.charging = mosfetDriver_->chargeEnabled();
    pack.discharging = mosfetDriver_->dischargeEnabled();
    pack.balancing = mosfetDriver_->balanceEnabled();

    //-------------------------------------------------
    // Balance (futuro: integrar ao MosfetController)
    //-------------------------------------------------

    balance_.update(pack, protection_, battery_);

    // Para o v1.0 industrial, a camada de MosfetController é a fonte de verdade.
    // ProtectionManager continua existindo para gerar falhas e flags, mas não sobrescreve MOSFETs.
    (void)balance_.getStatus();

    printTelemetry(pack);

    //-------------------------------------------------
    // Telemetria industrial de falhas
    //-------------------------------------------------

    const FaultInfo& fi = fault_.getFaultInfo();

    // Push fault history only when we detect a NEW event
    const bool newEvent =
        (fi.active != lastFaultActive_) ||
        (fi.reason != lastFaultReason_) ||
        (fi.code != lastFaultCode_) ||
        (fi.source != lastFaultSource_);

    // Edge detection para gravação no FaultStorage
    static bool faultAlreadySaved = false;

    if (newEvent && fi.active && !faultAlreadySaved)
    {
        FaultInfo snapshot = fi;
        snapshot.timestamp = static_cast<std::uint32_t>(Clock::millis());

        // Deduplicação: percorre o faultHistory_ (já carregado no init) e evita append
        // se já existir entry com code+reason+source.
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

        // Mesmo que seja duplicata, marcamos como salvo para não reavaliar no mesmo active.
        faultAlreadySaved = true;

        lastFaultActive_ = fi.active;
        lastFaultReason_ = fi.reason;
        lastFaultCode_ = fi.code;
        lastFaultSource_ = fi.source;
    }
    else if (!fi.active)
    {
        faultAlreadySaved = false;

        // mantém atualização dos last* para nova detecção
        lastFaultActive_ = fi.active;
        lastFaultReason_ = fi.reason;
        lastFaultCode_ = fi.code;
        lastFaultSource_ = fi.source;
    }

    if (fi.active)
    {
        printf("\n=============== BMS INDUSTRIAL ===============\n");
        printf("STATE MACHINE : %s\n", bmsStateMachine_.toString());
        printf("STATE         : %s\n\n", stateManager_.toString());

        printf("FAULT CODE   : 0x%04X\n", fi.code);
        printf("FAULT NAME   : %s\n", faultReasonToString(fi.reason));
        printf("SOURCE CELL  : %u\n", static_cast<unsigned>(fi.source));
        printf("MEASURED     : %.3f V\n", fi.value);
        printf("LIMIT        : %.3f V\n\n", fi.limit);

        printf("ACTION\n");
        printf(" CHARGE MOSFET      %s\n", mosfetDriver_->chargeEnabled() ? "ON" : "OFF");
        printf(" DISCHARGE MOSFET   %s\n", mosfetDriver_->dischargeEnabled() ? "ON" : "OFF");
        printf(" BALANCE            %s\n", mosfetDriver_->balanceEnabled() ? "ON" : "OFF");
        printf("==============================================\n\n");
    }

    //-------------------------------------------------
    // Proteção (logs de apoio)
    //-------------------------------------------------

    char protectionLog[200];

    // Log do State Machine (BMS)
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

    // Sprint 3.2 — Exibir Fault History (após status industrial)
    printFaultHistory();

    //-------------------------------------------------
    // Heartbeat (Sprint Heartbeat)
    // Alimenta dados (estado, SOC, temperatura) e emite a cada 1000 ms.
    //-------------------------------------------------

    heartbeat_.setState(bmsStateMachine_.toString());
    heartbeat_.setSOC(pack.soc);
    heartbeat_.setTemperature(pack.averageTemperature);

    heartbeat_.update();
}
