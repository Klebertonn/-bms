#pragma once

// Stub: Watchdog baseado em heartbeats.
class WatchdogManager
{
public:
    void init();

    void heartbeat(unsigned /*taskIndex*/);

    void update();
};

