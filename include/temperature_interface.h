#pragma once

class ITemperatureSensor
{
public:

    virtual ~ITemperatureSensor() = default;

    virtual float readTemperatureC() = 0;
};