#include <iostream>

#include "app/app.h"
#include "system/clock/clock.h"
#include "system/scheduler/scheduler.h"

#include "system/logger/logger.h"

static Logger g_logger;

static void heartbeatTask()
{
    g_logger.info(EventSource::SYSTEM, 9001, "Heartbeat");
    g_logger.flush();
}

int main_scheduler_demo_disabled()
{
    g_logger.init();

    Scheduler scheduler;

    Task heartbeat;
    heartbeat.name = "HEARTBEAT";
    heartbeat.period_ms = 100;
    heartbeat.last_run_ms = 0;
    heartbeat.enabled = true;
    heartbeat.callback = &heartbeatTask;

    scheduler.addTask(&heartbeat);

    std::cout << "=== Scheduler demo start ===" << std::endl;

    while (true)
    {
        scheduler.run();
    }

    // Unreachable.
    return 0;
}


