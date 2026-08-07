/*
 * ==========================================================
 * Adaptador HAL — Controle de MOSFET via GPIO (ESP32)
 * ==========================================================
 * Implementa o HalMosfetController (header na mesma pasta).
 * Conecta o estado industrial do BMS às saídas reais.
 *
 * Apenas compilado no target esp32dev.
 * ==========================================================
 */

#include "hal_mosfet_controller.h"

namespace hal
{

HalMosfetController::HalMosfetController(IGpioOutput& charge,
                                         IGpioOutput& discharge,
                                         IGpioOutput& balance)
    : charge_(charge)
    , discharge_(discharge)
    , balance_(balance)
{
}

void HalMosfetController::init()
{
    // Estado seguro: tudo OFF.
    charge_.off();
    discharge_.off();
    balance_.off();

    chargeState_ = false;
    dischargeState_ = false;
    balanceState_ = false;
}

void HalMosfetController::applyState(BmsState state)
{
    // Regras iniciais (v1.0 industrial) — espelham a lógica do
    // MosfetController (core/mosfet) mas acionam GPIOs reais.
    // Default seguro: tudo OFF.
    switch (state)
    {
        case BmsState::INIT:
        case BmsState::SHUTDOWN:
        case BmsState::IDLE:
            charge_.off();
            discharge_.off();
            balance_.off();
            chargeState_ = false;
            dischargeState_ = false;
            balanceState_ = false;
            break;

        case BmsState::CHARGING:
            charge_.on();
            discharge_.on();
            balance_.off();
            chargeState_ = true;
            dischargeState_ = true;
            balanceState_ = false;
            break;

        case BmsState::DISCHARGING:
            charge_.off();
            discharge_.on();
            balance_.off();
            chargeState_ = false;
            dischargeState_ = true;
            balanceState_ = false;
            break;

        case BmsState::BALANCING:
            charge_.on();
            discharge_.on();
            balance_.on();
            chargeState_ = true;
            dischargeState_ = true;
            balanceState_ = true;
            break;

        case BmsState::FAULT:
            // Porta de falha segura: charge OFF, discharge ON (diagnóstico).
            charge_.off();
            discharge_.on();
            balance_.off();
            chargeState_ = false;
            dischargeState_ = true;
            balanceState_ = false;
            break;

        default:
            charge_.off();
            discharge_.off();
            balance_.off();
            chargeState_ = false;
            dischargeState_ = false;
            balanceState_ = false;
            break;
    }
}

bool HalMosfetController::chargeEnabled() const
{
    return chargeState_;
}

bool HalMosfetController::dischargeEnabled() const
{
    return dischargeState_;
}

bool HalMosfetController::balanceEnabled() const
{
    return balanceState_;
}

} // namespace hal
