#pragma once

#include <cstdint>

struct Task
{
    const char* name = nullptr;

    uint32_t period_ms = 0;

    uint64_t last_run_ms = 0;

    bool enabled = true;

    void (*callback)() = nullptr;
};

