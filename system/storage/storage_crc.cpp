#include "storage_crc.h"

uint32_t crc32(const void* data, size_t len)
{
    const uint8_t* p = static_cast<const uint8_t*>(data);

    uint32_t crc = 0xFFFFFFFFu;

    for (size_t i = 0; i < len; ++i)
    {
        crc ^= p[i];
        for (int k = 0; k < 8; ++k)
        {
            uint32_t mask = -(crc & 1u);
            crc = (crc >> 1) ^ (0xEDB88320u & mask);
        }
    }

    return ~crc;
}

