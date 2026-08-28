#include "storage_interface.h"

class NVSStorageDriver : public IStorageDriver
{
public:
    bool load(StorageData& data) override
    {
        data = storage_;
        return storage_.version != 0u || storage_.crc != 0u || storage_.soc != 0.0f || storage_.soh != 0.0f;
    }

    bool save(const StorageData& data) override
    {
        storage_ = data;
        return true;
    }

    bool erase() override
    {
        storage_ = StorageData{};
        return true;
    }

private:
    StorageData storage_{};
};

static NVSStorageDriver g_storage;

IStorageDriver& storageDriver = g_storage;