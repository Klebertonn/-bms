/*
 * ==========================================================
 * Driver HAL — Sensor de SOC MAX17048 (ESP32)
 * ==========================================================
 * Implementa ISOCSensor usando o MAX17048 (fuel gauge) via I²C.
 * Consome apenas as constantes de config/config.h.
 *
 * Apenas compilado no target esp32dev.
 * ==========================================================
 */
#include "lib/hal/soc/soc_sensor_interface.h"

#include "config/config.h"

#include <Wire.h>

class Max17048SocSensor : public ISOCSensor
{
public:
    Max17048SocSensor()
        : address_(HW_MAX17048_I2C_ADDR)
    {
    }

bool begin() override
    {
        Wire.begin(HW_I2C_SDA_PIN, HW_I2C_SCL_PIN);
        // Verifica presença do dispositivo
        Wire.beginTransmission(address_);
        return (Wire.endTransmission() == 0);
    }

    float readSOCPercent() override
    {
        // Registro SOC (0x04): SOC = (MSB + LSB/256) e 1% está em 0x40
        const std::uint16_t raw = readRegister16(0x04);
        return (raw >> 8) + ((raw & 0xFF) / 256.0f);
    }

    float readVoltage() override
    {
        // Registro VCELL (0x02): tensão em 78.125 µV por LSB
        const std::uint16_t raw = readRegister16(0x02);
        return (raw >> 4) * 78.125e-6f;
    }

private:
    std::uint16_t readRegister16(std::uint8_t reg)
    {
        Wire.beginTransmission(address_);
        Wire.write(reg);
        Wire.endTransmission(false);

        Wire.requestFrom(static_cast<int>(address_), 2);
        if (Wire.available() >= 2)
        {
            const std::uint8_t msb = Wire.read();
            const std::uint8_t lsb = Wire.read();
            return static_cast<std::uint16_t>((msb << 8) | lsb);
        }
        return 0;
    }

    std::uint8_t address_;
};

static Max17048SocSensor g_max17048;

ISOCSensor& socSensor = g_max17048;
