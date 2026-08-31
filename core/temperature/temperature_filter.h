#pragma once

class TemperatureFilter
{
public:
    void init();

    float update(float sample);

private:
    float filtered = 25.0f;

    static constexpr float alpha = 0.15f;
};

