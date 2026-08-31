#pragma once

class CoulombCounter
{
public:
    void init(float nominalCapacityAh);

    void update(float currentA,
                float deltaTimeSeconds);

    float getConsumedAh() const;
    float getChargedAh() const;

    void reset();

private:
    float consumedAh = 0;
    float chargedAh = 0;

    float nominalCapacity = 0;
};

