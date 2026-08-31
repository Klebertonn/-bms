/*
 * ==========================================================
 * Driver HAL — CAN TWAI (ESP32)
 * ==========================================================
 * Implementa ICANDriver usando o controlador TWAI do ESP32.
 * Consome apenas as constantes de config/config.h.
 *
 * Apenas compilado no target esp32dev.
 * ==========================================================
 */
#include "communication/can/can_interface.h"

#include "config/config.h"

#include "driver/twai.h"

class TwaiCanDriver : public ICANDriver
{
public:
    TwaiCanDriver()
    {
    }

bool begin() override
    {
        twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
            static_cast<gpio_num_t>(HW_CAN_TX_GPIO),
            static_cast<gpio_num_t>(HW_CAN_RX_GPIO),
            TWAI_MODE_NORMAL);

        twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
        twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

        if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK)
        {
            return false;
        }

        return (twai_start() == ESP_OK);
    }

    bool send(const CANFrame& frame) override
    {
        twai_message_t msg = {};
        msg.identifier = frame.id;
        msg.data_length_code = frame.dlc;
        msg.extd = (frame.id > 0x7FF);  // frames > 11 bits são extended

        for (std::uint8_t i = 0; i < frame.dlc && i < 8; ++i)
        {
            msg.data[i] = frame.data[i];
        }

        return (twai_transmit(&msg, pdMS_TO_TICKS(100)) == ESP_OK);
    }

    bool receive(CANFrame& frame) override
    {
        twai_message_t msg = {};
        if (twai_receive(&msg, pdMS_TO_TICKS(0)) != ESP_OK)
        {
            return false;
        }

        frame.id = msg.identifier;
        frame.dlc = msg.data_length_code;
        for (std::uint8_t i = 0; i < frame.dlc && i < 8; ++i)
        {
            frame.data[i] = msg.data[i];
        }
        return true;
    }

    bool available() override
    {
        // Valida se há mensagem na fila sem bloqueio.
        twai_message_t msg = {};
        return (twai_receive(&msg, 0) == ESP_OK);
    }
};

static TwaiCanDriver g_can;

ICANDriver& can = g_can;
