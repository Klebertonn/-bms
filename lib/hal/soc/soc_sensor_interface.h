#pragma once

/*
 * ==========================================================
 * Interface HAL — Sensor de SOC (Fuel Gauge)
 * ==========================================================
 * Abstrai sensores de estado de carga dedicados (ex.: MAX17048).
 * A lógica de negócio (SOCManager) pode consumir esta interface
 * para obter o SOC medido diretamente por hardware.
 * ==========================================================
 */
class ISOCSensor
{
public:
    virtual ~ISOCSensor() = default;

    // Inicializa o sensor de fuel gauge (ex.: MAX17048 via I²C).
    // Retorna true se o sensor foi inicializado com sucesso.
    virtual bool begin() = 0;

    // Estado de carga da bateria em % (0..100).
    virtual float readSOCPercent() = 0;

    // Tensão da célula/pack medida pelo sensor (V).
    virtual float readVoltage() = 0;
};
