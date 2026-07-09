#include "logger.h"

#include "event_queue.h"
#include "log_formatter.h"

#include <stdio.h>
#include <string.h>

static uint32_t g_fakeTimestamp = 0;

void Logger::init()
{
    queue.init();
}

void Logger::log(const Event& event)
{
    Event e = event;
    e.timestamp = (e.timestamp != 0) ? e.timestamp : (++g_fakeTimestamp);

    // Enqueue (fallback: também publica diretamente se cheio)
    if (!queue.push(e))
    {
        publish(e);
        save(e);
        return;
    }
}

void Logger::info(EventSource src, uint16_t code, const char* text)
{
    Event e;
    e.type = EventType::INFO;
    e.source = src;
    e.code = code;
    if (text)
        strncpy(e.message, text, sizeof(e.message) - 1);

    log(e);
}

void Logger::warning(EventSource src, uint16_t code, const char* text)
{
    Event e;
    e.type = EventType::WARNING;
    e.source = src;
    e.code = code;
    if (text)
        strncpy(e.message, text, sizeof(e.message) - 1);

    log(e);
}

void Logger::error(EventSource src, uint16_t code, const char* text)
{
    Event e;
    e.type = EventType::ERROR;
    e.source = src;
    e.code = code;
    if (text)
        strncpy(e.message, text, sizeof(e.message) - 1);

    log(e);
}

void Logger::critical(EventSource src, uint16_t code, const char* text)
{
    Event e;
    e.type = EventType::CRITICAL;
    e.source = src;
    e.code = code;
    if (text)
        strncpy(e.message, text, sizeof(e.message) - 1);

    log(e);
}

void Logger::flush()
{
    Event e;
    while (queue.pop(e))
    {
        publish(e);
        save(e);
    }
}

void Logger::publish(const Event& event)
{
    char buf[200];
    LogFormatter::format(event, buf, sizeof(buf));

    // Para desktop: imprime via stdout.
    // No firmware real, trocar por Serial/USB.
    printf("%s\n", buf);
}

void Logger::save(const Event& event)
{
    // Stub: persistência real será NVS/Flash/EEPROM/SD.
    (void)event;
}

