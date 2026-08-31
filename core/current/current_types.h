#pragma once

enum class CurrentDirection
{
    IDLE,
    CHARGING,
    DISCHARGING
};

struct CurrentData
{
    float currentA = 0.0f;
    float averageCurrentA = 0.0f;
    float powerW = 0.0f;
    float shuntVoltageMv = 0.0f;

    CurrentDirection direction = CurrentDirection::IDLE;
};

