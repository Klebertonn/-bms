#include "log_formatter.h"

#include <string.h>

#include <stdio.h>
#include <stddef.h>



static const char* typeToStr(EventType t)
{
    switch (t)
    {
    case EventType::INFO: return "INFO";
    case EventType::WARNING: return "WARNING";
    case EventType::ERROR: return "ERROR";
    case EventType::CRITICAL: return "CRITICAL";
    case EventType::DEBUG: return "DEBUG";
    }
    return "INFO";
}

static const char* sourceToStr(EventSource s)
{
    switch (s)
    {
    case EventSource::SYSTEM: return "SYSTEM";
    case EventSource::BATTERY: return "BATTERY";
    case EventSource::PROTECTION: return "PROTECTION";
    case EventSource::BALANCE: return "BALANCE";
    case EventSource::CURRENT: return "CURRENT";
    case EventSource::TEMPERATURE: return "TEMPERATURE";
    case EventSource::SOC: return "SOC";
    case EventSource::CAN: return "CAN";
    case EventSource::BLE: return "BLE";
    case EventSource::STORAGE: return "STORAGE";
    case EventSource::USER: return "USER";
    }
    return "SYSTEM";
}

void LogFormatter::format(const Event& e, char* out, size_t outLen)
{
    if (!out || outLen == 0) return;

    // Mensagem curta e segura.
    // timestamp é stub; no firmware real virá de um clock.
    snprintf(out, outLen, "[%s] %s code=%u msg=%s",
             typeToStr(e.type), sourceToStr(e.source),
             static_cast<unsigned>(e.code), e.message);
}

