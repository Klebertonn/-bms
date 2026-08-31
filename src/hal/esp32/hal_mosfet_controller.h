#pragma once

/*
 * ==========================================================
 * Adaptador HAL — Controle de MOSFET via GPIO (ESP32)
 * ==========================================================
 * Conecta o estado industrial do BMS (BmsState) às saídas
 * digitais reais (MOSFET de carga, descarga e balanceamento)
 * através da interface IGpioOutput.
 *
 * Este módulo é um adaptador (padrão Bridge/Adapter) que ISOLA
 * o hardware do core/. O MosfetController original (core/mosfet)
 * permanece intacto e continua sendo usado pelo ambiente native.
 *
 * Apenas compilado no target esp32dev (via build_src_filter).
 * ==========================================================
 */

#include "lib/hal/gpio/gpio_interface.h"
#include "lib/hal/gpio/mosfet_driver_interface.h"
#include "core/state/bms_state_manager.h"

namespace hal
{

class HalMosfetController : public IMosfetDriver
{
public:
    // Injeção de dependências: as saídas GPIO reais são fornecidas
    // pelo driver HAL (src/hal/esp32/hal_esp32_gpio.cpp).
    HalMosfetController(IGpioOutput& charge,
                        IGpioOutput& discharge,
                        IGpioOutput& balance);

    // Inicializa as saídas em estado seguro (tudo OFF).
    void init() override;

    // Aplica o estado industrial derivado de BmsState nas saídas reais.
    void applyState(BmsState state) override;

    // Estado real lido das saídas (espelho do hardware).
    bool chargeEnabled() const override;
    bool dischargeEnabled() const override;
    bool balanceEnabled() const override;

private:
    IGpioOutput& charge_;
    IGpioOutput& discharge_;
    IGpioOutput& balance_;

    bool chargeState_ = false;
    bool dischargeState_ = false;
    bool balanceState_ = false;
};

} // namespace hal
