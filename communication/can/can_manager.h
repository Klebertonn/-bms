#pragma once

#include "can_frame.h"

class CANManager
{
public:
    void init();
    void update();

    void sendHeartbeat();
    void sendPackStatus();
    void sendSOC();
    void sendTemperatures();
    void sendVoltages();
    void receive();

private:
    void processFrame(const CANFrame& frame);
};

