#include "temperature_manager.h"

#include "lib/hal/temperature/temperature_interface.h"

extern ITemperatureSensor& temperatureSensor;

void TemperatureManager::init()
{
    for (int i = 0; i < MAX_TEMP_SENSORS; i++)
    {
        filters[i].init();
        data.status[i] = TemperatureSensorStatus::INVALID;
    }
}

void TemperatureManager::update()
{
    for (int i = 0; i < MAX_TEMP_SENSORS; i++)
    {
        const float value = temperatureSensor.readTemperature(i);
        data.sensors[i] = filters[i].update(value);
    }

    calculateStatistics();
    detectFailures();
}

TemperatureData TemperatureManager::getData() const
{
    return data;
}

void TemperatureManager::calculateStatistics()
{
    data.maxTemperature = -100.0f;
    data.minTemperature = 200.0f;

    float sum = 0.0f;
    data.hottestSensor = 0;

    for (int i = 0; i < MAX_TEMP_SENSORS; i++)
    {
        const float t = data.sensors[i];
        sum += t;

        if (t > data.maxTemperature)
        {
            data.maxTemperature = t;
            data.hottestSensor = i;
        }

        if (t < data.minTemperature)
            data.minTemperature = t;
    }

    data.averageTemperature = sum / MAX_TEMP_SENSORS;
}

void TemperatureManager::detectFailures()
{
    for (int i = 0; i < MAX_TEMP_SENSORS; i++)
    {
        const float t = data.sensors[i];

        if (t < -50.0f)
            data.status[i] = TemperatureSensorStatus::SHORT_CIRCUIT;
        else if (t > 150.0f)
            data.status[i] = TemperatureSensorStatus::DISCONNECTED;
        else
            data.status[i] = TemperatureSensorStatus::OK;
    }
}

