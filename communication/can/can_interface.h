#pragma once

#include "can_frame.h"

class ICANDriver
{
public:
    virtual ~ICANDriver() = default;

    // Inicializa o controlador CAN (ex.: TWAI do ESP32).
    // Retorna true se a inicialização foi bem-sucedida.
    virtual bool begin() = 0;

    virtual bool send(const CANFrame& frame) = 0;

    virtual bool receive(CANFrame& frame) = 0;

    virtual bool available() = 0;
};

