#include "can_protocol.h"

#include "can_ids.h"

CANFrame CANProtocol::makeHeartbeat(uint8_t value)
{
    CANFrame frame;
    frame.id = CANID::HEARTBEAT;
    frame.dlc = 1;
    frame.data[0] = value;
    return frame;
}

