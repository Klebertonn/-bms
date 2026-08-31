/*
 * ==========================================================
 * Bridge HAL — BLE ESP32
 * ==========================================================
 * Conecta o BLEServiceManager (lógica/NVS) ao periférico BLE
 * real do ESP32 (Bluedroid/NimBLE).
 *
 * Mantém a lógica (BLEServiceManager) independente do driver.
 * Apenas compilado no target esp32dev.
 * ==========================================================
 */
#include "hal_esp32_ble.h"

#include "communication/ble/ble_service.h"

#include "config/config.h"

#include <Arduino.h>
#if CONFIG_BT_ENABLED
#include "BLEDevice.h"
#endif

static BLEServiceManager serviceManager_;

bool BleBridge::begin()
{
#if CONFIG_BT_ENABLED
    BLEDevice::init("BMS-CAN");
#endif
    // Registra/inicializa os serviços GATT definidos pela lógica.
    serviceManager_.init();
    return true;
}

void BleBridge::poll()
{
    serviceManager_.poll();
}

static BleBridge g_ble;

// Exposto para main_esp32
BleBridge& bleBridge = g_ble;
