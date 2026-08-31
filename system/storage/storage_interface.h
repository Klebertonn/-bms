#pragma once

#include "storage_types.h"

class IStorageDriver
{
public:
    virtual ~IStorageDriver() = default;

    virtual bool load(StorageData& data) = 0;
    virtual bool save(const StorageData& data) = 0;
    virtual bool erase() = 0;
};

