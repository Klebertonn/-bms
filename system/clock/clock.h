#pragma once

#include <cstdint>

class Clock
{
public:
    // Milliseconds since boot (or since first call implementation).
    static uint64_t millis();
};

