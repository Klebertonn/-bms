#include "event_queue.h"

void EventQueue::init()
{
    head = 0;
    tail = 0;
    count = 0;
}

bool EventQueue::push(const Event& e)
{
    if (count >= Capacity)
        return false;

    buffer[tail] = e;
    tail = (tail + 1) % Capacity;
    count++;
    return true;
}

bool EventQueue::pop(Event& out)
{
    if (count == 0)
        return false;

    out = buffer[head];
    head = (head + 1) % Capacity;
    count--;
    return true;
}

size_t EventQueue::size() const
{
    return count;
}

