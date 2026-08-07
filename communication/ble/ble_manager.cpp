#include "ble_manager.h"

#include "ble_protocol.h"
#include "ble_service.h"

#include <cstdint>

/*
 * ==========================================================
 * BLE Manager — publica telemetria nos serviços GATT
 * ==========================================================
 */

void BLEManager::init()
{
    createServices();
    isConnected = false;
}

void BLEManager::createServices()
{
    serviceManager_.init();
}

void BLEManager::update()
{
    // Em framework BLE real: processar eventos de conexão/escrita.
    serviceManager_.poll();

    processCommands();
    publishTelemetry();
}

bool BLEManager::connected() const
{
    return isConnected;
}

void BLEManager::processCommands()
{
    // Stub: decodificar comandos e aplicar autenticação/bloqueios.
    // Em framework real, comandos escritos pelo mobile seriam lidos aqui.
}

void BLEManager::publishTelemetry()
{
    std::uint8_t payload[BLE_PAYLOAD_MAX];

    // ---------- Serviço Battery (2 características) ----------

    // 0: Pack Status (voltage, current, soc, soh, flags)
    std::uint8_t n = BLEProtocol::encodeBattery(pack_, payload);
    serviceManager_.setCharacteristicData(BLEServiceId::BATTERY, 0, payload, n);

    // 1: State (1 byte)
    n = BLEProtocol::encodeState(state_, payload);
    serviceManager_.setCharacteristicData(BLEServiceId::BATTERY, 1, payload, n);

    // ---------- Serviço Fault (1 característica) ----------
    n = BLEProtocol::encodeFault(fault_, payload);
    serviceManager_.setCharacteristicData(BLEServiceId::FAULT, 0, payload, n);

    // ---------- Serviço Temperature (1 característica) ----------
    n = BLEProtocol::encodeTemperature(tempAvg_, tempMax_, tempMin_, payload);
    serviceManager_.setCharacteristicData(BLEServiceId::TEMPERATURE, 0, payload, n);

    // ---------- Serviço Current (1 característica) ----------
    n = BLEProtocol::encodeCurrent(currentA_, shuntMv_, payload);
    serviceManager_.setCharacteristicData(BLEServiceId::CURRENT, 0, payload, n);

    // ---------- Serviço Config (1 característica) ----------
    n = BLEProtocol::encodeConfig(cfgMaxV_, cfgMinV_, cfgMaxT_, cfgMaxI_, payload);
    serviceManager_.setCharacteristicData(BLEServiceId::CONFIG, 0, payload, n);

    // ---------- Serviço Logs (1 característica) ----------
    if (setLog_)
    {
        n = BLEProtocol::encodeLog(logTimestamp_, logCode_, payload);
        serviceManager_.setCharacteristicData(BLEServiceId::LOGS, 0, payload, n);
        setLog_ = false;
    }
}
