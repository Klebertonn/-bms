#pragma once

#include <stdint.h>

namespace CANID
{
    constexpr uint32_t HEARTBEAT = 0x100;
    constexpr uint32_t PACK_STATUS = 0x101;
    constexpr uint32_t CELL_VOLTAGES = 0x102;
    constexpr uint32_t TEMPERATURES = 0x103;
    constexpr uint32_t CURRENT = 0x104;
    constexpr uint32_t SOC = 0x105;
    constexpr uint32_t ALARMS = 0x106;
    constexpr uint32_t BALANCING = 0x107;
    constexpr uint32_t CONFIG = 0x108;
}

