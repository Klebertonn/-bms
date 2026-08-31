/*
 * ==========================================================
 * main_esp32.cpp — Ponto de entrada do ESP32 (Hardware Real)
 * ==========================================================
 * Orquestra a inicialização da camada HAL real e o loop
 * principal do BMS. Apenas compilado no target `esp32dev`
 * (excluído do ambiente native via build_src_filter).
 *
 * Fluxo de inicialização:
 *   Hardware (ADC, I2C, MOSFET, Relé, CAN, BLE, OLED)
 *   → App (lógica BMS)
 *   → loop: sensores → App.update → CAN → BLE → display → watchdog
 * ==========================================================
 */

#include <Arduino.h>

#include "app/app.h"

#include "config/config.h"

// Interfaces HAL (globals extern definidas pelos drivers esp32dev)
#include "lib/hal/adc/adc_interface.h"
#include "lib/hal/current/current_interface.h"
#include "lib/hal/temperature/temperature_interface.h"
#include "lib/hal/soc/soc_sensor_interface.h"
#include "lib/hal/display/display_interface.h"
#include "lib/hal/gpio/gpio_interface.h"
#include "lib/hal/gpio/mosfet_driver_interface.h"

#include "communication/can/can_interface.h"
#include "communication/can/can_ids.h"
#include "communication/can/can_frame.h"

#include "src/hal/esp32/hal_mosfet_controller.h"
#include "src/hal/esp32/hal_esp32_ble.h"

#include "system/clock/clock.h"

// ---------------------------------------------------------------------
// Globals extern fornecidas pelos drivers HAL esp32dev
// ---------------------------------------------------------------------
extern IADC& adc;
extern ICurrentSensor& currentSensor;
extern ITemperatureSensor& temperatureSensor;
extern ISOCSensor& socSensor;
extern IDisplay& display;
extern IGpioOutput& chargeMosfetDriver;
extern IGpioOutput& dischargeMosfetDriver;
extern IGpioOutput& balanceMosfetDriver;
extern IGpioOutput& relayDriver;
extern ICANDriver& can;
extern BleBridge& bleBridge;

// Driver de MOSFET real conectado aos GPIOs do ESP32
static hal::HalMosfetController halMosfetController(
    chargeMosfetDriver,
    dischargeMosfetDriver,
    balanceMosfetDriver);

// Aplicação BMS (lógica independente de hardware)
static App app;

// Último tick (ms) para latência de loop
static std::uint32_t lastLoopMs = 0;

// Contadores de emit para CAN/display (períodos da config)
static std::uint32_t lastCanMs = 0;
static std::uint32_t lastDisplayMs = 0;

void setup()
{
    Serial.begin(115200);
    delay(200);
    Serial.println("[BMS] ESP32 boot...");

    // -----------------------------------------------------------------
    // 1. Inicialização de hardware (HAL real)
    // -----------------------------------------------------------------

    // ADC (tensão das células) — configurado no construtor do driver.
    Serial.println("[HW] ADC ready");

    // I²C + INA219 (corrente)
    if (currentSensor.begin())
    {
        Serial.println("[HW] INA219 OK");
    }
    else
    {
        Serial.println("[HW] INA219 FAIL");
    }

    // MAX17048 (SOC / fuel gauge)
    if (socSensor.begin())
    {
        Serial.println("[HW] MAX17048 OK");
    }
    else
    {
        Serial.println("[HW] MAX17048 FAIL");
    }

    // NTC (temperatura) — configurado no construtor do driver.
    Serial.println("[HW] NTC ready");

    // Display OLED
    if (display.init())
    {
        display.clear();
        display.printLine(0, "BMS ESP32");
        display.refresh();
        Serial.println("[HW] OLED OK");
    }
    else
    {
        Serial.println("[HW] OLED FAIL");
    }

    // CAN TWAI
    if (can.begin())
    {
        Serial.println("[HW] CAN OK");
    }
    else
    {
        Serial.println("[HW] CAN FAIL");
    }

    // BLE
    if (bleBridge.begin())
    {
        Serial.println("[HW] BLE OK");
    }
    else
    {
        Serial.println("[HW] BLE FAIL");
    }

    // -----------------------------------------------------------------
    // 2. Injeção do driver de MOSFET real (GPIO) no App
    //    (deve ser chamado antes de App::init())
    // -----------------------------------------------------------------
    app.setMosfetDriver(reinterpret_cast<IMosfetDriver*>(&halMosfetController));

    // -----------------------------------------------------------------
    // 3. Inicialização da lógica BMS
    // -----------------------------------------------------------------
    app.init();

    lastLoopMs = static_cast<std::uint32_t>(Clock::millis());
    lastCanMs = lastLoopMs;
    lastDisplayMs = lastLoopMs;

    Serial.println("[BMS] READY");
}

void loop()
{
    const std::uint32_t now = static_cast<std::uint32_t>(Clock::millis());

    // -----------------------------------------------------------------
    // 1. Lógica BMS (telemetria, proteções, estado, MOSFETs)
    // -----------------------------------------------------------------
    app.update();

    // -----------------------------------------------------------------
    // 2. CAN — envio periódico de telemetria
    // -----------------------------------------------------------------
    if (now - lastCanMs >= TELEMETRY_PERIOD_MS)
    {
        lastCanMs = now;

        // Heartbeat (frame 0x100)
        CANFrame heartbeat;
        heartbeat.id = CANID::HEARTBEAT;
        heartbeat.dlc = 1;
        heartbeat.data[0] = 0x01;  // alive
        can.send(heartbeat);

        // Exemplo de frame de status do pack (0x101)
        CANFrame packStatus;
        packStatus.id = CANID::PACK_STATUS;
        packStatus.dlc = 2;
        packStatus.data[0] = 0;  // flags (placeholder)
        packStatus.data[1] = 0;  // estado (placeholder)
        can.send(packStatus);
    }

    // Recebe frames (diagnóstico/CLI)
    can.receive();

    // -----------------------------------------------------------------
    // 3. BLE — poll dos serviços GATT
    // -----------------------------------------------------------------
    bleBridge.poll();

    // -----------------------------------------------------------------
    // 4. Display — refresh periódico
    // -----------------------------------------------------------------
    if (now - lastDisplayMs >= TELEMETRY_PERIOD_MS)
    {
        lastDisplayMs = now;
        display.refresh();
    }

    // -----------------------------------------------------------------
    // 5. Controle de latência do loop (período principal)
    // -----------------------------------------------------------------
    const std::uint32_t elapsed = static_cast<std::uint32_t>(Clock::millis()) - now;
    if (elapsed < MAIN_LOOP_PERIOD_MS)
    {
        delay(MAIN_LOOP_PERIOD_MS - elapsed);
    }
}
