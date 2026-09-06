/*
 * ==========================================================
 * Driver HAL — ADC ESP32
 * ==========================================================
 * Implementa IADC para leitura de tensão das células via ADC.
 * Consome apenas as constantes de config/config.h.
 *
 * Apenas compilado no target esp32dev.
 * ==========================================================
 */
#include "lib/hal/adc/adc_interface.h"

#include "config/config.h"

// Arduino framework (ESP32)
#include "driver/adc.h"

class Esp32ADC : public IADC
{
public:
    Esp32ADC()
    {
        // Configura ADC1 (atenuação típica 11dB → 0..3.3V aprox.)
        for (std::uint8_t index = 0; index < PACK_CELL_COUNT; ++index)
        {
            adc1_config_width(ADC_WIDTH_BIT_12);
            adc1_config_channel_atten(
                static_cast<adc1_channel_t>(HW_CELL_ADC_CHANNELS[index]), ADC_ATTEN_DB_12);
        }
    }

    float readCell(int index) override
    {
        if (index < 0 || index >= static_cast<int>(PACK_CELL_COUNT))
        {
            return 0.0f;
        }

        const int raw = adc1_get_raw(
            static_cast<adc1_channel_t>(HW_CELL_ADC_CHANNELS[index]));

        // Converte ADC (12 bits, 0..4095) p/ 0..3.3V
        float voltage = (raw / 4095.0f) * 3.3f;

        // NOTE: células em série com divisor/front-end. Ajustar o fator
        // de escala conforme o hardware (divisor de tensão) aqui.
        constexpr float SCALE_FACTOR = 1.0f;
        return voltage * SCALE_FACTOR;
    }
};

static Esp32ADC g_adc;

IADC& adc = g_adc;
