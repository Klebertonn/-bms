#include "storage_interface.h"

class NVSStorageDriver : public IStorageDriver
{
public:
    bool load(StorageData& data) override
    {
        (void)data;
        return false;
    }

    bool save(const StorageData& data) override
    {
        (void)data;
        return true;
    }

    bool erase() override
    {
        return true;
    }
};

static NVSStorageDriver g_storage;

IStorageDriver& storageDriver = g_storage;