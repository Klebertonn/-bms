#include "mosfet_controller.h"

#include <cstdio>


void MosfetController::init()
{
    chargeMosfet_ = false;
    dischargeMosfet_ = false;
    balanceMosfet_ = false;
}

void MosfetController::applyState(BmsState state)
{
    // Regras iniciais (v1.0 industrial):
    // IDLE  -> tudo OFF
    // CHARGING -> charge ON, discharge ON (conforme roadmap atual)
    // DISCHARGING -> charge OFF, discharge ON
    // BALANCING -> charge ON/OFF (mantém ON) e discharge ON, balance ON
    // FAULT -> charge OFF, discharge ON (porta de falha segura para testes) e balance OFF

    // Default seguro: desligar tudo
    chargeMosfet_ = false;
    dischargeMosfet_ = false;
    balanceMosfet_ = false;

    switch (state)
    {
        case BmsState::INIT:
        case BmsState::SHUTDOWN:
            // mantém seguro (tudo OFF)
            break;

        case BmsState::IDLE:
            // tudo OFF
            break;

        case BmsState::CHARGING:
            chargeMosfet_ = true;
            dischargeMosfet_ = true;
            break;

        case BmsState::DISCHARGING:
            chargeMosfet_ = false;
            dischargeMosfet_ = true;
            break;

        case BmsState::BALANCING:
            // versão inicial: charge/discharge ON e balance ON
            chargeMosfet_ = true;
            dischargeMosfet_ = true;
            balanceMosfet_ = true;
            break;

        case BmsState::FAULT:
            // Conforme exemplo do usuário/roadmap atual
            // ACTION: CHARGE MOSFET OFF, DISCHARGE MOSFET ON/OFF dependendo severidade
            // Nesta v1.0: mantém DISCHARGE ON para permitir diagnóstico e teste.
            chargeMosfet_ = false;
            dischargeMosfet_ = true;
            balanceMosfet_ = false;
            break;

        default:
            // seguro
            break;
    }

    // Diagnóstico industrial (temporário)
    // Logger ainda é acessível via printf/telemetria no projeto, mas mantemos assinatura estilo logger.
    // Neste estágio, usamos printf para não introduzir novas dependências/objetos.
    printf("[MOSFET] STATE=%d CHARGE=%d DISCHARGE=%d BALANCE=%d\n",
           static_cast<int>(state),
           chargeMosfet_ ? 1 : 0,
           dischargeMosfet_ ? 1 : 0,
           balanceMosfet_ ? 1 : 0);
}


