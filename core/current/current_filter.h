#pragma once

class CurrentFilter
{
public:
    void init();

    float update(float sample);

private:
    static constexpr float alpha = 0.20f;
    float filtered = 0.0f;
};

