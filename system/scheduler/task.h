#pragma once

#include <cstdint>

#include "../rtos/task_priorities.h"

struct Task
{
    const char* name = nullptr;

    uint32_t period_ms = 0;

    uint64_t last_run_ms = 0;

    bool enabled = true;

    // Prioridade de execução (mais alto executa primeiro no scheduler cooperativo)
    TaskPriority priority = TaskPriority::MEDIUM;

    void (*callback)() = nullptr;
};

