#pragma once

#include "storage_types.h"
#include "storage_interface.h"

class StorageManager
{
public:
    StorageManager(IStorageDriver* driver = nullptr);

    void setStorageDriver(IStorageDriver& driver);
    void init();

    bool load();
    bool save();

    void factoryReset();

    StorageData& data();

private:
    StorageData storage{};
    IStorageDriver* driver_ = nullptr;

    bool validateCRC();
    void updateCRC();
    void loadDefaults();

    static constexpr uint32_t STORAGE_VERSION = 1;
};

