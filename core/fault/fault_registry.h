#pragma once

#include <cstddef>
#include <cstdint>

#include "fault_code.h"
#include "fault_event.h"

/*
 * ==========================================================
 * FaultRegistry – Registro de códigos DTC
 * ==========================================================
 * Responsável por:
 *   - cadastrar falhas (metadados: descrição, severidade padrão)
 *   - buscar descrição a partir de um código
 *   - converter código para texto legível
 *
 * Implementa o princípio de "registry" (tabela de metadados).
 * NÃO depende de hardware, ESP32, HAL ou drivers.
 * ==========================================================
 */

/* Metadados de um código DTC cadastrado
 *
 * NOTA: mantido como "aggregate" (sem inicializadores de membro) para
 * permitir inicialização por lista uniforme mesmo em C++11 estrito.
 * Campos:
 *   - code        : código DTC (FaultCode)
 *   - description : descrição legível da falha
 *   - severity    : severidade padrão da falha
 */
struct FaultMeta
{
    FaultCode code;
    const char* description;
    FaultSeverity severity;
};

class FaultRegistry
{
public:
    /* Valor que indica que um código não foi encontrado */
    static constexpr std::size_t NOT_FOUND = static_cast<std::size_t>(-1);

    /* Retorna a tabela de metadados (register) dos códigos suportados. */
    static const FaultMeta* table();
    static std::size_t      size();

    /* Busca a descrição de um código. Retorna nullptr se não encontrado. */
    static const char* findDescription(FaultCode code);

    /* Busca a severidade padrão de um código. */
    static FaultSeverity findSeverity(FaultCode code);

    /* Converte um código para texto (busca na tabela). */
    static const char* codeToString(FaultCode code);

    /* Converte um texto para código (inverso). */
    static FaultCode stringToCode(const char* text);

    /* Busca o índice de um código na tabela. */
    static std::size_t findIndex(FaultCode code);
};
