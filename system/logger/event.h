#pragma once

#include <stdint.h>

#include "event_types.h"

struct Event
{
    uint32_t timestamp = 0;

    EventType type = EventType::INFO;

    EventSource source = EventSource::SYSTEM;

    uint16_t code = 0;

    char message[128] = {0};
};

