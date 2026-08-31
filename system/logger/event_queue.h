#pragma once

#include <stddef.h>
#include <stdint.h>

#include "event.h"

// Ring buffer simples (placeholder de fila).
class EventQueue
{
public:
    static constexpr size_t Capacity = 64;

    void init();

    bool push(const Event& e);

    bool pop(Event& out);

    size_t size() const;

private:
    Event buffer[Capacity];
    size_t head = 0;
    size_t tail = 0;
    size_t count = 0;
};

