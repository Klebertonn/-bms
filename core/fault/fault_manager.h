#pragma once

#include <cstddef>
#include <cstdint>

#include "fault_flags.h"
#include "fault_reason.h"
#include "fault_code.h"
#include "fault_event.h"
#include "fault_sink.h"
#include "../battery/battery_pack.h"

/*
 * ==========================================================
 * FaultManager – Coordenador de falhas DTC
 * ==========================================================
 * Centraliza o gerenciamento do ciclo de vida das falhas:
 *   - recebe eventos (raiseFault) dos managers do domínio
 *   - cria FaultEvent (código, severidade, timestamp, estado, ocorrência)
 *   - registra no logger (via IFaultLogSink)
 *   - envia para storage (via IFaultStorageSink)
 *   - atualiza o histórico (FaultHistory)
 *
 * REGRAS DE ARQUITETURA:
 *   - NÃO depende de hardware, ESP32, HAL ou drivers.
 *   - Depende apenas de abstrações (sinks) e da estrutura FaultEvent.
 *   - Somente gerencia eventos (Clean Architecture / SOLID / DI).
 *
 * A API legada (FaultFlag, evaluate, addFault, getFaultInfo) é mantida
 * para compatibilidade com o restante do código existente.
 * ==========================================================
 */

class FaultManager
{
public:
    /* Capacidade máxima de eventos ativos (registro em RAM). */
    static constexpr std::size_t MAX_ACTIVE_FAULTS = 16u;

    void init();

    /* --------------------------------------------------
     * API DTC (Diagnostic Trouble Code)
     * -------------------------------------------------- */

    /* Registra uma falha. Cria o evento, loga, persiste e atualiza histórico. */
    void raiseFault(FaultCode code);

    /* Registra uma falha com severidade/estado personalizados. */
    void raiseFault(FaultCode code, FaultSeverity severity, FaultState state);

    /* Registra uma falha com contexto diagnóstico completo (origem, valor e limite). */
    void raiseFault(FaultCode code,
                    FaultSeverity severity,
                    FaultState state,
                    std::uint8_t source,
                    float measuredValue,
                    float limit);

    /* Limpa uma falha ativa (marca CLEARED). */
    void clearFault(FaultCode code);

    /* true se existe alguma falha ativa. */
    bool hasFault() const;

    /* true se o código informado está ativo. */
    bool hasFault(FaultCode code) const;

    /* Número de falhas ativas. */
    std::size_t getActiveFaults() const;

    /* Obtém um evento ativo por índice. Retorna false se índice inválido. */
    bool getActiveFault(std::size_t index, FaultEvent& out) const;

    /* --------------------------------------------------
     * Injeção de dependência (sinks)
     * -------------------------------------------------- */
    void setLogSink(IFaultLogSink* sink);
    void setStorageSink(IFaultStorageSink* sink);

    /* --------------------------------------------------
     * API legada (compatibilidade)
     * -------------------------------------------------- */
    void clear();
    void evaluate(const BatteryPack& pack);
    void addFault(FaultFlag fault);
    bool hasFault(FaultFlag fault) const;
    std::uint32_t getFaults() const;
    const FaultInfo& getFaultInfo() const { return faultInfo_; }

private:
    /* Encontra índice de um código ativo; retorna MAX_ACTIVE_FAULTS se não existe. */
    std::size_t findActive(FaultCode code) const;

    void publishToLog(const FaultEvent& event);
    void persistToStorage(const FaultEvent& event);

    std::uint32_t flags = FAULT_NONE;
    FaultInfo faultInfo_{};

    /* Eventos ativos (DTC) */
    FaultEvent activeFaults_[MAX_ACTIVE_FAULTS];
    std::size_t activeCount_ = 0;

    /* Sinks (abstrações) – injetados pela camada de aplicação */
    IFaultLogSink* logSink_ = nullptr;
    IFaultStorageSink* storageSink_ = nullptr;
};
