#include "ble_protocol.h"

#include <cstring>

/*
 * ==========================================================
 * Helpers de empacotamento (little-endian)
 * ==========================================================
 */

// Escreve um uint16 em little-endian no buffer.
static void putU16(std::uint8_t* buf, std::uint16_t value)
{
    buf[0] = static_cast<std::uint8_t>(value & 0xFF);
    buf[1] = static_cast<std::uint8_t>((value >> 8) & 0xFF);
}

// Escreve um uint32 em little-endian no buffer.
static void putU32(std::uint8_t* buf, std::uint32_t value)
{
    buf[0] = static_cast<std::uint8_t>(value & 0xFF);
    buf[1] = static_cast<std::uint8_t>((value >> 8) & 0xFF);
    buf[2] = static_cast<std::uint8_t>((value >> 16) & 0xFF);
    buf[3] = static_cast<std::uint8_t>((value >> 24) & 0xFF);
}

// Converte float para inteiro pela escala (com clamp).
static std::int32_t floatToScaled(float value, float scale)
{
    constexpr float MAX_SCALED = 32767.0f;
    constexpr float MIN_SCALED = -32768.0f;

    float scaled = value * scale;

    if (scaled > MAX_SCALED)
    {
        scaled = MAX_SCALED;
    }
    else if (scaled < MIN_SCALED)
    {
        scaled = MIN_SCALED;
    }

    return static_cast<std::int32_t>(scaled);
}

/*
 * ==========================================================
 * BLEProtocol
 * ==========================================================
 */

std::uint16_t BLEProtocol::packFloat(float value, float scale)
{
    return static_cast<std::uint16_t>(floatToScaled(value, scale) & 0xFFFF);
}

std::uint8_t BLEProtocol::encodeBattery(const BatteryPack& pack, std::uint8_t* out)
{
    // Layout (10 bytes):
    //   [0..1] totalVoltage (0.01 V)  -> uint16
    //   [2..3] current (0.01 A)       -> int16
    //   [4]    soc (%)                -> uint8
    //   [5]    soh (%)                -> uint8
    //   [6]    valid (0/1)            -> uint8
    //   [7]    charging (0/1)         -> uint8
    //   [8]    discharging (0/1)      -> uint8
    //   [9]    balancing (0/1)        -> uint8
    putU16(&out[0], packFloat(pack.totalVoltage, 100.0f));
    putU16(&out[2], packFloat(pack.current, 100.0f));
    out[4] = pack.soc;
    out[5] = pack.soh;
    out[6] = pack.valid ? 1 : 0;
    out[7] = pack.charging ? 1 : 0;
    out[8] = pack.discharging ? 1 : 0;
    out[9] = pack.balancing ? 1 : 0;
    return 10;
}

std::uint8_t BLEProtocol::encodeState(std::uint8_t state, std::uint8_t* out)
{
    // Layout (1 byte): estado do BMS
    out[0] = state;
    return 1;
}

std::uint8_t BLEProtocol::encodeFault(const FaultInfo& fi, std::uint8_t* out)
{
    // Layout (11 bytes):
    //   [0]    active (0/1)           -> uint8
    //   [1]    reason (enum)          -> uint8
    //   [2..3] code (uint16)          -> uint16
    //   [4]    source                 -> uint8
    //   [5..6] value (0.01 V)         -> uint16
    //   [7..8] limit (0.01 V)         -> uint16
    //   [9..12] timestamp (uint32)    -> uint32
    out[0] = fi.active ? 1 : 0;
    out[1] = static_cast<std::uint8_t>(fi.reason);
    putU16(&out[2], fi.code);
    out[4] = fi.source;
    putU16(&out[5], packFloat(fi.value, 100.0f));
    putU16(&out[7], packFloat(fi.limit, 100.0f));
    putU32(&out[9], fi.timestamp);
    return 13;
}

std::uint8_t BLEProtocol::encodeTemperature(float avg, float maxV, float minV, std::uint8_t* out)
{
    // Layout (6 bytes):
    //   [0..1] média (0.1 °C)  -> int16
    //   [2..3] máx (0.1 °C)    -> int16
    //   [4..5] mín (0.1 °C)    -> int16
    putU16(&out[0], packFloat(avg, 10.0f));
    putU16(&out[2], packFloat(maxV, 10.0f));
    putU16(&out[4], packFloat(minV, 10.0f));
    return 6;
}

std::uint8_t BLEProtocol::encodeCurrent(float currentA, float shuntMv, std::uint8_t* out)
{
    // Layout (4 bytes):
    //   [0..1] corrente (0.01 A)   -> int16
    //   [2..3] shunt (0.01 mV)     -> uint16
    putU16(&out[0], packFloat(currentA, 100.0f));
    putU16(&out[2], packFloat(shuntMv, 100.0f));
    return 4;
}

std::uint8_t BLEProtocol::encodeConfig(float maxV, float minV, float maxT, float maxI, std::uint8_t* out)
{
    // Layout (8 bytes):
    //   [0..1] max voltage (0.01 V)  -> uint16
    //   [2..3] min voltage (0.01 V)  -> uint16
    //   [4..5] max temp (0.1 °C)     -> int16
    //   [6..7] max current (0.01 A)  -> uint16
    putU16(&out[0], packFloat(maxV, 100.0f));
    putU16(&out[2], packFloat(minV, 100.0f));
    putU16(&out[4], packFloat(maxT, 10.0f));
    putU16(&out[6], packFloat(maxI, 100.0f));
    return 8;
}

std::uint8_t BLEProtocol::encodeLog(std::uint32_t timestamp, std::uint16_t code, std::uint8_t* out)
{
    // Layout (6 bytes):
    //   [0..3] timestamp (uint32)  -> uint32
    //   [4..5] code (uint16)       -> uint16
    putU32(&out[0], timestamp);
    putU16(&out[4], code);
    return 6;
}
