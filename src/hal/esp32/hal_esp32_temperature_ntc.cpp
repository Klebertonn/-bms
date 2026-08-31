/*
 * ==========================================================
 * Driver HAL — Sensor de Temperatura NTC (ESP32)
 * ==========================================================
 * Implementa ITemperatureSensor usando NTC + divisor de tensão
 * via ADC (equação de Steinhart-Hart / B-coefficient).
 * Consome apenas as constantes de config/config.h.
 *
 * Apenas compilado no target esp32dev.
 * ==========================================================
 */
#include "lib/hal/temperature/temperature_interface.h"

#include "config/config.h"

#include <cmath>

#include "driver/adc.h"

class NtcTemperatureSensor : public ITemperatureSensor
{
public:
    NtcTemperatureSensor()
    {
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(static_cast<adc1_channel_t>(HW_NTC_ADC_CHANNEL), ADC_ATTEN_DB_11);
    }

    float readTemperature(int sensor) override
    {
        (void)sensor;

        const int raw = adc1_get_raw(static_cast<adc1_channel_t>(HW_NTC_ADC_CHANNEL));

        // Tensão lida no divisor (NTC p/ GND, resistor série p/ VCC)
        const float vout = (raw / 4095.0f) * 3.3f;

        // Resistência do NTC pelo divisor de tensão
        // Vout = Vcc * Rntc / (Rserie + Rntc)
        const float rntc = HW_NTC_SERIES_RESISTOR_OHM * (vout / (3.3f - vout));

        if (rntc <= 0.0f)
        {
            return -40.0f;  // valor de erro (fora da faixa)
        }

        // Equação B-coefficient (aproximação de Steinhart-Hart)
        const float t0 = HW_NTC_NOMINAL_TEMP_C + 273.15f;
        const float beta = HW_NTC_B_COEFFICIENT;
        const float r0 = HW_NTC_NOMINAL_RESISTOR_OHM;

        const float invT = (1.0f / t0) + (1.0f / beta) * std::log(rntc / r0);
        const float tempK = 1.0f / invT;

        return tempK - 273.15f;
    }
};

static NtcTemperatureSensor g_ntc;

ITemperatureSensor& temperatureSensor = g_ntc;
