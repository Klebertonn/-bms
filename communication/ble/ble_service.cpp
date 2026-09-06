#include "ble_service.h"

#include <cstring>

/*
 * ==========================================================
 * UUIDs 16-bit (base personalizada para o BMS)
 * ==========================================================
 * Base: 0xB000 .. 0xB0FF
 *   Serviços    : 0xB000 (Battery), 0xB001 (Fault), 0xB002 (Temperature),
 *                 0xB003 (Current), 0xB004 (Config), 0xB005 (Logs)
 *   Características dentro de cada serviço (offset).
 * ==========================================================
 */

namespace BLEUUID
{
    namespace Service
    {
        constexpr uint16_t BATTERY     = 0xB000;
        constexpr uint16_t FAULT       = 0xB001;
        constexpr uint16_t TEMPERATURE = 0xB002;
        constexpr uint16_t CURRENT     = 0xB003;
        constexpr uint16_t CONFIG      = 0xB004;
        constexpr uint16_t LOGS        = 0xB005;
    }

    namespace Characteristic
    {
        // Battery
        constexpr uint16_t PACK_STATUS = 0xB010; // voltage, current, soc, soh
        constexpr uint16_t STATE       = 0xB011; // estado do BMS

        // Fault
        constexpr uint16_t FAULT_INFO  = 0xB020; // código, razão, fonte, valor, limite

        // Temperature
        constexpr uint16_t TEMP_INFO   = 0xB030; // média, máx, mín

        // Current
        constexpr uint16_t CURRENT_INFO = 0xB040; // corrente, shunt

        // Config
        constexpr uint16_t CONFIG_INFO = 0xB050; // limites

        // Logs
        constexpr uint16_t LOG_INFO    = 0xB060; // último evento
    }
}

void BLEServiceManager::init()
{
    createServices();
}

void BLEServiceManager::createServices()
{
    // ---------- Serviço Battery ----------
    services_[static_cast<int>(BLEServiceId::BATTERY)].name = "Battery";
    services_[static_cast<int>(BLEServiceId::BATTERY)].uuid = BLEUUID::Service::BATTERY;
    services_[static_cast<int>(BLEServiceId::BATTERY)].characteristicCount = 2;
    services_[static_cast<int>(BLEServiceId::BATTERY)].characteristics = batteryChars_;

    batteryChars_[0].name = "Pack Status";
    batteryChars_[0].uuid = BLEUUID::Characteristic::PACK_STATUS;
    batteryChars_[0].properties = BLE_CHAR_READ | BLE_CHAR_NOTIFY;
    batteryChars_[0].length = 0;

    batteryChars_[1].name = "State";
    batteryChars_[1].uuid = BLEUUID::Characteristic::STATE;
    batteryChars_[1].properties = BLE_CHAR_READ;
    batteryChars_[1].length = 0;

    // ---------- Serviço Fault ----------
    services_[static_cast<int>(BLEServiceId::FAULT)].name = "Fault";
    services_[static_cast<int>(BLEServiceId::FAULT)].uuid = BLEUUID::Service::FAULT;
    services_[static_cast<int>(BLEServiceId::FAULT)].characteristicCount = 1;
    services_[static_cast<int>(BLEServiceId::FAULT)].characteristics = faultChars_;

    faultChars_[0].name = "Fault Info";
    faultChars_[0].uuid = BLEUUID::Characteristic::FAULT_INFO;
    faultChars_[0].properties = BLE_CHAR_READ | BLE_CHAR_NOTIFY;
    faultChars_[0].length = 0;

    // ---------- Serviço Temperature ----------
    services_[static_cast<int>(BLEServiceId::TEMPERATURE)].name = "Temperature";
    services_[static_cast<int>(BLEServiceId::TEMPERATURE)].uuid = BLEUUID::Service::TEMPERATURE;
    services_[static_cast<int>(BLEServiceId::TEMPERATURE)].characteristicCount = 1;
    services_[static_cast<int>(BLEServiceId::TEMPERATURE)].characteristics = tempChars_;

    tempChars_[0].name = "Temperature Info";
    tempChars_[0].uuid = BLEUUID::Characteristic::TEMP_INFO;
    tempChars_[0].properties = BLE_CHAR_READ | BLE_CHAR_NOTIFY;
    tempChars_[0].length = 0;

    // ---------- Serviço Current ----------
    services_[static_cast<int>(BLEServiceId::CURRENT)].name = "Current";
    services_[static_cast<int>(BLEServiceId::CURRENT)].uuid = BLEUUID::Service::CURRENT;
    services_[static_cast<int>(BLEServiceId::CURRENT)].characteristicCount = 1;
    services_[static_cast<int>(BLEServiceId::CURRENT)].characteristics = currentChars_;

    currentChars_[0].name = "Current Info";
    currentChars_[0].uuid = BLEUUID::Characteristic::CURRENT_INFO;
    currentChars_[0].properties = BLE_CHAR_READ | BLE_CHAR_NOTIFY;
    currentChars_[0].length = 0;

    // ---------- Serviço Config ----------
    services_[static_cast<int>(BLEServiceId::CONFIG)].name = "Config";
    services_[static_cast<int>(BLEServiceId::CONFIG)].uuid = BLEUUID::Service::CONFIG;
    services_[static_cast<int>(BLEServiceId::CONFIG)].characteristicCount = 1;
    services_[static_cast<int>(BLEServiceId::CONFIG)].characteristics = configChars_;

    configChars_[0].name = "Config Info";
    configChars_[0].uuid = BLEUUID::Characteristic::CONFIG_INFO;
    configChars_[0].properties = BLE_CHAR_READ;
    configChars_[0].length = 0;

    // ---------- Serviço Logs ----------
    services_[static_cast<int>(BLEServiceId::LOGS)].name = "Logs";
    services_[static_cast<int>(BLEServiceId::LOGS)].uuid = BLEUUID::Service::LOGS;
    services_[static_cast<int>(BLEServiceId::LOGS)].characteristicCount = 1;
    services_[static_cast<int>(BLEServiceId::LOGS)].characteristics = logChars_;

    logChars_[0].name = "Log Info";
    logChars_[0].uuid = BLEUUID::Characteristic::LOG_INFO;
    logChars_[0].properties = BLE_CHAR_READ | BLE_CHAR_NOTIFY;
    logChars_[0].length = 0;
}

void BLEServiceManager::poll()
{
    // Em um framework BLE real, aqui ocorreria o processamento de eventos
    // (conexão, escrita, callbacks de leitura). Nesta camada de abstração,
    // a leitura é feita diretamente pelo buffer de cada característica.
}

BmsBLEService* BLEServiceManager::getService(BLEServiceId id)
{
    const int idx = static_cast<int>(id);
    if (idx < 0 || idx >= static_cast<int>(BLEServiceId::COUNT))
    {
        return nullptr;
    }
    return &services_[idx];
}

BmsBLECharacteristic* BLEServiceManager::getCharacteristic(BLEServiceId service, std::uint8_t index)
{
    BmsBLEService* svc = getService(service);
    if (svc == nullptr || index >= svc->characteristicCount)
    {
        return nullptr;
    }
    return &svc->characteristics[index];
}

bool BLEServiceManager::setCharacteristicData(BLEServiceId service, std::uint8_t index,
                                              const std::uint8_t* data, std::uint8_t len)
{
    BmsBLECharacteristic* ch = getCharacteristic(service, index);
    if (ch == nullptr || data == nullptr)
    {
        return false;
    }

    if (len > BLE_CHAR_MAX_LEN)
    {
        len = BLE_CHAR_MAX_LEN;
    }

    std::memcpy(ch->data, data, len);
    ch->length = len;
    return true;
}
