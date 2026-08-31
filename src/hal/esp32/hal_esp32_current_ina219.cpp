/*
 * ==========================================================
 * Driver HAL — Sensor de Corrente INA219 (ESP32)
 * ==========================================================
 * Implementa ICurrentSensor usando o INA219 via I²C.
 * Consome apenas as constantes de config/config.h.
 *
 * Apenas compilado no target esp32dev.
 * ==========================================================
 */
#include "lib/hal/current/current_interface.h"

#include "config/config.h"

#include <Wire.h>
#include <Adafruit_INA219.h>

class Ina219CurrentSensor : public ICurrentSensor
{
public:
    Ina219CurrentSensor()
        : ina_()
    {
    }

bool begin() override
    {
        Wire.begin(HW_I2C_SDA_PIN, HW_I2C_SCL_PIN);
        return ina_.begin(HW_INA219_I2C_ADDR);
    }

    float readCurrentA() override
    {
        return ina_.getCurrent_mA() / 1000.0f;
    }

    float readShuntVoltageMv() override
    {
        return ina_.getShuntVoltage_mV();
    }

private:
    Adafruit_INA219 ina_;
};

static Ina219CurrentSensor g_ina219;

ICurrentSensor& currentSensor = g_ina219;
