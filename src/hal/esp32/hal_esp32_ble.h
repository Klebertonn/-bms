#pragma once

/*
 * ==========================================================
 * Bridge HAL — BLE ESP32 (Header)
 * ==========================================================
 * Exponha a API do bridge BLE para o main_esp32.
 * A implementação (hal_esp32_ble.cpp) conecta o
 * BLEServiceManager (lógica) ao periférico BLE real.
 * ==========================================================
 */
class BleBridge
{
public:
    bool begin();
    void poll();

    // Conecta a telemetria do BMS aos serviços GATT.
    // (Pode ser expandida conforme necessidade.)
};
