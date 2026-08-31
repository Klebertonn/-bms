struct BatteryCell
{
    float voltage;
    float temperature;

    bool balancing;
    bool valid;

    bool overVoltage;
    bool underVoltage;
    bool overTemperature;
};
