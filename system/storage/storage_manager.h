#pragma once

#include "storage_types.h"

class StorageManager
{
public:
    void init();

    bool load();
    bool save();

    void factoryReset();

    StorageData& data();

private:
    StorageData storage{};

    bool validateCRC();
    void updateCRC();
    void loadDefaults();

    static constexpr uint32_t STORAGE_VERSION = 1;
};

