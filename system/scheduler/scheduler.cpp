#include "scheduler.h"

#include "../clock/clock.h"

void Scheduler::addTask(Task* task)
{
    if (task == nullptr)
        return;

    tasks_.push_back(task);
}

void Scheduler::setTaskEnabled(Task* task, bool enabled)
{
    if (task == nullptr)
        return;

    task->enabled = enabled;
}

void Scheduler::run()
{
    const uint64_t now = Clock::millis();

    for (Task* task : tasks_)
    {
        if (task == nullptr)
            continue;

        if (!task->enabled)
            continue;

        const uint64_t last = task->last_run_ms;
        const uint32_t period = task->period_ms;

        if ((now - last) >= period)
        {
            if (task->callback)
            {
                task->callback();
            }

            task->last_run_ms = now;
        }
    }
}


