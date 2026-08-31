#include "storage_manager.h"

#include "storage_crc.h"

#include "storage_interface.h"

// Driver global (NVS/EEPROM/Flash).
// Definido em drivers (ex.: nvs_driver.cpp) para resolver link/compilação.
extern IStorageDriver& storageDriver;

StorageManager::StorageManager(IStorageDriver* driver)
    : driver_(driver != nullptr ? driver : &storageDriver)
{
}

void StorageManager::setStorageDriver(IStorageDriver& driver)
{
    driver_ = &driver;
}

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
    if (driver_ == nullptr || !driver_->load(tmp))
        return false;

    storage = tmp;

    if (storage.version != STORAGE_VERSION)
    {
        return false;
    }

    if (!validateCRC())
    {
        return false;
    }

    return true;
}

bool StorageManager::save()
{
    if (driver_ == nullptr)
        return false;

    updateCRC();
    return driver_->save(storage);
}

void StorageManager::factoryReset()
{
    if (driver_ != nullptr)
    {
        driver_->erase();
    }
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

