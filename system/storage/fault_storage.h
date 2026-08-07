#pragma once

#include <vector>

#include "../../core/fault/fault_reason.h"
#include "../../core/fault/fault_event.h"

/*
 * ==========================================================
 * FaultStorage – Persistência do histórico de falhas
 * ==========================================================
 * Abstrai o backend de armazenamento do histórico de falhas.
 *
 * Suporta dois formatos:
 *   - FaultInfo  (legado, compatibilidade)
 *   - FaultEvent (novo modelo DTC)
 *
 * A implementação NÃO pertence ao domínio core/fault; fica na
 * camada de infraestrutura (system/storage). O FaultManager
 * depende apenas da abstração IFaultStorageSink (fault_sink.h).
 * ==========================================================
 */

class FaultStorage
{
public:
    FaultStorage();

    bool init();

    /* ---- Formato legado (FaultInfo) ---- */
    bool append(const FaultInfo& fault);
    bool load(std::vector<FaultInfo>& history);
    bool clear();

    /* ---- Formato DTC (FaultEvent) ---- */
    bool appendEvent(const FaultEvent& event);
    bool loadEvents(std::vector<FaultEvent>& history);
};
