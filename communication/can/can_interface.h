#pragma once

#include "can_frame.h"

class ICANDriver
{
public:
    virtual ~ICANDriver() = default;

    virtual bool send(const CANFrame& frame) = 0;

    virtual bool receive(CANFrame& frame) = 0;

    virtual bool available() = 0;
};

