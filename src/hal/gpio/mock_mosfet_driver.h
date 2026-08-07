#pragma once

/*
 * ==========================================================
 * Mock HAL — Driver de MOSFET (ambiente native)
 * ==========================================================
 * Implementa IMosfetDriver usando apenas flags internas, sem
 * qualquer acesso a hardware. Usado no ambiente native (PC)
 * para testes e desenvolvimento.
 *
 * A lógica de mapeamento estado → MOSFETs é idêntica à do
 * MosfetController original (core/mosfet), garantindo
 * comportamento equivalente no PC.
 * ==========================================================
 */

#include "lib/hal/gpio/mosfet_driver_interface.h"

class MockMosfetDriver : public IMosfetDriver
{
public:
    void init() override
    {
        charge_ = false;
        discharge_ = false;
        balance_ = false;
    }

    void applyState(BmsState state) override
    {
        // Default seguro: tudo OFF.
        charge_ = false;
        discharge_ = false;
        balance_ = false;

        switch (state)
        {
            case BmsState::INIT:
            case BmsState::SHUTDOWN:
            case BmsState::IDLE:
                break;

            case BmsState::CHARGING:
                charge_ = true;
                discharge_ = true;
                break;

            case BmsState::DISCHARGING:
                charge_ = false;
                discharge_ = true;
                break;

            case BmsState::BALANCING:
                charge_ = true;
                discharge_ = true;
                balance_ = true;
                break;

            case BmsState::FAULT:
                charge_ = false;
                discharge_ = true;
                balance_ = false;
                break;

            default:
                break;
        }
    }

    bool chargeEnabled() const override { return charge_; }
    bool dischargeEnabled() const override { return discharge_; }
    bool balanceEnabled() const override { return balance_; }

private:
    bool charge_ = false;
    bool discharge_ = false;
    bool balance_ = false;
};
