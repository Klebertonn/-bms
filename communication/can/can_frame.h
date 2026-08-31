#pragma once

#include <stdint.h>

struct CANFrame
{
    uint32_t id = 0;

    uint8_t dlc = 0;

    uint8_t data[8]{};
};

