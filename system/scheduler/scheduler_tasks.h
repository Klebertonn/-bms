#pragma once

#include <cstdint>

#include "task.h"
#include "../rtos/task_priorities.h"

/*
 * ==========================================================
 * Definições das Tasks do Scheduler (industrial)
 * ==========================================================
 * Centraliza prioridade e período de cada task periódica.
 *
 *   Task Current      | HIGH       |  100 ms
 *   Task Temperature  | HIGH       |  100 ms
 *   Task Protection   | VERY_HIGH  |   50 ms
 *   Task Logger       | MEDIUM     | 1000 ms
 *   Task CAN          | MEDIUM     |  100 ms
 *   Task BLE          | LOW        |  500 ms
 *   Task Dashboard    | LOW        | 1000 ms
 *   Task Watchdog     | VERY_HIGH  |  100 ms
 *
 * Não altera a lógica de execução do Scheduler:
 * apenas organiza prioridades e períodos.
 * ==========================================================
 */

enum class SchedulerTaskId
{
    CURRENT,
    TEMPERATURE,
    PROTECTION,
    LOGGER,
    CAN,
    BLE,
    DASHBOARD,
    WATCHDOG
};

struct SchedulerTaskProfile
{
    SchedulerTaskId id;
    const char* name;
    TaskPriority priority;
    uint32_t periodMs;
};

// Nota: prioridades mais altas executam primeiro no scheduler cooperativo.
namespace SchedulerProfiles
{
    constexpr SchedulerTaskProfile CURRENT      = {SchedulerTaskId::CURRENT,      "CURRENT",      TaskPriority::HIGH,      100u};
    constexpr SchedulerTaskProfile TEMPERATURE  = {SchedulerTaskId::TEMPERATURE,  "TEMPERATURE",  TaskPriority::HIGH,      100u};
    constexpr SchedulerTaskProfile PROTECTION   = {SchedulerTaskId::PROTECTION,   "PROTECTION",   TaskPriority::VERY_HIGH,  50u};
    constexpr SchedulerTaskProfile LOGGER       = {SchedulerTaskId::LOGGER,       "LOGGER",       TaskPriority::MEDIUM,    1000u};
    constexpr SchedulerTaskProfile CAN          = {SchedulerTaskId::CAN,          "CAN",          TaskPriority::MEDIUM,     100u};
    constexpr SchedulerTaskProfile BLE          = {SchedulerTaskId::BLE,          "BLE",          TaskPriority::LOW,        500u};
    constexpr SchedulerTaskProfile DASHBOARD    = {SchedulerTaskId::DASHBOARD,    "DASHBOARD",    TaskPriority::LOW,       1000u};
    constexpr SchedulerTaskProfile WATCHDOG     = {SchedulerTaskId::WATCHDOG,     "WATCHDOG",     TaskPriority::VERY_HIGH, 100u};
}

// Configura uma Task a partir de um perfil (nome, prioridade, período).
// Declarada em scheduler_tasks.cpp.
void configureTaskFromProfile(Task& task, const SchedulerTaskProfile& profile);
