// Mock CAN driver.

#include "can_interface.h"

class MockCANDriver : public ICANDriver
{
public:
    bool begin() override
    {
        // Mock: sempre inicializado.
        return true;
    }

    bool send(const CANFrame& frame) override
    {
        (void)frame;
        return true;
    }

    bool receive(CANFrame& frame) override
    {
        (void)frame;
        return false;
    }

    bool available() override
    {
        return false;
    }
};

static MockCANDriver g_can;

ICANDriver& can = g_can;


