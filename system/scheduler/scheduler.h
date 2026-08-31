#pragma once

#include <vector>

#include "task.h"

class Scheduler
{
public:
    void addTask(Task* task);

    // Adiciona task com prioridade explícita (sobrescreve task->priority).
    void addTask(Task* task, TaskPriority priority);

    // Executes due tasks (cooperative scheduler).
    // As tasks são executadas em ordem de prioridade (mais alta primeiro).
    void run();

    // Optional: enable/disable tasks without removing them.
    void setTaskEnabled(Task* task, bool enabled);

private:
    // Ordena por prioridade decrescente antes de executar.
    void sortByPriority();

    std::vector<Task*> tasks_{};
};

