#pragma once

// Interface HAL para sensores de corrente do pack.
class ICurrentSensor
{
public:
    virtual ~ICurrentSensor() = default;

    // Corrente do pack (A). Sinal positivo = carregando (conforme convenção do projeto).
    virtual float readCurrentA() = 0;

    // Tensao do shunt (mV) - opcional para diagnóstico/calibração.
    virtual float readShuntVoltageMv() = 0;
};

