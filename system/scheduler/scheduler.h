#pragma once

#include <vector>

#include "task.h"

class Scheduler
{
public:
    void addTask(Task* task);

    // Executes due tasks (cooperative scheduler).
    void run();

    // Optional: enable/disable tasks without removing them.
    void setTaskEnabled(Task* task, bool enabled);

private:
    std::vector<Task*> tasks_{};
};

