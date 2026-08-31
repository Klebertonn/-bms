#pragma once

#include "fault_event.h"

/*
 * ==========================================================
 * Interfaces de saída (sinks) do FaultManager
 * ==========================================================
 * Implementam Inversão de Dependência (SOLID - D):
 *
 *   - O FaultManager depende de abstrações (interfaces), não de
 *     implementações concretas de logger/storage.
 *   - O domínio (core/fault) não conhece Logger, Storage, ESP32,
 *     HAL ou qualquer backend específico.
 *   - A composição (Dependency Injection) é feita na camada de
 *     aplicação (App), que injeta os adaptadores concretos.
 * ==========================================================
 */

/*
 * Sink de log: o FaultManager publica eventos de falha sem conhecer
 * o backend de log. A implementação concreta é injetada de fora.
 */
class IFaultLogSink
{
public:
    virtual ~IFaultLogSink() = default;

    /* Registra uma falha no sistema de log. */
    virtual void onFaultLogged(const FaultEvent& event) = 0;
};

/*
 * Sink de persistência: o FaultManager envia o evento para storage
 * sem conhecer o backend (flash/NVS/EEPROM/arquivo).
 */
class IFaultStorageSink
{
public:
    virtual ~IFaultStorageSink() = default;

    /* Persiste um evento de falha. */
    virtual bool onFaultPersist(const FaultEvent& event) = 0;
};
