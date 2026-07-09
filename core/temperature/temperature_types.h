#pragma once

#define MAX_TEMP_SENSORS 8

enum class TemperatureSensorStatus
{
    OK,
    DISCONNECTED,
    SHORT_CIRCUIT,
    INVALID
};

struct TemperatureData
{
    float sensors[MAX_TEMP_SENSORS]{};

    float maxTemperature = 0.0f;

    float minTemperature = 0.0f;

    float averageTemperature = 0.0f;

    int hottestSensor = 0;

    TemperatureSensorStatus status[MAX_TEMP_SENSORS]{};
};

