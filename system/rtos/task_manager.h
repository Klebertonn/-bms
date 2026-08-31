#pragma once

#include "task_definitions.h"

class TaskManager
{
public:
    void init();

    // Cria tarefas FreeRTOS em firmware real.
    void start();

private:
    void configureTaskList();
};

