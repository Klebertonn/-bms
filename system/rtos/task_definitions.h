#pragma once

#include <stdint.h>

#include "task_priorities.h"

// Definições de tarefas (placeholder com períodos em ms).

enum class TaskId
{
    PROTECTION,
    BATTERY,
    CURRENT,
    TEMPERATURE,
    BALANCE,
    SOC,
    LOGGER,
    CAN_TX,
    CAN_RX,
    BLE,
    STORAGE,
    DISPLAY,
    DIAGNOSTICS,
    WATCHDOG
};

struct TaskDefinition
{
    TaskId id;
    uint32_t periodMs;
    TaskPriority priority;
};


