#pragma once

class BLEManager
{
public:
    void init();
    void update();

    bool connected() const;

private:
    void createServices();
    void processCommands();
    void publishTelemetry();

    bool isConnected = false;
};

