#pragma once

#include "event.h"

#include "event_queue.h"

class Logger
{
public:

    void init();

    void log(const Event& event);

    void info(EventSource src, uint16_t code, const char* text);
    void warning(EventSource src, uint16_t code, const char* text);
    void error(EventSource src, uint16_t code, const char* text);
    void critical(EventSource src, uint16_t code, const char* text);

    void flush();

private:
    void save(const Event& event);
    void publish(const Event& event);

    EventQueue queue;
};

