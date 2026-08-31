#include "current_manager.h"

#include "lib/hal/current/current_interface.h"

// Definido no HAL (para mock/hardware real)
extern ICurrentSensor& currentSensor;

void CurrentManager::init()
{
    filter.init();
    data = {};
    data.direction = CurrentDirection::IDLE;
}

void CurrentManager::update(float packVoltage)
{
    const float rawCurrentA = currentSensor.readCurrentA();
    const float rawShuntVoltageMv = currentSensor.readShuntVoltageMv();

    data.currentA = filter.update(rawCurrentA);
    data.shuntVoltageMv = rawShuntVoltageMv;

    determineDirection();
    calculatePower(packVoltage);

    // Nesta versão inicial, média = valor filtrado.
    // Evoluir para janela móvel/integração temporal.
    data.averageCurrentA = data.currentA;
}

CurrentData CurrentManager::getData() const
{
    return data;
}

void CurrentManager::determineDirection()
{
    if (data.currentA > 0.2f)
        data.direction = CurrentDirection::CHARGING;
    else if (data.currentA < -0.2f)
        data.direction = CurrentDirection::DISCHARGING;
    else
        data.direction = CurrentDirection::IDLE;
}

void CurrentManager::calculatePower(float voltage)
{
    data.powerW = voltage * data.currentA;
}

