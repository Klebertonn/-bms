#include "scheduler.h"

#include <algorithm>

#include "../clock/clock.h"

void Scheduler::addTask(Task* task)
{
    if (task == nullptr)
        return;

    tasks_.push_back(task);
}

void Scheduler::addTask(Task* task, TaskPriority priority)
{
    if (task == nullptr)
        return;

    task->priority = priority;
    tasks_.push_back(task);
}

void Scheduler::setTaskEnabled(Task* task, bool enabled)
{
    if (task == nullptr)
        return;

    task->enabled = enabled;
}

void Scheduler::sortByPriority()
{
    // Ordena por prioridade decrescente (VERY_HIGH primeiro).
    std::sort(tasks_.begin(), tasks_.end(),
              [](const Task* a, const Task* b) {
                  if (a == nullptr || b == nullptr)
                  {
                      return false;
                  }
                  return static_cast<int>(a->priority) > static_cast<int>(b->priority);
              });
}

void Scheduler::run()
{
    const uint64_t now = Clock::millis();

    // Executa em ordem de prioridade (mais alta primeiro).
    sortByPriority();

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


