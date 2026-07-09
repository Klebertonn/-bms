#include "storage_manager.h"

#include "storage_crc.h"

#include "storage_interface.h"

// Driver global (NVS/EEPROM/Flash).
// Definido em drivers (ex.: nvs_driver.cpp) para resolver link/compilação.
extern IStorageDriver& storageDriver;

void StorageManager::init()
{
    storage.version = STORAGE_VERSION;

    if (!load())
    {
        loadDefaults();
        save();
    }
}

bool StorageManager::load()
{
    StorageData tmp{};
    if (!storageDriver.load(tmp))
        return false;

    storage = tmp;

    // Se version não bater, tentamos tratar como inválido para simplificar.
    if (storage.version != STORAGE_VERSION)
    {
        return false;
    }

    return validateCRC();
}

bool StorageManager::save()
{
    updateCRC();
    return storageDriver.save(storage);
}

void StorageManager::factoryReset()
{
    storageDriver.erase();
    loadDefaults();
    save();
}

StorageData& StorageManager::data()
{
    return storage;
}

bool StorageManager::validateCRC()
{
    StorageData tmp = storage;
    const uint32_t expected = tmp.crc;

    tmp.crc = 0;
    tmp.version = storage.version;

    const uint32_t actual = crc32(&tmp, sizeof(StorageData));

    return actual == expected;
}

void StorageManager::updateCRC()
{
    storage.version = STORAGE_VERSION;

    StorageData tmp = storage;
    tmp.crc = 0;

    storage.crc = crc32(&tmp, sizeof(StorageData));
}

void StorageManager::loadDefaults()
{
    storage.version = STORAGE_VERSION;

    storage.soc = 100.0f;
    storage.soh = 100.0f;
    storage.cycleCount = 0;

    storage.batteryCapacityAh = 100.0f;

    storage.overVoltageLimit = 4.2f;
    storage.underVoltageLimit = 3.0f;
    storage.overTemperatureLimit = 60.0f;
    storage.overCurrentLimit = 200.0f;

    storage.crc = 0;
}

