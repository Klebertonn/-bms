#pragma once

#include <stdint.h>

#include "event.h"

class IStorageBackend
{
public:
    virtual ~IStorageBackend() = default;

    virtual void save(const Event& e) = 0;
};

