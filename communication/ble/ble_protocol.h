#pragma once

#include <stdint.h>

#include "ble_commands.h"

struct BLECommandPacket
{
    BLECommand cmd = BLECommand::NONE;
    uint8_t payload[16]{};
};

class BLEProtocol
{
public:
    static BLECommandPacket decode(const uint8_t* data, uint32_t len);
};

