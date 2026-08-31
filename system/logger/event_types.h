#pragma once

enum class EventType
{
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
    DEBUG
};

enum class EventSource
{
    SYSTEM,
    BATTERY,
    PROTECTION,
    BALANCE,
    CURRENT,
    TEMPERATURE,
    SOC,
    CAN,
    BLE,
    STORAGE,
    USER
};

