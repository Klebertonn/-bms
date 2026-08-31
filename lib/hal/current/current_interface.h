#pragma once

// Interface HAL para sensores de corrente do pack.
class ICurrentSensor
{
public:
    virtual ~ICurrentSensor() = default;

    // Inicializa o sensor de corrente (ex.: INA219 via I²C).
    // Retorna true se o sensor foi inicializado com sucesso.
    virtual bool begin() = 0;

    // Corrente do pack (A). Sinal positivo = carregando (conforme convenção do projeto).
    virtual float readCurrentA() = 0;

    // Tensao do shunt (mV) - opcional para diagnóstico/calibração.
    virtual float readShuntVoltageMv() = 0;
};

