#include "task_manager.h"

#include <stdint.h>

#include "queue_manager.h"
#include "mutex_manager.h"
#include "event_manager.h"
#include "watchdog_manager.h"

static QueueManager g_queueManager;
static MutexManager g_mutexManager;
static EventManager g_eventManager;
static WatchdogManager g_watchdog;

void TaskManager::init()
{
    g_queueManager.init();
    g_mutexManager.init();
    g_eventManager.init();
    g_watchdog.init();

    configureTaskList();
}

void TaskManager::start()
{
    // Stub: aqui entra xTaskCreate/xTaskStartScheduler no firmware real.
}

void TaskManager::configureTaskList()
{
    // Stub: manter tabela de tarefas e prioridades (documentar períodos).
    // A implementação real usará TaskDefinition.
}

