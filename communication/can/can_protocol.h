#pragma once

#include "can_frame.h"

// Placeholder para camada de protocolo: serialização/deserialização e
// definição de payloads.
class CANProtocol
{
public:
    static CANFrame makeHeartbeat(uint8_t value);
};

