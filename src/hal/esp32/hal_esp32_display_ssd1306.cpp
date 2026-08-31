/*
 * ==========================================================
 * Driver HAL — Display OLED SSD1306 (ESP32)
 * ==========================================================
 * Implementa IDisplay usando o SSD1306 via I²C.
 * Consome apenas as constantes de config/config.h.
 *
 * Apenas compilado no target esp32dev.
 * ==========================================================
 */
#include "lib/hal/display/display_interface.h"

#include "config/config.h"

#include <Wire.h>
#include <Adafruit_SSD1306.h>

class Ssd1306Display : public IDisplay
{
public:
    Ssd1306Display()
        : display_(HW_OLED_WIDTH, HW_OLED_HEIGHT, &Wire, -1)
    {
    }

    bool init() override
    {
        Wire.begin(HW_I2C_SDA_PIN, HW_I2C_SCL_PIN);
        return display_.begin(SSD1306_SWITCHCAPVCC, HW_OLED_I2C_ADDR);
    }

    void clear() override
    {
        display_.clearDisplay();
    }

    void printLine(uint8_t line, const char* text) override
    {
        display_.setCursor(0, static_cast<int16_t>(line) * 8);
        display_.print(text);
    }

    void refresh() override
    {
        display_.display();
    }

private:
    Adafruit_SSD1306 display_;
};

static Ssd1306Display g_display;

IDisplay& display = g_display;
