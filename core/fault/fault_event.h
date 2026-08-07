#pragma once

#include <cstdint>
#include <cstring>

#include "fault_code.h"

/*
 * ==========================================================
 * FaultEvent – Evento de falha DTC
 * ==========================================================
 * Representa uma ocorrência de falha no sistema de diagnóstico.
 *
 * Contém o código da falha, severidade, timestamp, estado do
 * ciclo de vida, contador de ocorrências e descrição legível.
 *
 * Este arquivo pertence ao domínio (core/fault) e NÃO depende de
 * hardware, ESP32, HAL ou drivers.
 * ==========================================================
 */

/* Nível de severidade da falha */
enum class FaultSeverity : std::uint8_t
{
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
    FATAL
};

/* Estado do ciclo de vida da falha */
enum class FaultState : std::uint8_t
{
    ACTIVE,    // Condição presente agora
    CLEARED,   // Condição normalizada / limpa
    WARNING,   // Pré-falha (alerta)
    CRITICAL,  // Falha grave
    LOCKED     // Falha latched (exige reset manual)
};

/* Tamanho máximo da descrição do evento */
constexpr std::size_t FAULT_DESC_MAX = 64u;

struct FaultEvent
{
    FaultCode code = FaultCode::NONE;          // Código DTC
    FaultSeverity severity = FaultSeverity::INFO; // Severidade
    FaultState state = FaultState::ACTIVE;     // Estado da falha
    std::uint32_t timestamp = 0;               // Tempo do evento (ms)
    std::uint32_t occurrence = 0;              // Contador de ocorrências
    char description[FAULT_DESC_MAX] = {0};    // Descrição legível

    // Atalho: define a descrição com segurança (trunca se necessário).
    void setDescription(const char* text)
    {
        if (text == nullptr)
        {
            description[0] = '\0';
            return;
        }
        std::strncpy(description, text, sizeof(description) - 1u);
        description[sizeof(description) - 1u] = '\0';
    }
};

/*
 * Converte um estado de falha para texto (para telemetria/log).
 * Retorna string estática — não precisa liberar.
 */
static inline const char* faultStateToString(FaultState state)
{
    switch (state)
    {
        case FaultState::ACTIVE:   return "ACTIVE";
        case FaultState::CLEARED:  return "CLEARED";
        case FaultState::WARNING:  return "WARNING";
        case FaultState::CRITICAL: return "CRITICAL";
        case FaultState::LOCKED:   return "LOCKED";
        default:                   return "UNKNOWN";
    }
}

/*
 * Converte uma severidade de falha para texto.
 */
static inline const char* faultSeverityToString(FaultSeverity sev)
{
    switch (sev)
    {
        case FaultSeverity::INFO:     return "INFO";
        case FaultSeverity::WARNING:  return "WARNING";
        case FaultSeverity::ERROR:    return "ERROR";
        case FaultSeverity::CRITICAL: return "CRITICAL";
        case FaultSeverity::FATAL:    return "FATAL";
        default:                      return "UNKNOWN";
    }
}
