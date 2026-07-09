#pragma once

// Prioridades (stubs) - ajuste fino será feito junto com uxTaskGetStackHighWaterMark
// e métricas de latência/tempo de execução.

enum class TaskPriority
{
    VERY_HIGH = 4,
    HIGH = 3,
    MEDIUM = 2,
    LOW = 1
};

