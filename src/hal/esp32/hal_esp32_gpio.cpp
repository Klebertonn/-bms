/*
 * ==========================================================
 * Driver HAL — Saídas Digitais ESP32 (MOSFET / Relé)
 * ==========================================================
 * Implementa IGpioOutput para controle de GPIO no ESP32.
 * Consome apenas as constantes de config/config.h.
 *
 * Apenas compilado no target esp32dev.
 * ==========================================================
 */
#include "lib/hal/gpio/gpio_interface.h"

#include "config/config.h"

#include <Arduino.h>

/*
 * GpioOutputEsp32
 * - pin  : GPIO físico (de config/config.h)
 * - activeLow : true se o sinal ativo é LOW (ex.: MOSFET com transistor)
 */
class GpioOutputEsp32 : public IGpioOutput
{
public:
    explicit GpioOutputEsp32(std::uint8_t pin, bool activeLow = false)
        : pin_(pin)
        , activeLow_(activeLow)
    {
    }

    void init()
    {
        pinMode(pin_, OUTPUT);
        off();
    }

    void on() override
    {
        digitalWrite(pin_, activeLow_ ? LOW : HIGH);
        state_ = true;
    }

    void off() override
    {
        digitalWrite(pin_, activeLow_ ? HIGH : LOW);
        state_ = false;
    }

    bool isOn() const override
    {
        return state_;
    }

private:
    std::uint8_t pin_;
    bool activeLow_;
    bool state_ = false;
};

// ---------------------------------------------------------------------
// Instâncias globais para MOSFETs e Relé
// ---------------------------------------------------------------------
static GpioOutputEsp32 g_chargeMosfet(HW_MOSFET_CHARGE_PIN);
static GpioOutputEsp32 g_dischargeMosfet(HW_MOSFET_DISCHARGE_PIN);
static GpioOutputEsp32 g_balanceMosfet(HW_MOSFET_BALANCE_PIN);
static GpioOutputEsp32 g_relay(HW_RELAY_PIN);

// Referências expostas para a lógica / main_esp32
IGpioOutput& chargeMosfetDriver = g_chargeMosfet;
IGpioOutput& dischargeMosfetDriver = g_dischargeMosfet;
IGpioOutput& balanceMosfetDriver = g_balanceMosfet;
IGpioOutput& relayDriver = g_relay;
