#pragma once

/*
 * ==========================================================
 * Interface HAL — Saída Digital (GPIO)
 * ==========================================================
 * Abstrai o controle de saídas digitais (MOSFETs e Relé).
 * A lógica de negócio consome esta interface, mantendo-se
 * independente do hardware (ESP32).
 * ==========================================================
 */
class IGpioOutput
{
public:
    virtual ~IGpioOutput() = default;

    // Ativa a saída (HIGH).
    virtual void on() = 0;

    // Desativa a saída (LOW).
    virtual void off() = 0;

    // Retorna o estado atual da saída (true = ON).
    virtual bool isOn() const = 0;
};
