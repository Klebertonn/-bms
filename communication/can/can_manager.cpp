#include "can_manager.h"

#include "can_ids.h"
#include "can_protocol.h"

#include "communication/can/can_interface.h"

extern ICANDriver& can;


void CANManager::init()
{
    // Placeholder: inicialização do driver pode acontecer via HAL/driver.
}

void CANManager::update()
{
    receive();

    sendHeartbeat();
    sendPackStatus();
    sendSOC();
    sendTemperatures();
    sendVoltages();
}

void CANManager::sendHeartbeat()
{
    // Versão simples: envia periodicamente.
    can.send(CANProtocol::makeHeartbeat(0xAA));
}

void CANManager::sendPackStatus()
{
    CANFrame frame;
    frame.id = CANID::PACK_STATUS;
    frame.dlc = 8;

    // TODO: preencher com métricas reais (tensões/corrente/estados/eventos)
    for (int i = 0; i < 8; i++)
        frame.data[i] = 0;

    can.send(frame);
}

void CANManager::sendSOC()
{
    CANFrame frame;
    frame.id = CANID::SOC;
    frame.dlc = 8;

    // TODO: preencher com SOC/SOH e capacidade estimada.
    for (int i = 0; i < 8; i++)
        frame.data[i] = 0;

    can.send(frame);
}

void CANManager::sendTemperatures()
{
    CANFrame frame;
    frame.id = CANID::TEMPERATURES;
    frame.dlc = 8;

    // TODO: preencher com temperaturas.
    for (int i = 0; i < 8; i++)
        frame.data[i] = 0;

    can.send(frame);
}

void CANManager::sendVoltages()
{
    CANFrame frame;
    frame.id = CANID::CELL_VOLTAGES;
    frame.dlc = 8;

    // TODO: preencher com tensões.
    for (int i = 0; i < 8; i++)
        frame.data[i] = 0;

    can.send(frame);
}

void CANManager::receive()
{
    CANFrame frame;

    while (can.available())
    {
        if (can.receive(frame))
        {
            processFrame(frame);
        }
    }
}

void CANManager::processFrame(const CANFrame& frame)
{
    switch (frame.id)
    {
    case CANID::CONFIG:
        // TODO: atualizar parâmetros via payload.
        break;

    default:
        break;
    }
}

