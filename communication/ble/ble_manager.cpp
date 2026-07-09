#include "ble_manager.h"

#include "ble_service.h"

#include <stdint.h>

// Mantenha tudo em stub por enquanto: BLE real depende do framework (Arduino-ESP32, NimBLE, ESP-IDF etc.)

static BLEService g_bleService;

void BLEManager::init()
{
    createServices();
    isConnected = false;
}

void BLEManager::update()
{
    // Stub: normalmente aqui você chamaria:
    // - pilha BLE processando eventos
    // - leitura de características escritas pelo mobile
    // - emissão de notificações (telemetria)
    g_bleService.poll();

    processCommands();
    publishTelemetry();
}

bool BLEManager::connected() const
{
    return isConnected;
}

void BLEManager::createServices()
{
    g_bleService.begin();
}

void BLEManager::processCommands()
{
    // Stub: decodificar comandos e aplicar autenticação/bloqueios.
}

void BLEManager::publishTelemetry()
{
    // Stub: publicar notificações com dados do BMS.
}

