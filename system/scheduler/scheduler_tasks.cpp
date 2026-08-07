#include "scheduler_tasks.h"

/*
 * ==========================================================
 * Instanciação das 8 tasks do Scheduler industrial.
 * ==========================================================
 * Fornece utilitários para configurar uma Task a partir de um
 * perfil (SchedulerTaskProfile) do scheduler_tasks.h.
 *
 * Não altera a lógica existente: apenas organiza prioridades e
 * períodos. Os callbacks devem ser atribuídos externamente
 * (ex.: App) para executar os módulos reais (Current, Temperature,
 * Protection, Logger, CAN, BLE, Dashboard, Watchdog).
 * ==========================================================
 */

// Configura uma Task a partir do perfil (nome, prioridade, período).
void configureTaskFromProfile(Task& task, const SchedulerTaskProfile& profile)
{
    task.name = profile.name;
    task.period_ms = profile.periodMs;
    task.priority = profile.priority;
    task.last_run_ms = 0;
    task.enabled = true;
    task.callback = nullptr; // atribuído externamente
}
