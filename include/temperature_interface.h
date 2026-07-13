#pragma once

#include <cstdint>

class ITemperatureSensor
{
public:
    virtual ~ITemperatureSensor() = default;

    // Lê a temperatura do sensor informado
    virtual float readTemperatureC(std::uint8_t sensor) = 0;
};